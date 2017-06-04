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
    homebridge.registerAccessory('homebridge-growbox-soilhumidity', 'SoilSensor', GrowBoxSoilSensorAccessory);
}


function GrowBoxSoilSensorAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxSoilSensorAccessory.prototype.getCurrentHumidity = function (callback) {
        callback(null, currenthumidity);
	}
	GrowBoxSoilSensorAccessory.prototype.queryi2cbus = function () {
	var connection = mysql.createConnection({
	host     : 'localhost',
	user     : 'root',
	password : 'qweqwe!23',
	database : 'GrowBox'
	});
	connection.query('SELECT soilsensorvalue FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
	currenthumidity = results[0].soilsensorvalue;
	});
	connection.end();
	SoilSensorService.setCharacteristic(Characteristic.CurrentRelativeHumidity, currenthumidity);
        }

GrowBoxSoilSensorAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	SoilSensorService = new Service.HumiditySensor(this.name);
        SoilSensorService
        .getCharacteristic(Characteristic.CurrentRelativeHumidity)
        .on('get', this.getCurrentHumidity.bind(this));
	services.push(SoilSensorService);
	this.service = SoilSensorService;
        return services;
	}

GrowBoxSoilSensorAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

