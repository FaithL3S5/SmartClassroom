#include <string.h>
#define KIPAS 8
#define LAMPU 9
String status_;
String lamp;
String fan;
String sentence;
void setup() {
  // Open serial communications and wait for port to open:
  pinMode(LAMPU, OUTPUT);
  pinMode(KIPAS, OUTPUT);
  digitalWrite(LAMPU, LOW);
  digitalWrite(KIPAS, LOW);
//  lamp = "OFF";
//  fan = "OFF";
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
void loop() { // run over and over
  //digitalWrite(13, LOW);
  //delay(5000);

if (Serial.available()) {
sentence = Serial.readString();
//Serial.println(sentence);
lamp = String(getValue(sentence,' ',0));
Serial.print("lamp:");
Serial.println(lamp); 
fan = String(getValue(sentence,' ',1));
Serial.print("fan:");
Serial.println(fan); 
}

 if (fan  == "ON") {
    digitalWrite(KIPAS, HIGH);   // set the LED on
    //Serial.println("ON");
  } else {
    digitalWrite(KIPAS, LOW); // xturn the LED off by making the voltage LOW
  }

  if (lamp == "ON") {
    digitalWrite(LAMPU, HIGH);   // set the LED on
  } else {
    digitalWrite(LAMPU, LOW); // turn the LED off by making the voltage LOW

  }

}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = {0,-1};
  int maxIndex = data.length()-1;

  for(int i =0; i<=maxIndex && found<=index; i++ ){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]): "";
  }
