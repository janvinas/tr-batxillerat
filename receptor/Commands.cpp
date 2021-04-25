#include "Arduino.h"
#include "Commands.h"
#include <FS.h>

void handleCommand(String c){
  String cmd = c.substring(0, c.indexOf(" "));
  
  if(cmd.equalsIgnoreCase("help")){
    cmd::help();
  }else if(cmd.equalsIgnoreCase("?")){
    cmd::help();
  }else if(cmd.equalsIgnoreCase("readstr")){
    cmd::readstr(c);
  }else if(cmd.equalsIgnoreCase("writestr")){
    Serial.println(F("Command writestr not implemented yet."));
  }else if(cmd.equalsIgnoreCase("delfile")){
    Serial.println(F("Command delfile not implemented yet."));
  }else{
    Serial.println(F("Unknown command. Type \"help\" for a list of commands."));
  }
}

void cmd::help(){
  Serial.println(F("*------------*"));
  Serial.println(F("|command list|"));
  Serial.println(F("*------------*"));
  Serial.println(F(""));
  Serial.println(F("help, ?                        Shows this page."));
  Serial.println(F("readstr <filename>             Reads string from file."));
  Serial.println(F("writestr <filename> <string>   Writes a string to file."));
  Serial.println(F("delfile <filename>             Deletes a file."));

}

void cmd::readstr(String c){
  String filename = c.substring(c.indexOf(" ") + 1, c.length());
  Serial.println("Reading file: " + filename);
  File file = SPIFFS.open(filename, "r");
  Serial.println("Output: " + file.readString());
}
