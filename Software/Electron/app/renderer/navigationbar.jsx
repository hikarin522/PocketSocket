'use strict';
/** @jsx hJSX */

import {h, hJSX} from '@cycle/dom';
const {div, span, nav, h1, h2, h3, h4, h5, h6, p, ul, li, a, button, form, input, table, caption, thead, tbody, tr, th} = require('hyperscript-helpers')(h);

export default function navbar() {
	return nav('.navbar.navbar-default.navbar-fixed-top', [
		div('.container-fluid', [
			div('.navbar-header', [
				button('.navbar-toggle.collapsed', {type:'button',attributes:{'data-toggle':'collapse','data-target':'#bs-example-navbar-collapse-2','aria-expanded':false}}, [
					span('.sr-only', 'Toggle navigation'), span('.icon-bar'), span('.icon-bar'), span('.icon-bar')
				]),
				a('.navbar-brand', {href:'#'}, 'Easy-Sidebar')
			]),
			div('.navbar-collapse.collapse#bs-example-navbar-collapse-2', {attributes:{'aria-expanded':false}}, [
				ul('.nav.navbar-nav', [
					li('.active', a({href:'#'}, ['Link', span('.sr-only', '(current)')])),
					li(a({href:'#'}, 'Link')),
					li('.dropdown', [
						a('.dropdown-toggle', {href:'#',attributes:{role:'button','data-toggle':'dropdown','aria-expanded':false}}, [
							'Dropdown', span('.caret')
						]),
						ul('.dropdown-menu', {attributes:{role:'menu'}}, [
							li(a({href:'#'}, 'Action')),
							li(a({href:'#'}, 'Another action')),
							li(a({href:'#'}, 'Something else here')),
							li('.divider'),
							li(a({href:'#'}, 'Separated link')),
							li('.divider'),
							li(a({href:'#'}, 'One more separated link'))
						])
					])
				]),
				form('.navbar-form.navbar-left', {attributes:{role:'search'}}, [
					div('.form-group', [
						input('.form-control', {type:'text',placeholder:'Search'}),
					]),
					button('.btn.btn-default', {type:'submit'}, 'Submit')
				]),
				ul('.nav.navbar-nav.navbar-right', [
					li(a({href:'#'}, 'Link'))
				])
			])
		])
	]);
}

