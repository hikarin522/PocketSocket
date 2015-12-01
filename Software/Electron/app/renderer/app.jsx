'use strict';

import {Observable, Disposable, SerialDisposable} from 'rx';
import React from 'react';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Nav, NavItem, Navbar} from 'react-bootstrap';
import Immutable from 'immutable';
import defaultConfig from '../config.json';

export default React.createClass({
	mixins: [StateStreamMixin, PropsMixin],

	getInitialState() {
		return {
			activeKey: this.props.location.pathname.split('/')[1],
			chartConfig: Immutable.fromJS(defaultConfig),
			portName: '',
			serialPort: null,
			recData$: null,
			dispose: null
		};
	},

	getStateStream() {
		const activeKey$ = this.propsStream.map(x => ({activeKey: x.location.pathname.split('/')[1]}));

		this.setChartConfig = FuncSubject.create();
		const chartConfig$ = this.setChartConfig.map(chartConfig => ({chartConfig}));

		this.portSelect = FuncSubject.create(e => e.target.value);
		const portName$ = this.portSelect.map(portName => ({portName}));

		const tryOpen$ = this.portSelect.where(Boolean)
			.selectMany(name => this.props.serial.openSerialPort(name))
			.publish().refCount();

		const serialPort$ = tryOpen$.where(Boolean)
			.map(serialPort => ({serialPort}))
			.publish().refCount();

		const close$ = Observable.merge(this.portSelect, tryOpen$).where(x => !x)
			.map(() => {
				this.dispose();
				return {serialPort: null, portName: ''};
			}).publish().refCount();

		const read$ = serialPort$.map(({serialPort}) => {
			const read = serialPort.read.where(Boolean).startWith({V:0,I:0,W:0}).publish().refCount();
			//const read = Observable.interval(2000).map(() => ({V:Math.random()*800,I:Math.random()*3,W:Math.random()*500})).publish().refCount();
			const timeout = read.debounce(1500).selectMany(() => Observable.interval(1000)
					.takeUntil(read).map(() => ({V:0,I:0,W:0}))
				);
			const recData$ = Observable.merge(read, timeout).timestamp()
				.scan(({V, I, W}, {value, timestamp}) => ({
					V: V.push({x: timestamp, y: value.V}),
					I: I.push({x: timestamp, y: value.I}),
					W: W.push({x: timestamp, y: value.W}),
				}), {V: Immutable.List(), I: Immutable.List(), W:Immutable.List()})
				.publish();
			const dispose = recData$.connect();
			return {recData$, dispose};
		});

		return Observable.merge(
			chartConfig$, activeKey$, portName$, serialPort$, close$, read$
		);
	},

	dispose() {
		this.state.dispose && this.state.dispose.dispose();

		this.state.serialPort && this.state.serialPort.dispose();
	},

	componentWillMount() {
		window.addEventListener('unload', this.dispose);
	},

	componentWillUnmount() {
		window.removeEventListener('unload', this.dispose);
	},

	render() {
		let props = {
			...this.props,
			...this.state,
			portSelect: this.portSelect,
			setChartConfig: this.setChartConfig,
		};
		delete props.dispose;
		delete props.children;
		delete props.activeKey;
		const children = React.cloneElement(this.props.children, props);

		return (
			<div>
				<Navbar fixedTop>
					<Navbar.Header>
						<Navbar.Brand active>PocketSocket</Navbar.Brand>
						<Navbar.Toggle />
					</Navbar.Header>
					<Navbar.Collapse>
						<Nav activeKey={this.state.activeKey}>
							<NavItem eventKey="" href="#" >Home</NavItem>
							<NavItem eventKey="setting" href="#/setting">Setting</NavItem>
							<NavItem eventKey="chart" href={Boolean(this.state.portName) ? "#/chart" : "#"} disabled={!Boolean(this.state.portName)}>Chart</NavItem>
						</Nav>
						<Nav pullRight activeKey={this.state.activeKey}>
							<NavItem eventKey="about" href="#/about">About</NavItem>
						</Nav>
					</Navbar.Collapse>
				</Navbar>
				<div style={{paddingTop: '70px'}} className="container">{children}</div>
			</div>
		);
	}
})
