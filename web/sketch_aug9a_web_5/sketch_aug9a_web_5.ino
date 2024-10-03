//http://140.127.196.233/api/sensor
//http://140.127.196.233:7799/
#include <WiFi.h>
#include <esp_http_server.h>
#include <cJSON.h>
#include <PubSubClient.h>
#include <string.h>
#include <ArduinoJson.h>

IPAddress staticIP(192, 168, 0, 101); // Set your desired static IP address
IPAddress gateway(192, 168, 0, 1);    // Set your gateway IP address
IPAddress subnet(255, 255, 255, 0);   // Set your subnet mask

const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttUsername = "emqx";
const char* mqttPassword = "public";
WiFiClient espClient;
PubSubClient client(espClient);

//const char* ssid = "USER 4870";
//const char* password = "algorithm3704d";
//const char* ssid = "UniFi 3F";
//const char* password = "d721221s";
//const char* ssid = "GMFM_WEN_IPhone15_PRO";
//const char* password = "0908153519";
const char* ssid = "ISU_EE_CS_2.4G";
const char* password = "00000000";
httpd_handle_t server = NULL;

String jsonString1;
String jsonString2;
String jsonString3;
String jsonString4;
String jsonString5;
// 假設你已經在其他地方宣告了這些變數
float receivedHumidity1;
float receivedHumidity2;
float receivedHumidity3;
float receivedHumidity4;
float receivedHumidity5;

// 假設你已經在其他地方定義了這些函式
float generateRandomTemperature();
float generateRandomHumidity();
float generateRandomPressure();
float receivedTemperature;


const char* webpage = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>義守大學尿布顯示系統</title>
    <style>
        /*  CSS設定  */
      body {
        font-family: Arial, sans-serif;
        background-color: #fad482;
        margin: 0;
        padding: 0;
      }

      #sensor-container {
        display: flex;
        flex-direction: column;
        flex-wrap: nowrap; /* 讓子元素自動換行 */
        max-width: 600px; /* 設定容器的最大寬度 */
        max-height: 300px; /* 設定容器的最大高度 */
        
        }

      #sensor1, #sensor2, #sensor3, #sensor4, #sensor5 {
            background-color: #ffffff;
            padding: 20px 60px 20px 60px;/* 內邊距為上 ，右 ，下 ，左  */
            margin: 5px;/* 外邊距為  像素 */
            border: 5px solid #ccc;/* 邊框為  像素寬的灰色實線 */
            border-radius: 5px;/* 圓角半徑為  像素 */
            
        }
    </style>
</head>
<body>
    <!-- 創建一個顯示溫度的元素 -->
    
    <p style="color:red;font-size:50px">義守大學尿布警報系統</p>
    <p style="color:black;font-size:30px">尿布濕度狀態</p>
    <div id="sensor-container">
    <div id="sensor1"></div>
    <div id="sensor2"></div>
    <div id="sensor3"></div>
    <div id="sensor4"></div>
    <div id="sensor5"></div>
    </div>

    <script>
        // 使用原生 JavaScript 定期更新溫度數據
        function refreshHumidity() {
            fetch('http://140.127.196.233/api/sensor') // 替換為你的 API 路由
                .then(response => response.json())
                .then(data => {
                    document.getElementById('sensor1').textContent = `當前濕度sensor1: ${data.humidity1}%`;
                    document.getElementById('sensor2').textContent = `當前濕度sensor2: ${data.humidity2}%`;
                    document.getElementById('sensor3').textContent = `當前濕度sensor3: ${data.humidity3}%`;
                    document.getElementById('sensor4').textContent = `當前濕度sensor4: ${data.humidity4}%`;
                    document.getElementById('sensor5').textContent = `當前濕度sensor5: ${data.humidity5}%`;
                    console.log('Humidity data:', data);
                })
                .catch(error => {
                    console.log('當前濕度：', data.humidity);console.log('當前濕度：', data.humidity);console.log('當前濕度：', data.humidity);console.log('當前濕度：', data.humidity);
                });
        }

        // 頁面加載時刷新一次
        refreshHumidity();

        // 每隔 1 秒刷新一次
        setInterval(refreshHumidity, 1000);
    </script>
</body>
</html>
)HTML";


void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.print("接收到主題 ");
    Serial.print(topic);
    Serial.print(" 的訊息：");
    Serial.println(message);

    // Handle different topics and update data accordingly
    if (strcmp(topic, "isu/esp32s_soilmoisture_1") == 0) {
        jsonString1 = message;
        delay(2000);
    } else if (strcmp(topic, "isu/esp32s_soilmoisture_2") == 0) {
        jsonString2 = message;
        delay(2000);
    } else if (strcmp(topic, "isu/esp32s_soilmoisture_3") == 0) {
        jsonString3 = message;
        delay(2000);
    }
    // Other topic handling (if needed)
}



// 設置 CORS 標頭
void set_cors_headers(httpd_req_t *req) {
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
}

// 處理 HTTP 請求以獲取多個溫度值
esp_err_t handle_get_humiditys(httpd_req_t *req) {
    set_cors_headers(req);

    cJSON *jsonObject = cJSON_CreateObject();
    cJSON_AddNumberToObject(jsonObject, "humidity1", atof(jsonString1.c_str()));
    cJSON_AddNumberToObject(jsonObject, "humidity2", atof(jsonString2.c_str()));
    cJSON_AddNumberToObject(jsonObject, "humidity3", atof(jsonString3.c_str()));
    cJSON_AddNumberToObject(jsonObject, "humidity4", atof(jsonString4.c_str()));
    cJSON_AddNumberToObject(jsonObject, "humidity5", atof(jsonString5.c_str()));

    char *json_str = cJSON_Print(jsonObject);
    cJSON_Delete(jsonObject);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);

    return ESP_OK;
}

// 處理 HTTP 請求以獲取單個溫度值
esp_err_t handle_get_humidity(httpd_req_t *req) {
    set_cors_headers(req);

    // Handle MQTT data or other logic here
    // ...

    // Example: Create a JSON object with received data
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON_AddNumberToObject(jsonObject, "humidity1", atof(jsonString1.c_str()));
    // ...

    char *json_str = cJSON_Print(jsonObject);
    cJSON_Delete(jsonObject);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);

    return ESP_OK;
}

// 處理 HTTP 請求以獲取感測器數據（濕度1、濕度2、濕度3）測試多元件資料溫度濕度壓力
esp_err_t handle_get_sensor_data(httpd_req_t *req) {
    set_cors_headers(req);

    // Create a JSON object
    StaticJsonDocument<256> jsonObject;

    // Add received data to the JSON object
    jsonObject["humidity1"] = atof(jsonString1.c_str());
    jsonObject["humidity2"] = atof(jsonString2.c_str());
    jsonObject["humidity3"] = atof(jsonString3.c_str());
    jsonObject["humidity4"] = atof(jsonString4.c_str());
    jsonObject["humidity5"] = atof(jsonString5.c_str());

    // Serialize the JSON object to a string
    String json_str;
    serializeJson(jsonObject, json_str);

    // Send the JSON response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str.c_str(), json_str.length());

    return ESP_OK;
}

//add web set
esp_err_t handle_main_page(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, webpage, strlen(webpage));
    return ESP_OK;
}


void start_http_server() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    

    httpd_uri_t uri_get_humiditys = {
        .uri = "/api/humiditys",
        .method = HTTP_GET,
        .handler = handle_get_humiditys,
        .user_ctx = NULL
    };

    httpd_uri_t uri_get_humidity = {
        .uri = "/api/humidity",
        .method = HTTP_GET,
        .handler = handle_get_humidity,
        .user_ctx = NULL
    };

    httpd_uri_t uri_get_sensor_data = {
        .uri = "/api/sensor",
        .method = HTTP_GET,
        .handler = handle_get_sensor_data,
        .user_ctx = NULL
    };
    //main
    
    httpd_uri_t uri_main_page = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = handle_main_page,
        .user_ctx = NULL
    };


    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get_humiditys);
        httpd_register_uri_handler(server, &uri_get_humidity);
        httpd_register_uri_handler(server, &uri_get_sensor_data);
        httpd_register_uri_handler(server, &uri_main_page);
    }
}
//ADD




void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    client.subscribe("isu/esp32s_soilmoisture_1");
    client.subscribe("isu/esp32s_soilmoisture_2");
    client.subscribe("isu/esp32s_soilmoisture_3");

    start_http_server();
}

void loop() {
    // 可以在这里放置其他需要循环执行的代码
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}


void reconnect() {
    // 重新连接到 MQTT 服务器的逻辑
    // 设置连接参数，例如客户端 ID、用户名、密码等
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    if (client.connect("esp32-client")) {
        // 订阅主题
        client.subscribe("isu/esp32s_soilmoisture_1");
        client.subscribe("isu/esp32s_soilmoisture_2");
        client.subscribe("isu/esp32s_soilmoisture_3");
        Serial.println("已成功连接到 MQTT 服务器！");
    }
}

