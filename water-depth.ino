// Code developed by Reagan Pearce (UCL) and Bruce Main (King's College London)  //
///////////////////////////////////////////////////////////////////////////////////

/* LIBRARIES */
#include <SPI.h>                  //SPI communication library
#include <Wire.h>                 ////I2C communication library
#include <avr/sleep.h>            //low power library
#include <avr/power.h>            //low power library
#include "ds3234.h"               //DS3234 RTC library. Available from https://github.com/rodan/ds3234
#include <SdFat.h>                //SD card library. Available from https://github.com/greiman/SdFat

//pin assignments
const int CS = 10;             //SD module chip select
const int ss = 9;              //RTC module slave select
const int T_power = 4;          //thermistor power pin
const int V5_cntrl = 8;         //5V control pin
//A0=temperature,A1=pressure,2=Alarm pin

//SD housekeeping
SdFat sd;
SdFile file;
char newfile[] = "T3_C.csv"; //"T3_C" example file name

//alarm housekeeping
unsigned char wakeup_min;
struct ts t;
uint8_t sleep_period = 60;                 // the sleep interval in minutes between 2 consecutive alarms
volatile boolean extInterrupt1 = false;    //external interrupt flag1

//pressure & temperature variables
float T, T_volt, R_T;
int16_t P_dn, T_dn;
int P_off = 0;

/* ALARM SUBROUTINE */
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

/* SETUP */
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

/* LOOP */
void loop() {
  gotoSleep();
  delay(50);

  //calculate water temperature
  digitalWrite(T_power, HIGH);
  digitalWrite(V5_cntrl, HIGH);
  delay(1000);

  T_dn = analogRead(A0);
  T_volt = (float)T_dn * 3.3 / 1023.0;
  R_T = 10.0 * T_volt / (3.3 - T_volt);
  T = 1/(2.772539*pow(10,-3) + 2.50767*pow(10,-4)*log(R_T) + 3.37884*pow(10,-7)*pow(log(R_T),3))-273.15; // Temperature calibration in celsius

  digitalWrite(T_power, LOW);


  // read pressure and calculate m water
  P_dn = analogRead(A1);
  float P_v = (float)P_dn * 3.3 / 1023.0 * (4.7 + 10.0) / 10.0;
  float m_W = 113.04 * P_v - 17.67; // Water depth calibration x * P_v - y, x and y will depend on your calibration


  digitalWrite(V5_cntrl, LOW);

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
  //dataString += ",";
  //dataString += T_volt;  //for calibration
  dataString += ",";
  dataString += T;        // Calibrated temperature reading degrees celsius
  //dataString += ",";
  //dataString += P_v;    // for calibration of depth
  dataString += ",";
  dataString += m_W;      // Calibrated water depth (cm)

  while (!sd.begin(CS, SPI_HALF_SPEED)) {}
  file.open(newfile, O_WRITE | O_APPEND); //Opens the file
  file.println(dataString); //prints data string to the file
  delay(5);
  file.close(); //closes the file
  Serial.println(dataString);
  delay(100);

  attachInterrupt(0, alarm, FALLING);
}

/* SLEEP SUBROUTINE */
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

/* ALARM SUBROUTINE */
void alarm()
{
  detachInterrupt(0);
}
