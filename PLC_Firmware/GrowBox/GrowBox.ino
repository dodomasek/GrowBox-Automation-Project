#include <Wire.h>
#include <OneWire.h>

#include "DHT.h"

//Global Static values
#define LIGHTFANS_COUNT 2
#define SOIL_VALUE_LOW_TRESHOLD 512//value between 0-1024 treated as low trigger
#define CONTROL_TIMEOUT_TRESHOLD 10
#define WATERING_DURATION_MILLISECS 5000

//Pinout definitions
#define PIN_HEARTBEAT_RED 1
#define PIN_HEARTBEAT_GREEN 0
#define PIN_RESERVED_RELAY 10
#define PIN_WIND 11
#define PIN_LIGHT 4
#define PIN_LIGHTFAN 5
#define PIN_MAINFAN 6
#define PIN_WATERING 7
#define PIN_LIGHTFAN_RPM 2
#define PIN_MAINFAN_RPM 3
#define PIN_SPRINKLER 12
#define PIN_1WIRE 8
#define PIN_SOILSENSOR_VCC 13
#define PIN_SOILSENSOR_DATA 0//A0 reads voltage compared to VCCref
#define PIN_DHT22 9
//=====================

//Global Objects
OneWire  ds(PIN_1WIRE);
DHT ambientsensor(PIN_DHT22, DHT22);
//=====================

//Global Variables

bool FORCED_MAINFAN_ON;
bool SPRINKLER_RELAY_ON;
bool WATERING_RELAY_ON;
bool RESERVED_RELAY_ON;
bool WIND_ON;
bool LIGHT_ON;
bool LIGHT_FAN_ON;
bool MAIN_FAN_ON;
volatile long heartbeats;
volatile int loopcounter;
int controlflagcounter;
float LIGHT_TEMP;
float AMBIENT_TEMP;
float AMBIENT_HUMIDITY;
int SOIL_VALUE;
volatile int LIGHTFAN_RPM;//for internal purposes only, use calculated value below
volatile int MAINFAN_RPM;//for internal purposes only, use calculated value below
int CALC_LIGHTFAN_RPM;
int CALC_MAINFAN_RPM;
int ReceivedCommand;
//=====================

//Executes once on start
void setup() {
  FORCED_MAINFAN_ON = false;
  RESERVED_RELAY_ON = false;
  SPRINKLER_RELAY_ON = false;
  WIND_ON = false;
  LIGHT_ON = false;
  LIGHT_FAN_ON = false;
  MAIN_FAN_ON = false;
  WATERING_RELAY_ON = false;
  LIGHT_TEMP = 0.0;
  AMBIENT_TEMP = 0.0;
  AMBIENT_HUMIDITY = 0.0;
  CALC_LIGHTFAN_RPM = 0;
  CALC_MAINFAN_RPM = 0;
  SOIL_VALUE = 0;
  LIGHTFAN_RPM = 0;
  MAINFAN_RPM = 0;
  ReceivedCommand = 0;
  loopcounter = 0;
  heartbeats = 0;
  controlflagcounter = -100;
  pinMode(PIN_LIGHTFAN_RPM, INPUT);
  pinMode(PIN_MAINFAN_RPM, INPUT);
  attachInterrupt(0, LFRPMCOUNTER, RISING);
  attachInterrupt(1, MFRPMCOUNTER, RISING);
  cli();
  pinMode(PIN_HEARTBEAT_RED, OUTPUT);
  pinMode(PIN_HEARTBEAT_GREEN, OUTPUT);
  digitalWrite(PIN_HEARTBEAT_GREEN, HIGH);
  digitalWrite(PIN_HEARTBEAT_RED, HIGH);
  pinMode(PIN_LIGHT, OUTPUT);
  pinMode(PIN_LIGHTFAN, OUTPUT);
  pinMode(PIN_MAINFAN, OUTPUT);
  pinMode(PIN_WIND, OUTPUT);
  pinMode(PIN_RESERVED_RELAY, OUTPUT);
  pinMode(PIN_WATERING, OUTPUT);
  pinMode(PIN_SPRINKLER, OUTPUT);
  pinMode(PIN_SOILSENSOR_VCC, OUTPUT);
  ambientsensor.begin();
  Wire.begin(51);
  Wire.onReceive(IIC_RECEIVE_EVENT);
  Wire.onRequest(IIC_REQUEST_EVENT);
}

//message from master PC
void IIC_RECEIVE_EVENT(int bytescount)
{
  //controlflagcounter = loopcounter;
  if (bytescount == 4)
  {
    int i = Wire.read();
    i = Wire.read();
    i = Wire.read();
    ReceivedCommand = Wire.read();
  }
}

//Reply to I2C request with byte-coded data (15 bytes)
void IIC_REQUEST_EVENT()
{
  controlflagcounter = loopcounter;
  int firstlfrpmbyte = 0;
  int secondlfrpmbyte = 0;
  int firstmfrpmbyte = 0;
  int secondmfrpmbyte = 0;
  int firstsoilbyte = 0;
  int secondsoilbyte = 0;
  int firsthbbyte = 0;
  int secondhbbyte = 0;
  int thirdhbbyte = 0;
  int fourthhbbyte = 0;
  if (heartbeats > 16777215) firsthbbyte = heartbeats / 16777215;
  if ((heartbeats - firsthbbyte * 16777215) > 65535) secondhbbyte = (heartbeats - firsthbbyte * 16777215) / 65535;
  if ((heartbeats - firsthbbyte * 16777215 - secondhbbyte * 65535) > 255) thirdhbbyte = (heartbeats - firsthbbyte * 16777215 - secondhbbyte * 65535) / 255;
  fourthhbbyte = heartbeats - - firsthbbyte * 16777215 - secondhbbyte * 65535 - thirdhbbyte * 255;
  if (CALC_LIGHTFAN_RPM > 255) firstlfrpmbyte = CALC_LIGHTFAN_RPM / 255;
  secondlfrpmbyte = CALC_LIGHTFAN_RPM - firstlfrpmbyte * 255;
  if (CALC_MAINFAN_RPM > 255) firstmfrpmbyte = CALC_MAINFAN_RPM / 255;
  secondmfrpmbyte = CALC_MAINFAN_RPM - firstmfrpmbyte * 255;
  if (SOIL_VALUE > 255) firstsoilbyte = SOIL_VALUE / 255;
  secondsoilbyte = SOIL_VALUE - firstsoilbyte * 255;
  int wholelighttemp = LIGHT_TEMP;
  int fractlighttemp = (LIGHT_TEMP - wholelighttemp) * 100;
  int wholeambienttemp = AMBIENT_TEMP;
  int fractambienttemp = (AMBIENT_TEMP - wholeambienttemp) * 100;
  int wholeambienthum = AMBIENT_HUMIDITY;
  int fractambienthum = (AMBIENT_HUMIDITY - wholeambienthum) * 100;
  Wire.write(0);
  Wire.write(0);//dummy for hour and minute
  Wire.write(firstlfrpmbyte);
  Wire.write(secondlfrpmbyte);
  Wire.write(firstmfrpmbyte);
  Wire.write(secondmfrpmbyte);
  Wire.write(wholelighttemp);
  Wire.write(fractlighttemp);
  Wire.write(wholeambienttemp);
  Wire.write(fractambienttemp);
  Wire.write(wholeambienthum);
  Wire.write(fractambienthum);
  Wire.write(firstsoilbyte);
  Wire.write(secondsoilbyte);
  Wire.write((int)LIGHT_ON);
  Wire.write((int)LIGHT_FAN_ON);
  Wire.write((int)MAIN_FAN_ON);
  Wire.write((int)SPRINKLER_RELAY_ON);
  Wire.write((int)WATERING_RELAY_ON);
  Wire.write((int)RESERVED_RELAY_ON);
  Wire.write((int)WIND_ON);
  Wire.write(firsthbbyte);
  Wire.write(secondhbbyte);
  Wire.write(thirdhbbyte);
  Wire.write(fourthhbbyte);
}

//Count Main Fan RPM
void MFRPMCOUNTER()
{
  MAINFAN_RPM++;
}

//Count Light Fan RPM
void LFRPMCOUNTER()
{
  LIGHTFAN_RPM++;
}

//Calculate Light Temperature from DS18B21 Sensor via 1-Wire
void GetLightTemp()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  if ( !ds.search(addr)) {
    if ( !ds.search(addr))
    {
      ds.reset_search();
      delay(250);
      return;
    }
  }
  for ( i = 0; i < 8; i++) {
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    return;
  }
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      return;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  delay(1000);     // maybe 750ms is enough, maybe not
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  LIGHT_TEMP = (float)raw / 16.0;
}

//Set Ambient Vars
void SetAmbientVars()
{
  AMBIENT_HUMIDITY = ambientsensor.readHumidity();
  AMBIENT_TEMP = ambientsensor.readTemperature();
  GetLightTemp();
}


//Check Soil humidity value and make something if needed
void CheckSoilSensor()
{
  digitalWrite(PIN_SOILSENSOR_VCC, HIGH);
  delay(10);
  SOIL_VALUE = analogRead(PIN_SOILSENSOR_DATA);
  digitalWrite(PIN_SOILSENSOR_VCC, LOW);
  if (SOIL_VALUE <= SOIL_VALUE_LOW_TRESHOLD)
  {
    //TODO: make automatic watering see PIN_WATERING
  }
}

void ControlDevices()
{
  bool switchlightfan = (((LIGHT_TEMP >= (AMBIENT_TEMP + 5)) && (LIGHT_TEMP > 30)) || LIGHT_ON);
  digitalWrite(PIN_LIGHT, LIGHT_ON);
  digitalWrite(PIN_LIGHTFAN, switchlightfan);
  LIGHT_FAN_ON = switchlightfan;
  if (switchlightfan || FORCED_MAINFAN_ON)
  {
    digitalWrite(PIN_MAINFAN, HIGH);
    MAIN_FAN_ON = true;
  } else
  {
    digitalWrite(PIN_MAINFAN, LOW);
    MAIN_FAN_ON = false;
  }
}

void CheckIsControlledByMaster()
{
  int diff = 0;
  if (loopcounter >= controlflagcounter)
  {
    diff = loopcounter - controlflagcounter;
  } else
  {
    diff = loopcounter + 600 - controlflagcounter;
  }
  bool res = (diff > CONTROL_TIMEOUT_TRESHOLD);
  if (res)
  {
    digitalWrite(PIN_HEARTBEAT_GREEN, HIGH);
    digitalWrite(PIN_HEARTBEAT_RED, LOW);
    delay(150);
    digitalWrite(PIN_HEARTBEAT_GREEN, LOW);
    digitalWrite(PIN_HEARTBEAT_RED, HIGH);
    digitalWrite(PIN_WATERING, LOW);
    WATERING_RELAY_ON = false;
    digitalWrite(PIN_SPRINKLER, LOW);
    SPRINKLER_RELAY_ON = false;
    digitalWrite(PIN_RESERVED_RELAY, LOW);
    RESERVED_RELAY_ON = false;
    digitalWrite(PIN_WIND, LOW);
    WIND_ON = false;
    LIGHT_ON = false;
    LIGHT_FAN_ON = false;
    FORCED_MAINFAN_ON = false;
  } else
  {
    digitalWrite(PIN_HEARTBEAT_GREEN, LOW);
    digitalWrite(PIN_HEARTBEAT_RED, HIGH);
    delay(150);
    digitalWrite(PIN_HEARTBEAT_GREEN, HIGH);
    digitalWrite(PIN_HEARTBEAT_RED, LOW);
  }
}

void ProcessCmd(int cmd)
{
    if (cmd == 99) //Turn On ALL RELAYS, use it for testing ONLY
    {
      digitalWrite(PIN_LIGHT, HIGH);
      LIGHT_ON = true;
      delay(150);
      digitalWrite(PIN_LIGHTFAN, HIGH);
      LIGHT_FAN_ON = true;
      delay(150);
      digitalWrite(PIN_MAINFAN, HIGH);
      MAIN_FAN_ON = true;
      FORCED_MAINFAN_ON = true;
      delay(150);
      digitalWrite(PIN_WIND, HIGH);
      WIND_ON = true;
      delay(150);
      digitalWrite(PIN_RESERVED_RELAY, HIGH);
      RESERVED_RELAY_ON = true;
      delay(150);
      digitalWrite(PIN_WATERING, HIGH);
      WATERING_RELAY_ON = true;
      delay(150);
      digitalWrite(PIN_SPRINKLER, HIGH);
      SPRINKLER_RELAY_ON = true;
      return;
    }
    if (cmd == 100) //Turn Off ALL RELAYS, use it for testing ONLY
    {
      digitalWrite(PIN_LIGHT, LOW);
      LIGHT_ON = false;
      digitalWrite(PIN_LIGHTFAN, LOW);
      LIGHT_FAN_ON = false;
      digitalWrite(PIN_MAINFAN, LOW);
      MAIN_FAN_ON = false;
      FORCED_MAINFAN_ON = false;
      digitalWrite(PIN_WIND, LOW);
      WIND_ON = false;
      digitalWrite(PIN_RESERVED_RELAY, LOW);
      RESERVED_RELAY_ON = false;
      digitalWrite(PIN_WATERING, LOW);
      WATERING_RELAY_ON = false;
      digitalWrite(PIN_SPRINKLER, LOW);
      SPRINKLER_RELAY_ON = false;
      return;
    }
    if (cmd == 1) //Turn On Light
    {
      LIGHT_ON = true;
      return;
    }
    if (cmd == 2) //Turn Off Light
    {
      LIGHT_ON = false;
      return;
    }
    if (cmd == 3) //Turn On LightFan
    {
      LIGHT_FAN_ON = true;
      return;
    }
    if (cmd == 4) //Turn Off LightFan
    {
      LIGHT_FAN_ON = false;
      return;
    }
    if (cmd == 5) //Turn On MainFan
    {
      MAIN_FAN_ON = true;
      FORCED_MAINFAN_ON = true;
      return;
    }
    if (cmd == 6) //Turn Off MainFan
    {
      MAIN_FAN_ON = false;
      FORCED_MAINFAN_ON = false;
      return;
    }
    if (cmd == 7) //Turn On Wind
    {
      digitalWrite(PIN_WIND, HIGH);
      WIND_ON = true;
      return;
    }
    if (cmd == 8) //Turn Off Wind
    {
      digitalWrite(PIN_WIND, LOW);
      WIND_ON = false;
      return;
    }
    if (cmd == 9) //Turn On Reserved Relay
    {
      digitalWrite(PIN_RESERVED_RELAY, HIGH);
      RESERVED_RELAY_ON = true;
      return;
    }
    if (cmd == 10) //Turn Off Reserved Relay
    {
      digitalWrite(PIN_RESERVED_RELAY, LOW);
      RESERVED_RELAY_ON = false;
      return;
    }
    if (cmd == 11) //Turn On Watering
    {
      digitalWrite(PIN_WATERING, HIGH);
      WATERING_RELAY_ON = true;
      delay(WATERING_DURATION_MILLISECS);//safety action: turn watering off after milliseconds as WATERING_DURATION_MILLISECS
      digitalWrite(PIN_WATERING, LOW);
      WATERING_RELAY_ON = false;
      return;
    }
    if (cmd == 12) //Turn Off Watering
    {
      digitalWrite(PIN_WATERING, LOW);
      WATERING_RELAY_ON = false;
      return;
    }
    if (cmd == 13) //Turn On Sprinkler
    {
      digitalWrite(PIN_SPRINKLER, HIGH);
      SPRINKLER_RELAY_ON = true;
      return;
    }
    if (cmd == 14) //Turn Off Sprinkler
    {
      digitalWrite(PIN_SPRINKLER, LOW);
      SPRINKLER_RELAY_ON = false;
      return;
    }
}

//Main Loop
void loop() {
  loopcounter++;
  heartbeats++;
  if ((loopcounter == 600) || (loopcounter == 1))//check soil sensor every 600 cycles ~ 10min
  {
    if (loopcounter == 600) loopcounter = 0;
    CheckSoilSensor();
  }
  int cmd = ReceivedCommand;//we will handle only one command per loop cycle
  ReceivedCommand = 0;
  if (cmd != 0) ProcessCmd(cmd);
  SetAmbientVars();
  ControlDevices();
  CheckIsControlledByMaster();
  //==============================begin fans rmp counting
  LIGHTFAN_RPM = 0;
  MAINFAN_RPM = 0;
  if (LIGHT_FAN_ON || MAIN_FAN_ON)
  {
    sei();
    delay(1000);//count rotations per 1 second
    cli();
  } else
  {
    delay(1000);
  }
  CALC_LIGHTFAN_RPM = LIGHTFAN_RPM * 60 / LIGHTFANS_COUNT; //Times by 60 secs and divide by fans quantity which send pulses simultaneously, gives average RPM per fan
  CALC_MAINFAN_RPM = MAINFAN_RPM * 60;  //Just times by 60 secs gives average RPM
  //==============================end fans rpm counting
}
