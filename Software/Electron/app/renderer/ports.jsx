'use strict';

import {Observable} from 'rx';
import React from 'react';
import {StateStreamMixin, FuncSubject} from 'rx-react';
import {Tabs, Tab, Button, Label, Table} from 'react-bootstrap';

export default React.createClass({
	mixins: [StateStreamMixin],

	getStateStream() {
		this.handleClick = FuncSubject.create();
		console.log(this.props);

		const infoReq$ = this.handleClick.startWith(null);

		const portInfo$ = infoReq$.selectMany(() => this.props.serial.getPortInfo())
			.publish().refCount();

		const loading$ = infoReq$.map(() => true)
			.merge(portInfo$.map(() => false));

		return Observable.combineLatest(
			portInfo$,
			loading$,
			(portInfo, isLoading) => ({portInfo, isLoading})
		).startWith({portInfo: {}, isLoading: false});
	},

	render() {
		const {isLoading, portInfo} = this.state;
		const onClick = isLoading ? null : this.handleClick;
		const btnText = isLoading ? 'Loading...' : 'Reload';

		const tab = Object.keys(portInfo).map((name, i) =>
			<Tab eventKey={i + 2} title={name}>
				<Port name={name} info={portInfo[name]} />
			</Tab>
		);

		return (
			<Tabs defaultActiveKey={1} position="left" bsStyle="pills">
				<Tab eventKey={1} title="Setting">
					<Button bsSize="lg" bsStyle="primary" disabled={isLoading} onClick={onClick}>{btnText}</Button>
				</Tab>
				{tab}
			</Tabs>
		);
	}
})

class Port extends React.Component {
	render() {
		const {name, info} = this.props;
		const mclist = Object.keys(info).map((mc, i) => {
			const props = Object.keys(info[mc]).map((prop, j) =>
				<tr>
					<td>{j + 1}</td>
					<td>{prop}</td>
					<td>{info[mc][prop]}</td>
				</tr>
			);

			return (
				<div>
					<h3>
						{info[mc]['Name']}
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
		});

		return (
			<div>
				<h2>
					{name}
					<Button bsSize='lg' label={name} style={{position:'relative', left:'100px'}}>Connect</Button>
				</h2>
				{mclist}
			</div>
		);
	}
}
