#include <Ucglib.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>

/*
DISPLAY


LED -> 3.3V
SCK -> D13
SDA -> D11 (MOSI)
A0 -> D9
RST -> D8
CS -> D10
GND -> GND
VCC -> 5V
*/
#define DISPLAY_SCK 13
#define DISPLAY_SDA 11
#define DISPLAY_A0 9
#define DISPLAY_RST 8
#define DISPLAY_CS 10

/*
ULTRASOUND

TRIG -> 5
ECHO -> 6
*/
#define ULTRASOUND_TRIG 5
#define ULTRASOUND_ECHO 6

/*
SERVOMOTOR

-> 3
*/
#define SERVOMOTOR_PIN 3


/*
BUZZER

-> 2
*/
#define BUZZER_PIN 2


int Ymax = 128;
int Xmax = 128;
int base = 8;
int pos = base+6;
int deg=0;
int x;
int val =200;
int j = 2;


Ucglib_ST7735_18x128x160_SWSPI ucg(DISPLAY_SCK, DISPLAY_SDA, DISPLAY_A0, DISPLAY_CS, DISPLAY_RST);

Servo myServo;
// int pos = 0;
int motorDelay = 30;
int motorDisplacement = 1;


long duration;
int distance;
int k;


void setup() {
  /* 
  DISPLAY

  */
  ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.setFont(ucg_font_6x10_tr);
  ucg.clearScreen();
  ucg.setRotate270();

  /*
  ULTRASOUND

  */
  pinMode(ULTRASOUND_TRIG, OUTPUT);
  pinMode(ULTRASOUND_ECHO, INPUT);


  /*
  SERVOMOTOR
  */
  myServo.attach(SERVOMOTOR_PIN);
  myServo.write(80);

  /*
  BUZZER
  */
  pinMode(BUZZER_PIN, OUTPUT);


  Serial.begin(9600);
}

void loop() {

  fix();

  for (x = 80; x >= 10; x-=motorDisplacement) {
    distance = calculateDistance();
    Serial.println(distance);
    k = map(x, 80, 10, 15, 165);
    myServo.write(k);

    if (distance < 20) {
      int f = x + 6;
      tone(BUZZER_PIN, 1000);
      ucg.setColor(0, 0, 255);  //color when object is detected
      ucg.drawLine(Xmax / 2, pos, -val * cos(radians(f * 2)), val * sin(radians(f * 2)));
      delay(2000);
      noTone(BUZZER_PIN);
    } else {
      delay(90);
    }
    ucg.setColor(0, 207, 0);
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(x * 2)), 200 * sin(radians(x * 2)));

    int d = x + 1;
    ucg.setColor(0, 207, 0);

    int c = x + 2;
    ucg.setColor(0, 207, 0);

    int b = x + 3;
    ucg.setColor(0, 102, 0);

    int a = x + 4;
    ucg.setColor(0, 102, 0);

    int e = x + 5;
    ucg.setColor(0, 0, 0);
    //background color
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(e * 2)), 200 * sin(radians(e * 2)));
    ucg.setColor(220, 220, 220);  //color of horizontal labels
    ucg.setPrintPos(160, 0);
    ucg.setPrintDir(2);
    ucg.print("Deg :");
    deg = map(x, 80, 10, 0, 180);
    ucg.setPrintPos(120, 0);
    ucg.setPrintDir(2);
    ucg.print(deg);
    ucg.setPrintPos(10, 0);
    ucg.print(distance);

    ucg.setColor(220, 220, 220);  //colors of vertical axis (RED)
    ucg.setPrintPos(90, 38);
    ucg.setPrintDir(2);
    ucg.print("0.25");
    ucg.setPrintPos(90, 70);
    ucg.print("0.50");
    ucg.setPrintPos(90, 110);
    ucg.print("1.00");
  }

  fix();

  for (x = 10; x <= 80; x+=motorDisplacement) {
    distance = calculateDistance();
    Serial.println(distance);
    k = map(x, 10, 80, 165, 15);
    myServo.write(k);
    if (distance < 20) {
      int e = x - 5;
      tone(BUZZER_PIN, 1000);
      ucg.setColor(0, 0, 255);  //color when object is detected
      ucg.drawLine(Xmax / 2, pos, -val * cos(radians(e * 2)), val * sin(radians(e * 2)));
      delay(2000);
      noTone(BUZZER_PIN);
    } else {
      delay(90);
    }

    ucg.setColor(0, 207, 0);
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(x * 2)), 200 * sin(radians(x * 2)));

    int a = x - 1;

    int b = x - 2;
    ucg.setColor(0, 102, 0);

    int c = x - 3;
    ucg.setColor(0, 102, 0);

    int d = x - 4;
    ucg.setColor(0, 0, 0);  //background color
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(d * 2)), 200 * sin(radians(d * 2)));
    ucg.setColor(220, 220, 220);  // color or horizontal labels
    ucg.setPrintPos(160, 0);
    ucg.setPrintDir(2);
    ucg.print("Deg :");
    deg = map(x, 10, 80, 0, 180);
    ucg.setPrintPos(120, 0);
    ucg.setPrintDir(2);
    ucg.print(deg);
    ucg.setPrintPos(10, 0);
    ucg.print(distance);

    ucg.setColor(220, 220, 220);  //color of vertical axis labels
    ucg.setPrintPos(90, 38);
    ucg.setPrintDir(2);
    ucg.print("0.25");
    ucg.setPrintPos(90, 70);
    ucg.print("0.50");
    ucg.setPrintPos(90, 110);
    ucg.print("1.00");
  }
}

void fix() {
  ucg.setColor(255, 0, 0);
  ucg.drawDisc(Xmax / 2, base, 5, UCG_DRAW_LOWER_RIGHT);
  ucg.drawDisc(Xmax / 2, base, 5, UCG_DRAW_LOWER_LEFT);

  ucg.setColor(225, 255, 50);  // (YELLOW)
  ucg.drawCircle(80, base, 115, UCG_DRAW_LOWER_RIGHT);
  ucg.drawCircle(80, base, 115, UCG_DRAW_LOWER_LEFT);

  ucg.drawCircle(80, base, 78, UCG_DRAW_LOWER_RIGHT);
  ucg.drawCircle(80, base, 78, UCG_DRAW_LOWER_LEFT);

  ucg.drawCircle(80, base, 40, UCG_DRAW_LOWER_RIGHT);
  ucg.drawCircle(80, base, 40, UCG_DRAW_LOWER_LEFT);

  ucg.drawLine(0, base, 160, base);

  ucg.setColor(0, 0, 0);
  ucg.drawBox(100, 0, 30, 8);
}

int calculateDistance() {
  digitalWrite(ULTRASOUND_TRIG, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(ULTRASOUND_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASOUND_TRIG, LOW);
  duration = pulseIn(ULTRASOUND_ECHO, HIGH);  // Reads the echoPin, returns the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;
  return distance;
}
