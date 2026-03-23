#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define LORA_SS 5
#define LORA_RST 4
#define LORA_DIO0 2
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 21

// TFT Pins (change if needed)
#define TFT_CS 15
#define TFT_DC 13
#define TFT_RST 12

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Buttons
#define BTN_YES 32
#define BTN_NO 33

#define DEVICE_ID "101"

String lastMessage = "";
bool waitingReply = false;

void setup()
{
  Serial.begin(115200);

  pinMode(BTN_YES, INPUT_PULLUP);
  pinMode(BTN_NO, INPUT_PULLUP);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6))
  {
    while (1)
      ;
  }

  LoRa.setSyncWord(0xA5);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Waiting...");
}

void loop()
{

  // 📥 Receive Message
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    String received = "";

    while (LoRa.available())
    {
      received += (char)LoRa.read();
    }

    // Expect: MSG:ID:message
    if (received.startsWith("MSG"))
    {
      int first = received.indexOf(':');
      int second = received.indexOf(':', first + 1);

      String id = received.substring(first + 1, second);
      String msg = received.substring(second + 1);

      if (id == DEVICE_ID)
      {
        displayMessage(msg);
        waitingReply = true;
      }
    }
  }

  // 👍 YES Button
  if (waitingReply && digitalRead(BTN_YES) == LOW)
  {
    sendReply("YES");
    waitingReply = false;
    delay(500);
  }

  // 👎 NO Button
  if (waitingReply && digitalRead(BTN_NO) == LOW)
  {
    sendReply("NO");
    waitingReply = false;
    delay(500);
  }
}

void displayMessage(String msg)
{
  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(10, 20);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN);

  tft.println("Message:");
  tft.setTextColor(ILI9341_WHITE);
  tft.println(msg);

  tft.setCursor(10, 200);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println("YES / NO ?");
}

void sendReply(String reply)
{
  String packet = "RPLY:" + String(DEVICE_ID) + ":" + reply;

  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 50);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.println("Sent: " + reply);
}