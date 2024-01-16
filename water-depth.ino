#include <SPI.h>
#include "ds3234.h"
#include <avr/sleep.h>
#include <SdFat.h>
#include <avr/power.h>
#include <Wire.h>

//pin assignments
const int CS = 9; //
int ss = 10;
int T_power = 4;
int V5_cntrl = 8;
// A0=TDS,A1=turb,A2=pressure,A3=temperature

//Create objects
SdFat sd;
SdFile file;


//RTC time and alarm stuff
unsigned char wakeup_min;
struct ts t;
uint8_t sleep_period = 60;       // the sleep interval in minutes between 2 consecutive alarms
volatile boolean extInterrupt1 = false;    //external interrupt flag1 

char newfile[] = "T3_C.csv";

float T,T_volt,R_T;
int16_t P_dn,T_dn;
int P_off=0;


void set_alarm(void)
{  
    // calculate the minute when the next alarm will be triggered
    wakeup_min = (t.min / sleep_period + 1) * sleep_period;
    if (wakeup_min > 59) {
        wakeup_min -= 60;
    }

    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm - see datasheet
    // A1M1 (seconds) (0 to enable, 1 to disable)
    // A1M2 (minutes) (0 to enable, 1 to disable)
    // A1M3 (hour)    (0 to enable, 1 to disable) 
    // A1M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[4] = { 0, 1, 1, 1};

    // set Alarm1
    DS3234_set_a2(ss, wakeup_min, 0, 0, flags);

    // activate Alarm1
    DS3234_set_creg(ss, DS3234_INTCN | DS3234_A2IE);
}
 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode (V5_cntrl, OUTPUT);
  pinMode (T_power, OUTPUT);
  
//  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE3));
  DS3234_init(ss, DS3234_INTCN);  
  DS3234_get(ss, &t);
  set_alarm();
  DS3234_clear_a2f(ss);
  delay(10);
  
  while (!sd.begin(CS, SPI_HALF_SPEED)) {}
   
  file.open(newfile, O_WRITE | O_CREAT | O_APPEND);
  file.close();
  Serial.println("done");
  delay(100);
  
  attachInterrupt(0, alarm, FALLING); // setting the alarm interrupt 
 
 }

void loop() {

 gotoSleep();
  delay(50);
  
//calculate water temperature
  digitalWrite(T_power, HIGH);
  digitalWrite(V5_cntrl,HIGH);
  delay(1000);

  T_dn = analogRead(A0);
  T_volt = (float)T_dn*3.3/1024.0;
  R_T = 10.0*T_volt/(3.3-T_volt);
  T = -21.4*log(R_T/10.0)+25.172;
  
  digitalWrite(T_power, LOW);


  // read pressure and calculate m water
  P_dn = analogRead(A1);
  float P_v=(float)P_dn*3.3/1024.0*(4.7+10.0)/10.0;
 float m_W = 113.04*P_v - 17.67;


  digitalWrite(V5_cntrl,LOW);
 
//get current time and reset alarm    
  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE3));
  delay(20);
  DS3234_get(ss, &t);
  set_alarm();
  DS3234_clear_a2f(ss);

      String dataString = "";
       dataString += t.year;
      dataString += ",";
      dataString += t.mon;
      dataString += ",";
      dataString += t.mday;
      dataString += ",";
      dataString += t.hour;
       dataString += ",";
      dataString += t.min;
       dataString += ",";
       //dataString += P_v;
      // dataString += ",";
       dataString += T;
       dataString += ",";
       dataString += m_W;

       while (!sd.begin(CS,SPI_HALF_SPEED)) {}
       file.open(newfile, O_WRITE | O_APPEND); //Opens the file
       file.println(dataString); //prints data string to the file
       delay(5);
       file.close(); //closes the file
       Serial.println(dataString);   
       delay(100);    

  attachInterrupt(0, alarm, FALLING);
}
void gotoSleep(void)
{
   byte adcsra = ADCSRA;          //save the ADC Control and Status Register A
   ADCSRA = 0;  //disable the ADC
   sleep_enable();
   power_spi_disable(); 
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
   cli();
   sleep_bod_disable();
   sei();
   sleep_cpu();
   /* wake up here */
   sleep_disable();
   power_spi_enable(); 
   ADCSRA = adcsra;               //restore ADCSRA
}
void alarm()
{
  detachInterrupt(0);
}
