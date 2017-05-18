'use strict';
require('./qamster.css');
import Bootstrap from 'bootstrap/dist/css/bootstrap.css';
import { Row, Col } from 'react-bootstrap';
import React from 'react';
import ReactDOM from 'react-dom';
import {BootstrapTable, TableHeaderColumn} from 'react-bootstrap-table';

const testobj = [{id: "1", start:"2017-05-18T09:53:39.782589Z",end:"2017-05-18T09:53:44.269315Z",title:"",category:""},
		 {id: "2", start:"2017-05-18T09:53:44.274074Z",end:"2017-05-18T09:53:53.625158Z",title:"",category:""}];

class TopBar extends React.Component {
    render() {
	return (
	    <h1>TopBar</h1>
	)}
}

class MainSite extends React.Component{
    render() {
	return (
	    <Row>
		<Col md={1}></Col>
		<Col md={10}>
		    <div>
			<Row>
			    <TopBar />
			</Row>
			<Row>
			    <BootstrapTable data={testobj} striped={true} hover={true}>
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
    )}
}

ReactDOM.render(
    <MainSite />
    ,document.getElementById('root')
);
