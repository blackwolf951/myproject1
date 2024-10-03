#include <ESP8266WiFi.h>//esp8266
#include <PubSubClient.h>

// WiFi 設定
//const char* ssid = "GMFM_WEN_IPhone15_PRO";
//const char* password = "0908153519";
const char* ssid = "UniFi 3F";
const char* password = "d721221s";

// MQTT 設定
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
String mqtt_ClientID = "stonez56_IOT_Station_";
const char* mqtt_username = "emqx";
const char* mqtt_password = "public";
const char* pub_soilmoisture_topic_1 = "isu/esp32s_soilmoisture_1";

 // 水位

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
char msg1[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];

// 定義土壤濕度感測器的類比輸入引腳
const int soilMoisturePin_1 = 2;//esp8266

void setup_wifi() {
  delay(10);
  // 連接到 Wi-Fi 網路
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  reconnectMQTT();
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }

  // 讀取土壤濕度值
  int soilMoistureValue_1 = analogRead(soilMoisturePin_1);


  // 將濕度值轉換為百分比（根據您的傳感器和需求進行調整）
  float moisturePercentage_1 = 100 - map(soilMoistureValue_1, 0, 1023, 0, 100);

  // 將濕度值發佈到 MQTT 主題
  client.publish(pub_soilmoisture_topic_1, String(moisturePercentage_1).c_str());


  // 顯示回饋在串口監視器中
  Serial.print("Soil Moisture1: ");
  Serial.print(moisturePercentage_1);
  Serial.println("%");
  Serial.println("-----------------------");


  delay(2000); 
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect(mqtt_ClientID.c_str())) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
