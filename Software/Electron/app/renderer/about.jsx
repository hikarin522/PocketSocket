'use strict';

import React from 'react';
import {Jumbotron, Button} from 'react-bootstrap';
import {Link} from 'react-router';

export const About = React.createClass({
	render() {
		return <div><h1>About</h1></div>;
	}
});

export const Home = React.createClass({
	render() {
		return (
			<Jumbotron>
				<h1>PocketSocket</h1>
				<Button bsSize="lg" href="#/setting">Select Serial Port</Button>
			</Jumbotron>
		);
	}
});

