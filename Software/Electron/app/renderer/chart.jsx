'use strict';

import {Observable} from 'rx';
import React from 'react';

import {StateStreamMixin, PropsMixin, FuncSubject} from 'rx-react';

import {Jumbotron, PageHeader, Tabs, Tab, Button, ButtonToolbar, Grid, Row, Col, Nav, NavItem, NavDropdown, Navbar, MenuItem, Label, Table} from 'react-bootstrap';
import Chartjs from 'react-chartjs';
const {Line} = Chartjs;

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		let data = {
			labels: [],
			datasets: [{
				label: "My First dataset",
				fillColor: "rgba(220,220,220,0.2)",
				strokeColor: "rgba(220,220,220,1)",
				pointColor: "rgba(220,220,220,1)",
				pointStrokeColor: "#fff",
				pointHighlightFill: "#fff",
				pointHighlightStroke: "rgba(220,220,220,1)",
				data: []
			},{
				label: "My Second dataset",
				fillColor: "rgba(151,187,205,0.2)",
				strokeColor: "rgba(151,187,205,1)",
				pointColor: "rgba(151,187,205,1)",
				pointStrokeColor: "#fff",
				pointHighlightFill: "#fff",
				pointHighlightStroke: "rgba(151,187,205,1)",
				data: []
			}]
		};

		for (var i = 0; i <= 100; i++) {
			data.labels.push(i % 5 === 0 ? i : '');
			data.datasets[0].data.push(0);
			data.datasets[1].data.push(0);
		}


		return {
			data,
			menu: this.props.location.pathname.split('/').pop()
	};
	},

	getStateStream() {
		this.selectMenu = FuncSubject.create((eventKey, href) => ({eventKey, href}));
		const menu$ = this.selectMenu.do(i => location.href = i.href).map(i => ({menu: i.eventKey}));

		const recData$ = this.props.recData.map(({V, I, W}) => {
			this.state.data.datasets[0].data.push(V);
			this.state.data.datasets[0].data.shift();
			this.state.data.datasets[1].data.push(I);
			this.state.data.datasets[1].data.shift();
			return {data: this.state.data};
		});

		return Observable.merge(
			recData$,
			menu$
		);
	},

	render() {
		return (
			<div>
				<Nav bsStyle="tabs" justified activeKey={this.state.menu} onSelect={this.selectMenu}>
					<NavItem eventKey="power" href="#/chart/power">Power</NavItem>
					<NavItem eventKey="voltage" href="#/chart/voltage">Voltage</NavItem>
					<NavItem eventKey="current" href="#/chart/current">Current</NavItem>
				</Nav>
				<Line data={this.state.data} options={chartOptions} width={800} height={600} />
			</div>
		);
	}



});


const chartOptions = {

	responsive : true,
	animation: false,
	animationSteps: 30,

	///Boolean - Whether grid lines are shown across the chart
	scaleShowGridLines : true,

	scaleShowLabels : true,
	scaleLabel : "<%=value%>[W]",

	//String - Colour of the grid lines
	scaleGridLineColor : "rgba(0,0,0,.05)",

	//Number - Width of the grid lines
	scaleGridLineWidth : 1,

	//Boolean - Whether to show horizontal lines (except X axis)
	scaleShowHorizontalLines: true,

	//Boolean - Whether to show vertical lines (except Y axis)
	scaleShowVerticalLines: true,

	//Boolean - Whether the line is curved between points
	bezierCurve : true,

	//Number - Tension of the bezier curve between points
	bezierCurveTension : 0.3,

	//Boolean - Whether to show a dot for each point
	pointDot : true,

	//Number - Radius of each point dot in pixels
	pointDotRadius : 3,

	//Number - Pixel width of point dot stroke
	pointDotStrokeWidth : 1,

	//Number - amount extra to add to the radius to cater for hit detection outside the drawn point
	pointHitDetectionRadius : 20,

	//Boolean - Whether to show a stroke for datasets
	datasetStroke : true,

	//Number - Pixel width of dataset stroke
	datasetStrokeWidth : 2,

	//Boolean - Whether to fill the dataset with a colour
	datasetFill : true,

	//String - A legend template
	legendTemplate : "<ul class=\"<%=name.toLowerCase()%>-legend\"><% for (var i=0; i<datasets.length; i++){%><li><span style=\"background-color:<%=datasets[i].strokeColor%>\"></span><%if(datasets[i].label){%><%=datasets[i].label%><%}%></li><%}%></ul>"

};

