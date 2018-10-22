/*
 * app.js
 *
 * Our base app code, including Express configs
 */

var express = require('express'),
  engine = require('ejs-locals'),
  app = express();

var SerialPort = require('serialport').SerialPort;
var serialPort = new SerialPort('/dev/ttyAMA0', {
  baudrate: 9600
});

var locallydb = require('locallydb');

var whitelistManager = require('./whitelistManager.js');
var whitelist = whitelistManager.whitelist;
var logger = require('./homeLogger.js');



exports.init = function (port) {
  app.configure(function () {
    app.set('views', __dirname + '/views');
    app.set('view engine', 'ejs');
    app.use(express.bodyParser());
    app.use(express.methodOverride());
    app.use(express.static(__dirname + '/static'));
    app.use(app.router);
    app.enable('jsonp callback');
  });

  app.engine('ejs', engine);

  app.configure('development', function () {
    app.use(
      express.errorHandler({
        dumpExceptions: true,
        showStack: true
      })
    );
    // app.use(express.logger({ format: ':method :url' }));
  });

  app.configure('production', function () {
    app.use(express.errorHandler());
  });

  app.use(function (err, req, res, next) {
    res.render('500.ejs', {
      locals: {
        error: err
      },
      status: 500
    });
  });

  // var router = app.Router();
  app.get('/acon', whitelist, function (req, res) {
    logger.log('ac on request');
    serialPort.write('1\n', function (err, res) {});
    res.send('acon');
  });

  app.get('/test', whitelist, function (req, res) {
    logger.log('ac on request');
    serialPort.write('003:005', function (err, res) {});
    res.send('acon');
  });

  // var router = app.Router();
  app.get('/acoff', whitelist, function (req, res) {
    logger.log('ac off request');
    serialPort.write('2\n', function (err, res) {});
    res.send('acoff');
  });

  // Currently using only this one. smart home api
  app.get('/harq', whitelist, function (req, res) {
    try {
      var value = req.query.value;
      logger.log('harq with value:' + value);
      serialPort.write(value, function (err, res) {});
      res.send(value + 'sent...');
    } catch (err) {
      (err);
    }
  });

  app.post('/api/test', whitelist, function (req, res) {
    logger.log("post test request");
    res.send('post test ok');
  });

  app.get('/all-lights-on', whitelist, function (req, res) {
    try {
      logger.log("all lights on");
      handleTimeoutRequest('007:011', 100);
      handleTimeoutRequest('007:005', 1200);
      handleTimeoutRequest('007:011', 2300);
      handleTimeoutRequest('007:005', 3400);
      // TBD - add more lighs here
      res.send('all-lights-on - sent...');
    } catch (err) {
      (err);
    }
  });

  app.get('/all-lights-off', whitelist, function (req, res) {
    try {
      logger.log("all lights off");
      handleTimeoutRequest('007:012', 100);
      handleTimeoutRequest('007:006', 1200);
      handleTimeoutRequest('007:012', 2300);
      handleTimeoutRequest('007:006', 3400);
      handleTimeoutRequest('005:005', 102);
      // TBD - add more lighs here
      res.send('all-lighst-off - sent...');
    } catch (err) {
      (err);
    }
  });

  function handleTimeoutRequest(value, time) {
    var handle = setTimeout(function () {
      try {
        ('Performing now! -> req for time: ' + time + ' with value: ' + value);
        serialPort.write(value, function (err, res) {});
      } catch (err) {
        (err);
      }
    }, time);

    return handle;
  }

  var schedualer = {};
  var requestIdCounter = 0;
  app.get('/req-with-at-time', function (req, res) {
    var value = req.query.value;
    var time = req.query.time * 1000 * 60;
    logger.log('req for time: ' + time + ' with value:' + value);
    var handle = handleTimeoutRequest(value, time);

    schedualer[requestIdCounter] = {
      handle: handle,
      timestamp: new Date().getTime()
    };
    // res.send(schedualer[value]);
    res.send(requestIdCounter);
    requestIdCounter++;
  });

  app.get('/cancel-req-with-at-time', whitelist, function (req, res) {
    var reqId = req.query.reqId;
    //var time = req.query.time * 1000 * 60;

    logger.log("cancel req with time: id: " + reqId);
    var result = '';
    try {
      result = clearTimeout(schedualer[reqId].handle);
    } catch (err) {
      result = err;
    }

    res.send(result);
  });

  server = app.listen(3001);

  logger.log('Listening on port ' + 3001 + ' in ' + app.settings.env + ' mode');

  return app;
};

serialPort.on('open', function () {
  //logger.log('open');
  serialPort.on('data', function (data) {
    //logger.log(data);
    serialPort.write(data, function (err, res) {});
  });
  serialPort.write('Server is running and listening on serial port\n', function (err, res) {});
});

exports.serialPort = serialPort;