/* 
 * app.js
 * 
 * Our base app code, including Express configs
 */

var express = require('express')
    , engine = require('ejs-locals')
    , app = express();

var SerialPort = require("serialport").SerialPort
var serialPort = new SerialPort("/dev/ttyAMA0", {
    baudrate: 9600
});

var locallydb = require('locallydb');


exports.init = function (port) {

    app.configure(function () {
        app.set('views', __dirname + '/views');
        app.set('view engine', 'ejs');
        app.use(express.bodyParser());
        app.use(express.methodOverride());
        app.use(express.static(__dirname + '/static'));
        app.use(app.router);
        app.enable("jsonp callback");
    });

    app.engine('ejs', engine);

    app.configure('development', function () {
        app.use(express.errorHandler({ dumpExceptions: true, showStack: true }));
        // app.use(express.logger({ format: ':method :url' }));
    });

    app.configure('production', function () {
        app.use(express.errorHandler());
    });


    app.use(function (err, req, res, next) {
        res.render('500.ejs', { locals: { error: err }, status: 500 });
    });

    //var router = app.Router();
    app.get('/acon', function (req, res) {
        console.log("ac on request");
        serialPort.write("1\n", function (err, res) {
        });
        res.send('acon');
    });

    app.get('/test', function (req, res) {
        console.log("ac on request");
        serialPort.write("003:005", function (err, res) {
        });
        res.send('acon');
    });

    //var router = app.Router();
    app.get('/acoff', function (req, res) {
        console.log("ac off request");
        serialPort.write("2\n", function (err, res) {
        });
        res.send('acoff');
    });

    //Currently using only this one. smart home api
    app.get('/harq', function (req, res) {
        try
        {
        var value = req.query.value;
        console.log("harq with value:" + value);
        serialPort.write(value, function (err, res) {
        });
        res.send(value + "sent...");
        }
        catch(err)
        {
            console.log(err);
        }
    });


    function handleTimeoutRequest(value, time)
    {
        var handle = setTimeout(function() {
            try
            {
                console.log("Performing now! -> req for time: " + time + " with value: " + value);
                serialPort.write(value, function (err, res) {
                });
           
            }
            catch(err)
            {
                console.log(err);
            }
        }, time);

        return handle;
    }

    var schedualer = {}
    var requestIdCounter = 0;
    app.get('/req-with-at-time', function (req, res) {
      
        var value = req.query.value;
        var time = req.query.time*1000*60;
        console.log("req for time: " + time + " with value:" + value);
        var handle = handleTimeoutRequest(value,time)
       
        schedualer[requestIdCounter] = {handle: handle, timestamp: new Date().getTime()};
        //res.send(schedualer[value]);
        res.send(requestIdCounter);
        requestIdCounter++;
    });

    app.get('/cancel-req-with-at-time', function (req, res) {
      
        var reqId = req.query.reqId;
        var time = req.query.time*1000*60;
        
        var result = "";
        try
        {
            result = clearTimeout(schedualer[reqId].handle);
        }
        catch(err)
        {
            result = err;
        }

        res.send(result);
       
    });

    server = app.listen(3001);

    console.log("Listening on port %d in %s mode", 3001, app.settings.env);

    return app;
}

serialPort.on("open", function () {
    console.log('open');
    serialPort.on('data', function (data) {
        console.log(data);
        serialPort.write(data, function (err, res) {
        });


    });
    serialPort.write("Server is running and listenning on serial port\n", function (err, res) {
    });
});


exports.serialPort = serialPort;