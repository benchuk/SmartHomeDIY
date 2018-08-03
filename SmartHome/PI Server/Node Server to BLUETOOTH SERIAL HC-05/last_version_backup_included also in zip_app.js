var SerialPort = require("serialport").SerialPort
var serialPort = new SerialPort("/dev/ttyAMA0", {
 baudrate: 9600
});

serialPort.on("open", function(){
console.log('open');
serialPort.on('data', function(data) {
console.log(data);
    //serialPort.write("ls\n" ,function(err, res)
	//{
	//});
serialPort.write(data ,function(err, res)
{
});


});
serialPort.write("ls\n",function(err, results) {
   console.log('err ' + err);
   console.log('results ' + results);
  });
});

