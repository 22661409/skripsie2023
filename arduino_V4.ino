#include <WiFiS3.h>
#include<Arduino.h>
int status = WL_IDLE_STATUS;
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

unsigned int localPort = 2390;

char packetBuffer[256];
char  ReplyBuffer[] = "acknowledged\n";

WiFiUDP Udp;

float duration1, duration2, distance1, distance2;
float v = 0.0343;

#define speed1 3
#define speed2 11
#define speed3 5
#define speed4 6

#define direction1 4
#define direction2 12
#define direction3 8
#define direction4 7

#define trig1 10
#define trig2 9

#define echo1 13
#define echo2 2

void setup() {
  pinMode(speed1,OUTPUT);
  pinMode(speed2,OUTPUT);
  pinMode(speed3,OUTPUT);
  pinMode(speed4,OUTPUT);
  pinMode(direction1,OUTPUT);
  pinMode(direction2,OUTPUT);
  pinMode(direction3,OUTPUT);
  pinMode(direction4,OUTPUT);
  pinMode(trig1,OUTPUT);
  pinMode(trig2,OUTPUT);
  pinMode(echo1,INPUT);
  pinMode(echo2,INPUT);


  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

 
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
}

void loop() {

  digitalWrite(trig1,LOW);
  delayMicroseconds(200);
  digitalWrite(trig1,HIGH);
  delayMicroseconds(100);
  digitalWrite(trig1,LOW);
  duration1 = pulseIn(echo1,HIGH);
  distance1 = (duration1/2)*v;
  Serial.print("Forward: ");
  Serial.println(distance1);
  digitalWrite(trig2,LOW);
  delayMicroseconds(200);
  digitalWrite(trig2,HIGH);
  delayMicroseconds(100);
  digitalWrite(trig2,LOW);
  duration2 = pulseIn(echo2,HIGH);
  distance2 = (duration2/2)*v;
  Serial.print("Backward: ");
  Serial.println(distance2);

  int packetSize = Udp.parsePacket();
  if (packetSize)  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    // Example packetBuffer 1010255100255100
    // Control Robot from string sent

    digitalWrite(direction1,packetBuffer[0]-'0');
    digitalWrite(direction2,packetBuffer[1]-'0');
    digitalWrite(direction3,packetBuffer[2]-'0');
    digitalWrite(direction4,packetBuffer[3]-'0');
    analogWrite(speed1,((packetBuffer[4]-'0')*100+(packetBuffer[5]-'0')*10+(packetBuffer[6]-'0')));
    analogWrite(speed2,((packetBuffer[7]-'0')*100+(packetBuffer[8]-'0')*10+(packetBuffer[9]-'0')));
    analogWrite(speed3,((packetBuffer[10]-'0')*100+(packetBuffer[11]-'0')*10+(packetBuffer[12]-'0')));
    analogWrite(speed4,((packetBuffer[13]-'0')*100+(packetBuffer[14]-'0')*10+(packetBuffer[15]-'0')));


    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(packetBuffer);
    Udp.endPacket();
  }
  if ( (distance1 <= 10) && (packetBuffer[0]-'0' == 0) && (packetBuffer[1]-'0' == 0) && (packetBuffer[2]-'0' == 0) && (packetBuffer[3]-'0' == 0) ){
    analogWrite(speed1,0);
    analogWrite(speed2,0);
    analogWrite(speed3,0);
    analogWrite(speed4,0);
  }
  if ( (distance2 <= 10) && (packetBuffer[0]-'0' == 1) && (packetBuffer[1]-'0' == 1) && (packetBuffer[2]-'0' == 1) && (packetBuffer[3]-'0' == 1) ){
    analogWrite(speed1,0);
    analogWrite(speed2,0);
    analogWrite(speed3,0);
    analogWrite(speed4,0);
  }

}


void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
