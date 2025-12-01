#include "AnalogUVSensor.h"
#include "DHT.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ------------------ BLYNK -------------------
#define BLYNK_TEMPLATE_ID "TMPL6-C05hzA7"
#define BLYNK_TEMPLATE_NAME "Fire Detector"
#define BLYNK_AUTH_TOKEN "WXZXUrVyM7aJNcECKulm4JKkK75jVFsy"

char ssid[] = "ADNYANA";
char pass[] = "DWIJA11062314";

// ------------------ PIN ANALOG (AO) -------------------
#define MQ2pin   34     // asap
#define GUVApin  35     // UV flame
#define IRpin1   32     // flame IR
#define IRpin2   33
#define IRpin3   36
#define IRpin4   39
#define DHT22pin 4      // suhu

// ------------------ LED & Buzzer -------------------
#define LED_RED     25
#define LED_YELLOW  26
#define LED_GREEN   27
#define BUZZER      16

// ------------------ THRESHOLD -------------------
#define Threshold_Temp     40
#define Threshold_Smoke    250
#define Threshold_Flame    1500   // sesuaikan dengan IR/UV kamu

// ------------------ Sensor Values -------------------
float MQ2_value, UV_value;
float IR1_value, IR2_value, IR3_value, IR4_value;
float temp_value, last_temp = 0;
bool temp_rising = false;

DHT dht(DHT22pin, DHT22);

// ------------------ Debounce Analog -------------------
int readAnalogDebounced(int pin, int samples = 8, int delayMicrosTime = 3000) {
  long total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(pin);
    delayMicroseconds(delayMicrosTime);
  }
  return total / samples;
}

void setConditionNormal() {
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER);
}

void setConditionWarning() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER);
}

void setConditionDanger() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);
  tone(BUZZER, 1000);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  setConditionNormal();
  dht.begin();

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

// ----------------------------------------------------
// MAIN LOOP 
// ----------------------------------------------------
void loop() {
  Blynk.run();

  // 1) Ambil semua sensor
  MQ2_value  = readAnalogDebounced(MQ2pin);
  UV_value   = readAnalogDebounced(GUVApin);
  IR1_value  = readAnalogDebounced(IRpin1);
  IR2_value  = readAnalogDebounced(IRpin2);
  IR3_value  = readAnalogDebounced(IRpin3);
  IR4_value  = readAnalogDebounced(IRpin4);

  temp_value = dht.readTemperature();

  // 2) Cek apakah suhu naik?
  temp_rising = (temp_value > last_temp);
  last_temp = temp_value;

  // 3) Logika deteksi asap
  bool smokeDetected = (MQ2_value > Threshold_Smoke);

  // 4) Logika deteksi api (IR/UV)
  bool flameDetected =
        (IR1_value > Threshold_Flame ||
         IR2_value > Threshold_Flame ||
         IR3_value > Threshold_Flame ||
         IR4_value > Threshold_Flame ||
         UV_value  > Threshold_Flame);

  // --------------------------
  // 5) LOGIKA KEPUTUSAN 
  // --------------------------

  // --- KONDISI BAHAYA ---
  if (flameDetected || (temp_value > Threshold_Temp && smokeDetected)) {

    setConditionDanger();

    Blynk.logEvent("bahaya", "API TERDETEKSI!");
    Serial.println(">>> KONDISI BAHAYA");

  }
  // --- KONDISI WASPADA ---
  else if (smokeDetected || temp_rising) {

    setConditionWarning();

    Blynk.logEvent("waspada", "WASPADA! Kenaikan suhu atau asap terdeteksi");
    Serial.println(">>> KONDISI WASPADA");

  }
  // --- KONDISI NORMAL ---
  else {

    setConditionNormal();

    Serial.println(">>> KONDISI NORMAL");
  }

  // kirim ke Blynk
  Blynk.virtualWrite(V0, MQ2_value);
  Blynk.virtualWrite(V1, temp_value);
  Blynk.virtualWrite(V2, UV_value);

  delay(500);
}
