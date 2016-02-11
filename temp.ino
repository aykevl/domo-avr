

const float celsiusToKelvin = 273.15;

const uint8_t tempPin = 1;

const float seriesResistor = 4790; // Ohm, as measured
// Spec: http://floris.cc/shop/en/general-components/1309-thermistor-ntc-47kohm.html
// It should then be the N_03M00472 NTC Disc Thermistor from AVX
const float thermistorNominalR = 4700; // from spec
const float thermistorNominalK = 25.0 + celsiusToKelvin;
const float thermistorBetaCoefficient = 3950;

unsigned long previousMillis = 0;
const unsigned long avgDelay = 10;
float tempAvg = NAN; // GNU extension
unsigned long tempSum = 0;
uint8_t tempCount = 0;

void tempSetup() {
  pinMode(tempPin, INPUT);
}

float tempRead() {
  return tempCalc(tempReadRaw());
}

float tempReadAvg() {
  if (isnan(tempAvg)) {
    return tempCalc(tempSum / float(tempCount));
  }
  return tempAvg;
}

int tempReadRaw() {
  return analogRead(tempPin);
}

// Calculate the temperature in degrees Kelvin.
// See: https://learn.adafruit.com/thermistor/using-a-thermistor
// Input is the raw temperature from analogRead().
float tempCalc(float input) {
  float thermistorR = seriesResistor / (1023.0 / input - 1.0);
  float temperatureK = 1.0 / ( 1.0 / thermistorNominalK + log(thermistorR / thermistorNominalR) / thermistorBetaCoefficient );
  return temperatureK;
}

void tempLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis > previousMillis + avgDelay) {
    previousMillis = currentMillis;
    tempSum += analogRead(tempPin);
    tempCount++;
    if (tempCount == 0) {
      tempAvg = tempCalc(float(tempSum) / 256.0);
      tempSum = 0;
    }
  }
}
