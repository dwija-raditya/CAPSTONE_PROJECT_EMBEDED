#ifndef CONFIG_H
#define CONFIG_H

// SENSOR PINS
#define MQ2PIN 34
#define DHT22PIN 4
const int IR_PINS[] = {32, 33, 36, 39, 35};

// MQ-2 CALIBRATION CONSTANTS
#define RL 1.0              // Load Resistor
#define R0 1.1              // Reference Resistance (SESUAIKAN DENGAN HASIL KALIBRASI ANDA!)
#define MQ2_SLOPE -0.473    // Slope dari kurva datasheet
#define MQ2_INTERCEPT 1.413 // Y-Intercept dari kurva datasheet
#define VOLTAGE_DIVIDER 0.6 // Faktor kompensasi resistor divider

// OUTPUT PINS
#define LED_RED 17
#define LED_YELLOW 18
#define LED_GREEN 19
#define BUZZER 16

// THRESHOLDS
#define THRESHOLD_TEMP 40
#define THRESHOLD_SMOKE 35
#define THRESHOLD_FLAME 1500

// BLYNK WATCHDOG
#define CONNECT_TIMEOUT_MS 15000UL
#define MAX_FAILURES 2

#endif
