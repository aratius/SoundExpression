#include <ArduinoOSC.h>
#include <Adafruit_NeoPixel.h>

#define LED_COUNT (100)
#define DIGITAL_PIN (13)

//neopixel
Adafruit_NeoPixel led = Adafruit_NeoPixel(LED_COUNT, DIGITAL_PIN , NEO_GRB + NEO_KHZ800);


//WiFiルータ Settings
const char* ssid = "synology_01";  // ネットワーク名
const char* pwd = "synologyTD";
const IPAddress gateway(192, 168, 1, 1);  // ゲートウェイ = ネットワークのベース
const IPAddress subnet(255, 255, 255, 0);  // サブネット = だいたいこの値

// 受信先ポート
const int portIncoming = 8002;
// 送信先ポート
const int portOutgoing = 12000;

// 送信先IP
const char* host = "192.168.1.236";

bool isConnecting = false;

const int pin1 = 23;
bool isLighting = false;

int delaytime=5000;



void setup () {
  led.begin();
  led.show();
 
  Serial.begin(115200);
  pinMode(pin1, OUTPUT);

  // ESP自身のIPを決定する 空いているかpingコマンドで事前に確認しておくのが良い
  IPAddress ip(192, 168, 1, 152);

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
    OscWiFi.subscribe(portIncoming, "/human", onOscReceivedStatus);
  } else {
    Serial.println("INFO : Wi-Fi Connect Failed.");
  }
  
  //コネクト処理終了フラグ
  isConnecting = false;

}

float blightnessAdd = 0;

// sinの波長の係数
const float sinAmp = 3;

void loop () {
  // 必須
  OscWiFi.update();
  
  unsigned long ms = millis();
  float t = (float)ms * 0.001 * sinAmp;
  float sinVal1 = (sin(t) + 1) * 0.5;
  float sinVal2 = (sin(t * 0.78) + 1) * 0.5;

  float blightness = (sinVal1 + sinVal2) / 2;
  blightness *= 10;
  
  blight(blightness + blightnessAdd);

  blightnessAdd *= 0.997;
  
  delay(1);
}

void onOscReceivedStatus(OscMessage& m) {
  blightnessAdd += 50;
  if(blightnessAdd > 50) blightnessAdd = 50;
}

void blight(float blightness) {
  led.fill(led.Color(blightness * 0.8, blightness, blightness * 0.8), 0, LED_COUNT);
  led.show(); 
}
