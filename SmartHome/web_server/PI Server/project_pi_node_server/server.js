/* 
 * server.js
 * 
 * The main file, to be invoked at the command line. Calls app.js to get 
 * the Express app object.
 */

var theApp = require('./app');
var app = theApp.init(80);


var locallydb = require('locallydb');
var db = new locallydb('./mydb3');
var remotesCollection = db.collection('remotes');

// remotesCollection.insert([
//     {groupName: "AC living room", commands: [{"commandFile": "001", "commandName": "Power"}]}
// ]);
try {
    console.log("remotes: " + JSON.stringify(remotesCollection.items));
} catch (e) {
    console.log(e);
}

var locals = {
    title: 'Node | Express | EJS | Boostrap',
    description: 'A Node.js applicaton bootstrap using Express 3.x, EJS, Twitter Bootstrap 2, and CSS3',
    author: 'Jalal Hejazi',
    _layoutFile: true,
    remotes: remotesCollection.items
};

app.post('/newcommand', function (req, res) {
            //res.send('Saved: name: ' + req.body.name + ' address: ' + req.body.address + ' command: ' + req.body.command + 'groupName: ' + req.body.group);
            console.log("Saving user remote command: " + ' name: ' + req.body.name + ' address: ' + req.body.address + ' command: ' + req.body.command + ' group: ' + req.body.group);
            var dbRes = remotesCollection.where({
                groupName: req.body.group
            });
            console.log("res: " + JSON.stringify(dbRes));
            if (dbRes && dbRes.items.length > 0) {
                var isUpdate = false;
                for (var c in dbRes.items[0].commands) {
                    c = dbRes.items[0].commands[c];
                    console.log("c.address: " + c.address);
                    console.log("c.commandFile: " + c.commandFile);
                    console.log("check");
                    if (c.address == req.body.address && c.commandFile == req.body.command) {
                            isUpdate = true;
                            c.commandName = req.body.name;
                            console.log("update existing dataset");
                        }
                    }
                    if (!isUpdate) {
                        dbRes.items[0].commands.push({
                            "commandFile": req.body.command,
                            "commandName": req.body.name,
                            "address": req.body.address
                        }) //{"commandFile":"001","commandName":"Power"}

                    }

                    remotesCollection.update(dbRes.cid, dbRes);
                    console.log("update ok");
                } else {
                    remotesCollection.insert([{
                        groupName: req.body.group,
                        commands: [{
                            "commandFile": req.body.command,
                            "commandName": req.body.name,
                            "address": req.body.address
                        }]
                    }]);
                    console.log("insert ok");
                }

                remotesCollection.save();

                console.log("remotesCollection: " + JSON.stringify(remotesCollection));

                var value = req.body.address + ":" + req.body.command;

                theApp.serialPort.write(value, function (err, res) {
                    console.log("serial command sent");
                    console.log("err: " + err);
                    console.log("res: " + res);
                });
                res.redirect('/');
                //res.send("click remote");
            });


        app.get('/dbclear', function (req, res) {
            remotesCollection.items.forEach(function (r) {
                remotesCollection.remove(r.cid);
            });
            remotesCollection.save();
            res.send("db cleared");
        });


        app.get('/', function (req, res) {

            locals.date = new Date().toLocaleDateString();
            locals.remotes = remotesCollection.items;
            res.render('home.ejs', locals);
        });

        /* The 404 Route (ALWAYS Keep this as the last route) */
        app.get('/*', function (req, res) {
            res.render('404.ejs', locals);
        });