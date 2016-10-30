/**
 * NodeMCU (ESP12E) + Si7021 = Meteostation
 *
 * made by Petus
 * http://petus.cz
 * http://time4ee.com
 *
 * Si7021 library - https://github.com/LowPowerLab/SI7021
 * 
 * It's working!!!
 */

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SI7021.h>

#define SDA 4 // GPIO4 on D2
#define SCL 5 // GPIO5 on D1

#define ARDUINO_ARCH_ESP8266

#define SLEEP_DELAY_IN_SECONDS  30

const char* ssid     = "SSID";
const char* password = "password";

const char* host = "HostName.tmep.cz";

SI7021 sensor;

void setup() {
  Serial.begin(9600); // Open serial connection to report values to host
  delay(10);

   
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  sensor.begin(SDA,SCL);

}


void loop() {
  delay(500);
  int temperature = sensor.getCelsiusHundredths();
  int temperatureH = temperature/100; 
  int temperatureL = temperature - (temperatureH * 100);
  int humidity = sensor.getHumidityPercent();
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.print("\n");
  Serial.print("TemperatureH:");
  Serial.print(temperatureH);
  Serial.print("\n");
  Serial.print("TemperatureL:");
  Serial.print(temperatureL);
  Serial.print("\n");
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.print("\n");

  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }

    // We now create a URI for the request
  String url = "/index.php?";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + "tempC=" + temperatureH + "." + temperatureL + "&humV=" + humidity + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Respond:");
  while(client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  //use if you want to use deep sleep - connect RST and D0 (GPIO16)
  //ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);  
  delay(60000);
  
}
