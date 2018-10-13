var logger = require('./homeLogger');
var fs = require('fs');
function whitelist(request, res, next) {
	var agent = request.headers['user-agent'];
	if (request.query.appid == 'IFTTT' && agent == undefined) {
		log('IFTTT OK -> req: ' + request.url);
		next();
		return;
	}
	var lines = fs.readFileSync('whitelist.db', 'utf-8').split(/\r?\n/);
	for (var i = 0; i < lines.length; i++) {
		var line = lines[i];
		line = line.replace(/(\r\n|\n|\r)/gm, '');
		console.log('\r\n -> checking permission: ' + line);
		if (agent == line) {
			console.log('\r\n -> permission ok!');
			next();
			return;
		}
	}

	var clientIPaddr = null;
	var clientProxy = null;

	console.log(request.headers);
	console.log(request.url);

	if (request.headers['via']) {
		// yes
		clientIPaddr = request.headers['x-forwarded-for'];
		clientProxy = request.headers['via'];
	} else {
		// no
		clientIPaddr = request.connection.remoteAddress;
		clientProxy = 'none';
	}

	logger.log('rejecting client');
	logger.log('Client : ' + request.headers['user-agent']);
	logger.log('IP address ' + clientIPaddr + ' via proxy ' + clientProxy);
	res.send('404');
}

exports.whitelist = whitelist;
