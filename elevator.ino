#include <L293D.h>
#include <HCSR04.h>

byte triggerPin = 3;
byte echoPin = 2;
#define BUTTON_PIN 4
#define ZERO_DIST 0.9
#define MIN_PART 20
#define UP_DIST 30 
const long PART_FACTOR = 70000;
const long FULL_TIME = 120l * 1000L;

volatile byte target = 30;
bool lastbutton = false;
unsigned int cycle = 0;
volatile byte part = 0;
double *distances ;
double diff;
double diffTime;
double diffDist;
int direction;

const double TARGET_V = 30.0 / FULL_TIME;
//in/ms
long unsigned int lastClick = 0;
double lastClickDist = 0;

L293D motor(9, A0, A1);

void targetSet() {
  target = (!target) * UP_DIST;
  withoutTargetSet();
}
void withoutTargetSet() {
  lastClick = millis() - 1;
}

void setup()
{
  Serial.begin(115200);
  HCSR04.begin(triggerPin, echoPin);
  pinMode(INPUT, BUTTON_PIN);
  withoutTargetSet();
}

void loop()
{
  bool thisbutton = digitalRead(BUTTON_PIN) || (Serial.available() > 0);
  if (! lastbutton && thisbutton) { // last time wasn't but this time is
    Serial.read();
    targetSet();
  }
  lastbutton = thisbutton;



  if (cycle % 200 == 3) { //read every 200 mils

    distances = HCSR04.measureDistanceIn();
    distances[0] = distances[0] - ZERO_DIST;
    if (distances[0] < -1) {
      distances[0] = target - diff;
    }
    diff =  ((double) target - distances[0]);
    

    long int timeLeft = FULL_TIME - (millis() - lastClick);
    double speedReq = abs(diff)/abs(timeLeft);
    
    direction = (int)(diff / abs(diff));


    double Vdiff = speedReq - TARGET_V;



    part = Vdiff * PART_FACTOR + MIN_PART;

    if (part > 50) {
      part = 50;
    }
    if (abs(diff) < 0.2) {
      part = 0;
      withoutTargetSet();
    }


    Serial.print("1: ");
    Serial.print(distances[0]);
    Serial.print(" diff:");
    Serial.print(diff);
    Serial.print(" target vel: ");
    Serial.print(TARGET_V * 1000);
    Serial.print("  req speed: ");
    Serial.print(speedReq * 1000);
    Serial.print("  diff in vel * comp: ");
    Serial.print(Vdiff * PART_FACTOR);
    Serial.print("  part: ");
    Serial.print(part);
    Serial.print("  target: ");

    Serial.print(target);

        Serial.print("  diffTime: ");

    Serial.print(timeLeft/1000);


    Serial.print("  dir: ");
    Serial.println(direction);
    Serial.println(" ---");
  }


  if (cycle < part) {
    motor.set(direction * -1.0); //negative bc of physical polarity
  } else {
    motor.set(0.0);
  }

  cycle ++;
  cycle %= 1000;
  delay(1);

}
