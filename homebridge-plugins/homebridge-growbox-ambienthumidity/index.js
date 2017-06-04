var Service, Characteristic;

var mysql      = require('mysql');

var connection = mysql.createConnection({
host     : 'localhost',
user     : 'root',
password : 'qweqwe!23',
database : 'GrowBox'
});


var currenthumidity = 0;
var acc;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory('homebridge-growbox-ambienthumidity', 'AmbientHumidity', GrowBoxAmbientHumidityAccessory);
}


function GrowBoxAmbientHumidityAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxAmbientHumidityAccessory.prototype.getCurrentHumidity = function (callback) {
        callback(null, currenthumidity);
	}
	GrowBoxAmbientHumidityAccessory.prototype.queryi2cbus = function () {
	var connection = mysql.createConnection({
	host     : 'localhost',
	user     : 'root',
	password : 'qweqwe!23',
	database : 'GrowBox'
	});
	connection.query('SELECT ambienthum FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
	currenthumidity = results[0].ambienthum;
	});
	connection.end();
	AmbientHumidityService.setCharacteristic(Characteristic.CurrentRelativeHumidity, currenthumidity);
        }

GrowBoxAmbientHumidityAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	AmbientHumidityService = new Service.HumiditySensor(this.name);
        AmbientHumidityService
        .getCharacteristic(Characteristic.CurrentRelativeHumidity)
        .on('get', this.getCurrentHumidity.bind(this));
	services.push(AmbientHumidityService);
	this.service = AmbientHumidityService;
        return services;
	}

GrowBoxAmbientHumidityAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

