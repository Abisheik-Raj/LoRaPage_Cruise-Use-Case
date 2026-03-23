#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 4
#define LORA_DIO0 2
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 21

String incomingData = "";

void setup()
{
  Serial.begin(115200);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6))
  {
    while (1)
      ;
  }

  LoRa.setSyncWord(0xA5);
}

void loop()
{
  // 📤 From PC → LoRa
  if (Serial.available())
  {
    incomingData = Serial.readStringUntil('\n');
    incomingData.trim();

    if (incomingData.length() > 0)
    {
      LoRa.beginPacket();
      LoRa.print(incomingData);
      LoRa.endPacket();
    }
  }

  // 📥 From LoRa → PC
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    String received = "";

    while (LoRa.available())
    {
      received += (char)LoRa.read();
    }

    Serial.println(received); // send to Python
  }
}