'use strict';
require('./qamster.css');

import Bootstrap from 'bootstrap/dist/css/bootstrap.css';
import { Row, Col, Button, FormControl } from 'react-bootstrap';
import React from 'react';
import ReactDOM from 'react-dom';
import {BootstrapTable, TableHeaderColumn} from 'react-bootstrap-table';

const testobj = [{id: "1", start:"2017-05-18T09:53:39.782589Z",end:"2017-05-18T09:53:44.269315Z",title:"",category:""},
		 {id: "2", start:"2017-05-18T09:53:44.274074Z",end:"2017-05-18T09:53:53.625158Z",title:"",category:""}];

class InputBar extends React.Component {
    render() {
	return (
	    <div>
		<Col md={1}></Col>
		<Col md={4}>
		    <FormControl></FormControl>
		</Col>
		<Col md={1}>
		    <Button bsStyle="success">
			Add Task
		    </Button>
		</Col><Col md={1}></Col>
		<Col md={1}>
    		    <Button bsStyle="danger">
			Stop
		    </Button>
		</Col>
	    </div>
	)}
}

class TopBar extends React.Component {
    render() {
	return (
	    <div>
		<Row>
		    <h1>TopBar</h1>
		</Row>
		<Row>
		    <InputBar />
		</Row>
	    </div>
	)}
}

function myFormatTime(t) {
    var d = new Date(t);
    return d.getHours() + ":" + d.getMinutes();
}

function prepareTime(currentValue, index) {
    return {
	"id": index+1,
	"start": myFormatTime(currentValue.start),
	"end": myFormatTime(currentValue.end),
	"title": currentValue.title,
	"category": currentValue.category,
    }
}

class MainSite extends React.Component{
    constructor(props) {
	super(props);
	this.state = {times: []};
    }

    componentDidMount() {
	var req = new Request("/go/timetable");
	fetch("/go/timetable")
	    .then((responseText) => responseText.json())
	    .then((response) => this.setState({times: response.map(prepareTime)}));
    }

    render() {
	return (
	    <div>
		<Row>
		    <h1> THIS IS NOT SECURE, SECURE APP.YAML
		    </h1>
		</Row>
		<Row>
		    <Col md={1}></Col>
		    <Col md={10}>
			<div>
			    <Row>
				<TopBar />
			    </Row>
			    <Row>
				<BootstrapTable data={this.state.times} striped hover>
				    <TableHeaderColumn isKey dataField='id'>Id</TableHeaderColumn>
				    <TableHeaderColumn dataField='start'>Start</TableHeaderColumn>
				    <TableHeaderColumn dataField='end'>End</TableHeaderColumn>
				    <TableHeaderColumn dataField='title'>Title</TableHeaderColumn>
				    <TableHeaderColumn dataField='category'>Category</TableHeaderColumn>
				</BootstrapTable>
			    </Row>
			</div>
		    </Col>
		</Row>
	    </div>
    )}
}

ReactDOM.render(
    <MainSite />
    ,document.getElementById('root')
);
