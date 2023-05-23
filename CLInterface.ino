void CLInerface() 
{  
  String strNew;
  int n;

  if(CommandStrIsComplete==0)
  {
    if(Serial.available() > 0)
    { 
      Serial.setTimeout(100),
      strNew = Serial.readString()  ;
      Serial.print(strNew);
      Commandstr = Commandstr + strNew;
    
      for(n=0; n<Commandstr.length();n++) //Backspace implementation
        if((Commandstr[n]==127))
          if(n>0)
            Commandstr=Commandstr.substring(0,n-1)+Commandstr.substring(n+1,Commandstr.length());
          else
          {
            Commandstr="";
            Serial.print(">");
          }//End of backspace implementation 
    }
  
    if ((Commandstr[Commandstr.length()-1]==10)||(Commandstr[Commandstr.length()-1]==13)) //accepts CR or LF at the end of the command
    {
      Serial.println();
      Commandstr = Commandstr.substring(0,Commandstr.length()-1);
      CommandStrIsComplete=1;
    }
    if ((Commandstr.length()>0 && (Commandstr[0]<63 || Commandstr[0]>90 || Commandstr[0]==64)) || (Commandstr.length()==3 && Commandstr[1] != 44) || (Commandstr.length()>3 && ((Commandstr[0] != 87 && Commandstr[0] != 67 && Commandstr[0] != 83 && Commandstr[0] != 77 && Commandstr[0] != 76) || Commandstr[1] != 44)))
    {
      Serial.println("Invalid command\r\n");
      Serial.print(">");
      Commandstr="";
      CommandStrIsComplete=0;
    }
  }
  else
  {
    switch (Commandstr[0])
    {
      //case 32: //Space
      //  DoNothing();
      case 63: //?
        Help();
        break;
      case 72: //H
        Help();
        break;
      case 82: //R
        SystemReset();
        break;
      case 86: //V
        DeviceVersion();
        break;
      case 84: //T
        DeviceType();
        break;
      case 80: //P 
        PowerSwitch(Commandstr);
        break;
      case 85: //U 
        USB_ctrl(Commandstr);
        break;
      case 87: //W
        KeyboardEmulator(Commandstr, 2500);
        break;
      case 67: //C
        KeyboardEmulator(Commandstr, 2500);
        break;
      case 83: //S
        SetServo(Commandstr);
        break;
      case 77: //M
        RelayControl(Commandstr);
        break;
      case 76: //L
        LedControl(Commandstr);
        break;
      case 66: //B
        BuzzerControl();
        break;
      case 78: //N
        KeyboardEmulatorLine(Commandstr);
        break;
      case 71: //G
        GetLedColor();
        break;
      case 65: //A
        OpenGabarit();
        break;
      case 70: //F
        CloseGabarit();
        break;
      default:
        Serial.println("Invalid command\r\n");
        break;
    }
  Serial.print(">");
  Commandstr="";
  CommandStrIsComplete=0;
  }
}

void SystemReset()
{
/*
  Serial.println("Killing AccelTask");
  vTaskDelete(Handle_AccelTask);
  digitalWrite(Power_En1, LOW); //Turn power 4V OFF
  digitalWrite(IO15_3V3_EN, LOW); //Turn power 3.3V OFF
  Serial.println("Periferals were powered off");
  delay(2000); //Power down all the periferals during 2 second before powering UP. User for software reset  
*/ 
  ESP.restart();
  Serial.print("r,ACK");
}

void DoNothing()
{
}

void DeviceVersion()
{
  Serial.print("v,ACK,"); 
  Serial.println(FirmwareVersion);
}

void DeviceType()
{
  Serial.print("t,ACK,"); 
  Serial.println(DeviceTypeStr);
}

void PowerSwitch(String Commandstr)
{
  if (Commandstr.length()!=3)
  {
    Serial.println("p,NACK,Invalid command");
  }
  else if (Commandstr[2] == 48 || Commandstr[2] == 49)
  {
    if (Commandstr[2] == 48)
    {
      Relay(3, false);
      Serial.println("p,ACK,0");
    }
    else
    {
      Relay(3, true);
      Serial.println("p,ACK,1");
    }
  }
  else
  {
    Serial.print("p,NACK,Invalid position");
  }
}

void USB_ctrl(String Commandstr)
{
  if (Commandstr.length()!=3)
  {
    Serial.println("u,NACK,Invalid command");
  }
  else if (Commandstr[2] == 48 || Commandstr[2] == 49)
  {
    if (Commandstr[2] == 48)
    {
      digitalWrite(USB,LOW);
      Serial.println("u,ACK,0");
    }
    else
    {
      digitalWrite(USB,HIGH);
      Serial.println("u,ACK,1");
    }
  }
  else
  {
    Serial.print("p,NACK,Invalid position");
  }
}

void KeyboardEmulator(String Commandstr, unsigned long timeout)
{
  char dec_char;
  String expectedstr;
  if (Commandstr[0] == 87)
  {
    if (Commandstr.length()<3 and Commandstr[1] != 44)
    {
      Serial.println("w,NACK,Invalid command");
    }
    else if (Serial2)
    {
      String command = Commandstr + "\r\n";
      
      Serial2.write(command.c_str());
      unsigned long start_time = millis();
      while(true)
      {
        if (millis()-start_time>timeout)
        {
          Serial.println("w,NACK,Timeout exceded");
          break;
        }
        if (Serial2.available()) 
        {
          dec_char = Serial2.read();
          if (dec_char==10) //if newline
          {  
            expectedstr = "w,ACK," + Commandstr.substring(2,Commandstr.length());
            if (finalstr == expectedstr)
            {
              Serial.println(expectedstr);
            }
            else
            {
              Serial.print("w,NACK,Something went wrong sending Keyboard key");
              Serial.println(finalstr);
            }
            finalstr="";
            break;
          }
          else if (dec_char!=13)
          {
            finalstr += (char)dec_char;
          }  
        } 
      }
    }
    else
    {
      Serial.println("w,NACK,Impossible to communicate with Keyboard Emulator");
    }
  }
  else
  {
    if (Commandstr.length()<3)
    {
      Serial.println("c,NACK,Invalid command");
    }
    else if (Serial2)
    {
      String command = Commandstr + "\r\n";
      
      Serial2.write(command.c_str());
      unsigned long start_time = millis();
      while(true)
      {
        if (millis()-start_time>timeout)
        {
          Serial.println("c,NACK,Timeout exceded");
          break;
        }
        if (Serial2.available()) 
        {
          dec_char = Serial2.read();
          if (dec_char==10) //if newline
          {  
            expectedstr = "c,ACK," + Commandstr.substring(2,Commandstr.length());
            if (finalstr == expectedstr)
            {
              Serial.println(expectedstr);
            }
            else
            {
              Serial.print("c,NACK,Something went wrong sending Keyboard key - ");
              Serial.println(finalstr);
            }
            finalstr="";
            break;
          }
          else if (dec_char!=13)
          {
            finalstr += (char)dec_char;
          }  
        } 
      }
    }
    else
    {
      Serial.println("c,NACK,Impossible to communicate with Keyboard Emulator");
    }
  }
}

void KeyboardEmulatorLine(String Commandstr)
{
  
  if (Commandstr.length()!=3)
  {
    Serial.println("n,NACK,Invalid command");
  }
  else if (Commandstr[2] == 48 || Commandstr[2] == 49)
  { 
    if (Serial2)
    {
      String command = "L," + String(Commandstr[2]) + "\r\n";
      Serial2.write(command.c_str());
      delay(100);
      while (Serial2.available()){
          char dec_char = Serial2.read();
          if (dec_char==10) //if newline
          {  
            String expectedstr = "l,ACK," + String(Commandstr[2]);
            if (finalstr == expectedstr)
            {
              Serial.println("n,ACK," + String(Commandstr[2]));
            }
            else
            {
              Serial.println("n,NACK,Something went wrong configuring line feed");
            }
            finalstr="";
            break;
          }
          else if (dec_char!=13)
          {
            finalstr += (char)dec_char;
          }  
        }
    }
  }
  else
  {
    Serial.print("n,NACK,Sate must be '0' or '1'");
  }
}

void SetServo(String Commandstr)
{
  uint8_t i = Commandstr.length()-1;
  if (Commandstr.length()<5 || Commandstr[3]!=44)
  {
    Serial.println("s,NACK,Invalid command");
    return;
  }
  else if (Commandstr[2]<49 || Commandstr[2]>55)
  {
    Serial.println("s,NACK,Invalid Servo. Must be between 1 and 7");
    return;
  }
  while(Commandstr[i]!=44)
  {
    if (Commandstr[i]<48 || Commandstr[i]>57)
    {
      Serial.println("s,NACK,Invalid degree");
      return;
    }
    i--;
  }
  int degree = Commandstr.substring(4,Commandstr.length()).toInt();
  if (Commandstr[2]==49) servo_1.write(degree); else 
  if (Commandstr[2]==50) servo_2.write(degree); else 
  if (Commandstr[2]==51) servo_3.write(degree); else 
  if (Commandstr[2]==52) servo_4.write(degree); else 
  if (Commandstr[2]==53) servo_5.write(degree); else 
  if (Commandstr[2]==54) servo_6.write(degree); else 
  if (Commandstr[2]==55 && Servo7Active) servo_7.write(degree);
  Serial.print("s,ACK,");
  String servo (Commandstr[2]);
  String state (Commandstr.substring(4,Commandstr.length()));
  Serial.print(servo);
  Serial.print(",");
  Serial.println(state);
}

void RelayControl(String Commandstr)
{
  if (Commandstr.length()<5 || Commandstr[3]!=44)
  {
    Serial.println("l,NACK,Invalid command");
  }
  else if (Commandstr[2]!=49 && Commandstr[2]!=50)
  {
    Serial.println("l,NACK,Invalid relay. Must be 1 or 2");
  }
  else if (Commandstr[4]!=48 && Commandstr[4]!=49)
  {
    Serial.println("l,NACK,Invalid state. Must be 0 or 1");
  }
  else
  {
    String relay (Commandstr[2]);
    String state (Commandstr[4]);
    if (Commandstr[4]!=48) Relay(relay.toInt(), true);else Relay(relay.toInt(), false);
    Serial.print("l,ACK,");
    Serial.print(relay);
    Serial.print(",");
    Serial.println(relay);
  }
}

void LedControl(String Commandstr)
{
  if (Commandstr.length()<9 || Commandstr[3]!=44)
  {
    Serial.println("l,NACK,Invalid command");
  }
  else if (Commandstr[4]!=40 || Commandstr[Commandstr.length()-1]!=41)
  {
    Serial.println("l,NACK,Color definition must be between '(' and ')'");
  }
  else
  {
    uint8_t count = 0;
    for (uint8_t i=0; i<Commandstr.length(); i++)
      if (Commandstr[i] == ',') count++;
    if (count!=4)Serial.println("l,NACK,Invalid command");else
    {
      String idStr = Commandstr.substring(2,3);
      String redStr = Commandstr.substring(5,Commandstr.indexOf(',',5));
      String greenStr = Commandstr.substring(Commandstr.indexOf(',',5)+1,Commandstr.indexOf(',',Commandstr.indexOf(',',5)+1));
      String blueStr = Commandstr.substring(Commandstr.indexOf(',', Commandstr.indexOf(',', 5) + 1) + 1, Commandstr.indexOf(')'));
      int idInt = idStr.toInt();
      int redInt = redStr.toInt();
      int greenInt = greenStr.toInt();
      int blueInt = blueStr.toInt();
      uint8_t id = static_cast<uint8_t>(idInt);
      uint8_t red = static_cast<uint8_t>(redInt);
      uint8_t green = static_cast<uint8_t>(greenInt);
      uint8_t blue = static_cast<uint8_t>(blueInt);

      if (redInt>255 || greenInt>255 || blueInt>255) Serial.println("l,NACK,Invalid RGB values. Must be between 0 and 255"); else
      {
        LedCtrl(id, red, green, blue);
      Serial.print("l,ACK,");
      Serial.print(id);
      Serial.print(",(");
      Serial.print(red);
      Serial.print(",");
      Serial.print(green);
      Serial.print(",");
      Serial.print(blue);
      Serial.println(")"); 
      }
    }
  }
}

void BuzzerControl()
{
  PlayBuzzer();
  Serial.println("b,ACK,");
}

void GetLedColor()
{
  GetColor();
}

void CloseGabarit()
{
  if(Close()==0) 
    Serial.println("f,ACK,");
  else
    Serial.println("f,NACK,Emergency");
}

void OpenGabarit()
{
  Open();
  Serial.println("a,ACK,");;
}


void Help()
{
  String HelpString=""; 
  Serial.println("This is HELP");
  HelpString+="H                     - Use ? or H to get help\n\r";
  HelpString+="R                     - Reset the unit\n\r";
  HelpString+="T                     - Get device type\n\r";
  HelpString+="V                     - Get device firmware version\n\r";  
  HelpString+="P,<state>             - Set Power Switch state ('0'- Off and '1' - On).Ex to power on: P,1\n\r";
  HelpString+="U,<state>             - Set USB Switch state ('0'- Off and '1' - On).Ex to switch on: U,1\n\r";
  HelpString+="W,<key>               - Send <key> through keyboard\n\r";
  HelpString+="N,<state>              - Configure Remote Emulator line feed('0' - Off and '1' - On). Ex to activate line feed: L,1\n\r";
  HelpString+="C,<key>               - Send control characters(hexadecimal) through keyboard. Ex for right arrow.:C,57\n\r";
  HelpString+="S,<servo>,<degres>    - Set servo to certain degree. Ex set servo 1 to 180 degrees: S,1,180\n\r";
  HelpString+="M,<relay>,<state>     - Set relay ('1' or '2') state ('0' - Off and '1' - On).Ex to close relay 1: M,1,1\n\r";
  HelpString+="L,<id>,<(R,G,B)>      - Set Led's Color(RGB). Ex change led 1 to white: L,1,(255,255,255)\n\r";
  HelpString+="B                     - Play buzzer during 1s\n\r";
  HelpString+="G                     - Get color from Led\n\r";
  HelpString+="A                     - Open Gabarit\n\r";
  HelpString+="F                     - Close Gabarit\n\r";
  Serial.println(HelpString);
}
