#  Sistem Deteksi Kebakaran & Kebocoran Gas Cerdas (IoT & Fuzzy Logic)

Sistem deteksi dini bencana berbasis mikrokontroler yang mengintegrasikan pemantauan lingkungan *real-time* via **IoT (Blynk)** dengan **Logika Fuzzy** untuk pengambilan keputusan yang akurat. Sistem ini dirancang untuk meminimalisir *false alarm* melalui mekanisme verifikasi sensor ganda (**Logika "2-dari-4"**).

---

## Fitur Utama

* **Deteksi Multi-Sensor:** Memantau **empat parameter** kritis: Api IR, Api UV, Gas/Asap, dan Suhu.
* **Logika "2-dari-4" Anti-*False Alarm*:** Alarm fisik (Buzzer/LED) hanya akan aktif jika **minimal dua sumber bahaya** terdeteksi secara bersamaan.
* **Fuzzy Logic Cerdas:** Menggabungkan data **Flame Sensor** dan **DHT22** untuk menentukan derajat risiko kebakaran, menghasilkan respons yang lebih nuansa.
* **Peringatan Bertingkat:**
    * **Level 1 (Waspada):** Notifikasi *push* Blynk jika **satu** sensor terpicu.
    * **Level 2 (Kritis):** Alarm fisik (Buzzer/LED) dan Notifikasi Kritis jika **≥ 2** sensor terpicu.

---

## Komponen & Perangkat Keras

| Kategori | Komponen | Fungsi Utama |
| :--- | :--- | :--- |
| **Mikrokontroler** | ESP32 / ESP8266 | Pemrosesan logika dan konektivitas Wi-Fi. |
| **Sensor Api** | Flame Sensor (Infrared) | Deteksi api berbasis sinyal IR (Input Fuzzy Logic). |
| **Sensor Api Cepat** | **GUVA-S12SD (UV)** | Deteksi emisi sinar UV api (Thresholding Instan). |
| **Sensor Gas** | MQ-2 | Deteksi gas mudah terbakar (LPG, Asap) (Thresholding Instan). |
| **Sensor Lingkungan**| DHT22 | Pengukuran Suhu & Kelembapan (Input Fuzzy Logic & Hard Threshold). |
| **Output** | Buzzer Aktif & LED | Indikator Peringatan Fisik. |

---

## Perangkat Lunak & Pustaka

Proyek ini dikembangkan menggunakan **Arduino IDE**. Pustaka (Library) wajib diinstal melalui **Arduino Library Manager**.

| Nama Library | Kebutuhan/Fungsi |
| :--- | :--- |
| `BlynkSimpleEsp32` | Koneksi ke platform IoT Blynk. |
| `DHT` & `Adafruit Unified Sensor` | Pembacaan data dari Sensor DHT22. |
| `eFLL` (atau setara) | **Logika Fuzzy** (Fuzzy Inference System). |
| `MQUnifiedsensor` (Opsional) | Memudahkan kalibrasi dan pembacaan MQ-2. |
| **GUVA-S12SD** | **Tidak memerlukan library khusus** (Hanya `analogRead()`). |

### Catatan Penting
* **Kalibrasi MQ-2:** Sensor harus melewati fase *burn-in* (pemanasan awal) dan wajib dikalibrasi di udara bersih untuk menentukan nilai **R0** yang akurat.
* **Thresholds:** Nilai ambang batas dalam kode (misalnya 350 untuk MQ-2 atau 150 untuk UV) harus disesuaikan berdasarkan hasil pengujian dan kalibrasi di lokasi proyek Anda.

---

## Alur Kerja Logika

### 1. Logika Thresholding Instan
Menentukan status bahaya (`True`/`False`) dari setiap sensor berdasarkan ambang batas yang telah ditetapkan:

| Sumber Bahaya | Ambang Batas Pemicu | Status Variabel |
| :--- | :--- | :--- |
| Gas (MQ-2) | `> 350` (ADC) | `Gas_Bahaya` |
| Api UV (GUVA) | `> 150` (ADC) | `UV_Bahaya` |
| Suhu Kritis (DHT22) | `> 50.0°C` | `Suhu_Bahaya` |
| Risiko Fuzzy | `Output Fuzzy >= 80` | `Fuzzy_Bahaya` |

### 2. Logika Keputusan Akhir ("2-dari-4")
Sistem menjumlahkan semua variabel status (`total_bahaya`):

```cpp
int total_bahaya = Gas_Bahaya + UV_Bahaya + Suhu_Bahaya + Fuzzy_Bahaya;

// Logika Alarm Fisik (Kritis - Level 2)
if (total_bahaya >= 2) {
    // AKTIFKAN BUZZER & LED MERAH + Notifikasi Kritis
} 
// Logika Notifikasi (Waspada - Level 1)
else if (total_bahaya == 1) {
    // KIRIM NOTIFIKASI SPESIFIK via Blynk (tanpa alarm fisik)
}
