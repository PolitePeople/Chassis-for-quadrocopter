#include <Servo.h>          //Подключаем библиотеку для работы с SERVO
#include <PinChangeInt.h>   //Подключаем библиотеку для работы с прерываниями

// Определяем переменные RX
#define PIN1 2
#define PIN1dataReg PIND
#define pin1state (PIN1dataReg & (1 << PIN1))
volatile long pulseStart1; // pulse start time measured from millies in ISR for pin 1
volatile long pulseTime1; // pulse width time measured from millies in ISR for pin 1 <-- this is the output we require


Servo myservo;  // Создаем сервообъект для управления сервоприводом
int pos = 0;    // Переменная для хранения положения сервопривода


// Назначаем контакты микропереключателя
const int msdown = 5;
const int msup = 4;
int msdownstate = 0;
int msupstate = 0;


// Назначаем переменные для светодиодов
int led1 = 6;                    // выходной контакт для светодиода 1
int led2 = 7;                    // выходной контакт для светодиода 2
int led3 = 8;                    // выходной контакт для светодиода 3
int led4 = 9;                    // выходной контакт для светодиода 4
int led13 = 13;                  // встроенный светодиодный индикатор ARDUINO на случай, если другие не подключены
unsigned long time;              // holds current time for change of state
int flashtime = 400;             // Время вспышки светодиодов
boolean ledon = false;           // remembers start of LED
boolean movedown = true;         // safety feature
boolean moveup = true;           // safety feature


// The below procedure handles the pulse from the RX input
void handlePin1() 
{ 
  if(pin1state) 
    pulseStart1=micros(); // we got a positive edge 
  else 
    pulseTime1=micros()-pulseStart1; // Negative edge: get pulsewidth 
} 


// LED Control - very basic, can be expanded.
 void turnledoff() {
    digitalWrite(led1, LOW);     
    digitalWrite(led2, LOW);     
    digitalWrite(led3, LOW);     
    digitalWrite(led4, LOW);
    digitalWrite(led13, LOW);   
  }
  void turnledon() {
    digitalWrite(led1, HIGH);     
    digitalWrite(led2, HIGH);     
    digitalWrite(led3, HIGH);     
    digitalWrite(led4, HIGH);     
    digitalWrite(led13, HIGH); 
}
  void flashled() {
      // LED Flash
  if (millis() - time > flashtime){
      time = millis();
     
      if (ledon){
        turnledoff();
        ledon = false;
      } else {
        turnledon();
        ledon = true;
      }
     
  }


  }


// Lets set everything up
void setup() {
 // initialize serial communication at 115200 bits per second so we can see whats going on:
  Serial.begin(115200);
 
  pinMode(PIN1, INPUT);          // Initialize rx input pin
  digitalWrite(PIN1, HIGH);      // Set the rx pin to High to start the counter
  PCintPort::attachInterrupt(PIN1, &handlePin1, CHANGE); // Attach the interrupt
 
  pinMode(led1, OUTPUT);         // Assign output mode to pin for LED 1
  pinMode(led2, OUTPUT);         // Assign output mode to pin for LED 2
  pinMode(led3, OUTPUT);         // Assign output mode to pin for LED 3
  pinMode(led4, OUTPUT);         // Assign output mode to pin for LED 4
  pinMode(led13, OUTPUT);         // Assign output mode to pin for LED 4


  pinMode(msup, INPUT);
  pinMode(msdown, INPUT);
  msupstate = digitalRead(msup);
  msdownstate = digitalRead(msdown);
  time = millis();               // Save current millis reading (this is a counter that always runs in milliseconds)
  turnledon();
 }


// Main Program
void loop() {
 
  msupstate = digitalRead(msup);
  msdownstate = digitalRead(msdown);
  // read the input channels


   if (pulseTime1 > 900 && pulseTime1 < 1300){ //legs down
   myservo.attach(3);
    pos = 0;
    moveup = true;
    if (msupstate == LOW){
      if (movedown){myservo.write(pos);}
    flashtime = 200;
    flashled();
    } else {
      movedown = false;
      myservo.detach();
      turnledon();
    }
   }
    if (pulseTime1 > 1299 && pulseTime1 < 1701){ //no movement
    myservo.detach();
   
    pos = 100;
   }
   
  if (pulseTime1 > 1700) { //legs up
  myservo.attach(3);
    pos = 180;
    movedown = true;
    if (msdownstate == LOW){
       if (moveup) {myservo.write(pos);}
    flashtime = 200;
    flashled();
    } else {
      moveup = false;
      turnledon();
      myservo.detach();
    }
  } 
   
}
