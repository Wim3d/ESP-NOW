/*
  W. Hoogervorst november 2018
  ESP - NOW communication based on by Dani No www.esploradores.com
  http://www.esploradores.com/practica-6-conexion-esp-now/
   
  SLAVE
*/

#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}

void setup() {
  WiFi.mode(WIFI_AP); // important to set the slave in AP mode, otherwise it only receives on the STA MAC and not on the AP MAC
  WiFi.softAP("ESP-now", "password", 1, 1);
  // *** INITIALIZATION OF THE SERIAL PORT *** //
  Serial.begin(115200); Serial.println();

  // *** INITIALIZATION OF THE ESP-NOW PROTOCOL *** //
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW protocol not initialized ...");
    ESP.restart();
    delay(1);
  }

  // *** MAC DATA (Access Point and Station) of ESP *** //
  Serial.print("AP MAC:"); Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC:"); Serial.println(WiFi.macAddress());

  // *** DECLARATION OF THE ROLE OF THE ESP DEVICE IN THE COMMUNICATION *** //
  // 0 = LOOSE, 1 = MASTER, 2 = SLAVE and 3 = MASTER + SLAVE
  esp_now_set_self_role(2);

  // *** RECEPTION OF THE ESP-NOW COMMUNICATION *** //
  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {

    /*
      char MACmaster[6];

      sprintf(MACmaster, "% 02X:% 02X:% 02X:% 02X:% 02X:% 02X", mac [0], mac [1], mac [2], mac [3], mac [4], mac [5]);
      Serial.print("Reception from ESP MAC:");
      Serial.println(MACmaster);
      Serial.print("length:");
      Serial.println(len);
    */
    //Serial.print("data: ");
    Serial.println((char *)data);

  });
}

void loop() {
  yield();
  delay(1);
}
