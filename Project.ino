#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <string.h>

const int buzzer = 16;
const int echoPin = 13;
const int trigPin = 15;
const int irPin = 12;
int level = 0, pos = 0; long distance = 0;
char str1[100],str2[100];

Servo myservo;

ESP8266WebServer server(80);


void setup() {

  // Setting up Ultrasonic Sensor
  pinMode(echoPin,INPUT);
  pinMode(trigPin,OUTPUT);

  // Setting up IR sensor
  pinMode(irPin,INPUT);

  // Setting up Servo Motor
  myservo.attach(14);
  myservo.write(90);

  // Begin Serial Monitor
  Serial.begin(9600);
  delay(1000);

  noTone(buzzer);
  
  // Connecting to local Network
  WiFi.disconnect();
  Serial.println("Starting..."); 
  WiFi.begin("Inside bsnl","corei3inside");
  Serial.print("Connecting");
  while(!(WiFi.status() == WL_CONNECTED)){
    delay(500);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("Connected Successfully!");

  // IP of Local Network
  Serial.println("Local IP:");
  Serial.println(WiFi.localIP()); 

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started...");

}

void handle_OnConnect(){
  
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  int time = pulseIn(echoPin,HIGH);
  distance = (time*0.034)/2;
  
  if (distance>12){
    level=0;}
  else if (distance<=12 and distance>11){
    level=1;}
  else if (distance<=11 and distance>10){
    level=2;}
  else if (distance<=10 and distance>9){
    level=3;}
  else if (distance<=9 and distance>8){
    level=4;}
  else if (distance<=8 and distance>7){
    level=5;}
  else if (distance<=7 and distance>6){
    level=6;}
  else if (distance<=6 and distance>5){
    level=7;}
  else if (distance<=5 and distance>4){
    level=8;}
  server.send(200, "text/html", SendHTML(level));
  Serial.print("Waterlevel: ");
  Serial.print(level);
  Serial.println(" cm");
  delay(10);

  bool ir = digitalRead(irPin);
  
  Serial.print("Status of ir: ");
  Serial.println(ir);
  if (ir==1 and distance<5){
    myservo.write(180);
    delay(500); 
    strcpy(str1,"Dam almost full!");
    strcpy(str2,"Flood gates clear of any obstacles");     
    Serial.println(str1);
    tone(buzzer,250);
    delay(500);
    noTone(buzzer);
    }
  else if (ir==1 and level==7){
    strcpy(str1,"Dam levels high...Take necessay precautions!");
    strcpy(str2,"Flood gates clear of any obstacles"); 
    myservo.write(135);
    delay(500);
  }
  else{
    strcpy(str1,"Water levels low and safe!");
    strcpy(str2,"Flood gates clear of any obstacles"); 
    myservo.write(90);
    delay(500);
  }
  if(ir==0){
    strcpy(str2,"Objects present in front of the flood gates!"); 
  }
  Serial.print("Position of flood gates: ");
  Serial.println(myservo.read());
  
}


void loop() {

  server.handleClient();
  delay(100);

}

void handle_NotFound(){
  
  server.send(404, "text/plain", "Not found");
  
}


String SendHTML(int a){
  
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<meta http-equiv='refresh' content='2'/>";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Water Level</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Water Level Detection and Alarm</h1>\n";
  ptr +="<p>Water Level: ";
  ptr +=(int)a;
  ptr +=" meters<br>";
  ptr +=str1;
  ptr +="<br>\n";
  ptr +=str2;
  ptr +="<br></p>\n";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  
}
