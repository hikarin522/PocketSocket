'use strict';
/** @jsx hJSX */

import {h, hJSX} from '@cycle/dom';
const {div, span, nav, h1, h2, h3, h4, h5, h6, p, ul, li, a, button, canvas, form, label, input, table, caption, thead, tbody, tr, th} = require('hyperscript-helpers')(h);

import sidebar from './sidebar.jsx';
import navbar from './navigationbar.jsx';

import chart from 'chart.js';

export default function view(state$) {
	return state$.map(({tab, toggle, serialPortInfo, serialPort, powerData}) =>
		div([
			sidebar(toggle, serialPortInfo, serialPort),
			div(`.container-fluid.tab-content.easy-sidebar-container${toggle ? '.toggled' : ''}`, [
				navbar(tab.value, serialPort),
				selectBody(tab, toggle, serialPortInfo, serialPort, powerData)
			])
		])
	);
}

function selectBody(tab, toggle, serialPortInfo, serialPort, powerData) {
	switch(tab.value) {
	case 'list':
		return bodyPortList(toggle, serialPortInfo, serialPort);
	case 'graph':
		return bodyGraph(tab.change , toggle, powerData);
	}
}


function bodyGraph(change , toggle, powerData) {
	return div('.container-fluid#body', [
		label(h1('Graph')),
		button('.btn.easy-sidebar-toggle.right-btn', toggle ? 'Close' : 'Open'),
		graphChart(change, powerData)
	]);
}

function bodyPortList(toggle , serialPortInfo, serialPort) {
	return div('.container-fluid#body', [
		label(h1('Serial Port List')),
		button('.btn.easy-sidebar-toggle.right-btn', toggle ? 'Close' : 'Open'),
		serialPortList(serialPortInfo, serialPort)
	]);
}

var data = {
	labels: ["January", "February", "March", "April", "May", "June", "July"],
	datasets: [
		{
			label: "My First dataset",
			fillColor: "rgba(220,220,220,0.2)",
			strokeColor: "rgba(220,220,220,1)",
			pointColor: "rgba(220,220,220,1)",
			pointStrokeColor: "#fff",
			pointHighlightFill: "#fff",
			pointHighlightStroke: "rgba(220,220,220,1)",
			data: [65, 59, 80, 81, 56, 55, 40]
		},
		{
			label: "My Second dataset",
			fillColor: "rgba(151,187,205,0.2)",
			strokeColor: "rgba(151,187,205,1)",
			pointColor: "rgba(151,187,205,1)",
			pointStrokeColor: "#fff",
			pointHighlightFill: "#fff",
			pointHighlightStroke: "rgba(151,187,205,1)",
			data: [28, 48, 40, 19, 86, 27, 90]
		}
	]
};

function graphChart(change, powerData) {
	var myNewChart = null;
	if (change) {
		var ob = new MutationObserver(() => {
			const ctx = document.getElementById('myChart').getContext('2d');
			if (myNewChart !== null) {
				myNewChart.destroy();
			}
			myNewChart = new chart(ctx).Line(data);
			ob.disconnect();
		})
		ob.observe(document.getElementById('body'), {childList: true, subtree:true});
	}
	var tmp = data.datasets[0].data.shift();
	tmp.value = powerData.voltage;
	data.datasets[0].data.push(tmp);
	if (myNewChart != null)
		myNewChart.update();
	return div(canvas('#myChart', {width:400,height:400}));
}

function serialPortList(serialPortInfo, serialPort) {
	return div(Object.keys(serialPortInfo).map(name => div([
		label(h2(`#${name}.easy-sidebar-toggle`, name)),
		button(`.btn.right-btn.btn-connect${serialPort === name ? '.active' : (serialPort === '' ? '' : '.disabled')}`, {value:name}, serialPort === name ? 'Disconnect' : 'Connect'),
		div('.row', Object.keys(serialPortInfo[name]).map(mc => div('.table-responsive.col-lg-6', [
			label(h3('.easy-sidebar-toggle', serialPortInfo[name][mc].Name)),
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

