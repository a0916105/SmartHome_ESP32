#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "time.h"
#include <HTTPClient.h>
#include "Adafruit_Si7021.h"
//#include <SoftwareSerial.h>      // 引入SoftwareSerial軟體模擬串列埠函式庫
#include <ArduinoJson.h> //用來處理Json格式資料
#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "BluetoothSerial.h"

/** Unique device name */
char apName[] = "SmartHome_ESP32-xxxxxxxxxxxx";

/** Flag if stored AP credentials are available */
bool hasCredentials = false;
/** Connection status */
volatile bool isConnected = false;
/** Connection change status */
bool connStatusChanged = false;

/**
 * Create unique device name from MAC address
 **/
void createName() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  // Write unique name into apName
  sprintf(apName, "SmartHome_ESP32-%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
}

#define LED_BUILTIN 2
//電器接腳
int LED[3] = {13, 12, 14};
bool deviceState[3] = {false, false, false};
                    //{ AC  ,  DH  ,   AP }
bool isGetBT = false; //紀錄是否已使用藍牙控制電器

//感測器相關變數
Adafruit_Si7021 sensor = Adafruit_Si7021();//建立溫溼度感測器物件
float humidity, temperature, Ud;//記錄溫度、濕度、PM2.5的變數
//const int fTX = 17;   // 灰塵感測模組沒有RX腳，但需要設一個沒有用的腳位讓程式順利組譯
//const int fRX = 16;   // 灰塵感測模組 TxD
//HardwareSerial fayaSerial(2);  //使用SoftwareSerial可能會造成ISR，改用硬體的Serial2
//使用移動平均法統計PM2.5
#define timeDelay 15000
uint32_t LastTime = 0;
const int averageCount=12;
float pm2_5_data[averageCount];
int curIndex=0;
float pm2_5_total=0;
float pm2_5_average=0;

/** SSIDs of local WiFi networks */
String ssid;

/** Password for local WiFi network */
String pass;

IPAddress server_addr(192, 168, 43, 223); // IP of the MySQL *server* here
char user[] = "wst";              // MySQL user login username
char password[] = "1qaz@wsx";        // MySQL user login password

WiFiClient mySQLClient;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&mySQLClient);
bool isMySQL_Connected = false;
bool MySQL_Status_before;

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.43.223:8080/SmartHome/";

String week[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* ntpServer = "pool.ntp.org";
// 設定時區 *60分 * 60秒，例如:
// GMT +1 = 3600
// GMT +8 = 28800  台灣時區
// GMT 0 = 0
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;

unsigned long currentMillis=0; //計時

/** Callback for receiving IP address from AP */
void gotIP(system_event_id_t event) {
  isConnected = true;
  connStatusChanged = true;
}

/** Callback for connection loss */
void lostCon(system_event_id_t event) {
  isConnected = false;
  isMySQL_Connected = false;
  connStatusChanged = true;  
}

/** Callback for connection loss */
void gotCon(system_event_id_t event) {
  Serial.println("Connection established, waiting for IP");
}

/**
 * Start connection to AP
 */
void connectWiFi() {
  // Setup callback function for successful connection
  WiFi.onEvent(gotIP, SYSTEM_EVENT_STA_GOT_IP);
  // Setup callback function for lost connection
  WiFi.onEvent(lostCon, SYSTEM_EVENT_STA_DISCONNECTED);
  // Setup callback function for connection established
  WiFi.onEvent(gotCon, SYSTEM_EVENT_STA_CONNECTED);
  
  WiFi.disconnect(true);
  WiFi.enableSTA(true);
  WiFi.mode(WIFI_STA);

  Serial.println();
  Serial.print("Start connection to ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), pass.c_str());
}

// SerialBT class
BluetoothSerial SerialBT;

/** Buffer for JSON string */
// MAx size is 21 bytes for frame: 
// {"ssid":"","pass":""}
// + 2 x 53 bytes for 1 SSID and 1 password
StaticJsonDocument<128> jsonBuffer; //Buffer

/**
 * initBTSerial
 * Initialize Bluetooth Serial
 * Start BLE server and service advertising
 * @return <code>bool</code>
 *      true if success
 *      false if error occured
 */
bool initBTSerial() {
    if (!SerialBT.begin(apName)) {
      Serial.println("Failed to start BTSerial");
      return false;
    }
    Serial.println("BTSerial active. Device name: " + String(apName));
    return true;
}

/**
 * readBTSerial
 * read all data from BTSerial receive buffer
 * parse data for valid WiFi credentials
 */
void readBTSerial() {
  uint64_t startTimeOut = millis();
  String receivedData;
  // Read RX buffer into String
  while (SerialBT.available() != 0) {
    receivedData += (char)SerialBT.read();
    // Check for timeout condition
    if ((millis()-startTimeOut) >= 2000) break;
  }
  SerialBT.flush();
  Serial.println("Received message " + receivedData + " over Bluetooth");
  
  /** Json object for incoming data */
  auto error = deserializeJson(jsonBuffer, receivedData);
  if (!error) {
    if (jsonBuffer.containsKey("ssid") && jsonBuffer.containsKey("pass")) {
      ssid = jsonBuffer["ssid"].as<String>();
      pass = jsonBuffer["pass"].as<String>();

      Preferences preferences;
      preferences.begin("WiFiCred", false);
      preferences.putString("ssid", ssid);
      preferences.putString("pass", pass);
      preferences.putBool("valid", true);
      preferences.end();

      Serial.println("Received over bluetooth:");
      Serial.println("SSID: "+ssid+" password: "+pass);
      connStatusChanged = true;
      hasCredentials = true;
    } else if (jsonBuffer.containsKey("read")) { // {"read":true}
      Serial.println("BTSerial read request");
      String wifiCredentials;
      jsonBuffer.clear();

      jsonBuffer["ssid"] = ssid;
      jsonBuffer["pass"] = pass;
      // Convert JSON object into a string
      serializeJson(jsonBuffer, wifiCredentials);
      Serial.println("Stored settings: " + wifiCredentials);
      SerialBT.println(wifiCredentials);
      SerialBT.flush();
    } else if (jsonBuffer.containsKey("reboot")) {
      WiFi.disconnect();
      esp_restart();
    } else if (jsonBuffer.containsKey("erase")) { // {"erase":true}
      bool isErase = jsonBuffer["erase"].as<bool>();
      if(isErase){
        Serial.println("Received erase command");
        Preferences preferences;
        preferences.begin("WiFiCred", false);
        preferences.clear();
        preferences.end();
        connStatusChanged = true;
        hasCredentials = false;
        ssid = "";
        pass = "";

        int err=nvs_flash_init();
        Serial.println("nvs_flash_init: " + err);
        err=nvs_flash_erase();
        Serial.println("nvs_flash_erase: " + err);
        WiFi.disconnect();
        esp_restart();
      }      
    } else if (jsonBuffer.containsKey("connect")) { // {"connect":false}
      bool isConnect = jsonBuffer["connect"].as<bool>(); 
      if(isConnect != isConnected)
         connStatusChanged = true;
      
      if(!isConnect){
        Serial.println("Received disconnect command");
        WiFi.disconnect();
        isConnected = false;
        isMySQL_Connected = false;
        isGetBT = false;
      }      
    } else if (jsonBuffer.containsKey("devices")) {
      Serial.println("BTSerial devices state request");
      SerialBT.println(String("{\"AC\":")+(deviceState[0]==true?"true":"false")
                           +",\"DH\":"+(deviceState[1]==true?"true":"false")
                           +",\"AP\":"+(deviceState[2]==true?"true":"false")+"}");
      SerialBT.flush();
    }
    if (jsonBuffer.containsKey("AC")) {
      deviceState[0] = jsonBuffer["AC"].as<bool>(); 
      disable_Auto();//關閉自動控制
      if (deviceState[0])
        digitalWrite(LED[0], HIGH);
      else
        digitalWrite(LED[0], LOW);
      deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);
      SerialBT.println(receivedData);
      SerialBT.flush();
      isGetBT = true;
    }
    if (jsonBuffer.containsKey("DH")) {
      deviceState[1] = jsonBuffer["DH"].as<bool>(); 
      disable_Auto();//關閉自動控制
      if (deviceState[1])
        digitalWrite(LED[1], HIGH);
      else
        digitalWrite(LED[1], LOW);
      deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);
      SerialBT.println(receivedData);
      SerialBT.flush();
      isGetBT = true;
    }
    if (jsonBuffer.containsKey("AP")) {
      deviceState[2] = jsonBuffer["AP"].as<bool>(); 
      disable_Auto();//關閉自動控制
      if (deviceState[2])
        digitalWrite(LED[2], HIGH);
      else
        digitalWrite(LED[2], LOW);
      deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);
      SerialBT.println(receivedData);
      SerialBT.flush();
      isGetBT = true;
    }
  } else {
    Serial.println("Received invalid JSON");
  }
  jsonBuffer.clear();
}

void setup() {
  // Create unique device name
  createName();

  // Initialize Serial port
  Serial.begin(115200);

  Serial2.begin(2400);   // 灰塵感測模組使用baudRate = 2400
  while (!Serial); // wait for serial port to connect
  //初始化統計PM2.5要用到移動平均法的陣列
  for (int i = 0; i < averageCount; i++) 
    pm2_5_data[i] = 0;

  // initialize digital pins as output.
  for (int i = 0; i < sizeof(LED) / sizeof(int); i++) {
    pinMode(LED[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(LED) / sizeof(int); i++) {
    digitalWrite(LED[i], LOW);
  }
  //使用ESP32內建LED當上網指示燈
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }

  //載入flash內的Wi-Fi帳密
  Preferences preferences;
  preferences.begin("WiFiCred", false);
  bool hasPref = preferences.getBool("valid", false);
  if (hasPref) {
    ssid = preferences.getString("ssid","");
    pass = preferences.getString("pass","");

    if (ssid.equals("") || pass.equals("")) {
      Serial.println("Found preferences but credentials are invalid");
    } else {
      Serial.println("Read from preferences:");
      Serial.println("SSID: "+ssid+" password: "+pass);
      hasCredentials = true;
    }
  } else {
    Serial.println("Could not find preferences, need send data over BLE");
  }
  preferences.end();

  // Start BTSerial
  initBTSerial();

  if (hasCredentials)
    connectWiFi();

  if(WiFi.status() == WL_CONNECTED){
    deviceState_recovery();
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    //try to connect to mysql server
    Serial.println("Connecting...");
    if (conn.connect(server_addr, 3306, user, password)) {
      delay(1000);
      isMySQL_Connected = true;
    }
    else
      Serial.println("MYSQL server Connection failed.");
  }
}

void loop() {
  if (MySQL_Status_before != isMySQL_Connected) {
    digitalWrite(LED_BUILTIN, isMySQL_Connected);//用內建LED判斷是否連到伺服器
    MySQL_Status_before = isMySQL_Connected;//更新MySQL連線狀態
  }
  if (connStatusChanged) {    
    if (isConnected) {
      Serial.print("Connected to AP: ");
      Serial.print(WiFi.SSID());
      Serial.print(" with IP: ");
      Serial.print(WiFi.localIP());
      Serial.print(" RSSI: ");
      Serial.println(WiFi.RSSI());
    } else {
      if (hasCredentials) {
        Serial.println("Lost WiFi connection");
        // Received WiFi credentials
        connectWiFi();
      } 
    }
    connStatusChanged = false;
  }

  if(isConnected && !isMySQL_Connected){
    if (isGetBT)
      deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);//更新裝置狀態
    else
      deviceState_recovery();//回復裝置狀態
      
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    //try to connect to mysql server
    Serial.println("Connecting...");
    if (conn.connect(server_addr, 3306, user, password)) {
      delay(1000);
      isMySQL_Connected = true;      
    }
    else
      Serial.println("MYSQL server Connection failed.");
  }
  if(isMySQL_Connected){
    Serial.println();
    row_values *row = NULL;
    int id_index = -1, device_index = -1, date_index = -1, weekday_index = -1, switch_index = -1;

    //取得現在時間
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    //Date
    char dateStringBuff[11];
    strftime(dateStringBuff, sizeof(dateStringBuff), "%Y-%m-%d", &timeinfo);
    //Time
    char timeStringBuff[6];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
    //weekday
    String weekday = week[timeinfo.tm_wday];
    //print like "const char*"
    Serial.println(dateStringBuff + String(" ") + timeStringBuff + " " + weekday);

    String queryString = String("SELECT * FROM smart_home.home000 WHERE schedule='enable' AND ")
                        + "(weekday='Now' OR (Time LIKE '" + timeStringBuff + "%' AND "
                        + "(weekday LIKE '%" + weekday + "%' OR weekday='Only' OR Date='" + dateStringBuff + "')))";
    //  Serial.println(queryString);
    // Length (with one extra character for the null terminator)
    int str_len = queryString.length() + 1;
    // Prepare the character array (the buffer)
    char query[str_len];
    // Copy it over
    queryString.toCharArray(query, str_len);

    Serial.println("Running SELECT and printing results：");

    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Execute the query
    cur_mem->execute(query);
    // Fetch the columns and print them
    column_names *cols = cur_mem->get_columns();
    for (int f = 0; f < cols->num_fields; f++) {
      char* field = cols->fields[f]->name;
      //尋找id、Date、weekday、devices和switch的index
      if (strcmp(field, "id") == 0)
        id_index = f;
      if (strcmp(field, "Date") == 0)
        date_index = f;
      if (strcmp(field, "weekday") == 0)
        weekday_index = f;
      if (strcmp(field, "devices") == 0)
        device_index = f;
      if (strcmp(field, "switch") == 0)
        switch_index = f;

      Serial.print(cols->fields[f]->name);
      if (f < cols->num_fields - 1) {
        Serial.print(", ");
      }
    }
    Serial.println();
    // Read the rows and print them
    do {
      row = cur_mem->get_next_row();
      int device = -1;
      if (row != NULL) {
        for (int f = 0; f < cols->num_fields; f++) {
          Serial.print(row->values[f]);
          //有什麼電器要處理
          if (f == device_index) {
            if (strcmp(row->values[f], "0") == 0)
              device = 0;
            if (strcmp(row->values[f], "1") == 0)
              device = 1;
            if (strcmp(row->values[f], "2") == 0)
              device = 2;
          }
          //電器狀態
          if (f == switch_index) {
            if (strcmp(row->values[f], "On") == 0){
              if (deviceState[device] != true){
                digitalWrite(LED[device], HIGH);
                switch(device){
                  case 0:
                    SerialBT.println("{\"AC\":true}");
                    SerialBT.flush();
                    break;
                  case 1:
                    SerialBT.println("{\"DH\":true}");
                    SerialBT.flush();
                    break;
                  case 2:
                    SerialBT.println("{\"AP\":true}");
                    SerialBT.flush();
                    break;
                }
                deviceState[device] = true;
                deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);
              }
            }else if (strcmp(row->values[f], "Off") == 0){
              if (deviceState[device] != false){
                digitalWrite(LED[device], LOW);
                switch(device){
                  case 0:
                    SerialBT.println("{\"AC\":false}");
                    SerialBT.flush();
                    break;
                  case 1:
                    SerialBT.println("{\"DH\":false}");
                    SerialBT.flush();
                    break;
                  case 2:
                    SerialBT.println("{\"AP\":false}");
                    SerialBT.flush();
                    break;
                }
                deviceState[device] = false;
                deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);
              }
            }              
          }
          //處理Only和Now
          if (f == weekday_index) {
            if (strcmp(row->values[f], "Now") == 0 || strcmp(row->values[f], "Only") == 0)
              disable_UPDATE(row->values[id_index]);//取消已執行過的排程
          }else if (f == date_index) {//處理只有特定日期的排程
            if (strcmp(row->values[f], "NULL") != 0)
              disable_UPDATE(row->values[id_index]);//取消已執行過的排程
          }

          if (f < cols->num_fields - 1) {
            Serial.print(", ");
          }
        }
        Serial.println();
      }
    } while (row != NULL);
    Serial.println();

    // Deleting the cursor also frees up memory used
    delete cur_mem;

    sensor_INSERT();//新增感測器資料

    autoSwitch();//讀取自動控制設定並執行
  }

  // Check if Data over SerialBT has arrived
  if (SerialBT.available() != 0) {
    // Get and parse received data
    readBTSerial();
  }
}

String DB_CRUD(String serverSubName, String httpRequestData) {//透過Servlet來進行資料庫操作
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    //http.setTimeout(5000); http.setConnectTimeout(5000);//預設的timeout為5000ms，也就是5s
    // Your Domain name with URL path or IP address with path
    http.begin(serverName + serverSubName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare your HTTP POST request data
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode == 200) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      http.end();// Disconnect
      return "";//"[{\"Error\":\""+String(httpResponseCode)+"\"}]"
    }
  
    String response = http.getString();

    Serial.print("Response: ");
    Serial.println(response);
    // Free resources
    http.end();

    return response;
  }
  else
    Serial.println("WiFi Disconnected");
  
  return "WiFi Disconnected";
}

void disable_UPDATE(String id) {//取消已執行過的排程
  String httpRequestData = "id=" + id + "&schedule=disable";
  DB_CRUD("API_Update_ESP32", httpRequestData);
}

void deviceState_UPDATE(bool ac, bool dh, bool ap) {//更新裝置狀態
  String httpRequestData = String("AC=") + ac + "&DH=" + dh + "&AP=" + ap;
  DB_CRUD("API_Update_ESP32", httpRequestData);
}

void deviceState_recovery() {//回復裝置狀態
  String response = DB_CRUD("API_Show_ESP32", "devices=states");

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(5) + 55;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Extract values
  JsonObject root_0 = doc[0];
  int ac = root_0["ac"].as<int>();          
  if(ac == 1){
    digitalWrite(LED[0], HIGH);
    deviceState[0] = true;
  } else if(ac == 0){
    digitalWrite(LED[0], LOW);
    deviceState[0] = false;
  }    

  int dh = root_0["dh"].as<int>();          
  if(dh == 1){
    digitalWrite(LED[1], HIGH);
    deviceState[1] = true;
  } else if(dh == 0){
    digitalWrite(LED[1], LOW);
    deviceState[1] = false;
  }      

  int ap = root_0["ap"].as<int>();
  if(ap == 1){
    digitalWrite(LED[2], HIGH);
    deviceState[2] = true;
  } else if(ap == 0){
    digitalWrite(LED[2], LOW);
    deviceState[2] = false;
  }

  SerialBT.println(String("{\"AC\":")+(deviceState[0]==true?"true":"false")
                           +",\"DH\":"+(deviceState[1]==true?"true":"false")
                           +",\"AP\":"+(deviceState[2]==true?"true":"false")+"}");
  SerialBT.flush();
  
  isGetBT = false;
}

void sensor_INSERT() {//新增感測器資料
  humidity = sensor.readHumidity();
  temperature = sensor.readTemperature();
  Serial.print("Humidity:    ");
  Serial.print(humidity, 2);
  Serial.print("\tTemperature: ");
  Serial.println(temperature, 2);

  int incomeByte[7]; //儲存七個字節的資料
  int data; //暫存每個字節的資料
  int z = 0; // 字節計數器 數據傳輸共7字節 (0~6)
  int sum;  // 校驗位 = 第1~第4字節的總和 = Vout(H)+Vout(L)+Vref(H)+Vref(L)
  int A = 390;  // A = 比例係數 (280~500間)
  Ud = 0;
  while (Serial2.available() > 0 && Ud == 0)
  {
    data = Serial2.read();
    if (data == 170) // 170 = 0xaa, 起始字節
    {
      z = 0;          // 字節計數歸零
      incomeByte[z] = data; // 將data存到第0個字節(0Xaa)
    }
    else
    {
      z++;
      incomeByte[z] = data; // 依序儲存第1字節到第6字節的資料
    }
    if (z == 6)  //儲存完七個字節後
    {
      sum = incomeByte[1] + incomeByte[2] + incomeByte[3] + incomeByte[4]; //加總字節1~4
      if (incomeByte[5] == sum && incomeByte[6] == 255 ) //比對加總後是否和字節5(校驗位)相同，並且字節6是否為結束位元 0xff
      {
        Serial.print("Data OK! |");       // 完成6個字節的儲存
        for (int k = 0; k < 7; k++) //印出七個字節
        {
          Serial.print(incomeByte[k]);  // 起始位 | Vout(H) | Vout(L) | Vref(H) | Vref(L) | 校驗位 | 結束位
          Serial.print("|");            // 0xaa  |  0x??   |  0x??   |  0x??   |  0x??   |  sum  |  0xff
        }
        Serial.print(" Vo=");
        //感測器輸出電壓 = ((Vout(H) x 256 + Vout(L)) / 1024) x 5
        float Vout = (incomeByte[1] * 256.0 + incomeByte[2]) * 5 / 1024.0;
        Serial.print(Vout, 3);  // Vout取到小數第3位
        Serial.print("v | ");
        Ud = Vout * A; //粉塵濃度 Ud= Vout x A
        Serial.print(" PM2.5 = ");
        Serial.print(Ud, 2);   // Ud取到小數第2位
        Serial.print("ug/m3 ");
        Serial.println();
      }
      else     //完成運算與運算後，將資料歸零
      {
        Serial.flush();  //確認傳送緩衝區資料傳送完畢
        data = '/0';
        for (int m = 0; m < 7; m++) {
          incomeByte[m] = 0;
        }  // 七個字節內的資料歸零
      }
      z = 0; // Z=6後，Z歸零
    }
  }
  //使用移動平均法統計PM2.5
  if (millis() - LastTime > timeDelay) {
    pm2_5_total=pm2_5_total-pm2_5_data[curIndex];
    pm2_5_data[curIndex]=Ud;
    LastTime = millis();
    pm2_5_total=pm2_5_total+pm2_5_data[curIndex];
    curIndex=curIndex+1;
    pm2_5_average=pm2_5_total/averageCount;
    Serial.println("PM2.5平均："+String(pm2_5_average));
  }
  if (curIndex>=averageCount) {
    curIndex=0;
  }
  //請不要在採樣週期裡面添加延遲delay這樣的。不然串口寄存器只有64Byte，很可能被塞滿。
  if(millis()-currentMillis>=600000)  //間隔10分鐘以上傳送到資料庫
  {
    if(isnan(temperature) != 1 && isnan(humidity) != 1 && pm2_5_average != 0)
    {
      String httpRequestData = String("celsius=") + temperature + "&humidity=" + humidity + "&PM2_5=" + pm2_5_average;
      DB_CRUD("API_Insert_ESP32", httpRequestData);
    }    
    currentMillis=millis();  //紀錄時間
  }
}

void autoSwitch() {//讀取自動控制設定並執行
  String response = DB_CRUD("API_Show_ESP32", "");
  
  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(8) + 128;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Extract values
  JsonObject root_0 = doc[0];
  String item = root_0["item"].as<char*>();
  if(item == "set_values"){
    String _auto = root_0["auto"].as<char*>();
    if(_auto == "Y"){
      bool isStateChange = false;//紀錄裝置狀態是否改變
      if(isnan(temperature) != 1){
        if(!root_0["celsius_On"].isNull()){
          float celsius_On = root_0["celsius_On"].as<float>();          
          if(temperature >= celsius_On){            
            if (deviceState[0] != true){
              digitalWrite(LED[0], HIGH);
              deviceState[0] = true;
              isStateChange = true;
            }
          }
        }
        if(!root_0["celsius_Off"].isNull()){
          float celsius_Off = root_0["celsius_Off"].as<float>();
          if(temperature <= celsius_Off){
            if (deviceState[0] != false){
              digitalWrite(LED[0], LOW);
              deviceState[0] = false;
              isStateChange = true;
            }
          }
        }          
      }      

      if(isnan(humidity) != 1){
        if(!root_0["humidity_On"].isNull()){
          float humidity_On = root_0["humidity_On"].as<float>();          
          if(humidity >= humidity_On){
            if (deviceState[1] != true){
              digitalWrite(LED[1], HIGH);
              deviceState[1] = true;
              isStateChange = true;
            }
          }
        }
        if(!root_0["humidity_Off"].isNull()){
          float humidity_Off = root_0["humidity_Off"].as<float>();
          if(humidity <= humidity_Off){       
            if (deviceState[1] != false){
              digitalWrite(LED[1], LOW);
              deviceState[1] = false;
              isStateChange = true;
            }
          } 
        }         
      }      

      if(pm2_5_average != 0){
        if(!root_0["pm2_5_On"].isNull()){
          float pm2_5_On = root_0["pm2_5_On"].as<float>();
          if(pm2_5_average >= pm2_5_On){
            if (deviceState[2] != true){
              digitalWrite(LED[2], HIGH);
              deviceState[2] = true;
              isStateChange = true;
            }
          }
        }
        if(!root_0["pm2_5_Off"].isNull()){
          float pm2_5_Off = root_0["pm2_5_Off"].as<float>();
          if(pm2_5_average <= pm2_5_Off){
            if (deviceState[2] != false){
              digitalWrite(LED[2], LOW);
              deviceState[2] = false;
              isStateChange = true;
            }
          }          
        }  
      }
      if(isStateChange){
        SerialBT.println(String("{\"AC\":")+(deviceState[0]==true?"true":"false")
                           +",\"DH\":"+(deviceState[1]==true?"true":"false")
                           +",\"AP\":"+(deviceState[2]==true?"true":"false")+"}");
        SerialBT.flush();
        deviceState_UPDATE(deviceState[0],deviceState[1],deviceState[2]);
      }
    }
  }      
}

void disable_Auto(){//關閉自動控制
  String httpRequestData = "Auto=N";
  DB_CRUD("API_Update_settings", httpRequestData);
}