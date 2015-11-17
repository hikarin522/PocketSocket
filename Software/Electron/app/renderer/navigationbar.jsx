'use strict';
/** @jsx hJSX */

import {h, hJSX} from '@cycle/dom';
const {div, span, nav, h1, h2, h3, h4, h5, h6, p, ul, li, a, button, form, input, table, caption, thead, tbody, tr, th} = require('hyperscript-helpers')(h);

export default function navbar(tab) {
	return nav('.navbar.navbar-inverse.navbar-fixed-top', [
		div('.container-fluid', [
			div('.navbar-header', [
				button('.navbar-toggle.collapsed', {type:'button',attributes:{'data-toggle':'collapse','data-target':'#bs-example-navbar-collapse-2','aria-expanded':false}}, [
					span('.sr-only', 'Toggle navigation'), span('.icon-bar'), span('.icon-bar'), span('.icon-bar')
				]),
				a('.navbar-brand.easy-sidebar-toggle', {href:'#'}, 'Menu')
			]),
			div('.navbar-collapse.collapse#bs-example-navbar-collapse-2', {attributes:{'aria-expanded':false}}, [
				ul('.nav.navbar-nav', [
					li(tab === 'list' ? '.active' : '', a({href:'#',value:'list'}, 'SerialPort')),
					li(tab === 'graph' ? '.active' : '', a({href:'#',value:'graph'}, 'Graph'))
				])
			])
		])
	]);
}

