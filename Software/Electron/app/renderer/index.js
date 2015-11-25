'use strict';

import {Observable} from 'rx';
import React from 'react';
import {render} from 'react-dom';
//import {ClassSet, LinkedStateMixin, PureRenderMixin} from 'react-addons';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Router, Route, IndexRoute, Redirect, IndexRedirect, Link, IndexLink, Lifecycle, RouteContext} from 'react-router';
import createHistory from 'history/lib/createHashHistory';
import {Jumbotron, PageHeader, Tabs, Tab, Button, ButtonToolbar, Grid, Row, Col, Nav, NavItem, NavDropdown, Navbar, MenuItem, Label, Table} from 'react-bootstrap';
import {Line} from 'react-chartjs';


import path from 'path';
import remote from 'remote';

import serialInit from './EdgeSerialPort';
import App from './app.jsx';
import Setting from './setting.jsx';
import Ports from './ports.jsx';
import ChartSetting from './chartSetting.jsx';
import Chart from './chart.jsx';

async () => {
	await Observable.fromEvent(document, 'DOMContentLoaded').first().toPromise();
	serial = await serialInit();

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
					<IndexRedirect to="power" />
					<Route path="power" component={About} />
					<Route path="voltage" component={About} />
					<Route path="current" component={About} />
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

const About = React.createClass({
	render() {
		return <div><h1>About</h1><Button bsStyle="primary">Primary</Button></div>;
	}
});

const Home = React.createClass({
	render() {
		return (
			<Jumbotron>
				<h1>PocketSocket</h1>
			</Jumbotron>
		);
	}
});



