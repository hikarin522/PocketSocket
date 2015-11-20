'use strict';

import Rx from 'rx';

import path from 'path';
import remote from 'remote';

import serialInit from './EdgeSerialPort';

var serial
async () => {
	serial = await serialInit();
	var info = await serial.getPortInfo();
	console.log(info);
	var port = await serial.openSerialPort('COM4');
	console.log(port);
	port.read.subscribe(x => console.log(`${x.V} ${x.I} ${x.W}`));
}();
