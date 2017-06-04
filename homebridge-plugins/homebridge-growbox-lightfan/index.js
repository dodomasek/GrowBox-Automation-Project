var Service, Characteristic;

var mysql = require('mysql');

var powerstate = 0;
var rotationspeed = 0;
var acc;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory('homebridge-growbox-lightfan', 'LightFan', GrowBoxLightFanAccessory);
}


function GrowBoxLightFanAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxLightFanAccessory.prototype.getPowerState = function (callback) {
        callback(null, powerstate);
	}

GrowBoxLightFanAccessory.prototype.getRotationDirection = function (callback) {
        callback(null, 0);
        }

GrowBoxLightFanAccessory.prototype.getRotationSpeed = function (callback) {
        callback(null, rotationspeed);
        }

GrowBoxLightFanAccessory.prototype.queryi2cbus = function () {
        var connection = mysql.createConnection({
        host     : 'localhost',
        user     : 'root',
        password : 'qweqwe!23',
        database : 'GrowBox'
        });
        connection.query('SELECT lightfanon,lightfanrpm FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
        powerstate = results[0].lightfanon;
        rotationspeed = results[0].lightfanrpm * 100 / 2100;
	if (rotationspeed >= 100) rotationspeed = 100;
        });
	connection.end();
	LightFanService.setCharacteristic(Characteristic.On, powerstate);
	LightFanService.setCharacteristic(Characteristic.RotationDirection, 0);
	LightFanService.setCharacteristic(Characteristic.RotationSpeed, rotationspeed);
        }

GrowBoxLightFanAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	LightFanService = new Service.Fan(this.name);
        LightFanService
        .getCharacteristic(Characteristic.On)
        .on('get', this.getPowerState.bind(this));
	LightFanService
        .getCharacteristic(Characteristic.RotationDirection)
        .on('get', this.getRotationDirection.bind(this));
	LightFanService
	.getCharacteristic(Characteristic.RotationSpeed)
        .on('get', this.getRotationSpeed.bind(this));
	services.push(LightFanService);
	this.service = LightFanService;
        return services;
	}

GrowBoxLightFanAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

