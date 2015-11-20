using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Reactive;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Reactive.Threading.Tasks;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace EdgeSerialPort {
	using EdgeFunc = Func<dynamic, Task<object>>;
	public class Startup {
		public async Task<object> Invoke(object input) {
			return new ISerialPort();
		}
	}

	public class ISerialPort {
		public readonly EdgeFunc getPortInfo;
		public readonly EdgeFunc openSerialPort;

		public ISerialPort() {
			getPortInfo = _getPortInfo;
			openSerialPort = _openSerialPort;
		}

		private static async Task<object> _getPortInfo(object input) {
			return await ObservableManagement.Object("Win32_PnPEntity")
				.Merge(ObservableManagement.Object("Win32_SerialPort"))
				.Select(i => Tuple.Create(i, i["Name"] as string))
				.WhereDispose(i => i.Item2 != null, i => i.Item1)
				.Join(
					SerialPort.GetPortNames().ToObservable()
						.Select(i => Tuple.Create(i, new Regex(i + @"[\p{P}\p{Z}$]"))),
					_ => Observable.Never<Unit>(), _ => Observable.Never<Unit>(),
					Tuple.Create
				)
				.WhereDispose(i => i.Item2.Item2.IsMatch(i.Item1.Item2), i => i.Item1.Item1)
				.Select(i => {
					var list = new Dictionary<string, string>();
					foreach (var prop in i.Item1.Item1.Properties)
						list.Add(prop.Name, prop.Value as string);
					var ret = Tuple.Create(i.Item2.Item1, i.Item1.Item1.ClassPath.ClassName, list);
					i.Item1.Item1.Dispose();
					return ret;
				})
				.GroupBy(i => i.Item1)
				.SelectMany(async i => Tuple.Create(i.Key, await i.ToDictionary(j => j.Item2, j => j.Item3)))
				.ToDictionary(i => i.Item1, i => i.Item2);
		}

		private async Task<object> _openSerialPort(dynamic input) {
			ObservableSerialPort serialPort = null;
			try {
				var portName = input as string;
				var hasOptions = false;
				if (portName == null) {
					portName = input.portName as string;
					hasOptions = true;
				}
				serialPort = new ObservableSerialPort(portName);

				if (hasOptions) {
					try { serialPort.BaudRate = (int)input.baudRate; } catch { }
					try { serialPort.DataBits = (int)input.dataBits; } catch { }
					try { serialPort.Parity = (Parity)Enum.Parse(typeof(Parity), input.parity as string); } catch { }
					try { serialPort.StopBits = (StopBits)Enum.Parse(typeof(StopBits), input.stopBits as string); } catch { }
				}

				var regex = new Regex(@"V\[V\]=,\s{0,2}(\d{1,3}\.\d),I\[A\]=,(\d\.\d{3})", RegexOptions.Compiled | RegexOptions.Singleline);

				var source = serialPort.Where(i => i.Item1 == typeof(SerialData))
					.Select(i => i.Item2)
					.Scan(Tuple.Create(string.Empty, Enumerable.Empty<Tuple<double, double>>()), (Sum, New) => {
						var split = regex.Split(Sum.Item1 + New);
						var filter = split.Where((_, i) => i % 3 != 0).Select(double.Parse);
						var capt = filter.Where((_, i) => i % 2 == 0).Zip(filter.Where((_, i) => i % 2 != 0), Tuple.Create);
						return Tuple.Create(split.Last(), capt);
					}).SelectMany(i => i.Item2)
					.Select(i => new { V = i.Item1, I = i.Item2, W = i.Item1 * i.Item2 })
					.Finally(serialPort.Dispose)
					.Publish();

				serialPort.Open();
				var connect = source.Connect();

				return new {
					dispose = (EdgeFunc)(async _ => {
						connect.Dispose();
						serialPort.Dispose();
						return null;
					}),
					write = (EdgeFunc)(async str => { serialPort.Write(str); return null; }),
					subscribe = (EdgeFunc)(async ob => {
						EdgeFunc onNext, onError = null, onCompleted = null;
						onNext = ob as EdgeFunc;
						if (onNext == null) {
							try { onNext = ob.onNext as EdgeFunc; } catch { }
							try { onError = ob.onError as EdgeFunc; } catch { }
							try { onCompleted = ob.onCompleted as EdgeFunc; } catch { }
						}
						if (ob == null)
							return null;

						var dispose = source.Subscribe(
							i => onNext(i),
							e => { if (onError != null) onError(e); },
							() => { if (onCompleted != null) onCompleted(null); }
						);

						return (EdgeFunc)(async _ => {
							dispose.Dispose();
							return null;
						});
					})
				};
			} catch {
				if (serialPort != null)
					serialPort.Dispose();
				return null;
			}
		}
	}

	static class ObservableManagement {
		private static IObservable<ManagementBaseObject> observableFactory(ManagementClass mc) {
			var ob = new ManagementOperationObserver();
			return Observable.FromEventPattern<ObjectReadyEventHandler, ObjectReadyEventArgs>(
					h => { ob.ObjectReady += h; mc.GetInstances(ob); }, h => ob.ObjectReady -= h
				).TakeUntil(Observable.FromEventPattern<CompletedEventHandler, CompletedEventArgs>(
					h => ob.Completed += h, h => ob.Completed -= h
				)).Select(e => e.EventArgs.NewObject);
		}

		public static IObservable<ManagementBaseObject> Object(string name) {
			return Observable.Using(
				() => new ManagementClass(name),
				observableFactory
			);
		}

		public static IObservable<T> WhereDispose<T>(this IObservable<T> ob, Func<T, bool> predicate) where T : IDisposable {
			return ob.Where(i => {
				if (predicate(i))
					return true;
				i.Dispose();
				return false;
			});
		}

		public static IObservable<T1> WhereDispose<T1, T2>(this IObservable<T1> ob, Func<T1, bool> predicate, Func<T1, T2> selector) where T2 : IDisposable {
			return ob.Where(i => {
				if (predicate(i))
					return true;
				selector(i).Dispose();
				return false;
			});
		}
	}

	class ObservableSerialPort : SerialPort, IObservable<Tuple<Type, string>> {
		public ObservableSerialPort(string name) : base(name) {
		}

		public IDisposable Subscribe(IObserver<Tuple<Type, string>> observer) {
			var rcvEvent = Observable.FromEventPattern<SerialDataReceivedEventHandler, SerialDataReceivedEventArgs>(
					h => DataReceived += h, h => DataReceived -= h
				);

			var rcvEof = rcvEvent.Where(e => e.EventArgs.EventType == SerialData.Eof);
			var rcvChars = rcvEvent.Select(e => Tuple.Create(e.EventArgs.EventType.GetType(), ReadExisting()));

			var errEvent = Observable.FromEventPattern<SerialErrorReceivedEventHandler, SerialErrorReceivedEventArgs>(
					h => ErrorReceived += h, h => ErrorReceived -= h
				).Select(e => Tuple.Create(e.EventArgs.EventType.GetType(), e.EventArgs.EventType.ToString()));

			var pinEvent = Observable.FromEventPattern<SerialPinChangedEventHandler, SerialPinChangedEventArgs>(
					h => PinChanged += h, h => PinChanged -= h
				).Select(e => Tuple.Create(e.EventArgs.EventType.GetType(), e.EventArgs.EventType.ToString()));
			
			return Observable.Merge(rcvChars, errEvent, pinEvent)
				.TakeUntil(rcvEof).Subscribe(observer);
		}
	}
}
