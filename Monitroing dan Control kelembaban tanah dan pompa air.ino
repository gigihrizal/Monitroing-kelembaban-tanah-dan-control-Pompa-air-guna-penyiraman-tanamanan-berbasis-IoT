#define BLYNK_TEMPLATE_ID "TMPL6MXR4dLZh"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#define BLYNK_AUTH_TOKEN "XetKNi4O3nN4bow8T5efqZhaWAqUJTe5"
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int soilMoisturePin = A0;
int soilMoistureValue = 0;
int soilMoisturePercentage = 0;
int lcdColumns = 16; // Jumlah kolom pada LCD
int lcdRows = 2; // Jumlah baris pada LCD
int relayPin = 4;  // Change to the appropriate pin for your setup

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

bool isSoilStatus = true;  // True jika menampilkan status tanah, False jika menampilkan status koneksi Blynk
unsigned long lastSwitchMillis = 0;
const unsigned long switchInterval = 1000;  // Interval untuk bergantian setiap detik

void setup() {
  pinMode(relayPin,OUTPUT);
  Serial.begin(115200);
  
  WiFi.begin("LAPTOP IJAL", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi Connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, "LAPTOP IJAL", "12345678");
  Serial.println("Blynk Connected");

  lcd.init();
  lcd.backlight();
}

void loop() {
  Blynk.run();
  readSoilMoisture(); // Baca nilai kelembaban tanah
  Serial.println("Persentase Kelembaban Tanah: " + String(soilMoisturePercentage) + "%");

  displayStatus(); // Tampilkan status pompa dan koneksi Blynk pada LCD
  delay(1000);
}

void readSoilMoisture() {
  soilMoistureValue = analogRead(soilMoisturePin);
  soilMoisturePercentage = map(soilMoistureValue, 0, 1023, 0, 100);

  // Send percentage value to Blynk
  Blynk.virtualWrite(V0, soilMoisturePercentage);
}

void displayStatus() {
  lcd.clear();
  
  // Baris 1 (bergantian antara "IoT kelompok 7" dan status Blynk)
  lcd.setCursor(0, 0);

  if (isSoilStatus) {
    lcd.print("IoT kelompok 7");
  } else {
    lcd.print("Blynk: ");
    if (Blynk.connected()) {
      lcd.print("Connected");
    } else {
      lcd.print("Disconnected");
    }
  }
  
  // Baris 2
  lcd.setCursor(0, 1);

  if (isSoilStatus) {
    lcd.print("Kelembaban: ");
    lcd.print(soilMoisturePercentage);
    lcd.print("%");
  } else {
    lcd.print("Tanah: ");

    // Tentukan status kelembaban tanah
    if (soilMoisturePercentage > 35) {
      lcd.print("Kering");
      // Blynk.logEvent("plantwateringsystem", String("kering: ") + soilMoisturePercentage);
      // Jika kelembaban tanah di atas 35%, tambahkan logika atau tindakan yang diperlukan di sini
    } else {
      lcd.print("Basah");
      // Blynk.logEvent("plantbasah", String("basah: ") + soilMoisturePercentage);
      
      // Jika kelembaban tanah 35% ke bawah, tambahkan logika atau tindakan yang diperlukan di sini
    }
  }

  // Baris 3
  lcd.setCursor(0, 2);

  // Bergantian antara status tanah dan status koneksi Blynk setiap detik
  if (millis() - lastSwitchMillis >= switchInterval) {
    lastSwitchMillis = millis();
    isSoilStatus = !isSoilStatus;
  }
}
