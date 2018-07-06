#include <ESP8266WiFi.h>          // Load Wi-Fi Lib
#include <ESP8266WebServer.h>     // Load Web Server Lib
#include <Adafruit_NeoPixel.h>    // Load NeoPixel Lib

// NEO Pixel Configuration
#define PIXEL_PIN 14
#define NUM_PIXELS 3

// WiFi Configuration
const char* ssid     = "Newtork SSID";
const char* password = "Network Password";

// Animation Configuration
#define CHASE_SPEED 100
#define CHASE_COLOR strip.Color(255, 0, 0)
#define CHASE_HOLD  5000

#define PULSE_COLOR strip.Color(255, 0, 0)
#define PULSE_COUNT 10

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Set web server port number to 80
ESP8266WebServer server(80);

// Variable to store the HTTP request
String header;
short animation_mode = 0;  // 0 = Off; 1 = Heart Beat; 2 = Chase

void setup() {
  Serial.begin(115200);
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

  // Set up Request Handeler
  server.on("/", handleBody);

  server.begin();

  // Init NeoPixel Strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  server.handleClient(); //Handling of incoming requests

  switch (animation_mode) {
    case 0:
      // Off
      allOff();
      break;

    case 1:
      // Heart Beat
      Serial.println("Heart Beat Requested.");

      pulse(PULSE_COLOR, PULSE_COUNT);

      animation_mode = 0;
      break;

    case 2:
      // Chase
      Serial.println("Chase Requested.");

      colorWipe(CHASE_COLOR, CHASE_SPEED); // Turn On
      delay(CHASE_HOLD);
      colorWipe(strip.Color(0, 0, 0), CHASE_SPEED);

      animation_mode = 0;
      break;
  }
}

// === Endpoint Handeler ===
void handleBody() { //Handler for the body path

  if (server.hasArg("plain") == false) { //Check if body received

    server.send(406, "text/plain", "No Animation Request Recieved.");
    return;

  }

  String message = "Body received:\n";
  message += server.arg("plain");
  message += "\n";

  server.send(204);
  Serial.println(message);

  if (server.arg("plain").equalsIgnoreCase("pulse")) {
    animation_mode = 1;
  } else if (server.arg("plain").equalsIgnoreCase("chase")) {
    animation_mode = 2;
  } else if (server.arg("plain").equalsIgnoreCase("off")) {
    animation_mode = 0;
  }
}

// === Annimations ===

// Turn off all Pixels
void allOff() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  // From Adafruit's NeoPixel Example Code
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void pulse(uint32_t c, int count) {
  // Based off of the Heartbet function from https://github.com/zatamite/Neopixel-heartbeat

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }

  strip.show();
  delay (20);

  for (int i = 0; i < count; i++) {
    int x = 3;
    for (int ii = 1 ; ii < 252 ; ii = ii + x) {
      strip.setBrightness(ii);
      strip.show();
      delay(5);
    }

    x = 3;
    for (int ii = 252 ; ii > 3 ; ii = ii - x) {
      strip.setBrightness(ii);
      strip.show();
      delay(3);
    }
    delay(10);

    x = 6;
    for (int ii = 1 ; ii < 255 ; ii = ii + x) {
      strip.setBrightness(ii);
      strip.show();
      delay(2);
    }
    x = 6;
    for (int ii = 255 ; ii > 1 ; ii = ii - x) {
      strip.setBrightness(ii);
      strip.show();
      delay(3);
    }
    delay (50);
  }

  strip.setBrightness(255); // Reset Brightness for other Animations
}

