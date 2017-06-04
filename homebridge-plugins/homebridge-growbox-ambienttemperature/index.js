var Service, Characteristic;

var mysql = require('mysql');
var currenttemperature = 0;
var acc;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory('homebridge-growbox-ambienttemperature', 'AmbientTemperature', GrowBoxAmbientTemperatureAccessory);
}


function GrowBoxAmbientTemperatureAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxAmbientTemperatureAccessory.prototype.getCurrentTemperature = function (callback) {
        callback(null, currenttemperature);
	}

GrowBoxAmbientTemperatureAccessory.prototype.queryi2cbus = function () {
	 var connection = mysql.createConnection({
        host     : 'localhost',
        user     : 'root',
        password : 'qweqwe!23',
        database : 'GrowBox'
        });
        connection.query('SELECT ambienttemp FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
        currenttemperature = results[0].ambienttemp;
        });
	connection.end();
	AmbientTemperatureService.setCharacteristic(Characteristic.CurrentTemperature, currenttemperature);
        }

GrowBoxAmbientTemperatureAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	AmbientTemperatureService = new Service.TemperatureSensor(this.name);
        AmbientTemperatureService
        .getCharacteristic(Characteristic.CurrentTemperature)
        .on('get', this.getCurrentTemperature.bind(this));
	services.push(AmbientTemperatureService);
	this.service = AmbientTemperatureService;
        return services;
	}

GrowBoxAmbientTemperatureAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

