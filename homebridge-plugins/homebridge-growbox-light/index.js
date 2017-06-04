var Service, Characteristic;

var mysql = require('mysql');

var connection = mysql.createConnection({
host     : 'localhost',
user     : 'root',
password : 'qweqwe!23',
database : 'GrowBox'
});


var powerstate = 0;
var acc;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory('homebridge-growbox-light', 'Light', GrowBoxLightAccessory);
}


function GrowBoxLightAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxLightAccessory.prototype.getPowerState = function (callback) {
        callback(null, powerstate);
	}

GrowBoxLightAccessory.prototype.queryi2cbus = function () {
	var connection = mysql.createConnection({
        host     : 'localhost',
        user     : 'root',
        password : 'qweqwe!23',
        database : 'GrowBox'
        });
	connection.query('SELECT lighton FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
        powerstate = (results[0].lighton);
        });
	connection.end();
	LightService.setCharacteristic(Characteristic.On, powerstate);
        }

GrowBoxLightAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	LightService = new Service.Switch(this.name);
        LightService
        .getCharacteristic(Characteristic.On)
        .on('get', this.getPowerState.bind(this));
	services.push(LightService);
	this.service = LightService;
        return services;
	}

GrowBoxLightAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

