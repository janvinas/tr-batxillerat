void updateDisplay(String val1, String val2, String val3, String message, byte progressBar){
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(3, 25, val1 + "°C");
  display.setFont(ArialMT_Plain_10);
  display.drawString(3, 50, val2 + "Pa");
  display.drawString(80, 50, val3 + "%");
  display.drawString(0, 0, message);
  //display.drawRect(0, 0, 128, 64);
  if(progressBar != 0) display.drawRect(2, 16, 64, 5);
  display.drawHorizontalLine(4, 18, 15 * progressBar);
  display.display(); //actualitza la pantalla
}

void updateDisplay2(bool loading){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Temperatura");
  display.drawHorizontalLine(0, 11, 160);
  File loc = SPIFFS.open("loc", "r");
  int readPos = loc.readString().toInt();
  for(int i = 0; i < 64; i++){
    File temp = SPIFFS.open("t-" + readPos, "r");
    display.drawLine(i * 2, 60, i * 2, 60 - temp.readString().toFloat()); 
    readPos++;
    if(readPos > 64 ) readPos = 0;
  }
  display.display();
}

void updateTime(int t){
  int seconds = t % 60;
  int minutes = floor(t / 60);
  String result;
  if(seconds < 10){
    result = String(minutes) + ":" + "0" + String(seconds);
  }else{
    result = String(minutes) + ":" + String(seconds);
  }
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_10);
  display.setColor(BLACK);
  display.fillRect(display.getWidth() - 2 - display.getStringWidth(result), 0,  display.getStringWidth(result), 10);
  display.setColor(WHITE);
  display.drawString(display.getWidth() - 2, 0, result);
  display.display();
}
