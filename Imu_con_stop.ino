#include "FastIMU.h"
#include <Wire.h>
#include <SD.h>

#define IMU_ADDRESS 0x68    // Cambia con l'indirizzo della tua IMU
#define PERFORM_CALIBRATION // Commenta per disabilitare la calibrazione all'avvio

// Definizioni dei registri per la configurazione del MPU6500
#define MPU6500_ACCEL_CONFIG 0x1C
#define MPU6500_GYRO_CONFIG  0x1B

MPU6500 IMU;               // Cambia con il nome di qualsiasi IMU supportata
File dataFile;

// IMU supportate attualmente: MPU9255, MPU9250, MPU6886, MPU6500, MPU6050, ICM20689, ICM20690, BMI055, BMX055, BMI160, LSM6DS3, LSM6DSL, QMI8658

calData calib = { 0 };    // Dati di calibrazione
AccelData accelData;      // Dati dell'accelerometro
GyroData gyroData;        // Dati del giroscopio
MagData magData;          // Dati del magnetometro

bool acquisizioneAttiva = true; // Variabile per gestire l'acquisizione

// Funzione per impostare il range dell'IMU
void setIMURange() {
  // Imposta accelerometro a ±4g
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(MPU6500_ACCEL_CONFIG);
  Wire.write(0x08); // Valore per ±4g (secondo il datasheet)
  Wire.endTransmission();

  // Imposta giroscopio a ±500°/s
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(MPU6500_GYRO_CONFIG);
  Wire.write(0x08); // Valore per ±500°/s (secondo il datasheet)
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Wire.setClock(400000); // Clock a 400kHz
  Serial.begin(115200);
  while (!Serial) {
    ; // Attende che la porta seriale sia pronta
  }

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Errore nell'inizializzazione dell'IMU: ");
    Serial.println(err);
    while (true) {
      ; // Blocca l'esecuzione in caso di errore
    }
  }

  // Imposta il range dell'IMU
  setIMURange();

  // Opzionale: Verifica se l'impostazione del range ha generato errori
  // Nota: La funzione setIMURange() non restituisce errori. Se desideri gestire gli errori, dovresti modificare la funzione per restituire un valore di stato.

#ifdef PERFORM_CALIBRATION
  Serial.println("Esempio di calibrazione e dati FastIMU");
  if (IMU.hasMagnetometer()) {
    delay(1000);
    Serial.println("Muovi l'IMU in un pattern a figura 8 fino al completamento.");
    delay(3000);
    IMU.calibrateMag(&calib);
    Serial.println("Calibrazione magnetica completata!");
  }
  else {
    delay(5000);
  }

  delay(5000);
  Serial.println("Mantieni l'IMU a livello.");
  delay(5000);
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibrazione completata!");
  Serial.println("Bias accelerometro X/Y/Z: ");
  Serial.print(calib.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib.accelBias[2]);
  Serial.println("Bias giroscopio X/Y/Z: ");
  Serial.print(calib.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib.gyroBias[2]);
  if (IMU.hasMagnetometer()) {
    Serial.println("Bias magnetometro X/Y/Z: ");
    Serial.print(calib.magBias[0]);
    Serial.print(", ");
    Serial.print(calib.magBias[1]);
    Serial.print(", ");
    Serial.println(calib.magBias[2]);
    Serial.println("Scale magnetometro X/Y/Z: ");
    Serial.print(calib.magScale[0]);
    Serial.print(", ");
    Serial.print(calib.magScale[1]);
    Serial.print(", ");
    Serial.println(calib.magScale[2]);
  }
  delay(5000);
  IMU.init(calib, IMU_ADDRESS);
#endif

  // Controllo finale degli errori (opzionale, puoi rimuoverlo se non necessario)
  if (err != 0) {
    Serial.print("Errore nell'impostazione del range: ");
    Serial.println(err);
    while (true) {
      ; // Blocca l'esecuzione in caso di errore
    }
  }
}

void loop() {
  // Controllo del comando inviato dal monitor seriale
  if (Serial.available() > 0) {
    char comando = Serial.read();
    if (comando == 'S') {  // Ferma acquisizione
      acquisizioneAttiva = false;
      Serial.println("Acquisizione fermata.");
    } else if (comando == 'R') {  // Riprende acquisizione
      acquisizioneAttiva = true;
      Serial.println("Acquisizione ripresa.");
    }
  }

  // Se l'acquisizione è attiva, continua a raccogliere i dati
  if (acquisizioneAttiva) {
    IMU.update();
    IMU.getAccel(&accelData);
    Serial.print(accelData.accelX);
    Serial.print("\t");
    Serial.print(accelData.accelY);
    Serial.print("\t");
    Serial.print(accelData.accelZ);
    Serial.print("\t");
    IMU.getGyro(&gyroData);
    Serial.print(gyroData.gyroX);
    Serial.print("\t");
    Serial.print(gyroData.gyroY);
    Serial.print("\t");
    Serial.print(gyroData.gyroZ);
    if (IMU.hasMagnetometer()) {
      IMU.getMag(&magData);
      Serial.print("\t");
      Serial.print(magData.magX);
      Serial.print("\t");
      Serial.print(magData.magY);
      Serial.print("\t");
      Serial.print(magData.magZ);
    }
    if (IMU.hasTemperature()) {
      Serial.print("\t");
      Serial.println(IMU.getTemp());
    }
    else {
      Serial.println();
    }
    delay(50);  // Frequenza di acquisizione (50 ms)
  }
}