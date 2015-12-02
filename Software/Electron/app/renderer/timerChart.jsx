'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Tabs, Tab, Button, Nav, NavItem, Navbar, Input, Col, Row} from 'react-bootstrap';
import Highcharts from 'react-highcharts';
import Immutable from 'immutable';

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		return {
			isCounting: false,
			period: 30,
			count: 0,
			maxPower: 0,
			avePower: 0
		};
	},

	getStateStream() {
		this.start = FuncSubject.create();
		const start$ = this.start.map(() => ({isCounting: true}));
		this.counter = FuncSubject.create();
		const counter$ = this.counter;
		const end$ = this.counter.where(({count}) => count === 0)
			.map(() => ({isCounting: false}));

		this.period = FuncSubject.create(x => x.target.value);
		const period$ = this.period.where(x => x > 0 && x <= 3600).map(period => ({period}));

		return Observable.merge(start$, end$, counter$, period$);
	},

	render() {
		const format = new Intl.NumberFormat('ja-JP', {maximumFractionDigits: 2, minimumFractionDigits: 2});
		const count = new Intl.NumberFormat('ja-JP', {maximumFractionDigits: 1, minimumFractionDigits: 1}).format(this.state.count / 10);
		const isCount = this.state.isCounting;
		const period = this.state.period;
		return (
			<div style={{paddingTop:"20px"}}>
				<h3 style={{textAlign: 'center'}}>
					AVE: {format.format(this.state.avePower)} [W]　　MAX: {format.format(this.state.maxPower)} [W]
				</h3>
				<Row className style={{textAlign: 'center'}}>
					<Col xs={5} xsOffset={3}><h3>
						{count} Seconds
						<Button onClick={isCount ? null : this.start} style={{position:'relative', left:'50px'}}>{isCount ? "Stop" : "Start"}</Button>
					</h3></Col>
					<Col xs={4}><h3><Input type="number" onChange={this.period} defaultValue={period} min={1} max={3600} required label="period" labelClassName="col-xs-6" wrapperClassName="col-xs-6" /></h3></Col>
				</Row>
				<Chart config={this.props.chartConfig} recData$={this.props.recData$} start={this.start} period={period} counter={this.counter} />
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
			const counter = this.props.counter;

		this.dispose = this.props.start.subscribe(() => {
			const period = this.props.period;
			let first = true;

			rec$.first().selectMany(() => Observable.interval(100).startWith(-1))
				.map(x => ({count: period * 10 - x - 1}))
				.takeWhile(({count}) => count >= 0).subscribe(counter);

			rec$.scan(({prev, max, sum, x0}, {V, I, W}) => {
					const w = W.last(), v = V.last(), i = I.last();
					if (!prev) {
						chart.xAxis[0].update({min: w.x - 1000, max: w.x + period * 1000 + 1000});
						power.setData([w], false);
						voltage.setData([v], false);
						current.setData([i]);
						return {prev: w, max: w.y, sum: w.y, ave: w.y, x0: w.x};
					} else {
						power.addPoint(w, false);
						voltage.addPoint(v, false);
						current.addPoint(i);
						max = max < w.y ? w.y : max;
						sum += (w.x - prev.x) * (w.y + prev.y) / 2;
						const ave = sum / (w.x - x0);
						return {prev: w, max, sum, ave, x0};
					}
				}, {prev: null}
				).takeWhile(({prev, x0}) => (x0 + (period * 1000) >= prev.x))
				.subscribe(({max, ave}) => {
					counter({maxPower: max, avePower: ave});
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
