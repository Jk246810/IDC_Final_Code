//LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Servo.h>
#define servoPin 12
#define qtiPin 10 //for air sensing 
Servo s;

//accelerometer
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

//ping sensor 
#define trigPin 7
#define ePin 8

//color sensing 
#include <Wire.h>
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

//initialize button 
const int buttonPin = 11; 
int buttonState = 0; 

//end of Game Sum 
int sum = 0; 

//for earth sensing 
int inches = 0;
int cm = 0;

//end of Game Variables 
int speakerPin = 6;

int numTones = 62;
int tones[] = {587, 784, 880, 988, 988, 988, 988, 988, 988, 988, 932, 988, 784, 784, 784, 784, 784, 784, 784, 880, 988, 1047, 1047, 1319, 1319, 1319, 1319, 1175, 1047, 988, 988, 988, 784, 880, 988, 1047, 1047, 1319, 1319, 1319, 1319, 1175, 1047, 988, 988, 784, 784, 784, 784, 880, 988, 988, 988, 1047, 880, 880, 880, 988, 784, 784, 784, 784};
//            You are my sunshine


int tonesTwo[] = {523, 494,440, 392, 349, 330, 294};

int redPin = 3;
int greenPin = 2; 
int bluePin = 4; 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //initialize button that will capture a specific state; 
  
  //initialize servomotor
  s.attach(servoPin);
  s.write(0);

  //initialize button 
   pinMode(buttonPin, INPUT);

   setupAccelerometer();

   //initialize rgb
   pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  //initialize LCD
  lcd.init();
  lcd.backlight();
   
}

void loop() {
  if(Serial.available()){
    String c = Serial.readString(); 
    if(c.substring(0,1).equals("w")){
      waterSensingFunc(); 
      //start water sensing tasks 
    }else if(c.substring(0,1).equals("e")){
      earthSensingFunc(); 
      //start earth tasks
    }else if(c.substring(0,1).equals("f")){
      fireSensingFunc(); 
       
      //start fire tasks
    }else if(c.substring(0,1).equals("a")){
      //start air tasks
      airSensingFunc();  
    }else if(c.substring(0,1).equals("n")){
      Serial.println(c);
      int num = c.substring(1).toInt();
      Serial.println(num); 
      Serial.println("added: " + num);  
    }else if(c.substring(0,1).equals("d")){//function to end game
      endGame();
    }
  }

}
void earthSensingFunc(){

  int ring = 0; 
  String result = ""; 
     cm = 0.01723 * readUltrasonicDistance(trigPin, ePin);
  // convert to inches by dividing by 2.54
    inches = (cm / 2.54);

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.println("cm");
  delay(100); // Wait for 100 millisecond(s)
    //need to adjust threshold
  if(inches<=2)
  {
    ring = 1; 
    result = "1"; 
  }
  else if(inches>=3 && inches<=5){
    ring = 2; 
    result = "2"; 
   }
  else if(inches>=6 && inches<=8){
   ring = 3; 
   result = "3"; 
  }

  sum += ring; 
  lcd.clear(); 
  lcd.setCursor(0,1); 
  lcd.print(result);
  
}

void waterSensingFunc(){
 
  int wave = 0; 
  String result = ""; 
  while(wave == 0){
    sensors_event_t a, g, temp;
     mpu.getEvent(&a, &g, &temp);
    if(a.acceleration.x>=-0.8 && a.acceleration.x<=-0.1){
    
    Serial.print("object 2"); 
     wave = 2; 
     result = "2"; 
   }
  
   else if(a.acceleration.x<=-1){
    Serial.print("object 3"); 
    wave = 3; 
    result = ""; 
   }
  
   else if (a.acceleration.x>=-0.08){
    Serial.print("object 1"); 
    wave = 1;  
    result = "1"; 
    }
  }
 sum += wave; 
 lcd.clear(); 
 lcd.setCursor(0,1); 
 lcd.print(result); 
 delay(2000); 
}

void fireSensingFunc(){
  bool sensing = true;
  int fireState = 0; 
  String result = ""; 
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
  while(sensing){
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  //need to adjust thresholds
  if((g>=450 && g<=1300) && (b<=1200) && (c>=7200 && c<=10000))
  {
    Serial.println("red"); 
    fireState = 1; 
    result = "1"; 
    delay(1000);
  }
  else if((g>=1400 && g<=2200) && (c>=7100&& c<=11000)){
    Serial.println("orange");
    fireState = 2; 
    result = "2"; 
    delay(1000);
   }
  else if((colorTemp>4500)){
    Serial.println("blue");
    fireState = 3; 
    result = "3"; 
    delay(1000);
  }
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("Stored variable: " + fireState);
    sum += fireState; 
    
    //added lcd functionality
    lcd.clear();
    lcd.setCursor(0,1) ;
    lcd.print(result); 
    
    buttonState = 0; 
    sensing = false; 
    
  }
  }
}

void airSensingFunc(){
  //need to add part that disables sensing when the button is pressed
  long int count = 0;
  int t = 0;   
  int horn; 
  
   //rotates 
   while(t!=5){
   s.write(0);
   for (int i = 0; i <= 180; i += 3) { 
   s.write(i);
   delay(40);
   int qti1 = RCTime(qtiPin);
   //Serial.println(qti1); 
      
        count+= qti1; 
 
   }
    t++; 
  }
   Serial.println(count); 
   lcd.clear();
   lcd.setCursor(0,1); 
   if(count >= 14000 && count <= 20000){
          lcd.print("1"); 
          Serial.println("horn1"); 
          sum += 1; 
       }else if(count <= 12000){
         lcd.print("3");
          Serial.println("horn3"); 
          sum += 3; 
       }else if(count > 20000){
        Serial.println("horn2"); 
         sum += 3; 
       } 
 
}



void endGame(){//execute code 
  int nation = sum%4; 
  if(nation == 0){
    waterLightShow();  
  }else if(nation == 1){
    songInMinorKey(); 
  }else if(nation == 2){
    fireLightShow();
  }else if(nation == 3){
    whimsicalSong();
  }
  sum = 0; 
}

void whimsicalSong(){
    for (int i = 0; i < numTones; i++)
  {
    tone(speakerPin, tones[i]);
    delay(290);
  }
  noTone(speakerPin);

}

void songInMinorKey(){
  for(int i=0; i<20;i++){
    tone(speakerPin, tonesTwo[i]); 
    delay(300); 
  }
  noTone(speakerPin); 
}

void fireLightShow(){
  for(int i=0; i<20;i++){
    setColor(255, 0, 0);
    delay(1000);
    setColor(255, 10, 0); 
    delay(1000);
    setColor(255, 50, 0); 
    delay(1000);
    setColor(255,75,50); 
    delay(1000);
  }
}

void waterLightShow(){
  for(int i=0; i<20;i++){
    setColor(0, 0, 255);
    delay(1000);
    setColor(255, 0, 50); 
    delay(1000);
    setColor(0, 255, 150); 
    delay(1000);
    setColor(255,75,50); 
    delay(1000);
  }
}
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void setupAccelerometer() {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}


//retrieves a values from the qti sensor 
long RCTime(int sensorIn)
{
  
  long duration = 0;
  pinMode(sensorIn, OUTPUT); // Sets pin as OUTPUT
  digitalWrite(sensorIn, HIGH); // Pin HIGH
  delay(1); // Waits for 1 millisecond
  pinMode(sensorIn, INPUT); // Sets pin as INPUT
  digitalWrite(sensorIn, LOW); // Pin LOW

  while(digitalRead(sensorIn)) { // Waits for the pin to go LOW
    duration++;

  }
return duration; // Returns the duration of the pulse
}

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}
