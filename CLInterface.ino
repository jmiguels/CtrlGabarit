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
  }
  else
  {
    if (Commandstr.equals("")) DoNothing(); else
    if (Commandstr.equals("?")||Commandstr.equals("help")||Commandstr.equals("Help")) Help(); else
    if (Commandstr.equals("reset")) SystemReset(); else
    if (Commandstr.substring(0,3).equals("set")) set(Commandstr); else    
      Serial.println("Command Error\n");

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
}

void DoNothing()
{
}

void Help()
{
  String HelpString=""; 
  Serial.println("This is HELP");
  HelpString+="Help            - Use ?, help or Help to get help\n\r";
  HelpString+="Reset           - Reset the unit\n\r";
  HelpString+="version         - Returns type of device and version\n\r";  
  HelpString+="type            - Returns type of device\n\r";    
  HelpString+="set \"servoN\"  - set the servoN angle (between 0 and 180 degrees).\n\r";
  HelpString+="                  N is a number between 1 and 7\n\r";
  HelpString+="                  Example to set the servo2 angle to 20 degrees: set \"servo2\" \"20\"\n\r";
  Serial.println(HelpString);
}

/**************************************
 **************************************/
void GetVersion()
{
  Serial.println(VersionStr);
}

/**************************************
 **************************************/
void GetType()
{
  Serial.println(DeviceTypeStr);
}

void set(String InputString)
{
  String Parameter;
  String Value;
  
  Parameter=InputString.substring(5,InputString.indexOf("\"",6));
  Value=InputString.substring(5+Parameter.length()+3,InputString.indexOf("\"",5+Parameter.length()+3));

  if (Parameter.equals("servo1")) servo_1.write(Value.toInt()); else 
  if (Parameter.equals("servo2")) servo_2.write(Value.toInt()); else 
  if (Parameter.equals("servo3")) servo_3.write(Value.toInt()); else 
  if (Parameter.equals("servo4")) servo_4.write(Value.toInt()); else 
  if (Parameter.equals("servo5")) servo_5.write(Value.toInt()); else 
  if (Parameter.equals("servo6")) servo_6.write(Value.toInt()); else 
  if (Parameter.equals("servo7") && Servo7Active) servo_7.write(Value.toInt()); else 
    
  {  
    Serial.println("Set Command Error\n");
    return;
  }  
  Serial.println("OK");
}
