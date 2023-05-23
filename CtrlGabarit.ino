/************************************************************
Gabarit Control Unit
JMSantos 2023
*************************************************************/

#include "Arduino.h"
#include <Servo.h> // https://randomnerdtutorials.com/esp32-servo-motor-web-server-arduino-ide/
                   // Biblioteca "ServoESP32"de Jaroslav Paral
#include <Wire.h>  // Necessário para utilizar a biblioteca SparkFun_APDS9960.h
#include <SparkFun_APDS9960.h> //https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

String DeviceTypeStr="GABARIT_CTRL";                              //Device name
String FirmwareVersion="V0.02 - 16/03/2023";                      //Firmware Version
String VersionStr= DeviceTypeStr + " - " + FirmwareVersion; 

//############# PINs definitions #############

#define SDA_A      21  // Set i2c pin
#define SCL_A      22  // Set i2c pin
#define BUTTON1    36
#define BUTTON2    39
#define LED_PIN     2
#define Buzzer      4  
#define Relay1     14  
#define Relay2     27  
#define Relay3     25 // Multiplexado com o Servo7 e com o solenoide 6  
#define USB        13
#define Solenoid1  19  
#define Solenoid2  18  
#define Solenoid3   5  
#define Solenoid4  15  
#define Solenoid5  13  
#define Solenoid6  25

#define Servo1     19
#define Servo2     18
#define Servo3      5
#define Servo4     15
#define Servo5     33
#define Servo6     26
#define Servo7     25

#define D32IN      32 //utilizado para configurar o GND-Servo7 ou o ABERTO-Relé3

#define RXD2 16
#define TXD2 17  

hw_timer_t * timer = NULL; //******** Timer Interrupt definitions

#define RelayPulseDuration        500000   //pulse duration for the relay activation in micro seconds
#define InterruptActivationDelay  1000000  //Delay to activate interrupts again after a command
int LastCommandWasOpen = true;
//###################################################

Servo servo_1;  // create servo object to control a servo
Servo servo_2;  // create servo object to control a servo
Servo servo_3;  // create servo object to control a servo
Servo servo_4;  // create servo object to control a servo
Servo servo_5;  // create servo object to control a servo
Servo servo_6;  // create servo object to control a servo
Servo servo_7;  // create servo object to control a servo

bool Servo7Active=false;

//######### CLInterface #############################
String Commandstr = "";
int CommandStrIsComplete=0;
String finalstr="";
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

//***** Color sensor SparkFun_APDS9960
  SparkFun_APDS9960 apds = SparkFun_APDS9960();
  uint16_t ambient_light = 0;
  uint16_t red_light = 0;
  uint16_t green_light = 0;
  uint16_t blue_light = 0;
//***********************************

void IRAM_ATTR InterruptToOpen()
{
  Open();
}

void IRAM_ATTR InterruptToClose()
{  
  if(Close()==1)
  {
    Serial.println("Emergency pressed");
  }
}

void TimerInterruptToFinishPulse()
{
  timerEnd(timer); //Destroi o timer
  digitalWrite(Relay1,LOW);
  digitalWrite(Relay2,LOW);
  //Timer setup
  timer = timerBegin(0, 80, true); // Timer 0 programado comum prescale de 80 (o relógio é de 80MHz). countup (true) ou countdown (false)
  timerAttachInterrupt(timer, &ActivateInterruptsAgain, true); //Definição da rotina de serviço ao interrupt. edge (true) or level (false)  
  timerAlarmWrite(timer, InterruptActivationDelay, true); //configurar o timer interrupt TimerInterruptToFinishPulse() para daqui a 1s
  timerAlarmEnable(timer); 
}

void ActivateInterruptsAgain()
{
  timerEnd(timer); //Destroi o timer
  EnableButtonInterrupts();
}

void EnableButtonInterrupts()
{
  attachInterrupt(BUTTON1, InterruptToOpen, FALLING);
  attachInterrupt(BUTTON2, InterruptToClose, FALLING);   
}

void DetachButtonInterrupts()
{
  detachInterrupt(BUTTON1);
  detachInterrupt(BUTTON2);  
}

void setup()
{
  int n;
  
	Serial.begin(115200);                                                                                                   
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

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
  pinMode(D32IN, INPUT_PULLUP);


  init_APDS9960();
  
  if(digitalRead(D32IN)==LOW) 
  {
    Serial.println("Configurar servo 7");
    Servo7Active=true;  
  }

  servo_1.attach(Servo1);  // attaches the servo on pin Servo1 to the servo object
  servo_2.attach(Servo2);  // attaches the servo on pin Servo2 to the servo object
  servo_3.attach(Servo3);  // attaches the servo on pin Servo3 to the servo object
  servo_4.attach(Servo4);  // attaches the servo on pin Servo4 to the servo object
  servo_5.attach(Servo5);  // attaches the servo on pin Servo5 to the servo object
  servo_6.attach(Servo6);  // attaches the servo on pin Servo6 to the servo object

  if(Servo7Active) 
  {
    Serial.println("Configurar servo 7");
    servo_7.attach(Servo7);  // Não pode ser utilizado se quisermos utilizar o Relé3
  }
  
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
    LedCtrl(n, 0,0,255);

  delay(1000);
  for(n=0;n<NUM_LEDS;n++)
    LedCtrl(n, 0,0,0);

  EnableButtonInterrupts();
  
  Serial.print("Buton1: ");
  Serial.print(digitalRead(BUTTON1));
  Serial.print(" Buton2: ");
  Serial.print(digitalRead(BUTTON2));
  Serial.println();
  Serial2.println();  //Enviado comando para arduino para limpar buffer.
}

void loop()
{
  CLInerface(); // Client Line Interface processing over the serial port
  if((!LastCommandWasOpen)&&(digitalRead(BUTTON1)==LOW)) //ALTERAR PARA HIGH SE EMERGENGY BUTTON FOR NC
  {
    Open();
    LastCommandWasOpen=true;
  }
}

void Open()
{
  DetachButtonInterrupts();
  //Timer setup
  timer = timerBegin(0, 80, true); // Timer 0 programado comum prescale de 80 (o relógio é de 80MHz). countup (true) ou countdown (false)
  timerAttachInterrupt(timer, &TimerInterruptToFinishPulse, true); //Definição da rotina de serviço ao interrupt. edge (true) or level (false)  
  timerAlarmWrite(timer, RelayPulseDuration, true); //configurar o timer interrupt TimerInterruptToFinishPulse() para daqui a 0.5s
  timerAlarmEnable(timer); 
  
  digitalWrite(Relay2,LOW);  //desativa primeiro o rele1
  digitalWrite(Relay1,HIGH);  //ativa o rele2
  LastCommandWasOpen=true;
}

int Close()
{
  DetachButtonInterrupts();
  if(digitalRead(BUTTON1)==HIGH) //Emergency Button not pressed //ALTERAR PARA LOW SE EMERGENGY BUTTON FOR NC
  {
    //Timer setup
    timer = timerBegin(0, 80, true); // Timer 0 programado comum prescale de 80 (o relógio é de 80MHz). countup (true) ou countdown (false)
    timerAttachInterrupt(timer, &TimerInterruptToFinishPulse, true); //Definição da rotina de serviço ao interrupt. edge (true) or level (false)  
    timerAlarmWrite(timer, RelayPulseDuration, true); //configurar o timer interrupt TimerInterruptToFinishPulse() para daqui a 0.5s
    timerAlarmEnable(timer); 
  
    digitalWrite(Relay1,LOW);   //desativa primeiro o rele1
    digitalWrite(Relay2,HIGH);  //ativa o rele2
    LastCommandWasOpen=false;
    return(0);
  }
  else
  {
    //Timer setup
    timer = timerBegin(0, 80, true); // Timer 0 programado comum prescale de 80 (o relógio é de 80MHz). countup (true) ou countdown (false)
    timerAttachInterrupt(timer, &ActivateInterruptsAgain, true); //Definição da rotina de serviço ao interrupt. edge (true) or level (false)  
    timerAlarmWrite(timer, InterruptActivationDelay, true); //configurar o timer interrupt TimerInterruptToFinishPulse() para daqui a 1s
    timerAlarmEnable(timer); 
    return(1);
  }
}
