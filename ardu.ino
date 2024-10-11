#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>

// Configuration WiFi
const char* ssid = "PoleDeVinci_IFT";
const char* password = "*c.r4UV@VfPn_0";

// Pin pour la bande de Neopixel
#define NEOPIXEL_PIN 9
#define NUM_PIXELS 2 // Nombre de LEDs

// Pin pour le capteur de lumière ambiante
#define LDR_PIN 4 // Changez ceci si nécessaire, assurez-vous d'utiliser un pin analogique

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Serveur Web et WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int red = 0, green = 0, blue = 0;

void setColor() {
  pixels.fill(pixels.Color(red, green, blue));
  pixels.show();
}

void notifyClients() {
  ws.textAll("LED color updated");
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String msg = "";
    for(size_t i = 0; i < len; i++) {
      msg += (char) data[i];
    }

    // Extraire les valeurs RGB envoyées sous forme de "R,G,B"
    int separator1 = msg.indexOf(',');
    int separator2 = msg.lastIndexOf(',');

    if (separator1 != -1 && separator2 != -1 && separator1 != separator2) {
      red = msg.substring(0, separator1).toInt();
      green = msg.substring(separator1 + 1, separator2).toInt();
      blue = msg.substring(separator2 + 1).toInt();

      // Appliquer la couleur à la LED Neopixel
      setColor();
      notifyClients(); // Notifier les clients WebSocket que la couleur a changé
    }
  }
}

void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if(type == WS_EVT_DATA){
    handleWebSocketMessage(arg, data, len);
  }
}

void setup() {
  // Initialisation série
  Serial.begin(115200);

  // Initialisation WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion...");
  }
  
  // Afficher l'IP de l'ESP32 une fois connecté
  Serial.println("Connecté à WiFi");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());

  // Initialisation NeoPixel
  pixels.begin();
  pixels.clear();
  pixels.show();

  // WebSocket configuration
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Démarrer le serveur
  server.begin();
}

void loop() {
  // Lire la valeur du capteur de lumière
  int ldrValue = analogRead(LDR_PIN); // Lecture de la valeur analogique du capteur
  String ldrMessage = "LDR:" + String(ldrValue);
  
  // Envoyer la valeur du capteur aux clients WebSocket
  ws.textAll(ldrMessage);

  delay(500); // Envoi des données toutes les 2 secondes
}
