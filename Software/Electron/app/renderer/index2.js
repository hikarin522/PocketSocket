'use strict';
/** @jsx hJSX */

const jQuery = require('jquery');
require('bootstrap');

import Rx from 'rx';
import Cycle from '@cycle/core';
import {makeDOMDriver, h, hJSX} from '@cycle/dom';
const {div, span, nav, h1, h2, h3, h4, h5, h6, p, ul, li, a, button, canvas, form, label, input, table, caption, thead, tbody, tr, th} = require('hyperscript-helpers')(h);


import path from 'path';
import remote from 'remote';

import serialInit from './EdgeSerialPort';

var serial = null;

async () => {
	const ready = Rx.Observable.fromCallback(jQuery)().toPromise();
	serial = await serialInit();
	var info = await serial.getPortInfo();
	console.log(info);
	//var port = await serial.openSerialPort('COM4');
	//if (port != null) {
	//	jQuery(window).unload(port.dispose);
	//}
	//port.read.subscribe(x => console.log(`${x.V} ${x.I} ${x.W}`));
	await ready;
	Cycle.run(main, {
		DOM: makeDOMDriver('body'),
		SerialPortInfo: makeSerialPortInfoDriver()
	});
}();

function makeSerialPortInfoDriver() {
	return req$ => req$.selectMany(() => Rx.Observable.fromPromise(serial.getPortInfo()));
}

function main(responses) {
	console.log(responses);
	const actions = intent(responses.DOM);
	const state$ = model(actions, responses.SerialPortInfo);
	const vtree$ = view(state$);
	return {
		DOM: vtree$,
		SerialPortInfo: actions.getPortInfo$
	};
}

function intent(DOM) {
	return {
		getPortInfo$: DOM.select('#portInfo').events('click')
			.map(e => e.target.value)
	};
}

function model(actions, serialPortInfo$) {
	return Rx.Observable.combineLatest(
		actions.getPortInfo$.startWith(null),
		serialPortInfo$.do(x => console.log(x)).startWith({}),
		(portInfoReq, serialPortInfo) => ({portInfoReq, serialPortInfo})
	);
}

function view(state$) {
	return state$.map(({serialPortInfo}) => {

		return div([
			button('.btn#portInfo', 'getPortInfo'),
			serialPortList(serialPortInfo)
		]);
	});
}

function serialPortList(serialPortInfo) {
	return div(Object.keys(serialPortInfo).map(name => div([
		label(h2(name)),
		button('Connect'),
		div('.row', Object.keys(serialPortInfo[name]).map(mc => div('.table-responsive.col-lg-6', [
			label(h3(serialPortInfo[name][mc].Name)),
			table('.table.table-hover', [
				caption(mc),
				thead(tr([th('Property'), th('Value')])),
				tbody(Object.keys(serialPortInfo[name][mc]).map(prop => tr([
					th(prop), th(serialPortInfo[name][mc][prop])
				])))
			])
		])))
	])));
}
