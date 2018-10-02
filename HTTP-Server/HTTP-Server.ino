/* HTTP Web Server + WiFi Station example 

   Ranjit Bhinge,
   Device Interactions

   www.device-interactions.com
   www.github.com/deviceinteractions
   
   http://device-interactions.com/blog
   http://shop.device-interactions.com

   This example sets up an ESP8266 as an HTTP Server and connects to an existing WiFi Network
   The Client is expected to connect to this same WiFi Network and send commands using HTTP GET requests

   The Messages will be visible in the Serial monitor
  
*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define WIFI_SSID "YOUR-WIFI-NAME"          // change this to your WiFi name
#define WIFI_PASS "YOUR-PASSWORD"      // change this to your WiFi password
#define Light1 D5                           // Defining Light 1 Pin as D5
#define Light2 D6                           // Defining Light 1 Pin as D6

String msgBody = "";
String msgLightId = "";
String msgStatus = "";

ESP8266WebServer server(80);        // Use the default port 80 for HTTP comms



void setup() {

  pinMode(D4, OUTPUT);    // Internal LED for WiFi Status
  pinMode(D5, OUTPUT);    // Pin to control Light #1
  pinMode(D6, OUTPUT);    // Pin to control Light #2
  Serial.begin(9600);
  Serial.println();

  //setup the custom IP address
  WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("\nConnecting to your WiFi Network");
    
    // WiFi connection
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(D4, LOW);
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();

 
  server.on ( "/", handleRoot );
  server.on ( "/message", handleMessage );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(D4, HIGH);

  Serial.println("Use the following commands from your Browser:");
  Serial.println();
  for(int i=0; i<2; i++){
    for(int j=0; j<2; j++){
        Serial.println();
        Serial.print(i+j+1);
        Serial.print(") To switch ");
        if(j)Serial.print("ON");
        else Serial.print("OFF");
        Serial.print(" Light ");
        Serial.print(i+1);
        Serial.println(":");
        Serial.print(WiFi.localIP());
        Serial.print("/message?light=");
        Serial.print(i+1);
        Serial.print("&state=");
        Serial.print(j);
        Serial.println();
    }
    Serial.println();
  }


}

void loop() {
  // Constantly listen for Client requests
  server.handleClient();
}


// handle requests at the root endpoint (http://[SERVER_IP_ADDRESS]/)
void handleRoot() {
  Serial.println("Root page accessed by a client!");

// Respond with a mini HTML page
  server.send ( 200, "text/html", "<h1><b>Hi</b> There!</h1>");
}

// handle requests at the message endpoint (http://[SERVER_IP_ADDRESS]/message)
void handleMessage(){
  
  // Check for valid request with a message (in the expected format)
  if(server.hasArg("light") && server.hasArg("state")){
    digitalWrite(D5, HIGH);
    msgBody="";
    msgLightId = server.arg("light");
    msgStatus = server.arg("state");
    Serial.println("Message received from Client");
    Serial.print("\nLight ID :");
    Serial.println(msgLightId);
    Serial.print("\nLight Status :");
    Serial.println(msgStatus);

    if(msgLightId == "1"){
      if(msgStatus == "1"){
        digitalWrite(Light2, HIGH);
        Serial.println("Switched ON Light 1");
      }
      else {
        digitalWrite(Light2, LOW);
        Serial.println("Switched OFF Light 1");
      } 
    }

    else if(msgLightId == "2"){
      if(msgStatus == "1"){
        digitalWrite(Light1, HIGH);
        Serial.println("Switched ON Light 2");
      }
      else {
        digitalWrite(Light1, LOW);
        Serial.println("Switched OFF Light 2");
      } 
    }

    // Generate and send back an acknowledgement response
    msgBody = "Hi, this is the Server. I executed your command to switch " + msgLightId;
    msgBody += "to state " + msgStatus;
    server.send ( 200, "text/plain", msgBody);
    delay(500);
    digitalWrite(D5, LOW);
  }
  // handle messages with invalid bodies
  else {
    Serial.println("Invalid message received");
    server.send ( 200, "text/plain", "Recevied a message without a body!");
  }

  Serial.println("-------------------------------------");
}

// handle invalid requests
void handleNotFound() {
  server.send ( 404, "text/plain", "404, No resource found");
}