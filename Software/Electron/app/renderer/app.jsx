'use strict';

import React from 'react';
import {StateStreamMixin, FuncSubject} from 'rx-react';
import {Nav, NavItem, Navbar} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin],

	getStateStream() {
		this.handleSelect = FuncSubject.create((eventKey, href) => ({eventKey, href}));
		const navSelector = this.handleSelect.do(i => location.href = i.href)
			.map(i => i.eventKey)
			.startWith(1);

		return navSelector.map(x => ({activeKey: x}));
	},

	render() {
		return (
			<div>
				<Navbar fixedTop inverse>
					<Navbar.Header>
						<Navbar.Brand active>PocketSocket</Navbar.Brand>
						<Navbar.Toggle />
					</Navbar.Header>
					<Navbar.Collapse>
						<Nav activeKey={this.state.activeKey} onSelect={this.handleSelect}>
							<NavItem eventKey={1} href="#" >Home</NavItem>
							<NavItem eventKey={2} href="#/serialPort">SerialPort</NavItem>
							<NavItem eventKey={3} href="#/chart">Chart</NavItem>
						</Nav>
						<Nav pullRight activeKey={this.state.activeKey} onSelect={this.handleSelect}>
							<NavItem eventKey={4} href="#">Home1</NavItem>
							<NavItem eventKey={5} href="#/about">About2</NavItem>
							<NavItem eventKey={6} href="#/chart">Chart3</NavItem>
						</Nav>
					</Navbar.Collapse>
				</Navbar>
				<div style={{paddingTop: '70px'}} className="container">{this.props.children}</div>
			</div>
		);
	}
})
