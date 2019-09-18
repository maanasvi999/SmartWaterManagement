#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<Servo.h>
Servo myservo;

#define trig D5
#define echo D6 
void callback(char* topic, byte* payload, unsigned int payloadLength);
//-------- Customise these values -----------
const char* ssid = "Android On5";
const char* password = "17791779";

#define ORG "q9hbmx"
#define DEVICE_TYPE "NodeMCU"
#define DEVICE_ID "12345"
#define TOKEN "123456789"
//-------- Customise the above values --------
 //include device credentials
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char publictopic[] = "iot-2/evt/distance/fmt/json";
char subscribetopic[] = "iot-2/cmd/command/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
String command;
void setup() {
 
  myservo.attach(D4);
  
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  Serial.begin(9600);  
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");  } 
  Serial.println("");
 
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
   mqttConnect();
}
 
void loop() {
 
  float duration;
  double distance;
  double percent;
  // put your main code here, to run repeatedly:
  digitalWrite(trig,LOW);
  delay(1000);

  digitalWrite(trig,HIGH);
  delay(8000);
  digitalWrite(trig,LOW);

  duration = pulseIn(echo,HIGH);
  distance=(duration*0.034)/2;
  percent=(100-distance);
  Serial.print("distance level of unfilled water :");
  Serial.println(distance); 
  Serial.print("percentage of filled water :");
  Serial.println(percent); 
  
  PublishData(distance,percent);
  delay(1000);
  if (!client.loop()) {
    mqttConnect();  }
  delay(10);
  
}


void PublishData(float distance, float percent){
  if (!!!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(50);   }
    Serial.println();   }
  
  String payload = "{\"d\":{\"distance\":";
  payload += distance;
  payload += "," "\"percent\":";
  payload += percent;
  payload += "}}";
  Serial.print("Sending payload: ");
  Serial.println(payload);
  
  if (client.publish(publictopic, (char*) payload.c_str())) {
      Serial.println("Publish ok");   }
    else {
      Serial.println("Publish failed");   }
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  delay(100);
  if (client.subscribe(subscribetopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: "); 
  Serial.println(subscribetopic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    
    command += (char)payload[i];
    
  }
Serial.println(command);
  
if(command=="turn_on"){
 
  {

    int pos;
     for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(360);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  
    } 
  }
}
  else if(command=="turn_off")
  {  
    
    myservo.write(0);
    delay(15);   
    Serial.println("Motor is Turned OFF");    
  }
 
command ="";

}
