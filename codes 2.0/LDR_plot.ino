#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SPI.h>
#include <SD.h>
// Pin assignments:
const int muxControlPins[] = {1, 2, 3}; // Digital pins for MUX control (CD4051)
const int sdChipSelectPin = 4;          // SD card chip select pin

// Data storage:
#define NUM_PHOTORESISTORS 6
int sensorValues[NUM_PHOTORESISTORS];     // Array to store raw sensor readings
float filteredValues[NUM_PHOTORESISTORS]; // Array to store filtered sensor values

// Filtering parameters:
const int numSamples = 10;      // Number of samples for averaging (consider noise levels)
const int medianWindowSize = 3; // Size of window for median filter (experiment)

// Calibration factors (if applicable):
float calibrationFactors[NUM_PHOTORESISTORS] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // Modify if calibration is needed

// Timer for sampling rate control:
unsigned long prevMillis = 0;
const int samplingInterval = 0; // Adjust based on desired frequency (ms)
#define ADXL345_ADDR (0x53)     // ADXL345 I2C address

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(ADXL345_ADDR);

#define alpha 0.95 // Complementary filter constant

File gforceDataFile;
void setup()
{
  // Set up serial communication for debugging (optional):
  Serial.begin(9600);

  if (!accel.begin())
  {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1)
      ;
  }

  gforceDataFile = SD.open("gforce_data.csv", FILE_WRITE);
  if (!gforceDataFile)
  {
    Serial.println("Error opening gforce_data.csv for writing!");
    return;
  }

  gforceDataFile.println("G-force");
  // Initialize SD card:
  pinMode(sdChipSelectPin, OUTPUT);
  if (!SD.begin(sdChipSelectPin))
  {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Set up MUX control pins as outputs:
  for (int i = 0; i < sizeof(muxControlPins) / sizeof(muxControlPins[0]); i++)
  {
    pinMode(muxControlPins[i], OUTPUT);
  }
}

void loop()
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
  if (gforceDataFile)
  {
    gforceDataFile.println(gforce);
  }
  else
  {
    Serial.println("Error writing to gforce_data.csv!");
  }
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
  // Control sampling rate using timer:
  if (millis() - prevMillis >= samplingInterval)
  {
    prevMillis = millis();

    // Read data from each photoresistor:
    for (int i = 0; i < NUM_PHOTORESISTORS; i++)
    {
      selectMUXChannel(i);              // Select the appropriate MUX channel
      sensorValues[i] = analogRead(A1); // Read raw sensor value
    }

    // Apply median filter for robustness against noise (consider experimenting):
    for (int i = 0; i < NUM_PHOTORESISTORS; i++)
    {
      filteredValues[i] = movingAverageFilter(sensorValues[i], medianWindowSize);
    }

    // Apply calibration if needed:
    for (int i = 0; i < NUM_PHOTORESISTORS; i++)
    {
      filteredValues[i] *= calibrationFactors[i];
    }

    // Log data to SD card in Excel format:
    logDataToSDCard();
  }
}

void selectMUXChannel(int channel)
{
  // Implement logic to set MUX control pins based on CD4051 truth table
  // Example truth table for CD4051:
  // A   B   C   Channel
  // 0   0   0   0
  // 0   0   1   1
  // 0   1   0   2
  // 0   1   1   3
  // 1   0   0   4
  // 1   0   1   5
  // 1   1   0   6
  // 1   1   1   7
  // Implement logic to set MUX control pins based on CD4051 truth table
  // For CD4051, the channel selection requires setting three control pins based on binary representation of the channel number
  digitalWrite(muxControlPins[0], bitRead(channel, 0));
  digitalWrite(muxControlPins[1], bitRead(channel, 1));
  digitalWrite(muxControlPins[2], bitRead(channel, 2));
}

void movingAverageFilter(int value, int windowSize)
{
  static int window[NUM_SAMPLES];
  static int currentIndex = 0;

  // Add the new value to the window
  window[currentIndex] = value;

  // Update the current index for circular buffer
  currentIndex = (currentIndex + 1) % windowSize;

  // Compute the moving average
  int sum = 0;
  for (int i = 0; i < windowSize; i++)
  {
    sum += window[i];
  }
  filteredValue = sum / windowSize;
}

void logDataToSDCard()
{
  // Create a new file or append to an existing one:
  File dataFile = SD.open("photoresistor_data.csv", FILE_WRITE | FILE_APPEND);
  if (dataFile)
  {
    // Write header row if the file is new:
    if (!dataFile.position())
    {
      dataFile.println("Timestamp,LDR1,LDR2,LDR3,LDR4,LDR5,LDR6");
    }

    // Write current timestamp and sensor values:
    dataFile.print(millis());
    for (int i = 0; i < NUM_PHOTORESISTORS; i++)
    {
      dataFile.print(",");
      dataFile.print(filteredValues[i]);
    }
    dataFile.println();

    dataFile.close();
  }
  else
  {
    Serial.println("Failed to open SD card file for writing!");
  }
}