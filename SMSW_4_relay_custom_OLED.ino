/*********
  esp8266 webserver control module 4 relays with OLED display
  thelastvice custom
  base project details at http://randomnerdtutorials.com  
*********/
// Load OLED Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);
unsigned long delayTime;

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "G-Home";
const char* password = "uVhxyCnJn3HNIl3pR2cDEoNg";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output1State = "off";
String output2State = "off";
String output3State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output1 = 14;
const int output2 = 12;
const int output3 = 13;
const int output4 = 15;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(output3, LOW);
  digitalWrite(output4, LOW);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
   // init done
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextSize(1.75);
  display.setTextColor(WHITE);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /14/on") >= 0) {
              Serial.println("GPIO 14 on");
              output1State = "off";
              digitalWrite(output1, HIGH);
            } else if (header.indexOf("GET /14/off") >= 0) {
              Serial.println("GPIO 14 off");
              output1State = "on";
              digitalWrite(output1, LOW);
            } else if (header.indexOf("GET /12/on") >= 0) {
              Serial.println("GPIO 12 on");
              output2State = "off";
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /12/off") >= 0) {
              Serial.println("GPIO 12 off");
              output2State = "on";
              digitalWrite(output2, LOW);
            } else if (header.indexOf("GET /13/on") >= 0) {
              Serial.println("GPIO 13 on");
              output3State = "off";
              digitalWrite(output3, HIGH);
            } else if (header.indexOf("GET /13/off") >= 0) {
              Serial.println("GPIO 13 off");
              output3State = "on";
              digitalWrite(output3, LOW);
            } else if (header.indexOf("GET /15/on") >= 0) {
              Serial.println("GPIO 15 on");
              output4State = "off";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /15/off") >= 0) {
              Serial.println("GPIO 15 off");
              output4State = "on";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #177009; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #c93426;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Smart SWITCH 4</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO D5  
            client.println("<p>1 - Stato " + output1State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output1State=="on") {
              client.println("<p><a href=\"/14/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/14/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO D6  
            client.println("<p>2 - Stato " + output2State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output2State=="on") {
              client.println("<p><a href=\"/12/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

           // Display current state, and ON/OFF buttons for GPIO D7  
            client.println("<p>3 - Stato " + output3State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output3State=="on") {
              client.println("<p><a href=\"/13/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/13/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO D8  
            client.println("<p>4 - Stato " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="on") {
              client.println("<p><a href=\"/15/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/15/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

  // Composizione Display OLED
  display.setCursor(0,0);
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(22,0);
  display.print("Smart SWITCH 4");
  
 // SETUP RELAY 1 on OLED
  display.setTextSize(1);
  display.setCursor(5,16);
  display.print("SWITCH 1");
  display.setTextSize(1);
  display.setCursor(20,26);
  display.print(output1State);
  
 // SETUP RELAY 2 on OLED
  display.setTextSize(1);
  display.setCursor(70, 16);
  display.print("SWITCH 2");
  display.setTextSize(1);
  display.setCursor(84, 26);
  display.print(output2State);

  // SETUP RELAY 3 on OLED
  display.setTextSize(1);
  display.setCursor(5, 45);
  display.print("SWITCH 3");
  display.setTextSize(1);
  display.setCursor(20, 55);
  display.print(output3State);

  // SETUP RELAY 4 on OLED
  display.setTextSize(1);
  display.setCursor(70, 45);
  display.print("SWITCH 4");
  display.setTextSize(1);
  display.setCursor(84, 55);
  display.print(output4State);
  
  
  display.display();
  delay(1000);
  
}
