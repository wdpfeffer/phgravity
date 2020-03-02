
#include "PinChangeInterrupt.h"
#include "ph_grav.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"



//define Gravity pH meter parameters
Gravity_pH pH = Gravity_pH(A5);


//define LCD parameters
#define GPIO_ADDR     0x27
LiquidCrystal_I2C lcd(GPIO_ADDR, 16, 2); // set address & 16 chars / 2 lines

//define interrupt
#define menuButton 1 //button on pin 1
#define okButton 2 //button on pin 2
volatile byte menuLevel = 0; // variable for reading the menuStatus
volatile bool okPushed = false; //variable to seeing if OK was pushed.
static unsigned long last_menu_int_time = 0;
static unsigned long last_ok_int_time = 0;


void setup() {

  //Setup interrupt
  pinMode(menuButton, INPUT_PULLUP);
  pinMode(okButton, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(menuButton),intHandler,CHANGE);
  attachPCINT(digitalPinToPCINT(okButton),okHandler,CHANGE);
  
 
  //setup lcd
  lcd.init();                           // initialize the lcd
  lcd.backlight();                      // Print a message to the LCD.
  lcd.clear();
  lcd.home();
  lcd.print("   Willow Mt");
  lcd.setCursor(0,1);
  lcd.print("        pH");
  delay(2000);
  

  //check for calibration
  if(pH.begin()){
    lcd.setCursor(0,1);
    lcd.print(" Loaded EEPROM");
  }
  delay(500);
  lcd.clear();
  
}

void loop() {
  lcd.home();
  lcd.print("    pH = ");
  lcd.print(getph());
  switch (menuLevel)
  {
  case 0:
    //show menu  
    lcd.setCursor(0,1);
    lcd.print("menu            ");
    break;
  case 1:
    //show menu
    lcd.setCursor(0,1);
    lcd.print("Cal 4         OK");

    //check if ok pushed
    if (okPushed){
      //do the calibration an report back with a delay
      calibrate(0);
      lcd.setCursor(0,1);
      lcd.print("   Cal at pH 4  ");
      delay(1000);
    }
    okPushed = false;
    break;
  case 2:
    //show menu
    lcd.setCursor(0,1);
    lcd.print("Cal 7         OK");

    //check if ok pushed
    if (okPushed){
      //do the calibration an report back with a delay
      calibrate(1);
      lcd.setCursor(0,1);
      lcd.print("   Cal at pH 7  ");
      delay(1000);
    }
    okPushed = false;
    break;
  case 3:
    //show menu
    lcd.setCursor(0,1);
    lcd.print("Cal 10        OK");

    //check if ok pushed
    if (okPushed){
      //do the calibration an report back with a delay
      calibrate(2);
      lcd.setCursor(0,1);
      lcd.print("  Cal at pH 10  ");
      delay(1000);
    }
    okPushed = false;  
  default:
    break;
  }
}

void okHandler(){
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_ok_int_time > 200 && menuLevel > 0){
    okPushed = true;
  }
  last_ok_int_time = interrupt_time;
}
void intHandler(){
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_menu_int_time > 200){
    static unsigned long last_ok_int_time = 0;
    if (menuLevel < 3){
      //increment
      menuLevel +=1;
    }else{
      //reset
      menuLevel =0;
    }
  }
  last_menu_int_time = interrupt_time;
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
