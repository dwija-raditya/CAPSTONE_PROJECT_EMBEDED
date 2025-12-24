#include "AnalogSensor.h"
#include "Config.h"
#include <Arduino.h>

int readAnalogDebounced(int pin) {
    long total = 0;
    for (int i = 0; i < 10; i++) { // Dikurangi ke 10 samples agar lebih cepat
        total += analogRead(pin);
        delayMicroseconds(500);   // Dikurangi ke 500us
    }
    return total / 10;
}

float getMQ2PPM() {
    int adcValue = readAnalogDebounced(MQ2PIN);
    
    // 1. Hitung Voltase Sensor
    float voltaseESP = adcValue * (3.3 / 4095.0);
    float voltaseSensor = voltaseESP / VOLTAGE_DIVIDER; // Kompensasi resistor divider
    
    // 2. Hitung RS (Resistansi sensor saat ada asap)
    if (voltaseSensor <= 0.1) return 0;
    float RS_gas = ((5.0 - voltaseSensor) / voltaseSensor) * RL;
    
    // 3. Hitung Ratio (Perbandingan resistansi)
    float ratio = RS_gas / R0;
    if (ratio <= 0) return 0;
    
    // 4. Konversi Ratio ke PPM menggunakan rumus Logaritma
    // Rumus: PPM = 10 ^ ( (log10(ratio) - b) / m )
    double ppm_log = (log10(ratio) - MQ2_INTERCEPT) / MQ2_SLOPE;
    double ppm = pow(10, ppm_log);
    
    // Validasi hasil (tidak boleh negatif)
    return (ppm < 0) ? 0 : (float)ppm;
}

bool isFlameDetected() {
    for (int i = 0; i < 5; i++) {
        if (readAnalogDebounced(IR_PINS[i]) > THRESHOLD_FLAME) return true;
    }
    return false;
}
