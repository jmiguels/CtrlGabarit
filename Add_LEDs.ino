//###################################################
void LedCtrl(int LedId, int Red, int Green, int Blue)
{
  pixels.setPixelColor(LedId,pixels.Color(Red, Green, Blue));    
  pixels.show();
  delay(10);
}
