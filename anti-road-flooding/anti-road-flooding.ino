//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//================= INCLUDE =======================
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//================= DEFINE ========================
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;
int water_value_1 = 0, water_value_2 = 0, last_lcd_state = -1;
const int LED_R = 2, LED_Y = 3, LED_G = 4;
const int RELAY_PIN = A0, SENSOR_PIN_1 = A3, SENSOR_PIN_2 = A2, SERVO_PIN = 6, SENSOR_POWER_PIN_1 = 9, SENSOR_POWER_PIN_2 = 8;
const int MAX_WATER_THREASHOLD = 115;

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//================= SETUP ========================= 
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void setup() {
  //Begining for logging
  Serial.begin(9600);
  //Setting up the LEDs
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_G, LOW);

  //Setting up the sensor
  pinMode(SENSOR_POWER_PIN_1, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN_1, LOW);
  pinMode(SENSOR_POWER_PIN_2, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN_2, LOW);

  //Setting up the pump
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  //Setting up the screen
  lcd.init();
  lcd.clear();
  lcd.backlight();

  //Setting up the servo
  servo.attach(SERVO_PIN);
  
  lcd.clear();
  delay(100);
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//================= LOOP ==========================
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
void loop() {
  //Finding the water value from the sensor and printing it for debugging purposes
  water_value_1 = readSensorValueOne();
  water_value_2 = readSensorValueTwo();
  Serial.print("Sensor 1: ");
  Serial.print(water_value_1);
  Serial.println();
  Serial.print("Sensor 2: ");
  Serial.print(water_value_2);
  Serial.println();
  
  
  //If the value is less that the minimum threadshold, the road is safe to be passed by cars
  if (water_value_1 <= MAX_WATER_THREASHOLD && water_value_2 < MAX_WATER_THREASHOLD){    
    //We turn on the green LED
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_G, HIGH);

    //Write the necessary text on the screen
    if (last_lcd_state != 0) {
      lcd.clear();
      delay(10);
      lcd.setCursor(3,0);
      lcd.print("[ROAD SAFE]");
      lcd.setCursor(2,1);
      lcd.print("-PASS FREELY-");
      last_lcd_state = 0;
    }
    
    //Open the road's bar
    servo.write(180);
    delay(500);
    
    //Otherwise if the water value is less than the maximum threadshold, the road is to be passed with caution
  } else if (water_value_1 >= MAX_WATER_THREASHOLD && water_value_2 <= water_value_1 && water_value_2 < MAX_WATER_THREASHOLD){
    //We turn on the yellow LED
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_G, LOW);
    
    //Write the necessary text on the screen
    if (last_lcd_state != 1) {
      lcd.clear();
      delay(10);
      lcd.setCursor(4, 0);
      lcd.print("[CAUTION]");
      lcd.setCursor(0, 1);
      lcd.print("-PASS CAREFULLY-");
      last_lcd_state = 1;
    }

    //Open the road's bar
    servo.write(180);
    delay(100);
    
    //Otherwise if it's more than the other values, the road is not safe to be passed
} else {
    //We turn on the red LED
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_G, LOW);

    //Write the necessary text on the screen
    if (last_lcd_state != 2) {
      lcd.clear();
      delay(10);
      lcd.setCursor(4, 0);
      lcd.print("[DANGER]");
      lcd.setCursor(2, 1);
      lcd.print("-DO NOT PASS-");
      last_lcd_state = 2;
    }

    //Close the road's bar
    servo.write(90);
    delay(100);

    //Run the water pump for 5 seconds
    digitalWrite(RELAY_PIN, LOW);
    delay(13500);
    digitalWrite(RELAY_PIN, HIGH);
    delay(100);
  }
}

//Reads the value of the first water level sensor
int readSensorValueOne() {
  digitalWrite(SENSOR_POWER_PIN_1, HIGH);
  delay(100);
  water_value_1 = analogRead(SENSOR_PIN_1);
  digitalWrite(SENSOR_POWER_PIN_1, LOW);
  return water_value_1;
}

//Reads the value of the second water level sensor
int readSensorValueTwo() {
  digitalWrite(SENSOR_POWER_PIN_2, HIGH);
  delay(100);
  water_value_2 = analogRead(SENSOR_PIN_2);
  digitalWrite(SENSOR_POWER_PIN_2, LOW);
  return water_value_2;
}
