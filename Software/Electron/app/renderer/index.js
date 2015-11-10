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


async () => {
	const ready = Rx.Observable.fromCallback(jQuery)().toPromise();
	const serial = await serialInit();
	const info = await serial.getPortInfo().toPromise();
	console.log(info);

	await ready;
	Cycle.run(main, {
		DOM: makeDOMDriver('body')
	});

}();

function main({DOM}) {
	let actions = intent(DOM);
	let state$ = model(actions);
	return {
		DOM: view(state$)
	};
}

function intent(DOM) {
	return {
		toggle$: DOM.select('.easy-sidebar-toggle').events('click')
			.do(e => e.preventDefault())
			.map(e => e.target.value),
		sidebarPosition$: DOM.select('[name="sidebarPosition"]').events('change')
			.map(e => e.target.value)
	};
}

var toggle = true;
function model(actions) {
	return Rx.Observable.combineLatest(
		actions.toggle$.startWith('')
			.map(_ => (toggle = !toggle) ? '.toggled' : ''),
		actions.sidebarPosition$.startWith('left'),
		(toggle, sidebarPosition) =>
			({toggle, sidebarPosition})
	);
}
