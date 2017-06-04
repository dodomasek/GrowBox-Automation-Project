var Service, Characteristic;

var mysql = require('mysql');


var powerstate = 0;
var rotationspeed = 0;
var acc;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory('homebridge-growbox-mainfan', 'MainFan', GrowBoxMainFanAccessory);
}


function GrowBoxMainFanAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxMainFanAccessory.prototype.getPowerState = function (callback) {
        callback(null, powerstate);
	}

GrowBoxMainFanAccessory.prototype.getRotationDirection = function (callback) {
        callback(null, 0);
        }

GrowBoxMainFanAccessory.prototype.getRotationSpeed = function (callback) {
        callback(null, rotationspeed);
        }

GrowBoxMainFanAccessory.prototype.queryi2cbus = function () {
	var connection = mysql.createConnection({
        host     : 'localhost',
        user     : 'root',
        password : 'qweqwe!23',
        database : 'GrowBox'
        });
        connection.query('SELECT mainfanon,mainfanrpm FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
        powerstate = results[0].mainfanon;
	rotationspeed = results[0].mainfanrpm * 100 / 2700;
	if (rotationspeed >= 100) rotationspeed = 100;
        });
	connection.end();
	MainFanService.setCharacteristic(Characteristic.On, powerstate);
	MainFanService.setCharacteristic(Characteristic.RotationDirection, 0);
	MainFanService.setCharacteristic(Characteristic.RotationSpeed, rotationspeed);
        }

GrowBoxMainFanAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	MainFanService = new Service.Fan(this.name);
        MainFanService
        .getCharacteristic(Characteristic.On)
        .on('get', this.getPowerState.bind(this));
	MainFanService
        .getCharacteristic(Characteristic.RotationDirection)
        .on('get', this.getRotationDirection.bind(this));
	MainFanService
	.getCharacteristic(Characteristic.RotationSpeed)
        .on('get', this.getRotationSpeed.bind(this));
	services.push(MainFanService);
	this.service = MainFanService;
        return services;
	}

GrowBoxMainFanAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

