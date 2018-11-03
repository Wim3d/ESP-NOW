/*
   Remote for Sonoff via ESP-NOW and MQTT as backup
   Status LED is connected 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <credentials.h> //credentials for WiFi, IFTTT, MQTT
extern "C" {
#include <espnow.h>
}
#define WIFIMAXTIME 8000
#define CALLBACKMAXTIME 2000
#define LEDPIN 2         // Status LED at GPIO2
#define SERIALDEBUG 1    // Serial is active
#define VOLT_LIMIT 3.20

// MAC address of the ESP with which it is paired (slave), global defined since used in setup and loop
uint8_t mac_addr[6] = {0x6A, 0xC6, 0x3A, 0xC4, 0xA7, 0x5C}; // MAC address of acces point

ADC_MODE(ADC_VCC); //vcc read-mode

uint16_t time1, time2;
WiFiClient espClient;
PubSubClient client(espClient);

const char* topic = "sonoff2/Relay_sw";   // MQTT topic to publish to
const char* payload = "2";                // MQTT payload 

uint8_t result = 1;

void setup()
{
  WiFi.mode(WIFI_STA);
  if (SERIALDEBUG)  // initialize the serial port if needed for debug
  {
    Serial.begin(115200);
    Serial.println();
    Serial.println();
  }
  pinMode(LEDPIN, OUTPUT);     // Initialize the LED pin as an output
  digitalWrite(LEDPIN, HIGH);

  // Initialize the ESP-NOW protocol, if it fails, the module is restarted
  if (esp_now_init() != 0) {
    //Serial.println("*** ESP_Now init failed");
    ESP.restart();
    delay(1);
  }
  // display  MAC DATA of this of ESP on serial for debug
  /*
    Serial.print("Access Point MAC of this ESP:");
    Serial.println(WiFi.softAPmacAddress());
    Serial.print("Station MAC of this ESP:");
    Serial.println(WiFi.macAddress());
  */
  // *** DECLARATION OF THE ROLE OF THE ESP DEVICE IN THE COMMUNICATION *** //
  // 0 = LOOSE, 1 = MASTER, 2 = SLAVE and 3 = MASTER + SLAVE
  esp_now_set_self_role(1);   // sender

  // *** PAIRING WITH THE SLAVE *** //
  uint8_t role = 2;   // role of receiver = slave
  uint8_t channel = 1;  // WiFi channel of receiver access point
  esp_now_add_peer(mac_addr, role, channel, NULL, 0);   // NULL means there is no key, length of key is 0

  // set up the call-back function for the confirmation of the sent data. This is executed is ESP-NOW is used further on in the program
  esp_now_register_send_cb([](uint8_t *mac,  uint8_t result2) {
    char MACslave[6];
    // in this call back function the result is stored in the "result" variable, which is important in the program
    result = result2;
    sprintf(MACslave, "% 02X:% 02X:% 02X:% 02X:% 02X:% 02X", mac [0], mac [1], mac [2], mac [3], mac [4], mac [5]);

    // display result on serial if serial is initialized
    Serial.print("Data sent to ESP MAC:"); Serial.print(MACslave);
    Serial.print(". Reception(0 = 0K - 1 = ERROR):"); Serial.println(result);
  });
  time1 = millis();

  float Voltage = ESP.getVcc() / (float)1023; // * (float)1.07;
  if (Voltage < VOLT_LIMIT)      // if voltage of battery gets to low, the LED wil blink fast.
  {
    digitalWrite(LEDPIN, LOW);
    for (int i = 0; i < 10; i++)
    {
      delay(250);
      digitalWrite(LEDPIN, HIGH);
      delay(250);
      digitalWrite(LEDPIN, LOW);
    }
  }
}

void loop()
{
  // prepare the data to send
  char DATA[40];
  strcat(DATA, topic);   // Copy the topic to the array.
  strcat(DATA, "&");   // Copy "&" symbol to the array.
  strcat(DATA, payload);   // Copy payload to the array.
  //Serial.print("DATA: "); Serial.print(DATA);

  uint8_t data[sizeof(DATA)];
  memcpy(data, &DATA, sizeof(DATA));
  uint8_t   len = sizeof(data);

  //Serial.print("char data: "); Serial.println("%s", data);
  //send the data
  esp_now_send(mac_addr, data, len);

  yield();
  delay(200); //wait for the confirmation of the slave and the execution of the call back function
  if (result == 0)
  {
    digitalWrite(LEDPIN, LOW);
    //Serial.println("message deliverd, going to sleep");
    delay(100);
    ESP.deepSleep(0);
    delay(100);
  }
  if (millis() - time1 > CALLBACKMAXTIME)   // no message received from ESP-NOW slave, blink and try to connect via MQTT
  // this part is only for the MQTT connection as back-up for the failed ESP-NOW communication
  {
    digitalWrite(LEDPIN, LOW);
    for (int i = 0; i < 3; i++)
    {

      delay(100);
      digitalWrite(LEDPIN, HIGH);
      delay(100);
      digitalWrite(LEDPIN, LOW);
    }
    Serial.println("message not confirmed, trying via MQTT");
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    connectMQTT();
    client.publish(topic, payload);

    delay(500);
    client.disconnect();
    espClient.stop();
    Serial.println("Now going to sleep");
    digitalWrite(LEDPIN, LOW);
    delay(500);
    ESP.deepSleep(0);             //  go to sleep
    delay(100);
  }
}
// setup WiFi for the MQTT connection as back-up for the failed ESP-NOW communication
void setup_wifi() {
  time1 = millis();
  delay(10);
  WiFi.mode(WIFI_STA);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(mySSID);

  WiFi.begin(mySSID, myPASSWORD);

  while (WiFi.status() != WL_CONNECTED && time2 < WIFIMAXTIME) {
    delay(250);
    Serial.print(".");
    time2 = millis() - time1;
  }
  if (WiFi.status() != WL_CONNECTED)    // wifi connection not succeeded, blink once and go to sleep
  {
    espClient.stop();
    digitalWrite(LEDPIN, LOW);
    delay(500);
    digitalWrite(LEDPIN, HIGH);
    delay(500);
    digitalWrite(LEDPIN, LOW);
    delay(10);
    ESP.deepSleep(0);             //  go to sleep
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// connect to MQTT server,  MQTT connection as back-up for the failed ESP-NOW communication
boolean connectMQTT()
{

  Serial.println("Attempting MQTT connection...");
  if (client.connect("ESP-NOW-Remote")) {
    Serial.println("connected");
    // ... and resubscribe if needed
  }
  Serial.println(client.connected()); // prints "1" if connected
  return client.connected();
}
