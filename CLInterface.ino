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
  HelpString+="Help          - Use ?, help or Help to get help\n\r";
  HelpString+="Reset         - Reset the unit\n\r";
  Serial.println(HelpString);
}
