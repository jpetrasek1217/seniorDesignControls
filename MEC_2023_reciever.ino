#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo


int val;    // variable to read the value from the analog pin
int pos;
//Figure out the corresponding pins on your own!
//Right motor
int enableRightMotor=16; 
//enable A = 16
//anable b = 14
//input 1 = 2
//intput 2 = 0
//input 3 = 4
// input 4 = 5
//servo = 3
int rightMotorPin1= 2;
int rightMotorPin2= 0;
//Left motor 
int enableLeftMotor=14; 
int leftMotorPin1= 4;
int leftMotorPin2= 5;
int servoMotorPin = 3;

#define SIGNAL_TIMEOUT 1000 // This is signal timeout in milli seconds. We will reset the data if no signal
unsigned long lastRecvTime = 0;


// Structure example to receive data
// MUST MATCH THE SENDER STRUCTURE!!! Copy/paste them to ensure they are identical
typedef struct struct_message {
  int d_zero; //If you have more than one button, you need more here.
  int d_one;
  int d_two;
  int d_four;
  //you would put int d_one and int d_two in here as well if you had 3 digital buttons
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.print("Int1: ");
  Serial.println(myData.d_zero); //Can use this to troubleshoot if needed in the serial monitor
  //You will want to add cases below to turn the car.
  
  if (myData.d_two==1 && myData.d_one==0 && myData.d_zero==1)
  {
    Serial.println("Forward Pressed!");
    CAR_moveForward();
  } 

  if (myData.d_two==0 && myData.d_one==0 && myData.d_zero==1){
    Serial.println("left Triggered!");
    CAR_moveLeft();
  }

  if (myData.d_two==1 && myData.d_one==0 && myData.d_zero==0){
    Serial.println("right Triggered!");
    CAR_moveRight();
  }

  if (myData.d_four==1){
    Serial.println("Servo Triggered!");
    SERVO_move();
  }

  if (myData.d_two==0 && myData.d_one==0 && myData.d_zero==0)  {
    CAR_stop();
  }

  lastRecvTime = millis();
}
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(enableRightMotor, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(servoMotorPin, OUTPUT);
  myservo.attach(servoMotorPin); 
  digitalWrite(enableRightMotor, HIGH); // set full speed
  digitalWrite(enableLeftMotor, HIGH); // set full speed
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

// May need to change the pin definitions below if this doesn’t actually
// move your car forward. trial and error is simplest
void CAR_moveForward() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
}


void CAR_moveLeft() {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
}

void CAR_moveRight() {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
}

void SERVO_move(){
  if (myservo.read() == 0){
    myservo.write(60);                  // waits 15ms for the servo to reach the position
  } else if (myservo.read() == 60){
      myservo.write(0);  
      delay(15);            // tell servo to go to position in variable 'pos'     
  } 
}


void CAR_stop() {
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
}

// You will also need some void function definitions for moving your car left and right,
// as well as moving the servo CW & CCW. You will need to figure this out on your own.
void loop() {
  //Check Signal lost.
  val = analogRead(servoMotorPin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(15);  
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT )
  {
    CAR_stop();
  }
}