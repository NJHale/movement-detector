/**
* movement-detector.cpp
*/

#include <Arduino.h>

const boolean micro_is_5V = false;

const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;

const int minJerk = 4;

// Current acceleration vector - <x'', y'', z''>
// float acc[3];
int prev[3];

// Base acceleration at rest
int baseAcc[3];


/**
 * Return an array that's the absolute difference of arrays a and b : |b - a|
 * assuming a and b are of the same length
 */
void absDifference(int (&a)[3], int (&b)[3], int (&c)[3]) {
  for (int i = 0; i < 3; i++) {
    c[i] = abs(b[i] - a[i]);
  }
}

void setColor(int rgb[3])
{
  int red = rgb[0];
  int green = rgb[1];
  int blue = rgb[2];

  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  // Red pin
  analogWrite(RED_PIN, red);
  // Green pin
  analogWrite(GREEN_PIN, green);
  // Blue pin
  analogWrite(BLUE_PIN, blue);
}

void setup()
{
  // Initialize serial communication at 115200 baud
  Serial.begin(9600);

  Serial.println("Setting base acceleration, stop device movement until LED turns green...");
  int palette[] = {148, 0, 211};
  // Set the led to purple
  setColor(palette);

  // Get the base acceleration sample
  prev[0] = analogRead(A0);
  prev[1] = analogRead(A1);
  prev[2] = analogRead(A2);

  // Get a second read 25 ms later
  delay(25);

  baseAcc[0] = analogRead(A0);
  baseAcc[1] = analogRead(A1);
  baseAcc[2] = analogRead(A2);

  int jerk[3];
  absDifference(prev, baseAcc, jerk);

  // Continue taking samples until the device has stabilized
  while (jerk[0] > minJerk && jerk[1] > minJerk && jerk[2] > minJerk) {
    // Copy baseAcc to prev
    memcpy(prev, baseAcc, 3);
    // Wait 25 ms
    delay(25);
    // Get another reading
    baseAcc[0] = analogRead(A0);
    baseAcc[1] = analogRead(A1);
    baseAcc[2] = analogRead(A2);
    // Get the jerk for this reading
    absDifference(prev, baseAcc, jerk);
  }

  // Copy base to prev
  memcpy(prev, baseAcc, 3);

  Serial.println("Base acceleration set!");
  // Change palette to dark olive green to signify baseAcc set
  palette[0] = 85;
  palette[1] = 107;
  palette[2] = 47;

  setColor(palette);
  delay(2000);
  // Clear the palette
  memset(palette, 0, sizeof(palette));
  // Turn off the Cathode RGB LED
  setColor(palette);
  // Initialize position at 0, 0, 0
  //for (int i = 0; i < 3; i++) { acc[i] = 0; }
}

void loop() {
  // put your main code here, to run repeatedly


  // // Instantiate the next position
  // float next[] = {ddx, ddy, ddz};
  //
  // float diff[3];
  // // Go get the difference
  // difference(acc, next, diff);

  // Get the next acceleration sample
  int ddx = analogRead(A0);
  int ddy = analogRead(A1);
  int ddz = analogRead(A2);

  int next[] = {ddx, ddy, ddz};

  Serial.print("x'': "); Serial.println(next[0]);
  Serial.print("y'': "); Serial.println(next[1]);
  Serial.print("z'': "); Serial.println(next[2]);

  // Calculate the absolute difference between the base and next acceleration samples
  int diff[3];
  absDifference(baseAcc, next, diff);

  Serial.print("acceleration x: "); Serial.println(diff[0]);
  Serial.print("acceleration y: "); Serial.println(diff[1]);
  Serial.print("acceleration z: "); Serial.println(diff[2]);


  // Calculate the palette based on the change in acceleration
  int palette[] = {0, 0, 0};

  for (int i = 0; i < 3; i++) {
    if(diff[i] > minJerk) {
      // palette[i] = 255;
      palette[i] = next[i] % 255;
    }
  }

  Serial.print("red: "); Serial.println(palette[0]);
  Serial.print("green: "); Serial.println(palette[1]);
  Serial.print("blue: "); Serial.println(palette[2]);

  setColor(palette);

  // Overwrite prev's values with next's
  memcpy(prev, next, 3);

  delay(25);


}
