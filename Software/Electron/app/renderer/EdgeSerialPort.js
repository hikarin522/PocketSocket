'use strict';

import Rx from 'rx';
import path from 'path';
import remote from 'remote';
const edge = remote.require('electron-edge');
const __dlldir = remote.getGlobal('__dlldir');

function resolveVSdll(name, root = __dlldir) {
	return path.join(root, `${name}/${name}/bin/Release/${name}.dll`);
}

export default async function () {
	if (!EdgeSerialPort.prototype.dll) {
		const dllPath = resolveVSdll('EdgeSerialPort');
		EdgeSerialPort.prototype.dll = await Rx.Observable.fromNodeCallback(edge.func(dllPath))(null).toPromise();
	}
	return new EdgeSerialPort();
}

class EdgeSerialPort {
	getPortInfo() {
		return Rx.Observable.fromNodeCallback(this.dll.GetPortInfo)(null)
			.map(x => JSON.parse(x));
	}
	portInfoSource(ms) {
		return Rx.Observable.fromNodeCallback(this.dll.PortInfoSource)(ms)
			.selectMany(source => Rx.Observable.create(ob => {
				const dispose = Rx.Observable.fromNodeCallback(source.subscribe)({
					onNext: (data, cb) => {
						ob.onNext(data);
						cb();
					},
					onError: (err, cb) => {
						ob.onError(err);
						cb();
					},
					onCompleted: (_, cb) => {
						ob.onCompleted();
						cb();
					}
				}).toPromise();
				return async () => await Rx.Observable.fromNodeCallback((await dispose).dispose)(null).toPromise();
			})).map(x => JSON.parse(x))
			.do(x => console.log(x));
	}
	openSerialPort(name) {
		return Rx.Observable.fromNodeCallback(this.dll.OpenSerialPort)(name)
			.selectMany(source => Rx.Observable.create(ob => {
				const dispose = Rx.Observable.fromNodeCallback(source.subscribe)({
					onNext: (data, cb) => {
						ob.onNext(data);
						cb();
					},
					onError: (err, cb) => {
						ob.onError(err);
						cb();
					},
					onCompleted: (_, cb) => {
						ob.onCompleted();
						cb();
					}
				}).toPromise();
				return async () => await Rx.Observable.fromNodeCallback((await dispose).dispose)(null).toPromise();
			}));
	}
}
