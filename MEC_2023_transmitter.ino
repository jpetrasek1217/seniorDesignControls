#include <ESP8266WiFi.h>
#include <espnow.h>

// PUT THE PIN YOUR BUTTON(s) IS(are) ON HERE. Figure this out on your own!
int left = 16;
int up = 5;
int right = 4;
int servo = 2;

// REPLACE WITH RECEIVER MAC Address. The following is an example for MAC address 48:3F:DA:4C:A4:F2
uint8_t broadcastAddress[] = {0x48, 0x3F, 0xDA, 0x4C, 0xA4, 0xF2};

// Structure example to receive data
// MUST MATCH THE SENDER STRUCTURE!!! Copy/paste them to ensure they are identical
typedef struct struct_message {
  int d_zero; //If you have more than one button, you need more here.
  int d_one;
  int d_two;
  int d_four;
  //you would put int d_one and int d_two in here as well if you had 3 digital buttons
} struct_message;

struct_message myData;
unsigned long lastTime = 0;
unsigned long timerDelay = 50; // send readings timer, lower this for less delay

// Callback when data is sent. This is good for troubleshooting; open your serial monitor if you are having issues with signal transmission.
// If you are getting “delivery success”, chances are your problem is with your receiver or your button wiring
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  } else{
    Serial.println("Delivery fail");
  }
}

void setup() {
    // Init Serial Monitor. CHECK THIS FOR TROUBLESHOOTING.
    Serial.begin(115200);
    

    //Set button0 as an input for the ESP8266
    pinMode(left, INPUT);
    pinMode(up, INPUT);
    pinMode(right, INPUT);
    pinMode(servo, INPUT);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    myData.d_zero = digitalRead(left); //If you have more than one button, you need more here.
    // for example, another line for a second button would be myData.d_one =
    myData.d_one = digitalRead(up);
    myData.d_two = digitalRead(right);
    myData.d_four = digitalRead(servo);
    
    if (digitalRead(left) == HIGH){
      Serial.println("left Pressed");
    } else{
      Serial.println("Waiting for left");
    }

    if (digitalRead(right) == HIGH){
      Serial.println("right Pressed");
    } else{
      Serial.println("Waiting for right");
    }

    if (digitalRead(up) == HIGH){
      Serial.println("up Pressed");
    } else{
      Serial.println("Waiting for up");
    }

    if (digitalRead(servo) == HIGH){
      Serial.println("servo Pressed");
    } else{
      Serial.println("Waiting for servo");
    }
    
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    lastTime = millis();
  }
  
}