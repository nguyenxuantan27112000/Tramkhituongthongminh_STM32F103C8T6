#include <SoftwareSerial.h>// Bộ uart Mềm 2.5.0
const byte RX = D5;
const byte TX = D6;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);

#include <ArduinoJson.h> // 6.10.0

void Read_UART_STM32(void);

String DataSTM32 = "";

void XuLyJson(String Data);

float A = 0;
float B = 0;
float C = 0;
float D = 0;


#define BLYNK_TEMPLATE_ID "TMPLKbZDWwoX"
#define BLYNK_DEVICE_NAME "Trạm Khí Tượng Thông Minh"
#define BLYNK_AUTH_TOKEN "JhoyCoM9YfitZzPtRx_K2eFz6MQzcpjB"

  #include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "JhoyCoM9YfitZzPtRx_K2eFz6MQzcpjB";
char ssid[] = "Xiaomi_2.4G";
char pass[] = "12456789";

#define LED 2
WidgetLED led_connect(V0);
void BLynkConnect(void);
unsigned long times = millis();

String DataSendSTM = "";

unsigned long LastSend = millis();
BLYNK_WRITE(V3)
{
  int btnV3 = param.asInt();
  Serial.print("btnV3:");
  Serial.println(btnV3);
  DataSendSTM = "";
  DataSendSTM = "{\"TB1\":\"" + String(btnV3) + "\"}";
  mySerial.println(DataSendSTM);
  mySerial.flush();
  LastSend = millis();
  //DataSendSTM = "{\"TB1\":\"1\"}";
  // 1 = "+String(btnV3)+"
}
BLYNK_WRITE(V4)
{
  int btnV4 = param.asInt();
  Serial.print("btnV4:");
  Serial.println(btnV4);
  DataSendSTM = "";
  DataSendSTM = "{\"TB2\":\"" + String(btnV4) + "\"}";
  mySerial.println(DataSendSTM);
  mySerial.flush();
  LastSend = millis();

}

void setup()
{
  //Khởi tạo cổng UART giao tiếp STM32
  Serial.begin(115200);
  mySerial.begin(115200);//cổng giao tiếp STM32
  pinMode(LED, OUTPUT); // khai bao output
  Blynk.begin(auth, ssid, pass);

  Serial.println();
  Serial.println("Start ESP!!!");
  times = millis();
  LastSend = millis();


}

void loop()
{
  Blynk.run();
  BLynkConnect();
  Read_UART_STM32();
  SendLenh();

}
void SendLenh(void)
{
  if (millis() - LastSend >= 2000)
  {

    Serial.println("Đã gửi lệnh STM32");
    mySerial.println("{\"SEND\":\"1\"}");
    mySerial.flush();
    LastSend = millis();
  }
}
void BLynkConnect(void)
{
  if (millis() - times > 1500)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      if (Blynk.connected())
      {
        blinkLedBlynk();
        delay(10);
      }
    }
    times = millis();
  }
}
void blinkLedBlynk()
{
  if (led_connect.getValue())
  {
    led_connect.off();
  }
  else
  {
    led_connect.on();
  }
  digitalWrite(LED, !digitalRead(LED));
}
void Read_UART_STM32(void)
{

  while (mySerial.available())
  {
    char inChar = (char)mySerial.read();
    if (inChar != '\n')
    {
      DataSTM32 += inChar;
    }
    else
    {
      //hiển thị Serial để đối chiếu dữ liệu stm => ESP
      Serial.print("Data STM32 là: ");
      Serial.println(DataSTM32);
      XuLyJson(String(DataSTM32));

      DataSTM32 = "";
    }
  }
}

void XuLyJson(String Data)
{
  const size_t capacity = JSON_OBJECT_SIZE(2) + 256;
  DynamicJsonDocument JSON(capacity);
  DeserializationError error = deserializeJson(JSON, Data);
  if (error)
  {
    return;
  }
  else
  {
    Serial.println("Data nhận được STM32 là:");
    serializeJsonPretty(JSON, Serial);
    Serial.println();
    if (JSON.containsKey("A"))
    {
      String Data_A = JSON["A"];
      Serial.print("A:");
      Serial.println(Data_A);
      Blynk.virtualWrite(V1, Data_A.toFloat());

    }
    if (JSON.containsKey("B"))
    {
      String Data_B = JSON["B"];
      Serial.print("B:");
      Serial.println(Data_B);
      Blynk.virtualWrite(V2, Data_B.toFloat());
    }
    if (JSON.containsKey("C"))
    {
       String Data_C = JSON["C"];
      Serial.print("C:");
      Serial.println(Data_C);
      Blynk.virtualWrite(V3, Data_C.toFloat());

    }
    if (JSON.containsKey("D"))
    {
      String Data_D = JSON["D"];
      Serial.print("D:");
      Serial.println(Data_D);
      Blynk.virtualWrite(V4, Data_D.toFloat());
    }

    JSON.clear();

  }
}
