#include <SPI.h>
#include <LoRa.h>
#include <stdlib.h>


//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//BME280 definition
#define SDA 21
#define SCL 13

TwoWire I2Cone = TwoWire(1);
//Adafruit_BME280 bme;

//packet counter
int readingID = 0;

int counter = 0;
int CO2;
int Dust;
int sensor_number=1;
String LoRaMessage = "";

//float pressure = 3;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//Initialize OLED display
void startOLED(){
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER");
}

//Initialize LoRa module
void startLoRA(){
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}


void sendReadings() {
  
  CO2 = (rand()%50)+700; //700~750여기서는 700~750...
  Dust = (rand()%10)+3; //3~13까지 여기서.
  int mod = (rand()%100);
  if(mod<3){ //3퍼센트의 확률로 이산화탄소랑 dust 농도 올라감 이때 많이 안올라갈수도있음.
    Co2+=(rand()%400);
    Dust+=(rand()%40);
  }
  LoRaMessage = String(readingID) + "/" + String(CO2) + "&" + String(Dust)+"#"+String(sensor_number); //지금은 센서넘버1로 해서 보냄.
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("LoRa packet sent!");
  display.setCursor(0,20);
  display.print("CO2:");
  display.setCursor(72,20);
  display.print(CO2);
  display.setCursor(0,30);
  display.print("Dust:");
  display.setCursor(54,30);
  display.print(Dust);
  display.setCursor(0,40);
  display.print("Reading ID:");
  display.setCursor(66,50);
  display.print(readingID);
  display.display();
  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  startOLED();
  //startBME();
  startLoRA();
}
void loop() {
//  getReadings();
  srand(time(NULL));
  sendReadings();
  delay(1000);
}
