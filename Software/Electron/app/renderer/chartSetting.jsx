'use strict';

import {Observable} from 'rx';
import React from 'react';
import {FuncSubject} from 'rx-react';
import {Button, DropdownButton, ButtonInput, MenuItem, Glyphicon, Input, FormControls, Label} from 'react-bootstrap';
import Immutable from 'immutable';
import remote from 'remote';
const fs = remote.require('fs');

const filename = "config.json";

export default React.createClass({

	componentWillMount() {
		this.submit = FuncSubject.create();
		this.dispose = this.submit.subscribe(x => {
			x.preventDefault();
			const t = x.target;
			const nextConfig = Immutable.fromJS({
				chart: {
					type: t.chartType.value,
					backgroundColor: t.backgroundColor.value,
					plotBackgroundColor: t.plotBackgroundColor.value,
					borderColor: t.borderColor.value,
					plotBorderColor: t.plotBorderColor.value
				}
			});
			const config = this.props.chartConfig.mergeDeep(nextConfig);
			this.props.setChartConfig(config);
			fs.writeFile(filename, JSON.stringify(config.toJS(), null, '\t'));
		});
	},

	componentWillUnmount() {
		this.dispose.dispose();
	},

	render() {
		const chart = this.props.chartConfig.get('chart');
		const chartColor = name => <Input type="color" label={name} name={name} defaultValue={chart.get(name)} labelClassName="col-xs-3" wrapperClassName="col-xs-2" />;
		return (
			<form className="form-horizontal" onSubmit={this.submit}>
				<h3 className="col-xs-9 col-xs-offset-3">Chart</h3>
				<Input type="select" label="type" name="chartType" defaultValue={chart.get('type')} labelClassName="col-xs-3" wrapperClassName="col-xs-3">
					<option value="line">line</option>
					<option value="spline">spline</option>
					<option value="area">area</option>
					<option value="areaspline">areaspline</option>
				</Input>
				{chartColor('backgroundColor')}
				{chartColor('plotBackgroundColor')}
				{chartColor('borderColor')}
				{chartColor('plotBorderColor')}
				<ButtonInput type="reset" value="Reset" />
				<ButtonInput type="submit" value="Submit" />
			</form>
		);
	}
});
