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

function getNumber(length) {
    var number = parseInt(rawData.substr(0, length * BYTE_LENGTH), 16);
    if (length > 1) {
        number = littleEndianToBigEndian(number);
    }
    rawData = rawData.substr(length * BYTE_LENGTH);

    return number;
}

function getPlayer() {
    var player = {
        penalty: 0,
        secsTillUnpenalised: 0
    };

    player.penalty = getNumber(1);
    player.secsTillUnpenalised = getNumber(1);
    return player;
}

function getTeam() {
    var team = {
        teamNumber: 0,
        teamColour: 0,
        score: 0,
        penaltyShot: 0,
        singleShots: 0,
        coachMessage: '',
        coach: {},
        players: []
    };

    team.teamNumber = getNumber(1);
    team.teamColour = getNumber(1);
    team.score = getNumber(1);
    team.penaltyShot = getNumber(1);
    team.singleShots = getNumber(2);
    team.coachMessage = getString(40);
    team.coach = getPlayer();
    for (var i = 0; i < MAX_PLAYERS; ++i) {
        team.players[i] = getPlayer();
    }

    return team;
}

function getBase() {
    var data = {
        header: '',        //4
        version: 0,        //1
        packetNumber: 0,   //1
        playersPerTeam: 0,
        state: 0,          //1
        firstHalf: 0,
        kickOffTeam: 0,    //1
        secondaryState: 0,
        dropInTeam: 0,
        dropInTime: 0,
        secsRemaining: 0,
        secondaryTime: 0,
        teams: []
    }

    data.header = getString(4);
    data.version = getNumber(1);
    data.packetNumber = getNumber(1);
    data.playersPerTeam = getNumber(1);
    data.state = getNumber(1);
    data.firstHalf = getNumber(1);
    data.kickOffTeam = getNumber(1);
    data.secondaryState = getNumber(1);
    data.dropInTeam = getNumber(1);
    data.dropInTime = getNumber(2);
    data.secsRemaining = getNumber(2);
    data.secondaryTime = getNumber(2);
    data.teams[0] = getTeam();
    data.teams[1] = getTeam();

    return data;
}

function printPlayer(player) {
    console.log('\t\tPenalty: ' + player.penalty);
    console.log('\t\tSecs Until Unpenalised: ' + player.secsTillUnpenalised);
}

function printTeam(team, number) {
    console.log('Team ' + number);
    console.log('\tTeam Number: ' + team.teamNumber);
    console.log('\tTeam Colour: ' + team.teamColour);
    console.log('\tScore: ' + team.score);
    console.log('\tPenalty Shot: ' + team.penaltyShot);
    console.log('\tSingle Shots: ' + team.singleShots);
    console.log('\tCoach Message: ' + team.coachMessage);
    console.log('\tCoach:');
    printPlayer(team.coach)
    for (var i = 1; i <= MAX_PLAYERS; ++i) {
        console.log('\tTeam players[' + i + ']');
        printPlayer(team.players[i - 1]);
    }
}

function printData(data) {
    console.log('Header: '+ data.header);
    console.log('Version: ' + data.version);
    console.log('Packet Number: ' + data.packetNumber);
    console.log('State: ' + data.state);
    console.log('First Half: ' + data.firstHalf);
    console.log('Kick Off Team: ' + data.kickOffTeam);
    console.log('Secondary State: ' + data.secondaryState);
    console.log('Drop In Team: ' + data.dropInTeam);
    console.log('Drop In Time: ' + data.dropInTime);
    console.log('Seconds Remaining: ' + data.secsRemaining);
    console.log('Secondary Time: ' + data.secondaryTime);
    printTeam(data.teams[0], 1);
    printTeam(data.teams[1], 2);
}




rl.on('line', function (line) {
    rawData = line

    data = getBase();

    printData(data);
});
