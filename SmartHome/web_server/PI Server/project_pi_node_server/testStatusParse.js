var currentBuffer = Buffer.alloc(0);
var currentCommand = Buffer.alloc(0);
function hasOpeningAndClosingHeader(buf) {
	return (
		buf[0] == 43 &&
		buf[1] == 43 &&
		buf[2] == 43 &&
		buf[6] == 45 &&
		buf[7] == 45 &&
		buf[8] == 45
	);
}
function validateAndParse() {
	if (currentBuffer.length < 9) {
		console.log('still not enough data - try again later');
		return false;
	}
	if (!hasOpeningAndClosingHeader(currentBuffer)) {
		console.log('bad data - try algin to header');
		var hOffset = 8;
		var foundH = 0;
		while (hOffset < currentBuffer.length) {
			if (currentBuffer[hOffset] == 43) {
				foundH++;
			} else {
				foundH--;
			}
			if (foundH == 3) {
				console.log(
					'Algin to header - ok - try parse again - offset: ' + hOffset
				);
				currentBuffer = currentBuffer.slice(hOffset - 2, currentBuffer.length);
				return validateAndParse();
			}
			hOffset++;
		}
		console.log(
			'Algin to header - missing packets or bad data found  - index: ' + hOffset
		);
		var deleteTo = Math.min(0, foundH);
		currentBuffer = currentBuffer.slice(
			hOffset - deleteTo,
			currentBuffer.length
		);
		return validateAndParse();
	}

	currentCommand = currentBuffer.slice(0, 9);
	currentBuffer = currentBuffer.slice(9, currentBuffer.length);
	return true;
}

function onDataArrived(data) {
	console.log('--------------------------');
	console.log('data size: ' + data.length);
	console.log('data: ', data);

	currentBuffer = Buffer.concat([currentBuffer, data]);
	console.log('-currentBuffer: ', currentBuffer);
	console.log('total size: ' + currentBuffer.length);
	console.log('data value : ' + JSON.stringify(currentBuffer));

	if (!validateAndParse()) {
		return;
	}

	console.log('handle msg...');
	currentCommand = currentCommand.slice(3, currentCommand.length); //delete first '+' header items
	console.log('currentCommand: ', currentCommand);
	console.log('currentBuffer: ', currentBuffer);
	var address = currentCommand[0];
	console.log(address);
	var type = currentCommand[1];
	console.log(type);
	var state = currentCommand[2];
	console.log(state);
	console.log('address: ' + address);
	console.log('type: ' + type);
	console.log('state: ' + state);
}

function test() {
	const testBuf = Buffer.alloc(9);
	testBuf[0] = 43; //'+';
	testBuf[1] = 43; //'+';
	testBuf[2] = 43; //'+';
	testBuf[3] = 6;
	testBuf[4] = 100;
	testBuf[5] = 5;
	testBuf[6] = 45; //'-';
	testBuf[7] = 45; //'-';
	testBuf[8] = 45; //'-';
	onDataArrived(testBuf);
}
function test2() {
	const testBuf = Buffer.alloc(8);
	testBuf[0] = 43; //'+';
	testBuf[1] = 43; //'+';
	testBuf[2] = 43; //'+';
	testBuf[3] = 6;
	testBuf[4] = 100;
	testBuf[5] = 5;
	testBuf[6] = 45; //'-';
	testBuf[7] = 45; //'-';
	onDataArrived(testBuf);
	const testBuf2 = Buffer.alloc(1);
	testBuf2[0] = 45; //'-';
	onDataArrived(testBuf2);
}

function test3() {
	const testBuf = Buffer.alloc(8);
	testBuf[0] = 43; //'+';
	testBuf[1] = 43; //'+';
	testBuf[2] = 43; //'+';
	testBuf[3] = 6;
	testBuf[4] = 100;
	testBuf[5] = 5;
	testBuf[6] = 45; //'-';
	testBuf[7] = 45; //'-';
	onDataArrived(testBuf);
	test();
}
test3();
