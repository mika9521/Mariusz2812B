#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>




int iloscLedPodlaczonych = 30;
int szybkoscLed = 20;
bool ostatniStatus=false;
int pinWlacz = 23;
bool statusPrzycisku=false;
bool statusLed = false;
CRGB leds[9999];
AsyncWebServer server(80);
Preferences preferences;



const char* ssid = "LED";
const char* password = "12345678";

const char* PARAM_INPUT_1 = "iloscled";
const char* PARAM_INPUT_2 = "czasled";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Konfiguracja ESP32</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Ilosc LED: <input type="text" name="iloscled" value=%iloscLedWartosc%>
    <input type="submit" value="Zapisz!">
  </form><br>
  <form action="/get">
    Szybkosc LED: <input type="text" name="czasled" value=%czasLedWartosc%>
    <input type="submit" value="Zapisz!">
  </form><br>
  <h1>Restart ESP</h1>
    <button class="button" ontouchend="toggleCheckbox('restart');">Restart!</button>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
   </script>
</body></html>)rawliteral";
//wstawia do HTML wartosci aktualne
String processorHTML(const String& var){
  if(var == "iloscLedWartosc"){
    return String(iloscLedPodlaczonych);
  }
  else if(var == "czasLedWartosc"){
    return String(szybkoscLed);
  }
  return String();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void zapalaniaFade(int iloscLed, int czasMs){
  for(int i=0;i<=iloscLed;i++){
    leds[i] = CRGB::White;
    FastLED.show();
    delay(czasMs);
  }
}

void gaszenieFade(int iloscLed, int czasMs){
  for(int i=0;i<=iloscLed;i++){
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(czasMs);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("WERSJA 1.0.1");
  Serial.println("Wczytuje z pamieci eeprom");
  //EEPROM
  preferences.begin("ostatniStatus", false);
  preferences.begin("iloscLed", false);
  preferences.begin("szybkoscLed", false);
  ostatniStatus = preferences.getBool("ostatniStatus",false);
  iloscLedPodlaczonych = preferences.getInt("iloscLed",false);
  szybkoscLed = preferences.getInt("szybkoscLed",false);
  Serial.println("Wczytane parametry:");
  Serial.print("Ostatni status: ");
  Serial.println(ostatniStatus);
  Serial.print("Ilosc LED: ");
  Serial.println(iloscLedPodlaczonych);
  Serial.print("Szybkosc LED: ");
  Serial.println(szybkoscLed);


  //SEKCJA WIFI
  WiFi.softAP(ssid, password);
  Serial.print("Adres IP ESP32: ");
  IPAddress IP = WiFi.softAPIP();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processorHTML);
  });
  server.on("/restart", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
    ESP.restart();
  });
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      preferences.putInt("iloscLed", inputMessage.toInt());
      iloscLedPodlaczonych = preferences.getInt("iloscLed",false);
      inputParam = PARAM_INPUT_1;
    }
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      preferences.putInt("szybkoscLed", inputMessage.toInt());
      szybkoscLed = preferences.getInt("szybkoscLed",false);
      inputParam = PARAM_INPUT_2;
    }
    else {
      inputMessage = "Brak odebranej wartosci";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send_P(200, "text/html",index_html, processorHTML);
  });
  server.onNotFound(notFound);
  server.begin();

  //WEWYJ
  pinMode(pinWlacz, INPUT_PULLUP);

  //SEKCJA LED
  FastLED.addLeds<WS2812B, 4>(leds, iloscLedPodlaczonych);
  zapalaniaFade(iloscLedPodlaczonych,szybkoscLed);
  //gaszenieFade(iloscLedPodlaczonych,2);
  /*delay(500);
  if(ostatniStatus) {
    Serial.println("Ostatni status to wlaczone wiec wlaczam");
    zapalaniaFade(iloscLedPodlaczonych,szybkoscLed);
    statusLed = true;
  }
  else {
    Serial.println("Ostatni status to wylaczone wiec wylaczam");
    gaszenieFade(iloscLedPodlaczonych,szybkoscLed);
    statusLed = false;
  }
  */
}



void loop() {
  delay(1000);
  /*
  statusPrzycisku = digitalRead(pinWlacz);
  if(!statusLed && statusPrzycisku==LOW){
    Serial.println("Przycisk zostal wcisniety, wlaczam led");
    zapalaniaFade(iloscLedPodlaczonych,szybkoscLed);
    statusLed = true;
    preferences.putBool("ostatniStatus",statusLed);
    delay(100);
  };
  statusPrzycisku = digitalRead(pinWlacz);
  if(statusLed && statusPrzycisku==LOW){
    Serial.println("Przycisk zostal wcisniety, wylaczam led");
    gaszenieFade(iloscLedPodlaczonych,szybkoscLed);
    preferences.putBool("ostatniStatus",statusLed);
    statusLed = false;
    delay(100);
  };

  */

}