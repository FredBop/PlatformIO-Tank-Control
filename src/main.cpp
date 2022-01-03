/*
 * Reads an IR Sensor to determine when the water tank needs water added.
 * An IR sensor is mounted to an outer pipe at the level where an floating pipe within the outer pipe will sink to unblock the IR beam when the tank needs water
 * 
 * RULES to ensure that the control loop will not make erradic decisions or leave the water on forever if the sensor malfunctions
 * 1. The routine that reads the sensor will only report a change when a change is read 5 times in a row.
 * 2. Once the water is turned on or off it will stay on or off for a WATER_ON_TIME or WATER_OFF_TIME period of time, currently set at 10 minutes.
 * 3. If the water is on for more than a time limit, then it will be turned off and will not turn back on until the controller board is reset. 
 *    Reset would occur when the pump is turned off and then back on.
 *    This is intended to prevent a massive overflow if the curcuit malfunctions and should be set to more than the time to fill the tank.
 */


// #define IR_PRINT_ONLY_CHANGES   1     // uncomment this line to see all IR Sensor readings instead of just changes
#include <Arduino.h>

// Declarations:  (these are settings that we'll need to determine the correct value for
#define  TANKVALVE_1              7    // Relay 1 is controlled by digital pin 7
#define  TANKVALVE_2              6    // Relay 2 is controlled by digital pin 6
#define  SPRINKLERVALVE_1         5    // Relay 3 is controlled by digital pin 5
#define  SPRINKLERVALVE_2         4    // Relay 4 is controlled by digital pin 4

// IR sensor definitions
#define LEDPIN 13
  // Pin 13: Arduino has an LED connected on pin 13
  // Pin 11: Teensy 2.0 has the LED on pin 11
  // Pin  6: Teensy++ 2.0 has the LED on pin 6
  // Pin 13: Teensy 3.0 has the LED on pin 13
 
#define HIGH_DETECT_SENSORPIN 8
#define LOW_DETECT_SENSORPIN 12

#define MAX_CONSECUTIVE_WATER_ON_INTERVALS  60
const int PressureSensorPin = A0;

enum fillState {FILLING_ON, FILLING_OFF, FAILED_SHUTDOWN};

#define WATER_DEPTH_LOW     1000        // the value returned by the sensor when the water value is low
#define WATER_DEPTH_FULL    5000        // the vakue returned by the sensor when the tank is full

int   waterOnCounter = 0;
int waterDepth;
fillState currentState = FILLING_OFF;
    
int GetWaterDepthFromSensor()
{
  int sensorValue;
  // Get the depth of the water from the pressure sensor
  sensorValue = analogRead(PressureSensorPin);
  Serial.print("Pressure Sensor Value = ");
  Serial.println(sensorValue);
  return sensorValue;
}


void WaterOn()
{  //  Activate all four relays
   digitalWrite(TANKVALVE_1, HIGH);  // setting OE to low ensures that the shift register outputs are enabled
   digitalWrite(TANKVALVE_2, HIGH);  // setting OE to low ensures that the shift register outputs are enabled
   delay(5000);
   digitalWrite(SPRINKLERVALVE_1, HIGH);  // setting OE to low ensures that the shift register outputs are enabled
   digitalWrite(SPRINKLERVALVE_2, HIGH);  // setting OE to low ensures that the shift register outputs are enabled
  }


void WaterOff() 
{  //  Deactivate all four relays
  for (int count = 0; count < 15; ++count)
  {
    delay(60000);    // fill the tank for 15 more minutes before turning of the valves
  }
   digitalWrite(SPRINKLERVALVE_1, LOW);  // setting OE to low ensures that the shift register outputs are enabled
   digitalWrite(SPRINKLERVALVE_2, LOW);  // setting OE to low ensures that the shift register outputs are enabled
   delay(60000);    // let the water line drain into the tank
   digitalWrite(TANKVALVE_1, LOW);  // setting OE to low ensures that the shift register outputs are enabled
   digitalWrite(TANKVALVE_2, LOW);  // setting OE to low ensures that the shift register outputs are enabled
 
}

void setup() 
{  // put your setup code here, to run once:
  Serial.begin(9600);

  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);      
  
  //  Setup 4 output pins for the Relay board
  pinMode(TANKVALVE_1, OUTPUT);
  pinMode(TANKVALVE_2, OUTPUT);
  pinMode(SPRINKLERVALVE_1, OUTPUT);
  pinMode(SPRINKLERVALVE_2, OUTPUT);
}

// Psudo Code for loop()
// Switch Filling State.. is on, off or failed
//    Case: FILLING_OFF
//        If (water depth is too low)
//            Set the state to FILLING_ON
//            Turn on the water
//    Case: FILLING_ON
//        If (water depth is full)
//            Set the state to FILLING_OFF
//            Turn off the water
//        elseif (Filling Checkpoint Interval has expired)
//            If (the water level is not rising)
//                Set the state to FAILED_SHUTDOWN
//                Turn off the water
//            Else
//                Start a new Checkpoint Interval
//    Case: FAILED_SHUTDOWN
//        Do nothing, only a power cycle can reset a FAILED_SHUTDOWN
//         

void loop() 
{
  //  Read the depth of the water from the sensor
  //  waterDepth = GetWaterDepth(waterDepth);
  waterDepth = GetWaterDepthFromSensor();

  // Switch Filling State.. is on, off or failed
  switch (currentState)
  {
    case FILLING_OFF :
    if (waterDepth < WATER_DEPTH_LOW)
    { // If (water depth is too low)
      // Set the state to FILLING_ON
      currentState = FILLING_ON;
      // Turn on the water
      WaterOn();
    }
    break;
    case FILLING_ON :
    if (waterDepth >= WATER_DEPTH_FULL)
    {  // If (water depth is full)
       // Set the state to FILLING_OFF
       currentState = FILLING_OFF
//            Turn off the water
//        elseif (Filling Checkpoint Interval has expired)
//            If (the water level is not rising)
//                Set the state to FAILED_SHUTDOWN
//                Turn off the water
//            Else
//                Start a new Checkpoint Interval
      break;

     case FAILED_SHUTDOWN :
//        Do nothing, only a power cycle can reset a FAILED_SHUTDOWN
      break;
  }  // end switch
  delay(6000);  // This sets the delay time for each loop 
}  // end loop

/*
void loop() 
{
   //  Read the depth of the water from the sensor
//  waterDepth = GetWaterDepth(waterDepth);
  waterDepth = GetWaterDepthFromSensor();
  

  
  //  If depth is less than the minimum then turn on both valves, hysteresis 
  if(waterDepth <= WATER_DEPTH_LOW)
  {
   Serial.println("Water Depth is Low");
    if (waterOnCounter < MAX_CONSECUTIVE_WATER_ON_INTERVALS)
    {
      ++waterOnCounter;  // count the interval
      Serial.print("Water ON Counter ");
      Serial.println(waterOnCounter);
      Serial.println("Water ON");
      WaterOn();
      currentState = WATER_ON;

      // Run a test to make sure that the tank is filling up
      waterDepth1 = GetWaterDepthFromSensor();
      Serial.print(waterDepth1)
     delay(300000)
      waterDepth2 = GetWaterDepthFromSensor(); 
      Serial.print(waterDepth2)
      if (waterDepth1 >= waterDepth2)      
      {  // The tank is not filling, so something is wrong
         Serial.print("WaterOff")
        currentState = WATER_OFF;
      }
    }
    else 
    {
      Serial.println("MAX_WATER_LOW_INTERVALS exceeded, turning off the water");     
      WaterOff();
      currentState = WATER_OFF;
    }
  }
  
   //  If depth indicates that water is full, then turn off both valves
   if(waterDepth >= WATER_DEPTH_FULL)
   {
      Serial.println("Water Depth is Full");
      Serial.println("Water OFF");
      WaterOff();
      currentState = WATER_OFF;
      if (waterOnCounter < MAX_CONSECUTIVE_WATER_ON_INTERVALS)
          waterOnCounter = 0;  // reset the water on interval counter unless we hit the max indicating a fault

//    if (currentState == WATER_DEPTH_LOW)
//      delay(WATER_OFF_TIME);   // if we turn the water from on to off, leave it off for 10 minutes before checking again

    }
 
   delay(6000);  // This sets the loop time when the tank is just full or when the MAX_WATER_ON_INTERVALS fault is detected
}
*/

