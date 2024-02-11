#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SPI.h>
#include <SD.h>

#define ADXL345_ADDR (0x53) // ADXL345 I2C address

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(ADXL345_ADDR);

#define alpha 0.95 // Complementary filter constant

File logfile;

void setup(void)
{
    Serial.begin(9600);
    while (!Serial)
    {
    } // Wait for serial monitor to open

    Serial.println("Initializing SD card...");

    if (!SD.begin(10))
    {
        Serial.println("Initialization failed!");
        return;
    }
    Serial.println("Initialization done.");

    if (!accel.begin())
    {
        Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
        while (1)
            ;
    }
}

void loop(void)
{
    sensors_event_t accel_event;
    accel.getEvent(&accel_event);

    // Convert accelerometer data to acceleration in g-force
    float accelX_g = accel_event.acceleration.x / 9.8;
    float accelY_g = accel_event.acceleration.y / 9.8;
    float accelZ_g = accel_event.acceleration.z / 9.8;

    // Apply complementary filter to combine accelerometer data
    static float pitch_filtered = 0;
    static float roll_filtered = 0;

    // Calculate pitch and roll angles using accelerometer data
    float pitch_acc = atan2(-accelY_g, sqrt(pow(accelX_g, 2) + pow(accelZ_g, 2))) * 180 / PI;
    float roll_acc = atan2(accelX_g, sqrt(pow(accelY_g, 2) + pow(accelZ_g, 2))) * 180 / PI;

    // Apply complementary filter to combine accelerometer and filtered data
    pitch_filtered = alpha * (pitch_filtered + accel_event.acceleration.y) + (1 - alpha) * pitch_acc;
    roll_filtered = alpha * (roll_filtered + accel_event.acceleration.x) + (1 - alpha) * roll_acc;

    // Calculate g-force magnitude
    float gforce = sqrt(pow(accelX_g, 2) + pow(accelY_g, 2) + pow(accelZ_g, 2));

    // Log data to SD card
    if (logfile)
    {
        logfile.print(millis());
        logfile.print(",");
        logfile.print(accel_event.acceleration.x);
        logfile.print(",");
        logfile.print(accel_event.acceleration.y);
        logfile.print(",");
        logfile.print(accel_event.acceleration.z);
        logfile.print(",");
        logfile.print(pitch_filtered);
        logfile.print(",");
        logfile.print(roll_filtered);
        logfile.print(",");
        logfile.print(gforce);
        logfile.println();
    }
    else
    {
        Serial.println("Error opening log file!");
    }

    // Print data to Serial monitor
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print(", Raw X: ");
    Serial.print(accel_event.acceleration.x);
    Serial.print(", Raw Y: ");
    Serial.print(accel_event.acceleration.y);
    Serial.print(", Raw Z: ");
    Serial.print(accel_event.acceleration.z);
    Serial.print(", Filtered Pitch: ");
    Serial.print(pitch_filtered);
    Serial.print(", Filtered Roll: ");
    Serial.print(roll_filtered);
    Serial.print(", G-force: ");
    Serial.println(gforce);

    delay(100); // Adjust delay as needed
}
