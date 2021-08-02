#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>


static const uint8_t RST_PIN = 16;
static const uint8_t SS_PIN = 5;
static const uint8_t D2 = 0;
static const uint8_t PB = 4;
const char* ssid = "";
const char* password = "";

//Your Domain name with URL path or IP address with path
String str;
String lamp;
String fan;
String serverName = "https://pktmj18.woy.link/index.php/api";
String id_ruangan = "5";
String rfidtag;
int prodi;
int nilai;
int semester;
int id_jadwal = 0;
int st_ruangan = 0;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(D2, OUTPUT);
  pinMode(PB,OUTPUT);
  digitalWrite(D2,HIGH);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  getRFID(prodi, semester);
  remoteAlat();
  nilai = digitalRead(PB);
  if(nilai == 1){
    openClass(1);
  }
}

void getRFID(int prodi, int semester) {
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      rfidtag += rfid.uid.uidByte[i];
    }
    Serial.println(rfidtag);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    if (st_ruangan == 0) {
      startClass(rfidtag);
    } else {
      absensi(rfidtag, prodi, semester);
    }
  } else {
    Serial.println("WiFi Disconnected");
    if (rfidtag == "") {
      openClass(0);
    }
  }
  rfidtag = "";

}

void startClass(String rfidtag) {
  HTTPClient http;
  String api = serverName + "/start_class";
  http.begin(api);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "id_ruangan=" + id_ruangan + "&rfid=" + rfidtag;
  int httpResponseCode = http.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, http.getString());
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  const int code = doc["code"];
  String status_ = doc["message"];
  id_jadwal = doc["jadwal"]["id_jadwal"];
  prodi = doc["jadwal"]["prodi"];
  semester = doc["jadwal"]["semester"];
  Serial.print(prodi);
  Serial.println(semester);
  httpRequestData = "";
//   Free resources
  http.end();
  if (code == 200) {
    st_ruangan = 1;
    openClass(st_ruangan);
    getRFID(prodi, semester);
  } else {
    Serial.print("Wasuu");
  }
}



void absensi(String rfidtag, int prodi, int semester) {
  String p = String(prodi);
  String s = String(semester);
  HTTPClient HTTP;
  String api = serverName + "/absensi";
  HTTP.begin(api);
  HTTP.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "id_jadwal="+ String(id_jadwal) +"&rfid=" + rfidtag + "&prodi=" + p + "&semester=" + s;
  Serial.println(httpRequestData);
  Serial.print("");
  int httpResponseCode = HTTP.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, HTTP.getString());

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  const int code = doc["code"];
  const int type = doc["type"];
  String status_ = doc["message"];
  Serial.print(status_);
  httpRequestData = "";
  // Free resources
  HTTP.end();
  if(type == 1){
    openClass(1);
  }
}

void openClass(int statusclass) {
  if (statusclass == 1) {
    st_ruangan = 1;
    Serial.println("Kelas Terbuka");
   digitalWrite (D2, LOW);
   delay(5000);
    digitalWrite (D2, HIGH);
  } else {
    Serial.println("Siapa Kamu?");
     digitalWrite (D2, HIGH);
  }
}

void remoteAlat(){
if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
     HTTPClient http;
  String api = serverName + "/remote-alat";
  http.begin(api);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "id_ruangan=" + id_ruangan;
  int httpResponseCode = http.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, http.getString());
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  const int code = doc["code"];
  String status_ = doc["message"];
  int KIPAS = doc["alat"]["KIPAS"];
  int LAMPU = doc["alat"]["LAMPU"];
  httpRequestData = "";
//   Free resources
  http.end();
  if(LAMPU == 1){
    lamp = "ON"; 
  }else {
    lamp = "OFF";
  }
  if(KIPAS == 1){
    fan = "ON";
  }else {
    fan = "OFF";
  }
  str =String(lamp+" "+fan+" ");
  Serial1.println(str);
  delay(2000);
  Serial.println(str);
  }
}
