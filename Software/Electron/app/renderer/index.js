'use strict';

import {Observable} from 'rx';
import React from 'react';
import {render} from 'react-dom';
import {Router, Route, IndexRoute, Redirect, IndexRedirect} from 'react-router';
import createHistory from 'history/lib/createHashHistory';

import path from 'path';
import remote from 'remote';

import serialInit from './EdgeSerialPort';
import App from './app.jsx';
import Setting from './setting.jsx';
import Ports from './ports.jsx';
import ChartSetting from './chartSetting.jsx';
import Chart from './chart.jsx';
import {About, Home} from './about.jsx';
import RealTime from './realTimeChart.jsx';
import Timer from './timerChart.jsx';

async () => {
	await Observable.fromEvent(document, 'DOMContentLoaded').first().toPromise();
	serial = await serialInit();

	console.log(global.Highcharts);
	global.Highcharts.setOptions({
		global: {
			useUTC: false
		}
	});

	const Menu = wrapClass(App, {serial});

	render(
		<Router history={createHistory({queryKey: false})}>
			<Route path="/" component={Menu}>
				<IndexRoute component={Home} />
				<Route path="setting" component={Setting}>
					<IndexRedirect to="serialport" />
					<Route path="serialport" component={Ports} />
					<Route path="chart" component={ChartSetting} />
				</Route>
				<Route path="chart" component={Chart}>
					<IndexRedirect to="realtime" />
					<Route path="realtime" component={RealTime} />
					<Route path="timer" component={Timer} />
					<Route path="setting" component={ChartSetting} />
				</Route>
				<Route path="about" component={About} />
			</Route>
		</Router>,
		document.getElementById('react')
	);
}();

function wrapClass(Component, extProps) {
	return React.createClass({
		render() {
			return <Component {...this.props} {...extProps} />
		}
	});
}
