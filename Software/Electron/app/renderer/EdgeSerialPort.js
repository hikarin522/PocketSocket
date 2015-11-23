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
		return Rx.Observable.fromNodeCallback(this.dll.getPortInfo)(null);
	}
	async openSerialPort(name) {
		const port = await Rx.Observable.fromNodeCallback(this.dll.openSerialPort)(name).toPromise();
		if (port == null)
			return null;
		return {
			read: Rx.Observable.create(async ob => {
				const dispose = await Rx.Observable.fromCallback(port.subscribe)({
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
				return async () => await Rx.Observable.fromCallback(dispose)(null).toPromise();
			}),
			write: async str => await Rx.Observable.fromCallback(port.write)(str).toPromise(),
			dispose: async () => await Rx.Observable.fromCallback(port.dispose)(null).toPromise()
		};
	}
}
