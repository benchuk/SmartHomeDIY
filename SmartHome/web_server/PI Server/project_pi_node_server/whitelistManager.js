var logger = require('./homeLogger');

function whitelist(request, res, next) {
    if (
        request.headers['user-agent'] ==
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36'
    ) {
        next();
        return;
    }
    if (
        request.headers['user-agent'] ==
        'Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1'
    ) {
        next();
        return;
    }
    if (request.headers['user-agent'] == '$%7BPRODUCT_NAME%7D/700 CFNetwork/974.2.1 Darwin/18.0.0') {
        next();
        return;
    }

    if (request.query.appid == "IFTTT" && request.headers['user-agent'] == undefined) {
        next();
        return;
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