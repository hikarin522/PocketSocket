'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Jumbotron, PageHeader, Tabs, Tab, Button, ButtonToolbar, Grid, Row, Col, Nav, NavItem, NavDropdown, Navbar, MenuItem, Label, Table} from 'react-bootstrap';
import Highcharts from 'react-highcharts';
import Immutable from 'immutable';

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		return {
			isCounting: false,
			period: 30,
			count: 30
		};
	},

	getStateStream() {
		this.start = FuncSubject.create();
		const start$ = this.start.map(() => ({isCounting: true}));
		this.counter = FuncSubject.create();
		const counter$ = this.counter.map(count => ({count}));
		const end$ = this.counter.where(x => x <= 0)
			.map(() => ({isCounting: false}));

		return Observable.merge(start$, end$, counter$);
	},

	render() {
		return (
			<div style={{paddingTop:"20px"}}>
				<Button bsSize="lg" onClick={this.state.isCounting ? null : this.start}>{this.state.isCounting ? "Stop" : "Start"}</Button>
				<h3>{this.state.count}</h3>
				<Chart config={this.props.chartConfig} recData$={this.props.recData$} start={this.start} period={this.state.period} counter={this.counter} />
			</div>
		);
	}
});

const Chart = React.createClass({

	componentDidMount() {
		const chart = this.refs.chart.getChart();
			const power = chart.get('power');
			const voltage = chart.get('voltage');
			const current = chart.get('current');
			const rec$ = this.props.recData$;
			const period = this.props.period;

		this.dispose = this.props.start.subscribe(() => {

			const timer$ = rec$.take(1).do(({W}) => {
					power.setData([], false);
					voltage.setData([], false);
					current.setData([], false);
					const start = W.last().x - 5000;
					chart.xAxis[0].update({min: start, max: start + period * 1000 + 5000});
				})
				.selectMany(() => Observable.interval(1000))
				.takeWhile(i => i <= period).publish().refCount();
			timer$.subscribe(x => this.props.counter(period - x));
			rec$.takeUntil(timer$.last()).subscribe(({W, V, I}) => {
				power.addPoint(W.last(), false);
				voltage.addPoint(V.last(), false);
				current.addPoint(I.last());
			});
		});

	},

	shouldComponentUpdate() {
		return false;
	},

	componentWillUnmount() {
		this.dispose && this.dispose.dispose();
	},

	render() {
		const config = this.props.config.toJS();
		config.rangeSelector.enabled = false;
		config.title.text = "";
		config.xAxis = {
			type: "datetime"
		};

		return <Highcharts config={config} ref="chart" />;
	}
});
