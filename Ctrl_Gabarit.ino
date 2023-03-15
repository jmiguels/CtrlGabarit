/************************************************************
Gabarit Control Unit
JMSantos 2023
*************************************************************/

#include "Arduino.h"

char VersaoStr[] = "V0.01 15/03/2023";

//############# PINs definitions #############

#define SDA_A      21  // Set i2c pin
#define SCL_A      22  // Set i2c pin
#define BUTTON1    36
#define BUTTON2    39
#define LED_PIN     2
#define Buzzer      4  
#define Relay1     14  
#define Relay2     27  
#define Solenoid1  19  
#define Solenoid2  18  
#define Solenoid3   5  
#define Solenoid4  15  
#define Solenoid5  13  
#define Solenoid6  25  
//###################################################

//######### CLInterface #############################
String Commandstr = "";
int CommandStrIsComplete=0;
//###################################################

//############# Adressable LEDS WS2812B #############
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS,LED_PIN, NEO_GRB + NEO_KHZ800);
//###################################################

//############################## BUZZER
#define PWM_resolution    8  //number of bits used to define the duty cycle (can be from 8 to 16)
#define PWM3_Ch           2  //Definição do canal de PWM
#define PWM4_Ch           3  //Definição do canal de PWM
//###################################################

void setup()
{
  int n;
  
	Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Solenoid1, OUTPUT); 
  pinMode(Solenoid2, OUTPUT); 
  pinMode(Solenoid3, OUTPUT);
  pinMode(Solenoid4, OUTPUT);
  pinMode(Solenoid5, OUTPUT);
  pinMode(Solenoid6, OUTPUT);
  
  ledcAttachPin(Buzzer, PWM3_Ch); //Assign a PWM channel for the Buzzer  
  
  for(n=0;n<NUM_LEDS;n++)
    LedCtrl(n, 0,255,0);

  PlayBuzzer(); //Play the buzzer
  delay(500);
 
  delay(1000);
  for(n=0;n<NUM_LEDS;n++)
    LedCtrl(n, 255,0,0);
  
  delay(1000);
  for(n=0;n<NUM_LEDS;n++)
    LedCtrl(n, 0,0,0);
}

void loop()
{
  CLInerface(); // Client Line Interface processing over the serial port
  Relay(1, true);
  Solenoid(1, true);
  delay(1000);
  Relay(1, false);
  Solenoid(1, false);
  delay(1000);

}
