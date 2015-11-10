'use strict';
/** @jsx hJSX */

var jQuery = require('jquery');
require('bootstrap');

import Rx from 'rx';
import Cycle from '@cycle/core';
import {makeDOMDriver, h, hJSX} from '@cycle/dom';

import path from 'path';
import remote from 'remote';
const __dlldir = remote.getGlobal('__dlldir');
const edge = remote.require('electron-edge');

const hello = edge.func(path.join(__dlldir, 'hello.cs'));

hello('World', (error, result) => {
	console.log(result);
});

jQuery(() => {
	Cycle.run(main, {
		DOM: makeDOMDriver('body')
	});
});

function main({DOM}) {
	let actions = intent(DOM);
	let state$ = model(actions);
	return {
		DOM: view(state$)
	};
}

function intent(DOM) {
	return {
		valid$: DOM.select('#name').events('input')
			.map(ev => ev.target.validity.valid)
			.distinctUntilChanged(),
		name$: DOM.select('#submit').events('submit')
			.do(e => e.preventDefault())
			.map(ev => ev.target.name.value)
			.distinctUntilChanged()
	};
}

function model(actions) {
	return Rx.Observable.combineLatest(
		actions.valid$.startWith(false),
		actions.name$.startWith('')
			.do(x => console.log(`Name: ${x}`))
			.selectMany(x => Rx.Observable.fromNodeCallback(hello)(x)),
		(valid, name) =>
			({valid, name})
	);
}

function view(state$) {
	return state$.map(({valid, name}) =>
		<div>
			<header className="jumbotron">
				<div className="container">
					<h1>{name}</h1>
				</div>
			</header>
			<div className="container">
				<form id="submit" className="form-horizontal" action="">
					<div className={`form-group has-${valid ? 'success' : 'warning'} has-feedback`}>
						<label htmlFor="name" className="control-label col-sm-2">Name:</label>
						<div className="col-sm-10">
							<input id="name" type="text" className="form-control" pattern="[a-zA-Z0-9_-]{4,12}" autofocus required />
							<span className={`glyphicon glyphicon-${valid ? 'ok' : 'warning-sign'} form-control-feedback`}></span>
						</div>
					</div>
					<input type="submit" className="col-sm-offset-10" value="submit" />
				</form>
			</div>
		</div>
	);
}
