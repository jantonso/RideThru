#include <PS2X_lib.h>  //for v1.6

#include <avr/io.h> // interrupt stuff
#include <avr/interrupt.h>

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte type = 0;
byte vibrate = 0;
int counter = 0;
volatile int tickCounter = 0;
int delayCounter = 0;
int fanValue = 40;

int reedSwitchPin = 2; // PD2 = INTO = digital pin 2
int fanPin = 3; // PD3 = digital pin 3 = PWM pin

// Install the interrupt routine
ISR(INT0_vect) {
   tickCounter += 1;
}

void setup(){
 Serial.begin(115200);
 
 pinMode(fanPin, OUTPUT);
 
 pinMode(reedSwitchPin, INPUT);
 // Global enable INT0 interrupt
 GICR |= (1 << INT0);
 // Signal change triggers interrupt
 MCUCR |= (1 << ISC00); 
 //MCUCR |= (0 << ISC01);
 
 // MISO = PB4 = digital pin 12 = data
 // MOSI = PB3 = digital pin 11 = command
 // SCK = PB5 = digital pin 13 = clock
 // SS = PB2 = digital pin 10 = attention
  
 error = ps2x.config_gamepad(13,11,10,12, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 if(error == 2)
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
   //Serial.print(ps2x.Analog(1), HEX);
   
   type = ps2x.readType(); 
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");
       break;
     }
}

void loop(){
   /* You must Read Gamepad to get new values
   Read GamePad and set vibration values
   ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
   if you don't enable the rumble, use ps2x.read_gamepad(); with no values
   
   you should call this at least once a second
   */
   
   if (error != 1) {
      checkPS2();
   }
    
   // Change fan speed according to RPM of user
   if(delayCounter == 500){
     /* 5 is 60 */
     if(tickCounter < 2) fanValue = 40;
     if(5 > tickCounter > 2) fanValue = 10;
     if(tickCounter > 5) fanValue = 0;
     analogWrite(fanPin, fanValue);
     Serial.print("RPM = ");
     Serial.println(tickCounter * 12);
     Serial.print(", Fan Value after 5 seconds =");
     Serial.println(fanValue);
     delayCounter = 0;
     tickCounter = 0;
   }
 
 delay(10);
 delayCounter++;     
}

void checkPS2() {
  ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed
    
 if(ps2x.Button(PSB_START))                   //will be TRUE as long as button is pressed
     Serial.println("Start is being held");
 if(ps2x.Button(PSB_SELECT))
     Serial.println("Select is being held");      
 if(ps2x.Button(PSB_PAD_UP)) {         //will be TRUE as long as button is pressed
     Serial.print("Up held this hard: ");
     Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
  }
  if(ps2x.Button(PSB_PAD_RIGHT)){
     Serial.print("Right held this hard: ");
     Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
  }
  if(ps2x.Button(PSB_PAD_LEFT)){
     Serial.print("LEFT held this hard: ");
     Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
  }
  if(ps2x.Button(PSB_PAD_DOWN)){
   Serial.print("DOWN held this hard: ");
   Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
  }    
  //vibrate = ps2x.Analog(PSAB_BLUE);        //this will set the large motor vibrate speed based on 
                                           //how hard you press the blue (X) button    

  if (ps2x.NewButtonState()) {              //will be TRUE if any button changes state (on to off, or off to on)           
      if(ps2x.Button(PSB_L3))
          Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
          Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
          Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
          Serial.println("R2 pressed");
      if(ps2x.Button(PSB_GREEN))
          Serial.println("Triangle pressed");
   }   
   if(ps2x.ButtonPressed(PSB_RED))             //will be TRUE if button was JUST pressed
       Serial.println("Circle just pressed");
   if(ps2x.ButtonReleased(PSB_PINK))             //will be TRUE if button was JUST released
       Serial.println("Square just released");     
   if(ps2x.NewButtonState(PSB_BLUE))            //will be TRUE if button was JUST pressed OR released
       Serial.println("X just changed");    
   if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { // print stick values if either is TRUE
       Serial.print("Stick Values:");
       Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
       Serial.print(",");
       Serial.print(ps2x.Analog(PSS_LX), DEC); 
       Serial.print(",");
       Serial.print(ps2x.Analog(PSS_RY), DEC); 
       Serial.print(",");
       Serial.print(ps2x.Analog(PSS_RX), DEC);
       Serial.print(",");
       Serial.print("Fan should be on ");
       Serial.println(fanValue, DEC);
   } 
}

