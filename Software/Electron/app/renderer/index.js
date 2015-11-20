'use strict';
/** @jsx hJSX */

var jQuery = require('jquery');
require('bootstrap');

import Rx from 'rx';
import Cycle from '@cycle/core';
import {makeDOMDriver, h, hJSX} from '@cycle/dom';
import view from './view';

import path from 'path';
import remote from 'remote';

import serialInit from './EdgeSerialPort';

var serial = null;
async () => {
	const ready = Rx.Observable.fromCallback(jQuery)().toPromise();
	serial = await serialInit();
	await ready;
	Cycle.run(main, {
		DOM: makeDOMDriver('body'),
		SerialPort: makeSerialPortDriver()
	});
}();

function main({DOM}) {
	let actions = intent(DOM);
	actions.serialPortInfo$ = serial.portInfoSource(250);
	let state$ = model(actions);
	return {
		DOM: view(state$),
		SerialPort: openport(send$)
	};
}

function openport(send$) {
	return send$.map(send => {
		return {};
	});
}

function makeSerialPortInfoDriver() {
	return request$ => request$.subscribe();
}

function makeSerialPortDriver() {
	let ports = {};
	return serialPort$ => {
		serialPort$.subscribe(serialPort => {
			for (var key in serialPort) {
				if (key === 'Open') {
					ports[serialPort[key]] = serial.openSerialPort(serialPort[key]);
				} else if (key === 'Close') {
					delete ports[serialPort[key]];
				} else {
					ports[key].Send(serialPort[key]);
				}
			}
		});
		return ports;
	};
}

function serialPortDriver(name$) {
	let port = null;
	name$.subscribe(i => {
		port = serial.openSerialPort(i.name, i.write$);
	});
	return port;
}

function makeSockDriver(peerId) {
  let sock = new Sock(peerId);
  return function sockDriver(outgoing$) {
    outgoing$.subscribe(outgoing => sock.send(outgoing));
    return Rx.Observable.fromCallback(sock.onReceive)();
  };
}

function intent(DOM) {
	return {
		tab$: DOM.select('#bs-example-navbar-collapse-2 a').events('click')
			.do(e => console.log(e))
			.map(e => e.target.value),
		toggle$: DOM.select('.easy-sidebar-toggle').events('click')
			.do(e => e.preventDefault())
			.map(e => e.target.value),
		serialPort$: DOM.select('.btn-connect').events('click')
			.filter(e => !e.target.classList.contains('disabled'))
			.map(e => e.target.classList.contains('active') ? '' : e.target.value)
	};
}

function model(actions) {
	var toggle = true;
	var dispose = null;
	var powerData$ = new Rx.Subject();
	const regex = /^V\[V\]=, {0,2}(\d{1,3}\.\d),I\[A\]=,(\d\.\d{3})$/;

	return Rx.Observable.combineLatest(
		actions.tab$.startWith('list')
			.scan((acc, current) => ({value: current, change: acc.value !== current}), {value:'', change: true}),
		actions.toggle$.startWith(false)
			.map(_ => (toggle = !toggle)),
		actions.serialPortInfo$.startWith({}),
		actions.serialPort$.startWith('')
			.do(e => {
				if (dispose !== null && e === '') {
					dispose.dispose();
				}
				else if (dispose === null && e !== '') {
					dispose = serial.openSerialPort(e).subscribe(
						x => powerData$.onNext(x),
						e => powerData$.onError(e),
						() => powerData$.onCompleted()
					);

				}
			}),
		powerData$.map(e => e.match(regex))
			.filter(e => e[1] != null && e[2] != null)
			.map(e => ({voltage:e[1],current:e[2],power:e[1]*e[2]}))
			.startWith({voltage:0,current:0,power:0}).do(e => console.log(e)),
		(tab, toggle, serialPortInfo, serialPort, powerData) =>
			({tab, toggle, serialPortInfo, serialPort, powerData})
	);
}
