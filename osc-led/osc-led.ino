#include <ArduinoOSC.h>

//WiFiルータ Settings
const char* ssid = "My ASUS";  // ネットワーク名
const char* pwd = "aratamatsumoto";
const IPAddress gateway(192, 168, 43, 1);  // ゲートウェイ = ネットワークのベース
const IPAddress subnet(255, 255, 255, 0);  // サブネット = だいたいこの値

// 送信先ポート
const int port = 8000;

// 送信先IP
const char* host = "192.168.43.12";

bool isConnecting = false;

const int pin1 = 23;
bool isLighting = false;

void setup () {
  Serial.begin(115200);

  pinMode(pin1, OUTPUT);

  // 自分のIPを決定する 一旦自分のスマホ
  IPAddress ip(192, 168, 43, 16);

  // 初期設定 まえの接続が残ってたりするといけないので一回消す
  WiFi.disconnect(true, true);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, pwd);
  WiFi.config(ip, gateway, subnet);

  int cnt = 0;
  bool isConnected = true;
  delay(1000);

  
  Serial.print("INFO : Wi-Fi Start Connect.");

  // WiFiがつながるまでwhileを回す
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print(".");
    delay(500);
    if (cnt == 5) {
      isConnected = false;
      break;
    }
    cnt += 1;
  }
  Serial.println("");

  if (isConnected) {
    Serial.println("INFO : Wi-Fi Connected.");
    //受信のリスナー設定
        OscWiFi.subscribe(port, "/app/status", onOscReceivedStatus);
    //    OscWiFi.subscribe(port, "/app/threshold", onOscReceivedThreshold);
    //    OscWiFi.subscribe(port, "/app/connect", onOscReceivedConnectTest);
    //    OscWiFi.subscribe(port, "/app/usevibe", onOscReceivedUseVibe);
  } else {
    Serial.println("INFO : Wi-Fi Connect Failed.");
  }
  
  //コネクト処理終了フラグ
  isConnecting = false;
  
}

void onOscReceivedStatus(OscMessage& m) {
  int statusVal = m.arg<int>(0);
  Serial.print("onOscReceivedStatus : ");
  Serial.println(statusVal);

  if(isLighting) {
    digitalWrite(pin1, LOW);
  }else {
    digitalWrite(pin1, HIGH);
  }
  isLighting = !isLighting;
}

void loop () {
  OscWiFi.update();
}

void sendOsc(){
  OscWiFi.send(host, port, "/app/status", 1);
}
