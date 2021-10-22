#include "ESP_DHT.h"
#include "Adafruit_CCS811.h"
#include "ESP_Dust.h"
#include "ESP_Buzzer.h"

#include "ESP_WIFI.h"
#include "ESP_HTTP.h"

#include <Arduino_JSON.h>

#include <Arduino.h>
//Pins
const int dhtPin = 33; //DHT sensor data pin
const int dustLEDPin = 19; //Dust sensor led pin
const int dustDataPin = 34; //Dust sensor data pin
const int buzzerPin = 18; // Buzzer pin
const int buttonPin = 17; // Button pin

//Sensor Objects
ESP_DHT dht_sensor(dhtPin);
Adafruit_CCS811 ccs; 
ESP_Dust dust_sensor(dustLEDPin, dustDataPin);
ESP_Buzzer buzzer(buzzerPin);

ESP_WIFI wifi;

//Global Settings (updated by server)
bool alarm_on = false;
bool stop_alarm = false;
int data_interval = 7000;

int settings_interval = 3000; // update local settings from server time

int buttonState = 0; // state of button

String unqID = "";

//Wifi Details
const char* ssid = "AndroidAPa825";
const char* password = "pibo1438";  

//Server Details
const String serverSend = "http://smart-air-quality.herokuapp.com/api/v1/receive_data";
const String serverSettings = "https://smart-air-quality.herokuapp.com/api/v1/get_settings";
const String serverBuzzer = "http://smart-air-quality.herokuapp.com/api/v1/stop_alarm";

//Setup of device and initilizing task loops
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);

  wifi.connectWIFI(ssid, password);

  unqID = wifi.getMacAddress();
  Serial.println("Device Unique ID: " + unqID);

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  //Begin task loops (similar to running threads)
  xTaskCreate(sensor_loop, "Sensor Loop", 5000, NULL, 5, NULL); //Sensor reading loop
  xTaskCreate(buzzer_loop, "Buzzer Loop", 5000, NULL, 3, NULL); // Buzzer loop
  xTaskCreate(update_settings_loop, "Update Settings Loop", 5000, NULL, 4, NULL); // Update settings from server loop
  xTaskCreate(button_loop, "Button Read", 5000, NULL, 2, NULL); // checking button loop
}

//Reads the DHT sensor and returns the data in a formatted string
String dht_read() {

  dht_sensor.pollSensor(); // Read Sensor
  //get sensor values
  double t = dht_sensor.getTemperature();
  double h = dht_sensor.getHumidity();
  double hic = dht_sensor.getHeatIndex();

  String dataString = String(t, 2) + " " + String(h, 2) + " " + String(hic, 2) + " ";

  return dataString;
}

//Reads the CCS811 sensor and returns the data in a formatted string
String ccs811_read() {
  String dataString = "0 0";
  //read sensor values
    if(ccs.available()){
      if(!ccs.readData()){
          int co2 = ccs.geteCO2();
          int tvoc = ccs.getTVOC();
          dataString = String(co2) + " " + String(tvoc);
      }
    }
  return dataString;
}

//Reads the dust sensor and returns the data in a formatted string
String dust_read() {
    int num_reads = 25;

    double max_read = 0.0;
    double read_dust = 0.0;
    //read the sensor multiple times and use the highest value. (achieves best result)
    for (int i = 0; i < num_reads; i++) {
      dust_sensor.pollSensor();
      read_dust = dust_sensor.getDustDensity();
      
      if(read_dust > max_read){
        max_read = read_dust;
      }
      delay(50);   
    }

    return String(max_read, 2) + " "; 
}

//called by sensor_loop to send data to the server(needed due to the way xTaskCreate works)
void sendData(void * dataString){
  char *sdata = (char*)dataString;
  //Format data string
  String ssdata = "{\"data\":\"" + String(sdata) +" "+ unqID +"\"}";
  //send data
  send_data_json(ssdata, serverSend);
  delete[] sdata;
  vTaskDelete(NULL);
}
//Reads though sensors and gathers data to be sent to server
void sensor_loop(void * parameters){
  String dht, ccs, dust;
  while(1){
    //read dht sensor
    dht = dht_read();
    //delays are used to make sure sensors work correctly
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //read dust sensor
    dust = dust_read();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //read ccs sensor
    ccs = ccs811_read();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    //format and send data
    String dataS = dht + dust + ccs;
    char *dataString = new char[dataS.length()+1];
    dataS.toCharArray(dataString, dataS.length()+1); 
    //create task to send data
    xTaskCreate(sendData, "Send Sensor Data", 5000, dataString, 1, NULL);

    vTaskDelay(data_interval / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}
//converts string true to bool true
bool stringtobool(String s){
  if(s == "true"){
    return true;
  }else{
    return false;
  }
}
//update the settings of the device from the server 
void update_settings_loop(void * parameters){
  String *pjData;
  while(1){
    //get settings json from server
    String jsonn = get_data_json(serverSettings);
    Serial.println("\nServer Settings Recieved: " + jsonn);
    //if got data from the server
    if(jsonn != "{}"){
      //parse json data
      pjData = parse_json(jsonn);
      //update settings
      alarm_on = stringtobool(pjData[0]);
      stop_alarm = stringtobool(pjData[7]);
      data_interval = pjData[5].toInt() * 1000;

      Serial.println("\nServer Settings Updated: alarm: "+pjData[0] + "  stop_alarm: "+pjData[7]+"  data_interval: "+data_interval);

      delete[] pjData;
    }
    
    vTaskDelay(settings_interval / portTICK_PERIOD_MS);
  }
}
//parse json data into a string pointer
String *parse_json(String json){
  JSONVar jsonData = JSON.parse(json);

  JSONVar keys = jsonData.keys();
  String *jdata = new String[keys.length()];
  for (int i = 0; i < keys.length(); i++) {
    JSONVar value = jsonData[keys[i]];
    jdata[i] = JSONVar::stringify(value);
  }
  
  return jdata;
}
//send supplied data to supplied server via post 
void send_data_json(String dataString, String server) {

  Serial.println("\nSending: " + dataString + "  to:  " + server);
    
  //If wifi avaliable send data
  if(wifi.isConnected()){
    int responsecode = ESP_HTTP::sendJSON(server, dataString);
    Serial.println("Server: " + server + " :: " + "Send Response Code: " + String(responsecode));
  }else{
    Serial.println("NO WIFI");
  }
}
//get json data from server
String get_data_json(String server){
  String jdata = "{}";
  //if wifi is connected get data via get request
  if(wifi.isConnected()){
    jdata = ESP_HTTP::getJSON(server);
  }
  return jdata;
}
//used by buzzer loop to start the buzzer (needed due to xTaskCreate)
void start_buzz_loop(void * parameters){
  buzzer.startBuzzLoop(800, 750);
  vTaskDelete(NULL);
}
//loops and turns on and off buzzer depending on settings from the server
void buzzer_loop(void * parameters){
  while(1){
    if(alarm_on && !stop_alarm){
      if(!buzzer.isBuzzing()){
        xTaskCreate(start_buzz_loop, "Buzzer LoopTrue", 5000, NULL, 1, NULL);
      }
    }else{
      if(buzzer.isBuzzing()){
        buzzer.endBuzzLoop();
      }
    }
    vTaskDelay(150 / portTICK_PERIOD_MS);
  }
}
//loops checking if the button has been pressed, if it has send stop buzzer request to server
void button_loop(void * param){
  String serverBuzzData = "{\"alarm\":\"false\"}";
  while(1){
    //Read button state
    buttonState = digitalRead(buttonPin);
    
    //Send that the stop alarm button has been pushed to the server
    //if button is pressed stop the alarm and dont let it turn back on until the server says
    if (buttonState == HIGH){
      alarm_on=false;
      stop_alarm=true;
      send_data_json(serverBuzzData, serverBuzzer);

      while(stop_alarm){
        alarm_on=false;
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
    }
  
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void loop() {
  //if wifi is disconnected attempt to reconnect
  wifi.reconnectWIFI();
  
  vTaskDelay(10000 / portTICK_PERIOD_MS);
}
