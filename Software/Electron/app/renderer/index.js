'use strict';

import {Observable} from 'rx';
import React from 'react';
import {render} from 'react-dom';
import {ClassSet, LinkedStateMixin, PureRenderMixin} from 'react-addons';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Router, Route, IndexRoute, Redirect, IndexRedirect, Link, IndexLink, Lifecycle, RouteContext} from 'react-router';
import createHistory from 'history/lib/createHashHistory';
import {Jumbotron, PageHeader, Tabs, Tab, Button, ButtonToolbar, Grid, Row, Col, Nav, NavItem, NavDropdown, Navbar, MenuItem, Label, Table} from 'react-bootstrap';
import {Line} from 'react-chartjs';

import path from 'path';
import remote from 'remote';

import serialInit from './EdgeSerialPort';
import App from './app.jsx';
import Ports from './ports.jsx';
import Chart from './chart.jsx';

let serial;
async () => {
	serial = await serialInit();
	await Observable.fromEvent(document, 'DOMContentLoaded').first().toPromise();

	const wrapPorts = React.createClass({
		render() {
			return <Ports {...this.props} serial={serial} />;
		}
	});

	const wrapChart = React.createClass({
		render() {
			return <Chart {...this.props} serial={serial} />;
		}
	});

	render(
		<Router history={createHistory({queryKey: false})}>
			<Route path="/" component={App}>
				<IndexRoute component={Home} />
				<Route path="about" component={About} />
				<Route path="serialPort" component={wrapPorts} />
				<Route path="chart" component={wrapChart} />
			</Route>
		</Router>,
		document.getElementById('react')
	);
}();



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



