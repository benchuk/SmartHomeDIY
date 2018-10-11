/* 
 * server.js
 * 
 * The main file, to be invoked at the command line. Calls app.js to get 
 * the Express app object.
 */

var theApp = require('./app');
var whitelistManager = require('./whitelistManager.js');
var whitelist = whitelistManager.whitelist;
var logger = require('./homeLogger.js');
var app = theApp.init(80);

var locallydb = require('locallydb');
var db = new locallydb('./mydb3');
var remotesCollection = db.collection('remotes');

// remotesCollection.insert([
//     {groupName: "AC living room", commands: [{"commandFile": "001", "commandName": "Power"}]}
// ]);
// try {
//     log("remotes: " + JSON.stringify(remotesCollection.items));
// } catch (e) {
//     log(e);
// }

var locals = {
	title: 'Smart Home DIY',
	description: 'Control your home',
	author: 'bechuknii',
	_layoutFile: true,
	remotes: remotesCollection.items
};

app.post('/newcommand', whitelist, function (req, res) {
	//res.send('Saved: name: ' + req.body.name + ' address: ' + req.body.address + ' command: ' + req.body.command + 'groupName: ' + req.body.group);
	logger.log(
		'Saving user remote command: ' +
		' name: ' +
		req.body.name +
		' address: ' +
		req.body.address +
		' command: ' +
		req.body.command +
		' group: ' +
		req.body.group
	);
	var dbRes = remotesCollection.where({
		groupName: req.body.group
	});
	logger.log('res: ' + JSON.stringify(dbRes));
	if (dbRes && dbRes.items.length > 0) {
		var isUpdate = false;
		for (var c in dbRes.items[0].commands) {
			c = dbRes.items[0].commands[c];
			logger.log('c.address: ' + c.address);
			logger.log('c.commandFile: ' + c.commandFile);
			logger.log('check');
			if (c.address == req.body.address && c.commandFile == req.body.command) {
				isUpdate = true;
				c.commandName = req.body.name;
				logger.log('update existing dataset');
			}
		}
		if (!isUpdate) {
			dbRes.items[0].commands.push({
				commandFile: req.body.command,
				commandName: req.body.name,
				address: req.body.address
			}); //{"commandFile":"001","commandName":"Power"}
		}

		remotesCollection.update(dbRes.cid, dbRes);
		logger.log('update ok');
	} else {
		remotesCollection.insert([{
			groupName: req.body.group,
			commands: [{
				commandFile: req.body.command,
				commandName: req.body.name,
				address: req.body.address
			}]
		}]);
		logger.log('insert ok');
	}

	remotesCollection.save();

	logger.log('remotesCollection: ' + JSON.stringify(remotesCollection));

	var value = req.body.address + ':' + req.body.command;

	theApp.serialPort.write(value, function (err, res) {
		logger.log('serial command sent');
		logger.log('err: ' + err);
		logger.log('res: ' + res);
	});
	res.redirect('/');
	//res.send("click remote");
});

app.get('/deletecmd', whitelist, function (req, res) {
	try {
		var values = req.query.value.split(':');
		logger.log('deletecmd with values:' + values[0]);
		var dbRes = remotesCollection.where({
			groupName: values[0]
		});

		logger.log(dbRes);
		var shouldRemove = false;
		var removeObj = false;
		for (var c in dbRes.items[0].commands) {
			c = dbRes.items[0].commands[c];
			if (c.address == values[1] && c.commandFile == values[2]) {
				shouldRemove = true;
				removeObj = c;
			}
		}

		var index = dbRes.items[0].commands.indexOf(removeObj);
		if (index > -1) {
			dbRes.items[0].commands.splice(index, 1);
		}

		remotesCollection.update(dbRes.cid, dbRes);
		remotesCollection.save();

		//serialPort.write(value, function (err, res) {});
		res.redirect('/');
	} catch (err) {
		log(err);
	}
});

app.get('/dbclear', whitelist, function (req, res) {
	logger.log("dbclear request");
	remotesCollection.items.forEach(function (r) {
		remotesCollection.remove(r.cid);
	});
	remotesCollection.save();
	res.redirect('/');
});

app.get('/', function (req, res) {
	logger.log("home request: " + req.headers['user-agent']);
	locals.date = new Date().toLocaleDateString();
	locals.remotes = remotesCollection.items;
	res.render('home.ejs', locals);
});

/* The 404 Route (ALWAYS Keep this as the last route) */
app.get('/*', function (req, res) {
	logger.log('404');
	res.render('404.ejs', locals);
});