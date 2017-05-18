var webpack = require('webpack');
var path = require('path');

var BUILD_DIR = path.resolve(__dirname, 'public/');
var APP_DIR = path.resolve(__dirname, 'static/');

var config = {
    entry: APP_DIR + '/index.jsx',
    output: {
	path: BUILD_DIR,
	filename: 'bundle.js'
    },
    module : {
	loaders : [
	    {
		test : /\.jsx/,
		include : APP_DIR,
		loader : 'babel-loader'
	    },
	    {
		test: /\.css$/, loader: 'style-loader!css-loader'
	    },
	    { 
		test: /\.png$/, 
		loader: "url-loader?limit=100000" 
	    },
	    { 
		test: /\.jpg$/, 
		loader: "file-loader" 
	    },
	    {
		test: /\.(woff|woff2)(\?v=\d+\.\d+\.\d+)?$/, 
		loader: 'url-loader?limit=10000&mimetype=application/font-woff'
	    },
	    {
		test: /\.ttf(\?v=\d+\.\d+\.\d+)?$/, 
		loader: 'url-loader?limit=10000&mimetype=application/octet-stream'
	    },
	    {
		test: /\.eot(\?v=\d+\.\d+\.\d+)?$/, 
		loader: 'file-loader'
	    },
	    {
		test: /\.svg(\?v=\d+\.\d+\.\d+)?$/, 
		loader: 'url-loader?limit=10000&mimetype=image/svg+xml'
	    }
	]
    }  
};

module.exports = config;
