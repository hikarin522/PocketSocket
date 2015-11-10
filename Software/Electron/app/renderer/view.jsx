'use strict';
/** @jsx hJSX */

import {h, hJSX} from '@cycle/dom';
const {div, span, nav, h1, h2, h3, h4, h5, h6, p, ul, li, a, button, form, input, table, caption, thead, tbody, tr, th} = require('hyperscript-helpers')(h);

import sidebar from './sidebar.jsx';
import navbar from './navigationbar.jsx';

export default function view(state$) {
	return state$.map(({toggle, sidebarPosition}) =>
		div([
			sidebar(toggle),
			div(`.container-fluid.easy-sidebar-container${toggle}`, [
				navbar(),
				div('.container#body', [
					h1('Testing'),
					button('.btn.easy-sidebar-toggle', 'Toggle Sidebar'),
					div('.table-responsive', [
						table('.table.table-hover', [
							caption('table title and/or explanatory text'),
							thead(tr([th('header'), th('header2'), th('header3')])),
							tbody([
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')]),
								tr([th('data'), th('data2'), th('data3')])
							])
						])
					])
				])
			])
		])
	);
}
