'use strict';
require('./qamster.css');
import Bootstrap from 'bootstrap/dist/css/bootstrap.css';
import { Button } from 'react-bootstrap';
import React from 'react';
import ReactDOM from 'react-dom';
import {BootstrapTable, TableHeaderColumn} from 'react-bootstrap-table';

const testobj = [{start:"2017-05-18T09:53:39.782589Z",end:"2017-05-18T09:53:44.269315Z",title:"",category:""},
		 {start:"2017-05-18T09:53:44.274074Z",end:"2017-05-18T09:53:53.625158Z",title:"",category:""}];

// class TableRow extends React.Component {
//     render() {
// 	const {data} = this.props;
// 	const row = data.map((data) =>
// 			     <tr>
// 			     <td key={data.start}>{data.start}</td>
// 			     <td key={data.end}>{data.end}</td>
// 			     <td key={data.title}>{data.title}</td>
// 			     <td key={data.category}>{data.category}</td>
// 			     </tr>
// 			    );
// 	return (
// 		<span>{row}</span>
// 	);
//     }
// }

// class Table extends React.Component {
//     constructor(props) {
// 	super(props);
//     }

//     render() {
// 	return (
// 		<table>
// 		<TableRow data={this.props.data} />
// 		</table>
// 	);
//     }
// }

ReactDOM.render(
	<BootstrapTable data={testobj} striped hover>
	<TableHeaderColumn isKey dataField='start'>Start</TableHeaderColumn>
	<TableHeaderColumn dataField='end'>End</TableHeaderColumn>
	<TableHeaderColumn dataField='title'>Title</TableHeaderColumn>
	<TableHeaderColumn dataField='category'>Category</TableHeaderColumn>
	</BootstrapTable>,
    document.getElementById('root')
);
