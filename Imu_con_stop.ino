#include "FastIMU.h"
 #include <Wire.h>
 #include <SD.h>
 #define IMU_ADDRESS 0x68    //Change to the address of the IMU
 #define PERFORM_CALIBRATION //Comment to disable startup calibration
 MPU6500 IMU;               //Change to the name of any supported IMU! 
 File dataFile;
 // Currently supported IMUS: MPU9255 MPU9250 MPU6886 MPU6500 MPU6050 ICM20689 ICM20690 BMI055 BMX055 BMI160 LSM6DS3 LSM6DSL QMI8658

 calData calib = { 0 };  //Calibration data
 AccelData accelData;    //Sensor data
 GyroData gyroData;
 MagData magData;

 bool acquisizioneAttiva = true; // Variabile per gestire l'acquisizione

 // Funzione per impostare il range dell'IMU
 void setIMURange() 
  {
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

  void setup() 
{
  Wire.begin();
  Wire.setClock(400000); //400khz clock
  Serial.begin(115200);
  while (!Serial) 
  {
    ;
  }

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) 
  {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    while (true) 
    {
      ;
    }
  }

  // Imposta il range dell'IMU
  setIMURange();

 #ifdef PERFORM_CALIBRATION
  Serial.println("FastIMU calibration & data example");
  if (IMU.hasMagnetometer()) 
    {
    delay(1000);
    Serial.println("Move IMU in figure 8 pattern until done.");
    delay(3000);
    IMU.calibrateMag(&calib);
    Serial.println("Magnetic calibration done!");
    }
   else 
    {
    delay(5000);
    }

  delay(5000);
  Serial.println("Keep IMU level.");
  delay(5000);
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibration done!");
  Serial.println("Accel biases X/Y/Z: ");
  Serial.print(calib.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib.accelBias[2]);
  Serial.println("Gyro biases X/Y/Z: ");
  Serial.print(calib.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib.gyroBias[2]);

  if (IMU.hasMagnetometer()) 
  {
    Serial.println("Mag biases X/Y/Z: ");
    Serial.print(calib.magBias[0]);
    Serial.print(", ");
    Serial.print(calib.magBias[1]);
    Serial.print(", ");
    Serial.println(calib.magBias[2]);
    Serial.println("Mag Scale X/Y/Z: ");
    Serial.print(calib.magScale[0]);
    Serial.print(", ");
    Serial.print(calib.magScale[1]);
    Serial.print(", ");
    Serial.println(calib.magScale[2]);
  }
  delay(5000);
  IMU.init(calib, IMU_ADDRESS);
  #endif

  if (err != 0) 
    {
    Serial.print("Error Setting range: ");
    Serial.println(err);
    while (true) 
    {
      ;
    }
   }
}

float alpha = 0.01;  // Fattore di smorzamento
float accelX, accelY, accelZ;
float filteredX = 0, filteredY = 0, filteredZ = 0;

void loop() 
{
  // Controllo del comando inviato dal monitor seriale
  if (Serial.available() > 0) 
  {
    char comando = Serial.read();
    
    if (comando == 'S') 
    {  
      // Ferma acquisizione
      acquisizioneAttiva = false;
      Serial.println("Acquisizione fermata.");
    } 
    else if (comando == 'R') 
    {  
      // Riprende acquisizione
      acquisizioneAttiva = true;
      Serial.println("Acquisizione ripresa.");
    }
  }

  // Se l'acquisizione è attiva, continua a raccogliere i dati
  if (acquisizioneAttiva) 
  {
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

    accelX = accelData.accelX;
    accelY = accelData.accelY;
    accelZ = accelData.accelZ;

    // Applicazione del filtro passa-basso

    filteredX = alpha * accelX + (1 - alpha) * filteredX;
    filteredY = alpha * accelY + (1 - alpha) * filteredY;
    filteredZ = alpha * accelZ + (1 - alpha) * filteredZ;
    
    Serial.print("\t");
    Serial.print(filteredX);
    Serial.print("\t");
    Serial.print(filteredY);
    Serial.print("\t");
    Serial.print(filteredZ);

    if (IMU.hasMagnetometer()) 
    {
      IMU.getMag(&magData);
      Serial.print("\t");
      Serial.print(magData.magX);
      Serial.print("\t");
      Serial.print(magData.magY);
      Serial.print("\t");
      Serial.print(magData.magZ);
    }
    if (IMU.hasTemperature()) 
    {
      Serial.print("\t");
      Serial.println(IMU.getTemp());
    }
    else 
    {
      Serial.println();
    }
    delay(50);  // Frequenza di acquisizione
  }
}