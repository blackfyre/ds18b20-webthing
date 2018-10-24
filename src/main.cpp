#include <Arduino.h>

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <Thing.h>
#include <WebThingAdapter.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5

const char *ssid = "_________";
const char *password = "_________";

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

WebThingAdapter *adapter;

const char *bme280Types[] = {nullptr};
ThingDevice weather("ds18b20", "DS18B20 Temp Sensor", bme280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, nullptr);

void readData(DeviceAddress deviceAddress)
{
    float temp(NAN);

    temp = sensors.getTempC(deviceAddress);

    ThingPropertyValue value;
    value.number = temp;
    weatherTemp.setValue(value);
}

/*
 * Setup function. Here we do the basics
 */
void setup(void)
{
    // start serial port
    Serial.begin(9600);
    sensors.begin();

    if (!sensors.getAddress(insideThermometer, 0))
        Serial.println("Unable to find address for Device 0");

    sensors.setResolution(insideThermometer, 9);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    adapter = new WebThingAdapter("weathersensor", WiFi.localIP());

    weather.addProperty(&weatherTemp);
    adapter->addDevice(&weather);
    adapter->begin();
}
/*
 * Main function. It will request the tempC from the sensors and display on Serial.
 */
void loop(void)
{
    sensors.requestTemperatures();

    // It responds almost immediately. Let's print out the data
    readData(insideThermometer); // Use a simple function to print out the data
    adapter->update();
}