/**
 * Create HTTP webserver using ESP32 board
 * Toggle GPIO and see it with LED on breadboard. 
 * ESP resopnds with current switch / button status.
 * 
 * Wiring:
 * Connect using two boards (side chopped off if required)
 *   22 - resistor - LED }
 *   23 - resistor - LED }
 *   GND                 } <- connected in parallel
 * 
 * Flow:
 *   1) Client HTTP GET with url to ESP32
 *   2) ESP32 server sees GET and runs code.
 *      - Toggle GPIO, read ADC, prepare response
 *   3) ESP32 server sends HTTP to client
 *   4) Client recieves, parses and show webpage
 * 
 * !! ESP32 creates own wireless network
 * !! Default IP: 192.168.4.1
 */ 
#include <WiFi.h>

 
WiFiServer server(80);
 

void setup() {
  Serial.begin(115200);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(4, INPUT_PULLUP);
 
  WiFi.softAP("ESP32 Demo", "demodemo");                  // SSID, password
  server.begin();
}

 
void loop() {
  WiFiClient client = server.available();                 // Listen for incoming clients
 
  if (client) {                                           // If a client connects
    String header(""), response("");
    while (client.connected()) {          
      if (client.available()) {                           // Read the client's request until we run out of bytes
        char c = client.read();
        header += c;
        Serial.print(c);
      } else {                                            // Once done reading the client request, now we process & send back the data
                                                          // But first, we turn the GPIOs on or off as applicable
        if (header.indexOf("GET /22/on") >= 0) {
          digitalWrite(22, HIGH);
        } else if (header.indexOf("GET /22/off") >= 0) {
          digitalWrite(22, LOW);
        } else if (header.indexOf("GET /23/on") >= 0) {
          digitalWrite(23, HIGH);
        } else if (header.indexOf("GET /23/off") >= 0) {
          digitalWrite(23, LOW);
        }
                                                          
        response += "HTTP/1.1 200 OK\n";                  // HTTP header - we are serving HTML text
        response += "Content-type:text/html\n";
        response += "Connection: close\n";                // Ask browser to automatically close the connection after
        response += "\n";
 
 
 
                                                          // HTML miscellaneous bits
        response += "<!DOCTYPE html><html>\n";
        // Nice big heading
        response += "<body><h1>ESP32 Web Server</h1>\n";
        // Normal text goes in a <p>
        response += "<p>";
 
        // Display current state, and ON/OFF buttons for GPIOs
        response += "GPIO 22 - LED ";
        response += (digitalRead(22) ? "ON" : "OFF");
        response += "<br>\n";
        response += "<a href=\"/22/on\" ><button>ON </button></a><br>\n";
        response += "<a href=\"/22/off\"><button>OFF</button></a><br>\n";
 
        response += "GPIO 23 - LED ";
        response += (digitalRead(23) ? "ON" : "OFF");
        response += "<br>\n";
        response += "<a href=\"/23/on\" ><button>ON </button></a><br>\n";
        response += "<a href=\"/23/off\"><button>OFF</button></a><br>\n";
 
        response += "GPIO 4 - The switch is: ";
        response += digitalRead(4) ? "not " : "";
        response += "pressed<br>\n";
 
        response += "</p></body></html>\n";
        
        client.println(response);
        // Break out of the while loop, as we are done.
        break;
 
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}