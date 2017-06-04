var Service, Characteristic;

var mysql = require('mysql');

var currenttemperature = 0;
var acc;

module.exports = function (homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory('homebridge-growbox-lighttemperature', 'LightTemperature', GrowBoxLightTemperatureAccessory);
}


function GrowBoxLightTemperatureAccessory(log, config) {
	this.log = log;
	this.name = config["name"];
	this.manufacturer = config["manufacturer"];
	this.model = config["model"];
	this.serial = config["serial"];
	acc = this;
	this.statePolling();
	}

GrowBoxLightTemperatureAccessory.prototype.getCurrentTemperature = function (callback) {
        callback(null, currenttemperature);
	}

GrowBoxLightTemperatureAccessory.prototype.queryi2cbus = function () {
        var connection = mysql.createConnection({
        host     : 'localhost',
        user     : 'root',
        password : 'qweqwe!23',
        database : 'GrowBox'
        });
        connection.query('SELECT lighttemp FROM GrowBoxTelemetry ORDER BY ID DESC LIMIT 1', function (error, results, fields) {
        currenttemperature = results[0].lighttemp;
        });
	connection.end();
	LightTemperatureService.setCharacteristic(Characteristic.CurrentTemperature, currenttemperature);
        }

GrowBoxLightTemperatureAccessory.prototype.getServices = function () {
	var services = [],
	informationService = new Service.AccessoryInformation();
	informationService
	.setCharacteristic(Characteristic.Manufacturer, this.manufacturer)
        .setCharacteristic(Characteristic.Model, this.model)
        .setCharacteristic(Characteristic.SerialNumber, this.serial);
	services.push(informationService);
	LightTemperatureService = new Service.TemperatureSensor(this.name);
        LightTemperatureService
        .getCharacteristic(Characteristic.CurrentTemperature)
        .on('get', this.getCurrentTemperature.bind(this));
	services.push(LightTemperatureService);
	this.service = LightTemperatureService;
        return services;
	}

GrowBoxLightTemperatureAccessory.prototype.statePolling = function () {
       this.tout = setTimeout(function () {
	acc.queryi2cbus();
        // Setup next polling
        acc.statePolling();
  }, 1000);
}

