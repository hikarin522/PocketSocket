'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';
import {Jumbotron, PageHeader, Tabs, Tab, Button, ButtonToolbar, Grid, Row, Col, Nav, NavItem, NavDropdown, Navbar, MenuItem, Label, Table} from 'react-bootstrap';
import Highstock from 'react-highcharts/bundle/highstock';
import Immutable from 'immutable';

export default React.createClass({

	componentDidMount() {
		const chart = this.refs.chart.getChart();
		const power = chart.get('power');
		const voltage = chart.get('voltage');
		const current = chart.get('current');
		if (this.props.recData$) {
			let first = true;
			this.dispose = this.props.recData$.subscribe(({V, I, W}) => {
				if (first) {
					first = false;
					power.setData(W.toArray(), false);
					voltage.setData(V.toArray(), false);
					current.setData(I.toArray());
				}
				else {
					power.addPoint(W.last(), false);
					voltage.addPoint(V.last(), false);
					current.addPoint(I.last());
				}
			});
		}
	},

	componentWillUnmount() {
		if (this.dispose)
			this.dispose.dispose();
	},

	render() {
		return <div style={{paddingTop:"20px"}}><Highstock config={this.props.chartConfig.toJS()} ref="chart" /></div>;
	}
});
