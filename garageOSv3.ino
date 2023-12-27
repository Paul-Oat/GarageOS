#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#define STATIONID "WIFI SSID"
#define PASSWORD  "WIFI Password"


const int RDO = 5;
const int RDC = 0;


ESP8266WebServer server(80);

void setup(void) {
pinMode(LED_BUILTIN, OUTPUT); 
pinMode(RDO, OUTPUT);
pinMode(RDC, OUTPUT); 
  Serial.begin(19200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STATIONID, PASSWORD);
  
  Serial.printf("\nStarting wifi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  if (MDNS.begin("GarageRight")) {  //Start mDNS with name GarageRight
      Serial.println("MDNS started");
    }
  

  Serial.printf("\nConnected to %s\nIP address: ",STATIONID);
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/open", HTTP_POST, handleOpen);
   server.on("/close", HTTP_POST, handleClose);  
  server.onNotFound(handleNotFound);   

  server.begin();                           
  Serial.println("HTTP server started");
  digitalWrite(LED_BUILTIN, HIGH); 
  digitalWrite(RDO, HIGH); 
  digitalWrite(RDC, HIGH); 
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
 server.send(200, "text/html", " <head><title>Garage</title> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\"><script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js\"></script></head><body><div class=\"d-grid gap-2 h-100\"> <h2 style=\"text-align:center\">Garage Right</h2><form action=\"/open\" method=\"POST\"><input class=\"form-control form-control-lg btn-success h-50\" type=\"submit\" value=\"Open\"></form> <form action=\"/close\" method=\"POST\"><input class=\"form-control form-control-lg btn-danger h-50 \" type=\"submit\" value=\"Close\"></form></div></body></html>");
}

void handleClose() {    
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
  digitalWrite(RDC, LOW); // Close the Garage RHS door
  delay(500); // Wait for half a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  digitalWrite(RDC, HIGH); // Close the Garage RHS door // Turn the LED on
  delay(2000); // Wait for 5 seconds
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 t
}

void handleOpen() {    
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED on 
  digitalWrite(RDO, LOW); // Open the Garage RHS door
  delay(500); // Wait for half a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  digitalWrite(RDO, HIGH); // Open the Garage RHS door
   delay(2000); // Wait for 2 seconds
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
