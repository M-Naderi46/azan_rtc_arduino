#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DateConvL.h"
#include <stdio.h>
#include "RTClib.h"
RTC_DS3231 rtc;
//char daysOfTheWeek[7][12] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Sat" };
uint16_t current_year, current_month, current_day;
DateConvL dateC;

//#include "DFRobotDFPlayerMini.h"

//SoftwareSerial mySoftwareSerial(8, 9); // RX, TX
//DFRobotDFPlayerMini myDFPlayer;

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

int xegg, yegg;
int gYear, pYear = 0;
char gMonth, gDay, pMonth, pDay, Weekday = 0;

#define DS1307_I2C_ADDRESS 0x68  // This is the I2C address
long previousMillis = 0;         // will store last time time was updated
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
long interval = 200;
int displayx, displayy, displayradius, x2, y2, x3, y3;
int zero = 0;
char *Day[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
double fajr, sunRise, zuhr, asr, maghrib, isha;

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

// Gets the date and time from the ds1307 and prints result
void getDateDs1307() {
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  //Wire.write(0x00);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

  second = bcdToDec(Wire.read() & 0x7f);
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0x3f);
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
}

void setDateDs1307() {
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(zero);
  Wire.write(decToBcd(second));  // 0 to bit 7 starts the clock
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));  // for 12 hour am/pm, need to set bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}
String SetCurrentDate() {
  char const *date = __DATE__;
  char s_month[5];
  int _month, _day, _year;
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  sscanf(date, "%s %d %d", s_month, &current_day, &current_year);
  current_month = ((strstr(month_names, s_month) - month_names) / 3) + 1;
}

void printTime() {
  int hours, minutes, day;
  char s[12];
  display.clearDisplay();

  display.setCursor(0, 8);
  display.print(dateC.global_year);
  display.print('/');
  display.print(dateC.global_month);
  display.print('/');
  display.print('/');
  display.print(dateC.global_day);

  display.setCursor(1, 16);
  display.print(Day[dayOfWeek]);
  Serial.print(Day[dayOfWeek]);
  //display.print(":");
  Serial.print(":");
  display.print(char(current_day / 10 + 0x30));
  Serial.print(char(current_day / 10 + 0x30));
  display.print(char(current_day % 10 + 0x30));
  Serial.print(char(current_day % 10 + 0x30));
  display.print("/");
  Serial.print("/");
  display.print(char(month / 10 + 0x30));
  Serial.print(char(month / 10 + 0x30));
  display.print(char(month % 10 + 0x30));
  Serial.print(char(month % 10 + 0x30));
  Serial.print("/");
  display.print("/");
  display.print("20");
  Serial.print("20");
  display.print(char(year / 10 + 0x30));
  Serial.print(char(year / 10 + 0x30));
  display.print(char(year % 10 + 0x30));
  Serial.print(char(year % 10 + 0x30));
  Serial.println();

  display.setCursor(18, 26);
  display.print(char(hour / 10 + 0x30));
  Serial.print(char(hour / 10 + 0x30));
  display.print(char(hour % 10 + 0x30));
  Serial.print(char(hour % 10 + 0x30));
  display.print(":");
  Serial.print(":");
  display.print(char(minute / 10 + 0x30));
  Serial.print(char(minute / 10 + 0x30));
  display.print(char(minute % 10 + 0x30));
  Serial.print(char(minute % 10 + 0x30));
  display.print(":");
  Serial.print(":");
  display.print(char(second / 10 + 0x30));
  Serial.print(char(second / 10 + 0x30));
  display.print(char(second % 10 + 0x30));
  Serial.print(char(second % 10 + 0x30));
  Serial.println();
  display.display();
  delay(10000);

  doubleToHrMin(fajr, hours, minutes);
  //if(hour ==hours && minute == minutes)

  display.clearDisplay();
  display.setCursor(1, 1);
  display.print("Fajr ");
  Serial.print("Fajr ");
  display.print(hours);
  Serial.print(hours);
  display.print(":");
  Serial.print(":");
  display.print(minutes);
  Serial.print(minutes);
  display.display();
  Serial.println();

  doubleToHrMin(zuhr, hours, minutes);
  //if(hour ==hours && minute == minutes)

  display.setCursor(1, 10);
  display.print("Zuhr ");
  Serial.print("Zuhr ");
  display.print(hours);
  Serial.print(hours);
  display.print(":");
  Serial.print(":");
  display.print(minutes);
  Serial.print(minutes);
  display.display();
  Serial.println();

  doubleToHrMin(asr, hours, minutes);
  display.setCursor(1, 20);
  display.print("Asr ");
  Serial.print("Asr ");
  display.print(hours);
  Serial.print(hours);
  display.print(":");
  Serial.print(":");
  display.print(minutes);
  Serial.print(minutes);
  display.display();
  Serial.println();

  doubleToHrMin(maghrib, hours, minutes);
  //if(hour ==hours && minute == minutes)

  display.setCursor(1, 30);
  display.print("Maghrib ");
  Serial.print("Maghrib ");
  display.print(hours);
  Serial.print(hours);
  display.print(":");
  Serial.print(":");
  display.print(minutes);
  Serial.print(minutes);
  display.display();
  Serial.println();

  doubleToHrMin(isha, hours, minutes);
  display.setCursor(1, 40);
  display.print("Isha ");
  Serial.print("Isha ");
  display.print(hours);
  Serial.print(hours);
  display.print(":");
  Serial.print(":");
  display.print(minutes);
  Serial.print(minutes);
  display.display();
  Serial.println();
  delay(10000);
}



void setup() {

  Serial.begin(9600);
  Wire.begin();

  SetCurrentDate();
  dateC.ToShamsi(current_year, current_month, current_day);

  if (!rtc.begin()) {
    Serial.print("Couldn't find RTC");
    while (1)
      ;
  }

  // if (! rtc.isrunning())
  // {
  //   Serial.print("RTC is NOT running!");
  // }
  // Serial.print("RTC is NOT running!");

  display.begin();
  display.clearDisplay();
  display.setContrast(25);
  xegg = (display.width()) / 2;
  yegg = (display.height()) / 2;
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(22, 18);
  display.print("Mahmood");
  display.display();
  delay(1000);
  display.setCursor(24, 28);
  display.print("Naderi");
  display.display();
  delay(1500);
  getDateDs1307();
}


void loop() {
  getDateDs1307();
  // calcPrayerTimes(year, month, dayOfMonth, 39.8, 21.4, 3, -18.5, -19, fajr, sunRise, zuhr, asr, maghrib, isha); // year , month, day, Longitude, Latitude, Time Zone, Fajr Twilight, Esha Twilight
  calcPrayerTimes(year, month, dayOfMonth, 51.4, 35.57, 3.43, -14.5, -14, fajr, sunRise, zuhr, asr, maghrib, isha);

  printTime();
}

/*-------------------------------------------------------------------------------------*/
// PRAYER TIME (by Mahmoud Adly Ezzat, Cairo)

//convert Degree to Radian
double degToRad(double degree) {
  return ((3.1415926 / 180) * degree);
}

//convert Radian to Degree
double radToDeg(double radian) {
  return (radian * (180 / 3.1415926));
}

//make sure a value is between 0 and 360
double moreLess360(double value) {
  while (value > 360 || value < 0) {
    if (value > 360)
      value -= 360;

    else if (value < 0)
      value += 360;
  }

  return value;
}

//make sure a value is between 0 and 24
double moreLess24(double value) {
  while (value > 24 || value < 0) {
    if (value > 24)
      value -= 24;

    else if (value < 0)
      value += 24;
  }

  return value;
}

//convert the double number to Hours and Minutes
void doubleToHrMin(double number, int &hours, int &minutes) {
  hours = floor(moreLess24(number));
  minutes = floor(moreLess24(number - hours) * 60);
}

void calcPrayerTimes(int year, int month, int day,
                     double longitude, double latitude, float timeZone,
                     double fajrTwilight, double ishaTwilight,
                     double &fajrTime, double &sunRiseTime, double &zuhrTime,
                     double &asrTime, double &maghribTime, double &ishaTime) {
  double D = (367 * year) - ((year + (int)((month + 9) / 12)) * 7 / 4) + (((int)(275 * month / 9)) + day - 730531.5);

  double L = 280.461 + 0.9856474 * D;
  L = moreLess360(L);

  double M = 357.528 + (0.9856003) * D;
  M = moreLess360(M);

  double Lambda = L + 1.915 * sin(degToRad(M)) + 0.02 * sin(degToRad(2 * M));
  Lambda = moreLess360(Lambda);

  double Obliquity = 23.439 - 0.0000004 * D;
  double Alpha = radToDeg(atan((cos(degToRad(Obliquity)) * tan(degToRad(Lambda)))));
  Alpha = moreLess360(Alpha);

  Alpha = Alpha - (360 * (int)(Alpha / 360));
  Alpha = Alpha + 90 * (floor(Lambda / 90) - floor(Alpha / 90));

  double ST = 100.46 + 0.985647352 * D;
  double Dec = radToDeg(asin(sin(degToRad(Obliquity)) * sin(degToRad(Lambda))));
  double Durinal_Arc = radToDeg(acos((sin(degToRad(-0.8333)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));

  double Noon = Alpha - ST;
  Noon = moreLess360(Noon);


  double UT_Noon = Noon - longitude;


  ////////////////////////////////////////////
  // Calculating Prayer Times Arcs & Times //
  //////////////////////////////////////////

  // 2) Zuhr Time [Local noon]
  zuhrTime = UT_Noon / 15 + timeZone;

  // Asr Hanafi
  //double Asr_Alt =radToDeg(atan(2+tan(degToRad(latitude - Dec))));
  double Asr_Alt = radToDeg(atan(1.7 + tan(degToRad(latitude - Dec))));
  // Asr Shafii
  //double Asr_Alt = radToDeg(atan(1 + tan(degToRad(latitude - Dec))));
  double Asr_Arc = radToDeg(acos((sin(degToRad(90 - Asr_Alt)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
  Asr_Arc = Asr_Arc / 15;
  // 3) Asr Time
  asrTime = zuhrTime + Asr_Arc;

  // 1) Shorouq Time
  sunRiseTime = zuhrTime - (Durinal_Arc / 15);

  // 4) Maghrib Time
  maghribTime = zuhrTime + (Durinal_Arc / 15);

  double Esha_Arc = radToDeg(acos((sin(degToRad(ishaTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
  // 5) Isha Time
  ishaTime = zuhrTime + (Esha_Arc / 15);

  // 0) Fajr Time
  double Fajr_Arc = radToDeg(acos((sin(degToRad(fajrTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
  fajrTime = zuhrTime - (Fajr_Arc / 15);

  return;
}