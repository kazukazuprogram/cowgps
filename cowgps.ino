#include <KashiwaGeeks.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define ECHO true
ADB922S LoRa;
//==================
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
TinyGPSPlus gps;
SoftwareSerial mySerial(10, 11); // RX, TX

void start() {
  ConsoleBegin(57600);
  ConsolePrint(F("\n**** End-node_Device *****\n"));
  EnableInt0();
  if ( LoRa.begin(9600, DR3) == false ) {
    while (true) {
      LedOn();
      delay(300);
      LedOff();
      delay(300);
    }
  }
  LoRa.join();
  LedOff();
  // GPS PIN
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void sleep(void) {
  LoRa.sleep();
  DebugPrint(F("LoRa sleep.\n"));
}

void wakeup(void) {
  LoRa.wakeup();
  DebugPrint(F("LoRa wakeup.\n"));
}

void int0D2(void) {
  ConsolePrint(F("\nINT0 !!!\n"));
  task1();
}

#define LoRa_Port_NORM  12

void task1(void) {
  LedOn();
  mySerial.begin(9600);
  ConsolePrint(F("\n  Task1 invoked\n\n"));
  double la, lo;
  int lpn=0;
  Serial.println("Getting GPS ... ");
  while (true) {
    int b = 0;
    while (mySerial.available() > 0) {
      char c = mySerial.read();
      gps.encode(c);
      if (gps.location.isUpdated()) {
        la = gps.location.lat();
        lo = gps.location.lng();
        Serial.println("GPS acquisition complete.");
        Serial.print("LAT  = "); Serial.println(gps.location.lat(), 6);
        Serial.print("LONG = "); Serial.println(gps.location.lng(), 6);
        b = 1;
        break;
      }
    }
    if (b == 1) break;
    if (lpn>=3000) {
      Serial.println("GPS Unacquirable");
      Serial.println("Task1 Discarded.");
      digitalWrite(13, LOW);
      return ;
    }
    lpn += 1;
    delay(10);
  }
  long int lat, lon;
  lat = la * 1000000;
  lon = lo * 1000000;
  String sa, so;
  sa = String(lat, HEX);
  so = String(lon, HEX);
  Serial.print("\nsa:"); Serial.println(sa);
  Serial.print("so:"); Serial.println(so);
  String ya = sa;
  ya.concat(so);

  int rc = LoRa.sendData(LoRa_Port_NORM, ECHO, F("%c%c%c%c%c%c%c%c%c%c%c%c%c%c"), ya.charAt(0), ya.charAt(1), ya.charAt(2), ya.charAt(3), ya.charAt(4), ya.charAt(5), ya.charAt(6), ya.charAt(7), ya.charAt(8), ya.charAt(9), ya.charAt(10), ya.charAt(11), ya.charAt(12), ya.charAt(13));
  checkResult(rc);
  LedOff();
  digitalWrite(13, LOW);
  Serial.println("Task1 completed.");
}

void checkResult(int rc ) {
  if ( rc == LORA_RC_SUCCESS ) {
    ConsolePrint(F("\n SUCCESS\n"));
  }
  else if ( rc == LORA_RC_DATA_TOO_LONG ) {
    ConsolePrint(F("\n !!!DATA_TOO_LONG\n"));
  }
  else if ( rc == LORA_RC_NO_FREE_CH ) {
    ConsolePrint(F("\n !!!No free CH\n"));
    task1();
  }
  else if ( rc == LORA_RC_BUSY ) {
    ConsolePrint(F("\n !!!Busy\n"));
  }
  else if ( rc == LORA_RC_NOT_JOINED ) {
    ConsolePrint(F("\n !!!Not Joined\n"));
    LoRa.join();
    task1();
  }
  else if ( rc == LORA_RC_ERROR ) {
    ConsolePrint(F("\n !!!UNSUCCESS\n"));
  }
}

void task2() {
  Serial.println("task2");
  Serial.println("GPS awaken");
  digitalWrite(13, HIGH);
}

TASK_LIST = {
  TASK(task2, 0, 3),
  TASK(task1, 1, 3),
  END_OF_TASK_LIST
};