'use strict';

import React from 'react';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Nav, NavItem} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin, PropsMixin],

	getInitialState() {
		return {menu: this.props.location.pathname.split('/').pop()};
	},

	getStateStream() {
		return this.propsStream.map(x => ({menu: x.location.pathname.split('/').pop()}));
	},

	render() {
		let props = {...this.props};
		delete props.children;
		const children = React.cloneElement(this.props.children, props);

		return (
			<div>
				<Nav bsStyle="tabs" justified activeKey={this.state.menu}>
					<NavItem eventKey="serialport" href="#/setting/serialport">Serial Port Setting</NavItem>
					<NavItem eventKey="chart" href="#/setting/chart">Chart Setting</NavItem>
				</Nav>
				{children}
			</div>
		);
	}
});
