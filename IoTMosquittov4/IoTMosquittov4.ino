#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include "MQ135.h"
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SD.h>
 

// Sensor temp/humedad
#define DHTPIN 0
#define DHTTYPE DHT21

// pantalla
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO

// SD
#define CS_PIN 15
#define PORT_RELAY  16
// ventilador

#define CS_PIN 15

const char* ssid = "HAdEES-C";
const char* password = "hadeesc22";

/*
const char* ssid = "CESAR_Ext";
const char* password = "cesara2022";
*/
const char* mqtt_server = "test.mosquitto.org";
const char* MQTT_CLIENT_NAME = "ArduinoClient_2"; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char temp[MSG_BUFFER_SIZE];
char hum[MSG_BUFFER_SIZE];
char aire[MSG_BUFFER_SIZE];
char topico1[MSG_BUFFER_SIZE] = "hadeesc/Domo2/temperatura"; // domo 1 15 sec, domo 2 30 sec, domo 3 45 sec
char topico2[MSG_BUFFER_SIZE] = "hadeesc/Domo2/humedad";
char topico3[MSG_BUFFER_SIZE] = "hadeesc/Domo2/aire";
int value = 0;

//inicializacion de temp/hum
DHT dht(DHTPIN, DHTTYPE);
float h = 0;
float h_ant = 0;
float t = 0;
float t_ant = 0;
float f = 0;
float f_ant = 0;  
float hif = 0;
float hic = 0;
float air_quality = 0; 

int dataTime = 0;
int retraso = 1*5000;
int count = 0;
int sensor_fail = 0;

String controlRemoto = " ";

bool RELAY_NO = false;
bool abre=false;
bool existe=false;

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
File dataFile;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(PORT_RELAY, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();

// inicializar pantalla
  display.begin(i2c_Address, true); // Address 0x3C default

  display.clearDisplay();
  display.display();

  display.setCursor(48, 32);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.print("LOG 2 - Labs");
  display.display();

  Serial.println("pantalla inicial");

  SD.begin(CS_PIN);
  if (!SD.begin(CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }

  dataFile = SD.open("LOG2.txt", FILE_WRITE);
  
}
 
void subscribeMqtt() {
    client.subscribe("hadeesc/Domo2/temperatura");
    client.subscribe("hadeesc/Domo2/humedad");
    client.subscribe("hadeesc/Domo2/aire");
}

void setup_wifi() {
 
  delay(10);
  // We start by connecting to a WiFi network
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
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
 
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected() && count <= 5) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_NAME)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world puto");
      // ... and resubscribe
      client.subscribe("inTopic");
      
      subscribeMqtt();
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    Serial.println(count);
    count++;
  }
  
  count = 0;
}
 
void loop() {
 
  if (!client.connected()) {
    reconnect();
  }
  if (client.connected()){
    client.loop();
    }

  count = 0;
  

  MQ135 gasSensor = MQ135(A0);

  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  air_quality = gasSensor.getPPM();
  

  if (isnan(h) || isnan(t) || isnan(f)) {
    
    h = h_ant;
    t = t_ant;
    f = f_ant;
    sensor_fail = 1;
    /*
    Serial.println(F("Failed to read from DHT sensor!"));
    return; 
    */
  }

  hif = dht.computeHeatIndex(f, h);
  hic = dht.computeHeatIndex(t, h, false);




   delay(retraso);
  
   RELAY_NO = digitalRead(PORT_RELAY);
  if(t>=25 && RELAY_NO == true){
    RELAY_NO = true;
    digitalWrite(PORT_RELAY, HIGH);
  }
  else if(t>=25 && RELAY_NO == false){
    RELAY_NO = true;
    digitalWrite(PORT_RELAY, HIGH);
  }
  else if(t<25&& t>20 && RELAY_NO == true){
    RELAY_NO = true;
    digitalWrite(PORT_RELAY, HIGH);
  }
  else if(t<25&& t>20 && RELAY_NO == false){
    RELAY_NO = false;
    digitalWrite(PORT_RELAY, LOW);
  }
  else if(t<=20 && RELAY_NO == true){
    RELAY_NO = false;
    digitalWrite(PORT_RELAY, LOW);
  }
  else if(t<=20 && RELAY_NO == false){
    RELAY_NO = false;
    digitalWrite(PORT_RELAY, LOW);
  }

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (temp, MSG_BUFFER_SIZE, "%.2f", t);
    snprintf (hum, MSG_BUFFER_SIZE, "%.2f", h);
    snprintf (aire, MSG_BUFFER_SIZE, "%.2f", air_quality);
    Serial.print("Publish message: ");
    Serial.println(temp);
    Serial.println(hum);
    Serial.println(aire);
    client.publish(topico1, temp);
    client.publish(topico2, hum);
    client.publish(topico3, aire);
  }
  
  
  dataFile = SD.open("LOG2.txt", FILE_WRITE);
  // archivo abierto correctament, escribe datos
  if (dataFile) {
     Serial.println("Archivo abierto");
          
      dataFile.print(dataTime);
      dataFile.print(",");
      dataFile.print(RELAY_NO);
      dataFile.print(",");
      dataFile.print(h);
      dataFile.print(",");
      dataFile.print(t);
      dataFile.print(",");
      dataFile.println(air_quality);
 
      
      dataFile.close();
      abre = true;
  }
  // // archivo sin abrir
  else {
    Serial.println("no abre LOG2.txt");
    abre = false;
}

  if (SD.exists("LOG2.txt")) {
    Serial.println("LOG2.txt exists.");
    existe = true;
  } else {
    Serial.println("LOG2.txt doesn't exist.");
    existe = false;
  }

  
  
  delay(retraso);



// limpiar pantalla
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 8);  
    display.setTextSize(1);
  display.print("Temperature: ");
  display.print(t);
  display.write(167);
  display.println();
  display.print("Humidity: ");
  display.print(h);
  display.write(37);
  display.println();
  display.print("Air PPM: ");
  display.print(air_quality);
  display.print("PPM");
  display.println();
  display.print("exite: ");
  //display.println(existe);
  if (existe == true){
    display.println("SI existe");
  }else{
    display.println("NO existe");
  }
  
  display.print("abrio: ");
  if (abre == true){
    display.println("SI abre");
  }else{
    display.println("NO abre");
  }
  display.print("Relay: ");
  if (RELAY_NO == true){
    display.println("Encendido");
  }else{
    display.println("Apagado");
  }
  display.print("LOG2");
  if (sensor_fail == 0){
    display.println(" ");
  }else{
    display.println(" sensor fallo");
  }
  display.display();

  delay(retraso);
  count = 0;
  h_ant = h;
  t_ant = t;
  f_ant = f;
  sensor_fail = 0;
  

}
