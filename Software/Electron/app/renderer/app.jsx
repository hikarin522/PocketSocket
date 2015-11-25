'use strict';

import Rx from 'rx';
import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, FuncSubject} from 'rx-react';
import {Nav, NavItem, Navbar} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		return {
			activeKey: 1,
			portName: ''
		};
	},

	componentWillUnmount() {
		window.removeEventListener('unload', this.dispose);
	},

	dispose() {
		this.serialDisposable.dispose();
	},

	getStateStream() {
		this.handleSelect = FuncSubject.create((eventKey, href) => ({eventKey, href}));
		const navSelector$ = this.handleSelect.do(i => location.href = i.href)
			.map(i => ({activeKey: i.eventKey}));

		this.portSelect = FuncSubject.behavior('', e => e.target.value);

		const tryOpenPort$ = this.portSelect.where(Boolean)
			.selectMany(name => Observable.startAsync(() => this.props.serial.openSerialPort(name)))
			.publish().refCount();

		tryOpenPort$.where(x => !x).subscribe(() => this.portSelect({target:{value:''}}));
		const openPort$ = tryOpenPort$.where(Boolean).publish().refCount();

		this.serialDisposable = new Rx.SerialDisposable();
		window.addEventListener('unload', this.dispose);

		this.portSelect.where(x => !x)
			.subscribe(() => {
				const dispose = this.serialDisposable.getDisposable();
				dispose && dispose.dispose();
			});

		openPort$.map(x => Rx.Disposable.create(x.dispose))
			.subscribe(x => this.serialDisposable.setDisposable(x));

		const read$ = openPort$.selectMany(i => i.read)
			.finally(() => {
				const dispose = this.serialDisposable.getDisposable();
				dispose && dispose.dispose();
			});

		this.recData = Observable.merge(read$, Observable.interval(2000).map(() => null))
			.scan((acc, x) => ({prev: x, value: x || acc.prev ? x : {V:0, I:0, W:0}}), {prev: null, value: null})
			.where(i => i && i.value && !isNaN(i.value.V) && !isNaN(i.value.I) && !isNaN(i.value.W))
			.map(i => i.value)
			.do(({V, I, W}) => console.log(`V: ${V}, I: ${I}, W: ${W}`))
			.publish().refCount();

		this.recData.subscribe(x => console.log(x));

		return Observable.merge(
			navSelector$,
			this.portSelect.map(portName => ({portName}))
		);
	},

	render() {
		let props = {
			...this.props,
			portSelect: this.portSelect,
			recData: this.recData
		};
		delete props.children;
		const children = React.cloneElement(this.props.children, props);

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
							<NavItem eventKey={2} href="#/setting">Setting</NavItem>
							<NavItem eventKey={3} href="#/chart" disabled={!Boolean(this.state.portName)}>Chart</NavItem>
						</Nav>
						<Nav pullRight activeKey={this.state.activeKey} onSelect={this.handleSelect}>
							<NavItem eventKey={4} href="#/about">About</NavItem>
						</Nav>
					</Navbar.Collapse>
				</Navbar>
				<div style={{paddingTop: '70px'}} className="container">{children}</div>
			</div>
		);
	}
})
