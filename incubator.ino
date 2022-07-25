#include <DHT.h>
#include <DHT_U.h>

//temp&hum pins
const int DHTPIN = 2;    // Digital pin connected to the DHT sensor (Signal pin)##############
#define DHTTYPE DHT11   // DHT 11(Defining DHT type)
DHT dht(DHTPIN, DHTTYPE); // Creates a DHT object. Parameters: (DHT Pin, DHT Type)
const int ThermistorPin1 = A1; //skin temp sensor 
int Vo;
float R1 = 10000;
float logR2, R2, T,TC; 
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

//sound sensor pins
const int s_analogPin = A0; //analog signal sound
int s_analogValue=0;

//Alarm pins
int LED;
const int alarmLED_temp = 8; // LED Alarm
const int alarmLED_hum = 9; // LED Alarm
const int alarmLED_sound = 10; // LED Alarm
const int alarmLED_waterTank = 11; // LED Alarm
const int alarmLED_apenea = 12; // LED Alarm
const int alarmLED_stemp = 13; // LED Alarm
//const int flow_LED =  ;
//const int alarmLED_ox = ; //led output

const int buzzerPin = 3; // buzzer pin

// relay pins
const int relay_f = 6;
const int relay_h = 7;

//Apenea
int ThermistorPin2 = A2;
int ar[50];



//ultrasonic pins
// defines pins numbers
 int trigPin = 3;
 int echoPin = 4;
// defines variables
long duration;
int distance;
int initial_position = 10; // where the sensor is placed away from the bottom of the tank

/*
//Oxygen Concentration Control

int ox_analogPin = A2; //analog signal
int ox_analogValue = 0;

// flow control

int flow_analogPin = A0;
int flow_analogValue = 0;
*/


// Variable Ranges
int hum_min = 70; //(For babies 29 - 30 weeks gestation)
int hum_max = 80;
int temp_min = 33;   
int temp_max = 38;
int stemp_min = 37.5;//############
int stemp_max = 38.5;//#############
int waterLevel_min = 2;


//function prototypes
bool temp ();
void alarm(int);  
void hum (bool);
void sound();
void skin_temp();
void Water_level();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("DHT11 test!"));
  pinMode(alarmLED_temp, OUTPUT); // output the LED pin
  pinMode(alarmLED_hum, OUTPUT); // output the LED pin
  pinMode(alarmLED_sound, OUTPUT); // output the LED pin
  pinMode(alarmLED_waterTank, OUTPUT); // output the LED pin
  pinMode(alarmLED_stemp, OUTPUT);
  pinMode(alarmLED_apenea, OUTPUT);
  pinMode(buzzerPin, OUTPUT);//buzzer output
  pinMode(relay_h, OUTPUT);
  pinMode(relay_f, OUTPUT);
  digitalWrite(relay_h, HIGH);//at first heater is on
  digitalWrite(relay_f,HIGH);//at first fan is on
  digitalWrite(alarmLED_temp,LOW);//at first the led is off
  digitalWrite(alarmLED_hum,LOW);
  digitalWrite(alarmLED_sound,LOW);
  digitalWrite(alarmLED_waterTank,LOW);
  digitalWrite(alarmLED_stemp,LOW);
  digitalWrite(alarmLED_apenea,LOW);
  pinMode(s_analogPin,INPUT);
  dht.begin();// Launch the DHT11 sensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 // pinMode (ox_analogPin,INPUT);
 // pinMode (alarmLED_ox,OUTPUT);
}

void loop() {
  bool heater = temp();
  hum (heater);
  sound();
  skin_temp();
  Water_level();
  apenea();
  //oxygen();
  Serial.println ("--------------------------");
  //delay (3000);
   
}








/****************************************************/

//Alarm
void alarm(int LED){
  
  digitalWrite(LED, HIGH);
  tone(buzzerPin, 500);
//  delay (500);
  digitalWrite(LED, LOW);
  noTone(buzzerPin);
  
  }


//humidity
void hum (bool heater){
  //humidity reading 
  float h = dht.readHumidity();
  LED = alarmLED_hum;
  
  //failed probe
  if (isnan(h)) {  
    Serial.println("Failed to read from DHT sensor!");// Print error when there's the error
    return;
  }
  Serial.print("Humidity: ");
  Serial.println(h);
  
  //humidity alarm/control 
  //heater is on 
  if (heater == true){
    if ( (h >= hum_min) && (h<= hum_max) ){
       digitalWrite(relay_f,LOW); //fan off
    }
    if (h < hum_min) {
      Serial.println("HUMIDITY IS TOO LOW!");
       
      alarm(LED);
      digitalWrite(relay_f, HIGH); //fan on
      
    }
  
    if (h > hum_max) {
      Serial.println("HUMIDITY IS TOO HIGH!");
      alarm(LED);
      digitalWrite(relay_f, LOW);
      
    }
  }

  //heater is off 
  if (heater == false){
    if ( (h >= hum_min) && (h<= hum_max) ){
       digitalWrite(relay_f,LOW); //fan off
    }
    if (h < hum_min) {
      Serial.println("HUMIDITY IS TOO LOW!");
      alarm(alarmLED_hum);
      digitalWrite(relay_f, LOW); //fan off
    }
  
    if (h > hum_max) {
      Serial.println("HUMIDITY IS TOO HIGH!");
      alarm(alarmLED_hum);
      digitalWrite(relay_f, HIGH);//fann is on
    }
  }
}
/**************************************************/
//Air temperature 
bool temp (){
  bool heater;
  //temperature reading 
  float t = dht.readTemperature();
  LED = alarmLED_temp;
  
  //failed probe
  if (isnan(t)) {  
    Serial.println("Failed to read from DHT sensor!");// Print error when there's the error
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("°C");

  //temperature alarm
  //temp in desired range
  if ((t >= temp_min) && (t <= temp_max)) {
    digitalWrite(relay_h, LOW);//heater off
    heater = false;
  }

  if (t < temp_min) {
    Serial.println("Temperature IS TOO LOW!");
    alarm(LED);
    digitalWrite(relay_h, HIGH);//heater on
    heater=true;
  }

  if (t > 38) {
    Serial.println("Temperature IS TOO HIGH!");
    alarm(LED);
    digitalWrite(relay_h, LOW);//heater off
    heater= false;
  }
  return heater;
}
/****************************************************/
// Sound sensor
void sound() {
 s_analogValue = analogRead (s_analogPin);
 LED = alarmLED_sound;

 //probe fail alarm
 if(isnan(s_analogValue)){
  Serial.print("Failed to read from Sound Sensor!");// Print error when there's the error
  return;
 }
 Serial.print("Sound Level: ");
 Serial.println (s_analogValue, DEC);   
                                                               
 //Noise Alarm
 if (s_analogValue>55){
  digitalWrite (LED,HIGH);
//  delay(500);
  digitalWrite (LED,LOW);
 }
}

/****************************************************/

void apenea(){
  
    ar[0] = analogRead(ThermistorPin2);
    int maximum = ar[0];
    int minimum = ar[0];
    bool flag1 = false;
    bool flag2 = false;

    for (int i = 1; i < 20; i++){
        ar [i] = analogRead(ThermistorPin2);
        // Serial.println(ar[i]);
        maximum = max (ar[i], maximum);
        minimum = min (ar[i], minimum);
        delay (100);
    }
    if ((maximum-minimum) < 10){
      //digitalWrite(alarmLED_apenea, HIGH);
      Serial.println("LOW respiration");
      if (!flag1)
        flag1 = true;
      else if (!flag2)
        flag2 = true;
      else {
        // 4 seconds without breathing
        digitalWrite(alarmLED_apenea, HIGH);
        Serial.println("The baby is hardly breathing!");
        }
    }
    //else if ((maximum-minimum) > 50){
      //noise
      //}
    else{
      // Normal respiration
      digitalWrite(alarmLED_apenea, LOW);
      flag1 = false;
      flag2 = false;
    }
  }


 /****************************************************/
// Skin temp
void skin_temp() {

  Vo = analogRead(ThermistorPin1);
  LED = alarmLED_stemp;
  
/* //probe fail alarm
  if(isnan(Vo)){
    Serial.println("Failed to read from skin temperature Sensor!");// Print error when there's the error
    return;
  }
  */
  
 
  if(Vo ==0){
    Serial.println("Failed to read from skin temperature Sensor!");
    
    
  }
  else{
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  //T = (T * 9.0)/ 5.0 + 32.0;
  TC= (T-32)*(5.0/9); 

  Serial.print("Temperature: "); 
  Serial.print(TC);
  Serial.println(" C"); 

  
  

  //Skin temp Alarms
  if (TC < stemp_min) {
    Serial.println("Skin temperature IS TOO LOW!");
    alarm(LED);
  }

  if (TC > stemp_max) {
    Serial.println("Skin Temperature IS TOO HIGH!");
    alarm(LED);
  }
  
  }
  }

   /****************************************************/
// Water tank level 

void Water_level() {
  
  LED = alarmLED_waterTank;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  
  // Calculating the distance -> distance= duration*0.034/2;
  distance= initial_position - (duration*0.034/2) ;
  if (distance > 0)
  {
    Serial.print("Water Level: ");
    Serial.println(distance);
  
    if(distance < waterLevel_min)
    {
      
      Serial.println("----Low Water Level, please refill the tank----");
      alarm(LED);
      digitalWrite(relay_h, HIGH);//heater on
      
    }
  } 
}
/****************************************************/

// oxygen concentration sensor
/*
void oxygen(){

 ox_analogValue = analogRead (ox_analogPin);
 Serial.println (ox_analogValue, DEC);
 LED = alarmLED_ox;
 
 
 if ((ox_analogValue<82)||(ox_analogValue>98)){
  alarm(LED);
  
  }
  */

 // oxygen flow
 /*
void flow(){

 flow_analogValue = analogRead (flow_analogPin);
 Serial.println (flow_analogValue, DEC);
  
 if ((flow_analogValue<1.5)||(flow_analogValue>2)){
  digitalWrite(flow_LED , HIGH);

  
   if ( flow_analogValue>2 ){
       digitalWrite(relay_f,LOW); //fan off
    }
    if (flow_analogValue<1.5) {
     digitalWrite(relay_f, HIGH);//fann is on
    }
  
  }
  */
