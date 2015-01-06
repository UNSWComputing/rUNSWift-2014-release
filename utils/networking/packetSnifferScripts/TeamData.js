var readline = require('readline');
    rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});


var rawData = '';

var BYTE_LENGTH = 2;

var MAX_PLAYERS = 11;

function littleEndianToBigEndian(val) {
    return ((val & 0xFF) << 24)
        | ((val & 0xFF00) << 8)
        | ((val >> 8) & 0xFF00)
        | ((val >> 24) & 0xFF);
}


function getString(length) {
    var string = '';
    for (var i = 0; i < length; ++i) {
        var index = i * BYTE_LENGTH;
        string += String.fromCharCode(parseInt(rawData.substr(index, BYTE_LENGTH), 16));
    }

    rawData = rawData.substr(length * 2);

    return string;
}

function getFloat() {
    var floatHex = rawData.substr(0, 8 * 2);
    var float =  Buffer(floatHex, 'hex').readFloatLE(0);
    rawData = rawData.substr(8);
    return float;
}

function getVector() {
    var data = [];
    data[0] = getFloat();
    data[1]= getFloat();
    data[2] = getFloat();
    return data;
}


function getMatrix(type, rows, cols) {
    var matrix = [];
    for (var i = 0; i < rows; ++i) {
        var rowData = [];
        for (var j = 0; j < cols; ++j) {
            var data;
            if (type == "float") {
                data = getFloat();
            } else if (type == "int32") {
                data = getNumber(4);
            }
            rowData[j] = data;
        }
        matrix[i] = rowData;
    }
    return matrix;
}


function getAbsCoord() {
    var data = {
        vec: {},
        var: []
    };

    data.vec = getVector();
    data.var = getMatrix("float", 3, 3);
    return data;
}
function getRRCoord() {
    var data = {
        vec: {},
        var: []
    };

    data.vec = getVector();
    data.var = getMatrix("float", 3, 3);
    return data;

}

function getBool() {
    return getNumber(4) > 0;
}



function getNumber(length) {
    var number = parseInt(rawData.substr(0, length * BYTE_LENGTH), 16);
    if (length > 1) {
        number = littleEndianToBigEndian(number);
    }
    rawData = rawData.substr(length * BYTE_LENGTH);

    return number;
}

function getSharedLocalisation() {
    var data = {
        isUpdateValid: false,
        sharedUpdateMean: [],
        sharedUpdateCovariance: [],
        sharedDx: 0.0,
        sharedDy: 0.0,
        sharedDh: 0.0,
        sharedCovarianceDx: 0.0,
        sharedCovarianceDy: 0.0,
        sharedCovarianceDh: 0.0
    };

    data.isUpdateValid = getBool();
    data.sharedUpdateMean = getMatrix("float", 7, 1);
    data.sharedUpdateCovariance = getMatrix("float", 7, 7);
    data.sharedDx= getFloat();
    data.sharedDy= getFloat();
    data.sharedDh= getFloat();
    data.sharedCovarianceDx= getFloat();
    data.sharedCovarianceDy= getFloat();
    data.sharedCovarianceDh= getFloat();
    return data;

}

function getBehaviourShareData() {
    var data = {
        goalieAttacking: false,
        timeToReachBall: 0.0,
        currentRole: 0,
        doingBallLineUp: false
    };

    data.goalieAttacking = getBool();
    data.timeToReachBall = getFloat();
    data.currentRole = getNumber(4);
    data.doingBallLineUp = getBool();
    return data;
}


function getBase() {
    var data = {
        playerNumber: 0,        //4
        playerTeam: 0,        //1
        robotPos: {},
        ballPosAbs: {},
        ballPosRR: {},
        sharedLocalisation: {},
        behaviourShareData: {},
        actionCommand: 0,
        uptime: 0.0
    }

    data.playerNumber = getNumber(4);
    data.playerTeam = getNumber(4);
    data.robotPos = getAbsCoord();
    getFloat(); //don't know why this?
    data.ballPosAbs = getAbsCoord();
    getFloat(); //don't know why this?
    data.ballPosRR = getRRCoord();
    data.lostCount = getNumber(4);
    data.sharedLocalisation = getSharedLocalisation();
    data.behaviourShareData = getBehaviourShareData();
    data.actionCommand = getNumber(4);
    data.uptime = getFloat();

    return data;
}


var tabLevel = 0;

function printLine(line) {
    for (var i = 0; i < tabLevel; ++i) {
        line = '\t' + line;
    }
    console.log(line);
}


function printVector(vector) {
    console.log("x: " + vector.x);
    console.log("y: " + vector.y);
    console.log("theta: " + vector.theta);
}

function printSharedLocalisation(shared) {
    printLine("Shared localisation: ");
    ++tabLevel;
    printLine('Is update valid: ' + shared.isUpdateValid);
    printLine('Shared update mean: ' + shared.sharedUpdateMean);
    printLine('Shared update covariance in: ' + shared.sharedUpdateCovariance);
    printLine('Shared Dx: ' + shared.sharedDx);
    printLine('Shared Dy: ' + shared.sharedDy);
    printLine('Shared Dh: ' + shared.sharedDh);
    printLine('Shared Covariance Dx: ' + shared.sharedCovarianceDx);
    printLine('Shared Covariance Dy: ' + shared.sharedCovarianceDy);
    printLine('Shared Covariance Dh: ' + shared.sharedCovarianceDh);
    --tabLevel;
}

function printBehaviourShareData(shared) {
    printLine("Behaviour shared Data: ");
    ++tabLevel;
    printLine("Goalie attacking: " + shared.goalieAttacking);
    printLine("Time to reach ball: " + shared.timeToReachBall);
    printLine("Current Role: " + shared.currentRole);
    printLine("Doing ball line up: " + shared.doingBallLineUp);
    --tabLevel;
}

function printVar(v) {
    console.log(v);
}

function printAbsCoord(coord) {
    printLine("x: " + coord.vec[0]);
    printLine("y: " + coord.vec[1]);
    printLine("theta: " + coord.vec[2]);
    printLine("Var: " );
    ++tabLevel;
    printVar(coord.var);
    --tabLevel;
}

function printRRCoord(coord) {
    printLine("distance: " + coord.vec[0]);
    printLine("heading: " + coord.vec[1]);
    printLine("orientation: " + coord.vec[2]);
    printLine("Var: " );
    ++tabLevel;
    printVar(coord.var);
    --tabLevel;
}

function printData(data) {
    printLine('Player Number: '+ data.playerNumber);
    printLine('Player Team: ' + data.playerTeam);
    printLine('Robot Pos abs:');
    ++tabLevel;
    printAbsCoord(data.robotPos);
    --tabLevel;
    printLine('Ball pos abs:');
    ++tabLevel;
    printAbsCoord(data.ballPosAbs);
    --tabLevel;
    printLine('Ball pos rr:');
    ++tabLevel;
    printRRCoord(data.ballPosRR);
    --tabLevel;
    printLine('Lost Count: ' + data.lostCount);
    printSharedLocalisation(data.sharedLocalisation);
    printBehaviourShareData(data.behaviourShareData);
}




rl.on('line', function (line) {
    rawData = line

    data = getBase();

    printData(data);
});
