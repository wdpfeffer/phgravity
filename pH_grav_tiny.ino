#include "ph_grav.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Gravity_pH pH = Gravity_pH(A5);
const int menuBtn = 7;
const int enterBtn = 8;
static char myResult[5];


//button states
int lastMenuState = HIGH;  //current menu button reading
int lastEnterState = HIGH; //current enter button reading
int menuState;
int enterState;

//menu level
int menuLevel = 1;

//timings for debounce
unsigned long lastDBTmenu = 0;
unsigned long lastDBTenter = 0;
unsigned long dbDelay = 50;

//boolean for button pushed event
bool menuPushed = false;
bool enterPushed = false;

#define GPIO_ADDR     0x27
LiquidCrystal_I2C lcd(GPIO_ADDR, 16, 2); // set address & 16 chars / 2 lines

void setup() {
  //setup serial
  // Serial.begin(9600);
  //delay(200);
  //Serial.println(F("Use commands \"CAL,4\", \"CAL,7\", and \"CAL,10\" to calibrate \nthe circuit to those respective values"));
  
  //setup buttons
  pinMode(menuBtn, INPUT_PULLUP);
  pinMode(enterBtn, INPUT_PULLUP);
  
  //setup lcd
  lcd.init();                           // initialize the lcd
  lcd.backlight();                      // Print a message to the LCD.
  lcd.clear();
  lcd.home();
  lcd.print("WMT pH");
  

  //check for calibration
  if(pH.begin()){
    lcd.setCursor(0,1);
    lcd.print("Loaded EEPROM");
  }
  delay(500);
  lcd.clear();
  
}

void loop() {
  //check button for push
  int menuRead = digitalRead(menuBtn);
  int enterRead = digitalRead(enterBtn);
   //debounce menu button
  if (menuRead != lastMenuState){
    //reset db timer
    lastDBTmenu = millis();
  }
  
  if (enterRead != lastEnterState){
    //reset db timer
    lastDBTenter = millis();
  }

  //was a button pushed?
  if ((millis() - lastDBTmenu) > dbDelay){
    //button has been pushed
    menuPushed = true;
  }
  if ((millis() - lastDBTenter) > dbDelay){
    //button has been pushed
    enterPushed = true;
  }
  
  switch(menuLevel){
    case (0):
      //just reading ph
      lcd.setCursor(0,1); //col, row
      lcd.print("pH = ");
      dtostrf(getph(), 5, 2, myResult);
      lcd.print(myResult);
      delay(100);

      //check for menu push
      if (menuPushed){
        menuLevel = 1;
        menuPushed = false;
      }
      break;
      
    case (1):
      //begin menu
      //Confirm calibration
      //lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calibrate?");
      lcd.setCursor(0,1);
      lcd.print("Y");
      lcd.setCursor(15,1);
      lcd.print("N");
      if (menuPushed){
        //user want to calibrate
        menuLevel = 2;
        menuPushed = false;
        lcd.clear();
      }
      if (enterPushed){
        //user wants to cancel
        enterPushed = false;
        menuLevel = 0;
        lcd.clear();
      }
      break;
    case (2):
      //calibrate low
      //lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calibrate Low?");
      lcd.setCursor(1,1);
      lcd.print("Y");
      lcd.setCursor(1,12);
      lcd.print("Next");
      if (menuPushed){
        //user want to calibrate
        calibrate(0);
        lcd.clear();
        lcd.home();
        lcd.println("Low Calibrated");
        delay(1000);
        menuPushed = false;
        menuLevel = 0;
      }
      if (enterPushed){
        //user wants to calibrate next level
        enterPushed = false;
        menuLevel = 3;
      }
      break;
    case (3):
      //calibrate Mid
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Calibrate Mid?");
      lcd.setCursor(1,1);
      lcd.print("Y");
      lcd.setCursor(1,12);
      lcd.print("Next");
      if (menuPushed){
        //user want to calibrate
        calibrate(1);
        lcd.clear();
        lcd.home();
        lcd.println("Mid Calibrated");
        delay(1000);
        menuPushed = false;
        menuLevel = 0;
      }
      if (enterPushed){
        //user wants to calibrate next level
        enterPushed = false;
        menuLevel = 4;
      }
      break;
    case (4):
      //calibrate High
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Calibrate High?");
      lcd.setCursor(1,1);
      lcd.print("Y");
      lcd.setCursor(1,12);
      lcd.print("Exit");
      if (menuPushed){
        //user want to calibrate
        calibrate(2);
        lcd.clear();
        lcd.home();
        lcd.println("High Calibrated");
        delay(1000);
        menuPushed = false;
        menuLevel = 0;
 
      }
      if (enterPushed){
        //user wants to exit menu
        enterPushed = false;
        menuLevel = 0;
      }
      break;
  }
  lastMenuState = menuRead;
  lastEnterState = enterRead;
}


float getph(){
  float myph = 0;
  for (int i=0; i<=9;i++){
    myph += pH.read_ph();
    delay(50);
  }
  // get the average
  myph = myph/10.0;
  return myph;
}

void calibrate(int level) {
  switch(level){
    case(0):
      pH.cal_low();
      break;
    case(1):
      pH.cal_mid();
      break;
    case(2):
      pH.cal_high();
      break;
  } 
  
}
