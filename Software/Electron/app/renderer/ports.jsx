'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, FuncSubject} from 'rx-react';
import {Tabs, Tab, Button, Label, Table, Glyphicon} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin],

	getInitialState() {
		return {
			portInfo: {},
			isLoading: false
		};
	},

	getStateStream() {
		this.handleClick = FuncSubject.create();
		const infoReq$ = this.handleClick.startWith(null);
		const portInfo$ = infoReq$.selectMany(() => this.props.serial.getPortInfo());

		portInfo$.where(x => this.props.portName && !x[this.props.portName])
			.subscribe(() => this.props.portSelect({target:{value:''}}));

		return Observable.merge(
			infoReq$.map(() => ({isLoading: true})),
			portInfo$.map(portInfo => ({portInfo, isLoading: false}))
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
				<Port label={name} info={portInfo[name]} onClick={this.props.portSelect} portName={this.props.portName} />
			</Tab>
		);

		return (
			<Tabs defaultActiveKey={-1} position="left" bsStyle="pills" style={{paddingTop: '20px'}}>
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
				<Button className="col-md-2" bsStyle="primary" disabled={this.props.isLoading} onClick={this.props.onClick}><Glyphicon glyph="refresh" /> {btnText}</Button>
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
		const {label, info, portName, onClick} = this.props;
		const mclist = Object.keys(info).map(mc => <MC info={info[mc]} mc={mc} key={mc} />)
		const active = portName === label;
		const btnOpts = {
			active, onClick, label,
			bsSize: 'lg',
			disabled: active === false && portName !== '',
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

class Loading extends React.Component {
	render() {
		if (!this.props.active)
			return null;
		return (
			<div className="sk-fading-circle" style={{position:'relative',left:'70px',top:'-125px'}}>
				<div className="sk-circle1 sk-circle"></div>
				<div className="sk-circle2 sk-circle"></div>
				<div className="sk-circle3 sk-circle"></div>
				<div className="sk-circle4 sk-circle"></div>
				<div className="sk-circle5 sk-circle"></div>
				<div className="sk-circle6 sk-circle"></div>
				<div className="sk-circle7 sk-circle"></div>
				<div className="sk-circle8 sk-circle"></div>
				<div className="sk-circle9 sk-circle"></div>
				<div className="sk-circle10 sk-circle"></div>
				<div className="sk-circle11 sk-circle"></div>
				<div className="sk-circle12 sk-circle"></div>
			</div>
		);
	}
}
