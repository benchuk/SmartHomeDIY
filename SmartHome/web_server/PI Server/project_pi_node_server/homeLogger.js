var fs = require('fs');
var logger = fs.createWriteStream('logger' + getTime() + '.txt');

function log(msg) {
	console.log(getTime() + ' [debug]: ' + msg);
	logger.write(getTime() + ' [debug]: ' + msg + '\r\n');
}

function getTime() {
	var today = new Date();
	var date =
		today.getFullYear() + '-' + (today.getMonth() + 1) + '-' + today.getDate();
	var time =
		today.getHours() + ':' + today.getMinutes() + ':' + today.getSeconds();
	return date + ' ' + time;
}
exports.log = log;
