void PlayBuzzer()
{
  PlayNote(1,4000);
  delay(1000);
  PlayNote(0,0);
}

//  PlayNote(0,0) to silence
//  PlayNote(0,xxxx) to play note with frequency xxxx
//  Use the following frequencies to play different notes
//   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
//  4186,    4435,    4699,    4978,    5274,    5588,    5920,    6272,    6645,    7040,    7459,    7902     
void PlayNote(int On, int Note)
{
  ledcSetup(PWM3_Ch, Note, PWM_resolution);
  if (On==1)
    ledcWrite(PWM3_Ch, 128);
  else
    ledcWrite(PWM3_Ch, 0);
}
