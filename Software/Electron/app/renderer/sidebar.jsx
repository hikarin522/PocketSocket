'use strict';
/** @jsx hJSX */

import {h, hJSX} from '@cycle/dom';
const {div, span, nav, h1, h2, h3, h4, h5, h6, p, ul, li, a, button, form, input, table, label, caption, thead, tbody, tr, th} = require('hyperscript-helpers')(h);

export default function sidebar(toggle, serialPortInfo, serialPort) {
	return nav(`.navbar.navbar-inverse.easy-sidebar${toggle ? '.toggled' : ''}`, [
		div('.container-fluid', [
			div('.navbar-header', [
				button('.navbar-toggle.easy-sidebar-toggle', {type:'button',attributes:{'aria-expanded':false}}, [
					span('.sr-only', 'Toggle navigation'), span('.icon-bar'), span('.icon-bar'), span('.icon-bar')
				]),
				a('.navbar-brand.easy-sidebar-toggle', {href:'#'}, 'Close')
			]),
			ul('.nav.navbar-nav', Object.keys(serialPortInfo).map(name => li(serialPort === name ? '.active' : '', [
				a({href:`#${name}`}, name)
			])))
		])
	]);
}
