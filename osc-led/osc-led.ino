#include <ArduinoOSC.h>

//WiFiルータ Settings
const char* ssid = "My ASUS";  // ネットワーク名
const char* pwd = "aratamatsumoto";
const IPAddress gateway(192, 168, 43, 1);  // ゲートウェイ = ネットワークのベース
const IPAddress subnet(255, 255, 255, 0);  // サブネット = だいたいこの値

// 送信先ポート
const int portIncoming = 8000;
const int portOutgoing = 8001;

// 送信先IP
const char* host = "127.0.0.0";

bool isConnecting = false;

const int pin1 = 23;
bool isLighting = false;

void setup () {
  Serial.begin(115200);
  pinMode(pin1, OUTPUT);

  // ESP自身のIPを決定する 空いているかpingコマンドで事前に確認しておくのが良い
  IPAddress ip(192, 168, 43, 16);

  // WiFi設定
  // 初期設定 まえの接続が残ってたりするといけないので一回消す
  WiFi.disconnect(true, true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);
  WiFi.config(ip, gateway, subnet);

  // WiFi接続開始 ------
  Serial.print("INFO : Wi-Fi Start Connect.");
  int cnt = 0;
  bool isConnected = true;
  delay(1000);
  
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
    // WiFi接続完了 ------
    Serial.println("INFO : Wi-Fi Connected.");
    //受信のリスナー設定
    OscWiFi.subscribe(portIncoming, "/app/led", onOscReceivedStatus);
  } else {
    Serial.println("INFO : Wi-Fi Connect Failed.");
  }
  
  //コネクト処理終了フラグ
  isConnecting = false;

}

void loop () {
  // 必須
  OscWiFi.update();
}

// /app/statusにメッセージが来たときのイベントハンドラ
void onOscReceivedStatus(OscMessage& m) {
  // メッセージ内容の取得方法
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

// 送信時のサンプル
void sendOsc(int v){
  OscWiFi.send(host, portOutgoing, "/app/status", v);
}
