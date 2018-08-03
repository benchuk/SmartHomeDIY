var express = require('express');
var router = express.Router();
var SerialPort = require("serialport");
var serialPort = new SerialPort("/dev/ttyAMA0", {
  baudRate: 9600
});

// /* GET home page. */
// router.get('/', function(req, res, next) {
//   res.render('index', { title: 'Express' });
// });


var locals = {
  title: 		 'The Open Source SmartHomeDIY',
  description: 'The Open Source SmartHomeDIY',
  author: 	 'Benchuk',
  _layoutFile: true
};


router.get('/', function(req,res){
  res.render('home', locals);
});

router.get('/harq', function (req, res) {
  var value = req.query.value;
  console.log("harq with value:" + value);
  serialPort.write(value, function (err, res) {
  });
  res.send(value + "sent...");
});


module.exports = router;


// serialPort.on("open", function(){
// 	console.log('open');
// 	serialPort.on('data', function(data) {
// 		console.log(data);
// 		serialPort.write(data ,function(err, res)
// 		{
// 		});


// 	});
// 	serialPort.write("Server is running and listenning on serial port\n" ,function(err, res)
// 	{
// 	});
// });
