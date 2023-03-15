void Relay(int Id, bool DestinationState)
{
  if(Id==1)
    if(DestinationState==true) 
      digitalWrite(Relay1,HIGH);
    else  
      digitalWrite(Relay1,LOW);

  if(Id==2)
    if(DestinationState==true) 
      digitalWrite(Relay2,HIGH);
    else  
      digitalWrite(Relay2,LOW);
}


void Solenoid(int Id, bool DestinationState)
{
  if(Id==1)
    if(DestinationState==true) 
      digitalWrite(Solenoid1,HIGH);
    else  
      digitalWrite(Solenoid1,LOW);
  if(Id==2)
    if(DestinationState==true) 
      digitalWrite(Solenoid2,HIGH);
    else  
      digitalWrite(Solenoid2,LOW);
  if(Id==3)
    if(DestinationState==true) 
      digitalWrite(Solenoid3,HIGH);
    else  
      digitalWrite(Solenoid3,LOW);
  if(Id==4)
    if(DestinationState==true) 
      digitalWrite(Solenoid4,HIGH);
    else  
      digitalWrite(Solenoid4,LOW);
  if(Id==5)
    if(DestinationState==true) 
      digitalWrite(Solenoid5,HIGH);
    else  
      digitalWrite(Solenoid5,LOW);
  if(Id==6)
    if(DestinationState==true) 
      digitalWrite(Solenoid6,HIGH);
    else  
      digitalWrite(Solenoid6,LOW);
}
