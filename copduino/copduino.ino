#include <SoftwareSerial.h>

SoftwareSerial koboSerial(2, 3); // rx / tx

// INDEXED  bottom left -> top left, right 1 and repeat
int index[] = {13, 11, 9, 8, 12, 10, 14, 15, 16, 6, 18, 4, 7, 17, 5, 19};   // 16 total
bool prev[16];
bool updt[16];

void setup() {
  koboSerial.begin(9600);
}

void loop() {
  bool needsUpdate = false;
  
  for (int i = 0; i < 16; i++) {
    bool r = digitalRead(index[i]) == HIGH;
    if (prev[i] != r) {
      updt[i] = true;
      needsUpdate = true;
    }
    prev[i] = r;
  }

  if (needsUpdate) {
    for (int i = 0; i < 16; i++) {
      //u[i] = updt[i] ? '1' : '0';
      koboSerial.print(prev[i]);
    }
    koboSerial.print('\n');
  }
  
  delay(100);
}
