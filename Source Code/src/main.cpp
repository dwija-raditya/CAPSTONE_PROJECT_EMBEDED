#define BLYNK_TEMPLATE_ID "TMPL6fNFvhHxH"
#define BLYNK_TEMPLATE_NAME "Fire Detector"
#define BLYNK_PRINT Serial

#include "WiFi.h"
#include "BlynkEdgent.h"
#include "Config.h"
#include "DHT22.h"
#include "AnalogSensor.h"

// Watchdog Vars
unsigned long lastConnectAttempt = 0;
int connectFailures = 0;
bool isResetting = false;

// State Vars
unsigned int dangerCount = 0;
bool lastDangerState = false;
bool lastWarningState = false;
float temp_value, smoke_value;

// Timer Intervals
unsigned long lastFastCheck = 0;
unsigned long lastSlowCheck = 0;

void setup() {
    Serial.begin(115200);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    setupDHT();
    BlynkEdgent.begin();
    lastConnectAttempt = millis();
}

void loop() {
    BlynkEdgent.run();
    unsigned long now = millis();

    // 1. WATCHDOG KONEKSI
    if (BlynkState::is(MODE_CONNECTING_NET) && !isResetting) {
        if (now - lastConnectAttempt > CONNECT_TIMEOUT_MS) {
            connectFailures++;
            lastConnectAttempt = now;
            Serial.printf("Gagal koneksi ke-%d\n", connectFailures);
            if (connectFailures >= MAX_FAILURES) {
                Serial.println("!!! Resetting Config... !!!");
                isResetting = true;
                BlynkState::set(MODE_RESET_CONFIG);
            }
        }
    } else if (BlynkState::is(MODE_WAIT_CONFIG) || BlynkState::is(MODE_RUNNING)) {
        lastConnectAttempt = now;
        connectFailures = 0;
        isResetting = false;
    }

    // 2. FAST CHECK (100ms): Respon cepat untuk API & ASAP
    if (now - lastFastCheck >= 100) {
        smoke_value = getMQ2PPM();
        bool flameDetected = isFlameDetected();
        bool smokeDetected = smoke_value > THRESHOLD_SMOKE;

        bool dangerNow = flameDetected || (temp_value > THRESHOLD_TEMP && smokeDetected);
        bool warningNow = !dangerNow && (smokeDetected || temp_value > THRESHOLD_TEMP);

        if (dangerNow) {
            digitalWrite(LED_RED, HIGH); digitalWrite(LED_GREEN, LOW); digitalWrite(LED_YELLOW, LOW);
            tone(BUZZER, 1000);
            if (!lastDangerState) {
                Blynk.logEvent("bahaya", "BAHAYA API!");
                dangerCount++;
            }
        } else if (warningNow) {
            digitalWrite(LED_YELLOW, HIGH); digitalWrite(LED_GREEN, LOW); digitalWrite(LED_RED, LOW);
            noTone(BUZZER);
            if (!lastWarningState) Blynk.logEvent("waspada", "Asap/Suhu Meningkat");
        } else {
            digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_YELLOW, LOW); digitalWrite(LED_RED, LOW);
            noTone(BUZZER);
        }

        lastDangerState = dangerNow;
        lastWarningState = warningNow;
        lastFastCheck = now;
    }

    // 3. SLOW CHECK (2000ms): Update DHT & Kirim ke Blynk
    if (now - lastSlowCheck >= 2000) {
        temp_value = readTemperatureSafe();

        String kondisi = lastDangerState ? "Bahaya" : (lastWarningState ? "Waspada" : "Aman");
        Blynk.virtualWrite(V0, temp_value);
        Blynk.virtualWrite(V1, smoke_value);
        Blynk.virtualWrite(V3, kondisi);
        Blynk.virtualWrite(V4, dangerCount);

        lastSlowCheck = now;
    }
}
