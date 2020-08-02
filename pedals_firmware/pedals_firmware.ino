 
  
 // Flightsim Rudder Pedals Firmware
 // Copyright 2018 Bernhard Achatz 

  
  #include <Joystick.h>                             // Joystick Library by Matthew Heironimus: https://github.com/MHeironimus/ArduinoJoystickLibrary
  #include <EEPROM.h>
  #define JUMPER 3
  #define JUMPERTEST 8
  
  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,    // 0 buttons, 0 hat switches, Rx (left Brake), Ry (right Brake), RZ (Yaw) 
  JOYSTICK_TYPE_JOYSTICK , 0, 0,
  false, false, false, true, true, true,
  false, false, false, false, false);  
  



 // setup ===================================================================================================
  void setup() {

    pinMode(2,OUTPUT);             //use digital pin 2 as GND for Jumper
    digitalWrite(2,LOW);
    pinMode(JUMPER,INPUT_PULLUP);

    // Add ability to add jumper to enable / disable test mode
    pinMode(7,OUTPUT);      // use digital pin 7 as GND for test Jumper
    digitalWrite(7,LOW);
    pinMode(JUMPERTEST,INPUT_PULLUP);
    
    Joystick.begin();
    
    // restore calibration settings
    unsigned int leftBrakeMax;
    unsigned int rightBrakeMax;
    unsigned int yawMin;
    unsigned int yawMax;
    unsigned int temp;
    unsigned int leftBrakeMin;
    unsigned int rightBrakeMin;
    
    yawMin = ( EEPROM.read(0) | (EEPROM.read(1) << 8) );
    yawMax = ( EEPROM.read(2) | (EEPROM.read(3) << 8) );
    leftBrakeMin = ( EEPROM.read(4) | (EEPROM.read(5) << 8) );
    leftBrakeMax = ( EEPROM.read(6) | (EEPROM.read(7) << 8) );
    rightBrakeMin = ( EEPROM.read(8) | (EEPROM.read(9) << 8) );
    rightBrakeMax = ( EEPROM.read(10) | (EEPROM.read(11) << 8) );
    
    Joystick.setRzAxisRange(yawMin,yawMax);
    Joystick.setRxAxisRange(leftBrakeMin,leftBrakeMax);
    Joystick.setRyAxisRange(rightBrakeMin,rightBrakeMax);
    
  
  }
  
 // loop ===================================================================================================
  void loop() 
  {
    
      // get filtered ADC values and set Axis
      Joystick.setRzAxis(filter(0));  // Yaw
      Joystick.setRxAxis(filter(1));  // LeftBrake
      Joystick.setRyAxis(filter(2));  // RightBrake    
      
      // connect a jumper to digital pin 2 and 3 to run the calibration routine
      if (!digitalRead(JUMPER)) calibration();

      // check if Jumper is set on pins 7 and 8 to enable test mode
      if (!digitalRead(JUMPERTEST)) test();

  }


 //analog read with moving average filter  ===================================================================================================
  int filter(int channel) 
  {
    unsigned int sum=0;
    unsigned int i=0;

    for (i=0; i<50; i++)
    {
      sum = sum + analogRead(channel);
    }
    
    sum = sum / 50;
    return(sum);
  }




// calibration ===================================================================================================
  void calibration ()
  {

    unsigned int leftBrakeMax=0;
    unsigned int rightBrakeMax=0;
    unsigned int yawMin=1023;
    unsigned int yawMax=0;
    unsigned int temp=0;

    
    // start calibration with brakes in neutral position
    unsigned int leftBrakeMin = filter(1);
    unsigned int rightBrakeMin = filter(2);

    // capture max/min values as long as the jumper is connected
    while( !digitalRead(JUMPER))
    {

      temp=filter(0);
      if ( temp < yawMin)
      {
        yawMin=temp;
      }
      if ( temp > yawMax)
      {
        yawMax = temp;
      }

      
      temp=filter(1);
      if ( temp > leftBrakeMax)
      {
        leftBrakeMax = temp;
      }

      temp=filter(2);
      if ( temp > rightBrakeMax)
      {
        rightBrakeMax = temp;
      }

      
    }

    // set and save new Ranges
    Joystick.setRzAxisRange(yawMin,yawMax);
    Joystick.setRxAxisRange(leftBrakeMin,leftBrakeMax);
    Joystick.setRyAxisRange(rightBrakeMin,rightBrakeMax);

    EEPROM.write(0, lowByte(yawMin));
    EEPROM.write(1, highByte(yawMin));
    EEPROM.write(2, lowByte(yawMax));
    EEPROM.write(3, highByte(yawMax));
    EEPROM.write(4, lowByte(leftBrakeMin));
    EEPROM.write(5, highByte(leftBrakeMin));
    EEPROM.write(6, lowByte(leftBrakeMax));
    EEPROM.write(7, highByte(leftBrakeMax));
    EEPROM.write(8, lowByte(rightBrakeMin));
    EEPROM.write(9, highByte(rightBrakeMin));
    EEPROM.write(10, lowByte(rightBrakeMax));
    EEPROM.write(11, highByte(rightBrakeMax));
            

        
    delay(100);
  
  }





// testmode ===================================================================================================
  void test() 
  {

    Serial.print("Yaw: ");
    Serial.print(filter(0));
    Serial.print("\n");
   
    Serial.print("left Brake: ");
    Serial.print(filter(1));
    Serial.print("\n");

    Serial.print("right Brake: ");
    Serial.print(filter(2));
    Serial.print("\n");

  }


  
