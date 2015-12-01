'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Jumbotron, PageHeader, Tabs, Tab, Button, ButtonToolbar, Grid, Row, Col, Nav, NavItem, NavDropdown, Navbar, MenuItem, Label, Table} from 'react-bootstrap';

import Immutable from 'immutable';

export default React.createClass({
	mixins: [StateStreamMixin, PropsMixin],

	getInitialState() {
		return {
			menu: this.props.location.pathname.split('/').pop()
		};
	},

	getStateStream() {
		return this.propsStream.map(x => ({menu: x.location.pathname.split('/').pop()}));
	},

	render() {
		let props = {
			...this.props
		};
		delete props.children;
		const children = React.cloneElement(this.props.children, props);

		return (
			<div>
				<Nav bsStyle="tabs" justified activeKey={this.state.menu}>
					<NavItem eventKey="realtime" href="#/chart/realtime">All Data</NavItem>
					<NavItem eventKey="timer" href="#/chart/timer">Timer</NavItem>
					<NavItem eventKey="setting" href="#/chart/setting">Setting</NavItem>
				</Nav>
				{children}
			</div>
		);
	}
});
