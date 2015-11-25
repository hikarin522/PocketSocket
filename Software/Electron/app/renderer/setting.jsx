'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, FuncSubject} from 'rx-react';
import {Tabs, Tab, Button, Label, Table, PageHeader, Nav, NavItem} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		return {menu: this.props.location.pathname.split('/').pop()};
	},

	getStateStream() {
		this.selectMenu = FuncSubject.create((eventKey, href) => ({eventKey, href}));

		return this.selectMenu.do(i => location.href = i.href).map(i => ({menu: i.eventKey}));
	},

	render() {
		let props = {...this.props};
		delete props.children;
		const children = React.cloneElement(this.props.children, props);

		return (
			<div>
				<Nav bsStyle="tabs" justified activeKey={this.state.menu} onSelect={this.selectMenu}>
					<NavItem eventKey="serialPort" href="#/setting/serialport">Serial Port Setting</NavItem>
					<NavItem eventKey="chart" href="#/setting/chart">Chart Setting</NavItem>
				</Nav>
				{children}
			</div>
		);
	}
});
