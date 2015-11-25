'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, FuncSubject} from 'rx-react';
import {Tabs, Tab, Button, Label, Table} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		return {
			portInfo: {},
			isLoading: false,
			connect: this.props.portSelect.first()
		};
	},

	getStateStream() {
		this.handleClick = FuncSubject.create();
		const infoReq$ = this.handleClick.startWith(null)
		const portInfo$ = infoReq$.selectMany(() => this.props.serial.getPortInfo());

		portInfo$.where(x => this.state.connect && !x[this.state.connect])
			.subscribe(() => this.props.portSelect({target:{value:''}}));

		const connSelect$ = this.props.portSelect.map(connect => ({connect}));

		return Observable.merge(
			infoReq$.map(() => ({isLoading: true})),
			portInfo$.map(portInfo => ({portInfo, isLoading: false})),
			connSelect$
		);
	},

	render() {
		const {isLoading, portInfo} = this.state;
		const settingProps = {
			isLoading,
			portInfo,
			onClick: isLoading ? null : this.handleClick
		};

		const tab = Object.keys(portInfo).map((name, i) =>
			<Tab eventKey={i} title={name} key={name}>
				<Port label={name} info={portInfo[name]} onClick={this.props.portSelect} connect={this.state.connect} />
			</Tab>
		);

		return (
			<Tabs defaultActiveKey={-1} position="left" bsStyle="pills">
				<Tab eventKey={-1} title="Setting"><PortSetting {...settingProps} /></Tab>
				{tab}
			</Tabs>
		);
	}
})

class PortSetting extends React.Component {
	render() {
		const btnText = this.props.isLoading ? 'Loading...' : 'Reload';
		return (
			<form className="form-horizontal" style={{paddingTop: '20px'}}>
				<Button className="col-md-2" bsStyle="primary" disabled={this.props.isLoading} onClick={this.props.onClick}>{btnText}</Button>
				<span className="col-md-10" style={{position: 'relative', left: '50px'}}><h4>Rescan Serial Port</h4></span>
			</form>
		);
	}
}

class MC extends React.Component {
	render() {
		const {info, mc} = this.props;
		const props = Object.keys(info).map((prop, i) =>
			<tr key={prop}>
				<td>{i+1}</td>
				<td>{prop}</td>
				<td>{info[prop]}</td>
			</tr>
		);

		return (
			<div>
				<h3>
					{info.Name}
					<Label style={{position:'relative', left:'50px'}}>{mc}</Label>
				</h3>
				<Table striped bordered condensed hover>
					<thead><tr>
						<th>#</th>
						<th>Property</th>
						<th>Value</th>
					</tr></thead>
					<tbody>{props}</tbody>
				</Table>
			</div>
		);
	}
}

class Port extends React.Component {
	render() {
		const {label, info, connect, onClick} = this.props;
		const mclist = Object.keys(info).map(mc => <MC info={info[mc]} mc={mc} key={mc} />)
		const active = connect === label;
		const btnOpts = {
			active,
			onClick,
			bsSize: 'lg',
			label,
			disabled: active === false && connect !== '',
			value: active ? '' : label,
			style: {
				position: 'relative',
				left: '100px'
			}
		};
		const btnText = active ? 'Disconnect' : 'Connect';

		return (
			<div>
				<h2>
					{label}
					<Button {...btnOpts}>{btnText}</Button>
				</h2>
				{mclist}
			</div>
		);
	}
}
