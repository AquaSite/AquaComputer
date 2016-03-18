#include <Time.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <SPI.h>
#include "Ucglib.h"
#include <IRremote.h>
#include <SD.h>


// Verwendetes Board Arduino Mega

#define PinAQ1Lampe_1 22
#define PinAQ1Lampe_2 24
#define PinAQ1Lampe_3 26
#define PinAQ1CO2_1 28
// Pin 30 = 220V frei!
#define PinStromPumpe 32
#define PinFutterautomat 34
#define PinLEDLeiste 36
//22 24 26 28 30 32 34 36 Ports Relais (8er Modul) (vordere Reihe)

#define PinPumpe_A1 23
#define PinPumpe_A2 25
#define PinPumpe_AS 27
#define PinPumpe_B1 29
#define PinPumpe_B2 31
#define PinPumpe_BS 33
// 23 25 27 29 31 33 Ports Pumpen (hintere Reihen)

#define PinIR 37

#define EIN HIGH
#define AUS LOW
#define RelaisAus HIGH
#define RelaisEin LOW

int Timer_1[4]; // Timer Lampe 1
int Timer_2[4]; // Timer Lampe 2
int Timer_3[4]; // Timer Lampe 3
int Timer_4[4]; // Timer CO2
int Pumpe1An;
int Pumpe2An;
int Pumpe3An;
int Pumpe4An;
int RECV_PIN = PinIR;
String LastFutter;
const int chipSelect = 53;


IRrecv irrecv(RECV_PIN);
decode_results results;

Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);

unsigned long interval = 5000;
unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(9600);
  setSyncProvider(RTC.get);
  // Nur zur Kontrolle:
  if(timeStatus()!= timeSet) // wenn Status geaendert
  {
     Serial.println("Synchronisation fehlgeschlagen"); // bei Fehler
  }
  else
  {
     Serial.println("Synchronisation erfolgreich"); // alles ok
  }

  
  pinMode(PinAQ1Lampe_1, INPUT_PULLUP);
  pinMode(PinAQ1Lampe_2, INPUT_PULLUP);
  pinMode(PinAQ1Lampe_3, INPUT_PULLUP);
  pinMode(PinAQ1CO2_1, INPUT_PULLUP);
  pinMode(PinPumpe_A1, INPUT_PULLUP);
  pinMode(PinPumpe_A2, INPUT_PULLUP);
  pinMode(PinPumpe_AS, INPUT_PULLUP);
  pinMode(PinPumpe_B1, INPUT_PULLUP);
  pinMode(PinPumpe_B2, INPUT_PULLUP);
  pinMode(PinPumpe_BS, INPUT_PULLUP);
  pinMode(PinStromPumpe, INPUT_PULLUP);
  pinMode(PinFutterautomat, INPUT_PULLUP);
  pinMode(PinLEDLeiste, INPUT_PULLUP);

  pinMode(PinAQ1Lampe_1, OUTPUT);
  pinMode(PinAQ1Lampe_2, OUTPUT);
  pinMode(PinAQ1Lampe_3, OUTPUT);
  pinMode(PinAQ1CO2_1, OUTPUT);
  pinMode(PinPumpe_A1, OUTPUT);
  pinMode(PinPumpe_A2, OUTPUT);
  pinMode(PinPumpe_AS, OUTPUT);
  pinMode(PinPumpe_B1, OUTPUT);
  pinMode(PinPumpe_B2, OUTPUT);
  pinMode(PinPumpe_BS, OUTPUT);
  pinMode(PinStromPumpe, OUTPUT);
  pinMode(PinFutterautomat, OUTPUT);
  pinMode(PinLEDLeiste, OUTPUT);

  //ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.setRotate90();
  ucg.clearScreen();

  irrecv.enableIRIn();

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
}

void loop()
{
  //ucg.clearScreen();

  ucg.setColor(0, 0, 0);
  ucg.drawBox(0, 0, 160, 128); // Y,X,W,H
  
  ucg.setColor(255, 255, 255);
  if (millis() - previousMillis > interval) {
    previousMillis = millis();
    ucg.setPrintPos(0,20);
    ucg.setFont(ucg_font_ncenR12_tr);
    ucg.print(hour());
    ucg.print(":");
    ucg.print(minute());
    ucg.print(":");
    ucg.print(second());
    ucg.print(" - ");
    ucg.print(day());
    ucg.print("-");
    ucg.print(month());
    ucg.print("-");
    ucg.print(year());
  
    ucg.setFont(ucg_font_helvB08_tr);
    ucg.setPrintPos(0,45);
    if(weekday()==1) {
      ucg.print("Programm: Wochenende");
    }
    else {
      ucg.print("Programm: Normal");
    }
  
    ucg.setPrintPos(0,60);
    ucg.print("Lampen: ");
    ucgStatusPinRelais(PinAQ1Lampe_1);
  
    ucg.setPrintPos(70,60);
    ucgStatusPinRelais(PinAQ1Lampe_2);
  
    ucg.setPrintPos(95,60);
    ucgStatusPinRelais(PinAQ1Lampe_3);
  
    ucg.setPrintPos(0,72);
    ucg.print("CO2: ");
    ucgStatusPinRelais(PinAQ1CO2_1);
  
    ucg.setPrintPos(0,84);
    ucg.print("Düngepumpen: ");
    ucgStatusPinRelais(PinStromPumpe);
  
    ucg.setPrintPos(0,110);
    ucg.setColor(255, 0, 0);
    ucg.print("Letzte Fuetterung: ");
    ucg.print(LastFutter);
    ucg.setColor(255, 255, 255);
  }
  


  //ucg.setColor(255, 255, 255);
  //ucg.drawBox(50, 30, 45, 20); // Y,X,W,H

  //ucg.setColor(0, 255, 0, 0);
  //ucg.setColor(1, 0, 255, 0);
  //ucg.setColor(2, 255, 0, 255);
  //ucg.setColor(3, 0, 255, 255);
  //ucg.drawGradientBox(80, 60, 45, 20);
  
  //ucg.setColor(0, 255, 255, 255);
  //ucg.drawHLine(50, 40, 45); // X,Y,W  
  
  if(weekday()==1) {
    // Sonntags später, da man gerne mal ausschläft ohne Licht an
    Timer_1[0] = 1000;
    Timer_1[1] = 1400;
    Timer_1[2] = 1600;
    Timer_1[3] = 2100;
    Timer_2[0] = 1000;
    Timer_2[1] = 1400;
    Timer_2[2] = 1600;
    Timer_2[3] = 2100;
    Timer_3[0] = 1000;
    Timer_3[1] = 1400;
    Timer_3[2] = 1600;
    Timer_3[3] = 2100;
    Timer_4[0] = 1000;
    Timer_4[1] = 1400;
    Timer_4[2] = 1600;
    Timer_4[3] = 2100;
    Serial.println("Sonntag");
    }
  else {
    Timer_1[0] = 730;
    Timer_1[1] = 1200;
    Timer_1[2] = 1400;
    Timer_1[3] = 2000;
    Timer_2[0] = 730;
    Timer_2[1] = 1200;
    Timer_2[2] = 1400;
    Timer_2[3] = 2000;
    Timer_3[0] = 730;
    Timer_3[1] = 1200;
    Timer_3[2] = 1400;
    Timer_3[3] = 2000;
    Timer_4[0] = 730;
    Timer_4[1] = 1200;
    Timer_4[2] = 1400;
    Timer_4[3] = 2000;
    Serial.println("Wochentag");
    }
  
  Serial.println(">>-->>-->>-->>");
  Serial.print("Timer1[0] ");
  Serial.println(Timer_1[0]);
  Serial.print("Uhrzeit: ");
  Serial.println(uhrzeit());
  Serial.println("|");
  Serial.println("<<--<<--<<--<<");
  
  digitalUhrAnzeige(); // Aktuelle Uhrzeit anzeigen
  
  ZeitBisAn(Timer_1[0],Timer_1[1]); // Gibt aus wie lange das Licht schon an ist und wann es aus geht 
  // Wenn es aus geschalten ist wird die Zeit ausgegeben bis wann sie wieder eingeschalten wird
  Serial.println("-------");


  Serial.print("Pin Status ");
  Serial.print(PinAQ1Lampe_1);
  Serial.print(": ");
  Serial.println(digitalRead(PinAQ1Lampe_1));
  
  TimerCheck(Timer_1[0],Timer_1[1],Timer_1[2],Timer_1[3],PinAQ1Lampe_1);
  TimerCheck(Timer_1[0],Timer_1[1],Timer_1[2],Timer_1[3],PinAQ1Lampe_2);
  TimerCheck(Timer_1[0],Timer_1[1],Timer_1[2],Timer_1[3],PinAQ1Lampe_3);
  TimerCheck(Timer_1[0],Timer_1[1],Timer_1[2],Timer_1[3],PinAQ1CO2_1);
    
    //delay(5000);
    Serial.println("");
    Serial.println("");
    Serial.println("");
  if(uhrzeit()>700 and uhrzeit()<730) {
    // Düngepumpen werden auf Status 0 gesetzt, damit können sie wieder gestartet werden.
    // Dies soll verhindern, dass die Pumpe aus versehen am Tag mehrmals gestartet wird.
    Pumpe1An = 0;
    Pumpe2An = 0;
    Pumpe3An = 0;
    Pumpe4An = 0;
    }
  if(Pumpe1An != 1 and uhrzeit()>Timer_1[0]) {
    // Düngemittel werden erst nach Licht gestartet.
    Pumpe1An = DuengePumpe(200,1,5000); // Geschwindigkeit (PWM min 50 - max 255), Pumpen Nummer, Dauer in ms
    Pumpe2An = DuengePumpe(255,2,5000); // Geschwindigkeit (PWM min 50 - max 255), Pumpen Nummer, Dauer in ms
    Pumpe3An = DuengePumpe(200,3,3000); // Geschwindigkeit (PWM min 50 - max 255), Pumpen Nummer, Dauer in ms
    Pumpe4An = DuengePumpe(200,4,3000); // Geschwindigkeit (PWM min 50 - max 255), Pumpen Nummer, Dauer in ms
    }

  if (irrecv.decode(&results)) { // Wenn die IR Bibliothek ein decodiertes Signal liefert ...
  Serial.println(results.value, DEC); // ... dann soll dieses auf der Seriellen Schnittstelle ausgegeben werden
  Serial.println(results.value);
  if(results.value==551547150) {
    File dataFile = SD.open("logging.txt", FILE_WRITE);
    LastFutter = FutterAutomat(1000);
    dataFile.print("Fütterung, manuell: ");
    dataFile.print(hour());
    dataFile.print(":");
    dataFile.println(minute());
    dataFile.close();
  }
  if(results.value==551514510) {
    digitalWrite(PinLEDLeiste, RelaisAus);
  }
  if(results.value==551489010) {
    digitalWrite(PinLEDLeiste, RelaisEin);
  }
  if(results.value==551509410) {
    ucg.clearScreen();
  }
  irrecv.resume(); // neue Messung durchführen
  }  
  
  
}

int ucgStatusPinRelais(int Pin) {
  if(digitalRead(Pin)==0) {
    ucg.print("Ein");
    }
  else {
    ucg.print("Aus");
    }
  }

String FutterAutomat(int Timer) {
  File dataFile = SD.open("logging.txt", FILE_WRITE);
  dataFile.print("Fütterung: ");
  dataFile.print(hour());
  dataFile.print(":");
  dataFile.println(minute());
  Serial.println("");
  Serial.println("Raubtierfütterung!");
  Serial.println("");
  digitalWrite(PinFutterautomat, RelaisEin);
  delay(Timer);
  digitalWrite(PinFutterautomat, RelaisAus);
  String LastTimer;
  LastTimer = "";
  LastTimer += hour();
  LastTimer += ":";
  LastTimer += minute();
  return LastTimer;
  dataFile.close();
  }

int TimerCheck(int Timer_1, int Timer_2, int Timer_3, int Timer_4, int Pin) {
  File dataFile = SD.open("logging.txt", FILE_WRITE);
  if((uhrzeit()>=Timer_1 and uhrzeit()<Timer_2) or (uhrzeit()>=Timer_3 and uhrzeit()<Timer_4)) {
    // Lampe 1 sollte gestartet sein
    if(digitalRead(Pin)==1) {
    // Wenn der Pin von der Lampe 1 noch aus ist dann jetzt einschalten
      digitalWrite(Pin, RelaisEin);
      Serial.println("");
      Serial.print("Lampe ");
      Serial.print(Pin);
      Serial.println(" wird eingeschaltet");
      Serial.println("");
      dataFile.print("Lampe ");
      dataFile.print(Pin);
      dataFile.print(" an: ");
      dataFile.print(hour());
      dataFile.print(":");
      dataFile.println(minute());
      }
  }
  else {
    // Lampe 1 sollte ausgeschaltet sein
    if(digitalRead(Pin)==0) {
      // Wenn sie noch eingeschaltet ist dann jetzt ausschalten
      digitalWrite(Pin, RelaisAus);
      Serial.println("");
      Serial.print("Lampe ");
      Serial.print(Pin);
      Serial.println(" wird ausgeschaltet");
      Serial.println("");
       dataFile.print("Lampe ");
      dataFile.print(Pin);
      dataFile.print(" aus: ");
      dataFile.print(hour());
      dataFile.print(":");
      dataFile.println(minute());
      }
    }
    dataFile.close();
  }

int DuengePumpe(int speed, int pumpe, int time) {
  File dataFile = SD.open("logging.txt", FILE_WRITE);
  dataFile.print("DuengePumpe: ");
  dataFile.print(pumpe);
  dataFile.print(" / ");
  dataFile.print(hour());
  dataFile.print(":");
  dataFile.println(minute());
  dataFile.close();
  
  Serial.print("Düngepumpe ");
  Serial.print(pumpe);
  Serial.println(" EIN");
  digitalWrite(PinStromPumpe, RelaisEin); // Strom auf den Motortreiber
  if(pumpe==1) {
      // Drehrichtung 1
      digitalWrite(PinPumpe_A1, EIN);
      digitalWrite(PinPumpe_A2, AUS);
      analogWrite(PinPumpe_AS, speed);
      delay(time);
      analogWrite(PinPumpe_AS, AUS);
      digitalWrite(PinPumpe_A1, AUS);
      digitalWrite(PinPumpe_A2, AUS);
    }
  else if(pumpe==2) {
      // Drehrichtung 2
      digitalWrite(PinPumpe_A1, AUS);
      digitalWrite(PinPumpe_A2, EIN);
      analogWrite(PinPumpe_AS, speed);
      delay(time);
      analogWrite(PinPumpe_AS, AUS);
      digitalWrite(PinPumpe_A1, AUS);
      digitalWrite(PinPumpe_A2, AUS);
    }
  else if(pumpe==3) {
      // Drehrichtung 1
      digitalWrite(PinPumpe_B1, EIN);
      digitalWrite(PinPumpe_B2, AUS);
      analogWrite(PinPumpe_BS, speed);
      delay(time);
      analogWrite(PinPumpe_BS, AUS);
      digitalWrite(PinPumpe_B1, AUS);
      digitalWrite(PinPumpe_B2, AUS);
    }
  else if(pumpe==4) {
      // Drehrichtung 2
      digitalWrite(PinPumpe_B1, AUS);
      digitalWrite(PinPumpe_B2, EIN);
      analogWrite(PinPumpe_BS, speed);
      delay(time);
      analogWrite(PinPumpe_BS, AUS);
      digitalWrite(PinPumpe_B1, AUS);
      digitalWrite(PinPumpe_B2, AUS);
    }
  digitalWrite(PinStromPumpe, RelaisAus); // Strom vom Motortreiber aus
  Serial.print("Düngepumpe ");
  Serial.print(pumpe);
  Serial.println(" AUS");
  int antwort = 1;
  return antwort;
  }

int uhrzeit() {
  int zahl = (hour()*100) + minute();
  return zahl;
  }

  
int ZeitBisAn(int timerAn, int timerAus) {
  int verbleibend;
  if(uhrzeit()>timerAn) {
    verbleibend = uhrzeit()-timerAn;
    }
  else {
    verbleibend = timerAn-uhrzeit();
    }
  Serial.println("Zeit Bis An START >>>>>>>>>>>>>>>>>>>>");
  Serial.print("Timer1: ");
  Serial.println(timerAn);
  Serial.print("Uhrzeit: ");
  Serial.println(uhrzeit());
  Serial.print("Verbleibend: ");
  Serial.println(verbleibend);

  if(verbleibend<1000) {
    Serial.println("3-Stellig");
    Serial.print("HMM: ");
    Serial.println(verbleibend);
    }
  else {
    Serial.println("4-Stellig");
    Serial.print("HHMM: ");
    Serial.println(verbleibend);
    }
  unsigned x = verbleibend;
  Serial.print(x / 100);
  Serial.print(":");
  Serial.println(x % 100);
  
  Serial.println("Zeit Bis An Ende <<<<<<<<<<<<<<<<<<<<<");
  return verbleibend;
  }






// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// ab hier wird nur zur schöneren Ansicht verwendet

void digitalUhrAnzeigeUcg() {
  
  }

void digitalUhrAnzeige() // definieren Subroutine
{
   // digitale Anzeige der Zeit
   printZiffernGenulltOhnePunkt(hour()); // rufen Subroutine auf
   printZiffernGenulltmitPunkt(minute()); // rufen Subroutine auf
   printZiffernGenulltmitPunkt(second()); // rufen Subroutine auf
   Serial.print(" "); // fuegen Freizeichen ein
   printZiffernGenulltOhnePunkt(day()); // rufen Subroutine auf
   Serial.print("."); // fuegen Freizeichen ein
   printZiffernGenulltOhnePunkt(month()); // rufen Subroutine auf
   Serial.print("."); // fuegen Freizeichen ein
   Serial.println(year()); // Zeilenvorschub
}

void printZiffernGenulltOhnePunkt(int zahl) // definieren Subroutine
{
  // verschoenern Anzeige, indem wir ...
  if(zahl < 10) // wenn kleiner als 10 ...
   {
     Serial.print(""); // fuehrende 0 einfuegen
   }
   Serial.print(zahl); // Ausgabe auf serieller Konsole
}

void printZiffernGenulltmitPunkt(int zahl) // definieren Subroutine
{
  // verschoenern Anzeige, indem wir ...
  Serial.print(":"); // trennende ":" einfuegen
  if(zahl < 10) // wenn kleiner als 10 ...
  {
     Serial.print('0'); // fuehrende 0 einfuegen
  }
  Serial.print(zahl); // Ausgabe auf serieller Konsole
}
