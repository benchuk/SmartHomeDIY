var logger = require('./homeLogger');
var fs = require('fs');

function whitelist(req, res, next) {
	logger.log("url: " + req.url);
	var agent = req.headers['user-agent'];
	if (req.query.appid == 'IFTTT' && agent == undefined) {
		logger.log('IFTTT OK -> req: ' + req.url);
		next();
		return;
	}
	var lines = fs.readFileSync('whitelist.db', 'utf-8').split(/\r?\n/);
	for (var i = 0; i < lines.length; i++) {
		var line = lines[i];
		line = line.replace(/(\r\n|\n|\r)/gm, '');
		//logger.log('\r\n -> checking permission: ' + line);
		if (agent == line) {
			logger.log('\r\n -> permission ok! for agent:' + agent);
			next();
			return;
		}
	}

	logger.log('===========================');
	logger.log('\r\n -> NO!!! permission');
	logger.log('===========================');
	var clientIPaddr = null;
	var clientProxy = null;

	if (req.headers['via']) {
		// yes
		clientIPaddr = req.headers['x-forwarded-for'];
		clientProxy = req.headers['via'];
	} else {
		// no
		clientIPaddr = req.connection.remoteAddress;
		clientProxy = 'none';
	}

	logger.log('Rejecting client!');
	logger.log('Client Agent : ' + req.headers['user-agent']);
	logger.log('IP address ' + clientIPaddr + ' via proxy ' + clientProxy);
	logger.log('Url: ' + req.url);
	logger.log('============================================');
	logger.log('--full details--');
	logger.log(JSON.stringify(req.headers));
	logger.log(req.url);
	res.send('404');
}

exports.whitelist = whitelist;