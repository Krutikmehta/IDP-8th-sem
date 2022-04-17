/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#ifndef STASSID
#define STASSID "Nord" //"D-Link_DIR-816"
#define STAPSK "my phone" //"neildhami"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "34.125.211.48"; //"192.168.0.115";
const uint16_t port = 3389;
 
ESP8266WiFiMulti WiFiMulti;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);  //Rx Tx c2000

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
//    delay(5000);
    return;
  }
  else{
    Serial.print("connected to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
  }

  delay(500);
}


void loop() {
 

  // Use WiFiClient class to create TCP connections
//  WiFiClient client;
//
//  if (!client.connect(host, port)) {
//    Serial.println("connection failed");
//    Serial.println("wait 5 sec...");
//    delay(5000);
//    return;
//  }

  // This will send the request to the server
//  client.println("hello from ESP8266");
//
//  //read back one line from server
//  Serial.println("receiving from remote server");

    String to_mc = client.readString();
    
 
    if(to_mc!=""){
      Serial.println(to_mc);
      Serial.println("here");
      int len = to_mc.length();
      for(int i=0;i<100-len-2;i++) {
        to_mc += ' ';
      }
      to_mc += "\r\n";
      const char *cstr = to_mc.c_str();
      Serial1.write(cstr);
    }

    String from_mc = to_mc; //Serial1.read();

    client.write(from_mc.c_str());

}
