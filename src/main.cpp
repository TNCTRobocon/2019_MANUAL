#include <Arduino.h>

#include <FlexiTimer2.h>

#include "LIB/LCD/I2CLiquidCrystal.h"
#include "MOTOR/encoder.h"
#include "XCON/xcon.h"
#include "INTERRUPT/interrupt.h"
#include "SLV/slv.h"
#include "SW/sw.h"
#include "OPERATION/manual.h"
#include "CAR/car.h"

I2CLiquidCrystal lcd(63,true);

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  FlexiTimer2::set(5,1.0/10000,interrupt_service);//500us
  xcon_init();
  encoder_init();
  SLV_init();
  SW_init();
  manual_init();
  pinMode(13,OUTPUT);
  FlexiTimer2::start();
}

void loop() {
  lcd.clear();
  /*lcd.setCursor(0,0);
  lcd.print(car.get_current_x_velocity());
  lcd.setCursor(8,0);
  lcd.print(car.get_current_y_velocity());
  lcd.setCursor(0,1);
  lcd.print(car.get_current_angle());*/
  lcd.setCursor(0,0);
  lcd.print(encoder_get_RPM(0));
  lcd.setCursor(8,0);
  lcd.print(encoder_get_RPM(1));
  lcd.setCursor(0,1);
  lcd.print(encoder_get_RPM(2));
  lcd.setCursor(8,1);
  lcd.print(encoder_get_RPM(3));
  delay(100);
}