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
			count: 30,
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
		const period$ = this.period.map(period => ({period}));

		return Observable.merge(start$, end$, counter$, period$);
	},

	render() {
		const format = new Intl.NumberFormat('ja-JP', {maximumFractionDigits: 2, minimumFractionDigits: 2});
		const isCount = this.state.isCounting;
		const period = this.state.period;
		return (
			<div style={{paddingTop:"20px"}}>
				<h3 style={{textAlign: 'center'}}>
					AVE: {format.format(this.state.avePower)} [W]　　MAX: {format.format(this.state.maxPower)} [W]
				</h3>
				<Row className style={{textAlign: 'center'}}>
					<Col xs={5} xsOffset={3}><h3>
						{isCount ? this.state.count : period} Seconds
						<Button onClick={isCount ? null : this.start} style={{position:'relative', left:'50px'}}>{isCount ? "Stop" : "Start"}</Button>
					</h3></Col>
					<Col xs={4}><h3><Input type="number" onChange={this.period} defaultValue={period} label="period" labelClassName="col-xs-6" wrapperClassName="col-xs-6" /></h3></Col>
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
			const period = this.props.period;

		this.dispose = this.props.start.subscribe(() => {
			let first = true;

			const timer$ = rec$.first()
				.selectMany(() => Observable.interval(1000))
				.map(x => period - x - 1)
				.takeWhile(i => i >= 0).publish().refCount();

			timer$.subscribe(count => this.props.counter({count}));

			const powerCalc = FuncSubject.create();

			rec$.takeUntil(timer$.where(i => i === 0)).subscribe(({W, V, I}) => {
				const w = W.last(), v = V.last(), i = I.last();
				if (first) {
					first = false;
					chart.xAxis[0].update({min: w.x - 1000, max: w.x + period * 1000 + 1000});
					power.setData([w], false);
					voltage.setData([v], false);
					current.setData([i]);
					firstP = w;
				} else {
					power.addPoint(w, false);
					voltage.addPoint(v, false);
					current.addPoint(i);
				}
				powerCalc(w);
			});

			powerCalc.scan(
					(acc, value) => {
						if (!acc.prev)
							return {prev: value, max: value.y, ave: value.y, x: value.x, sum: value.y};
						const sum = acc.sum + (value.x - acc.prev.x) * (value.y + acc.prev.y) / 2;
						const ave = sum / (value.x - acc.x);
						const max = acc.max < value.y ? value.y : acc.max;
						return {prev: value, max, x: acc.x, ave, sum};
					},
					{prev: null, max: 0, ave: 0, x: 0, sum: 0}
				).subscribe(({max, ave}) => {
					this.props.counter({maxPower: max, avePower: ave});
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
