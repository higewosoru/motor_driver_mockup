

/*
Tiny motor driver mockup

The main goal was to test the control signals of a motor driver board with an automated test system.

I made a little tester board with some leds and buttons meant to simulate
different I/O signals and stuff of that driver board. Buttons are used to create fail states.

This program allows for two methods of control:

1) Serial - this was the original plan for the actual board under test.
2) Reference Voltage - turns out, the board is actually controlled by a reference voltage.
This mode allows us to control the fake motor driver board in the same way.

The serial_mode_pin should be pulled LOW to DISABLE serial mode.

*/

int leds[] = {8, 9, 10};
int buts[] = {3, 4, 5};
int serial_mode_pin{13};
int dig_outs[]={0, 1, 2};
int control_pin{A0};




//********* Basic Functions ************
//**************************************

int sizeof_Array(int arr[]){
  int tmp = (sizeof(arr)/sizeof(arr[0]));
  return tmp;
}


void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');
    delay(300);
  }
  Serial.read();
}

void startup_test() {
   for(int i=0; i<((int) sizeof_Array(leds)); i++) {
    digitalWrite(leds[i], HIGH);
    delay(500);
    digitalWrite(leds[i], LOW);
   }
}


//***** variables for setup *******
const int sizeof_int{2};
bool serial_mode=false;
int number_of_leds = (int) (sizeof(leds)/sizeof_int);



// ******* LED config ********
//****************************

void forward() {
  digitalWrite(leds[0], HIGH);
}
void fforward() {
  digitalWrite(leds[2], HIGH);
}

void reverse() {
  digitalWrite(leds[1], HIGH);
}
void rreverse() {
  digitalWrite(leds[0], HIGH);
  digitalWrite(leds[1], HIGH);
}

void clear_leds(int number_of_leds) {
  for(int i=0; i<number_of_leds; i++) {
    digitalWrite(leds[i], LOW);
  }
}




//**** Serial control mode ******
//*******************************

byte cmd{}; //buffer to store single-byte serial commands

void serial_control() {
  while(Serial.available() > 0) {
        cmd=Serial.read();
  }
  //Once command is recieved, switch/case will determine what LEDs to switch on.
  switch(cmd) {
    case 'f':
    case 'F':
      forward();
      break;
    case 'r':
    case'R':
      reverse();
      break;
    case 'x':
    case 'X':
      fforward();
      break;
    case 'b':
    case 'B':
      rreverse();
      break;
    case 'c':
    case 'C':
      clear_leds(number_of_leds);
       break;
    default:
      break;
   }
}




//******** Analog Control Mode *******
//************************************

//Pre-determined control voltages:
//Motor has forward, fast-forward, reverse, and fast-reverse modes.
float fwd_volt{3.5};
float ffwd_volt{5.0};
float rev_volt{3.0};
float rrev_volt{1.5};
float error{0.3}; // +/- error allowance

//convert 8-bit ADC value to floating point voltage
float adc_to_volt(int adc_val) {
  float voltage = 5*((float) adc_val/1024.0);
  return voltage;
}

//returns true if voltage is between value +/- error.
bool range_compare(float voltage, float value, float error) {
  if(((value-error)<voltage) && (voltage<(value+error))) {
    return true;
  }
  else {
    return false;
  }
}

void ref_control() {
  if (analogRead(control_pin)>0) {
    int adc_val = analogRead(control_pin);
    float voltage = adc_to_volt(adc_val);
    if (voltage<(0+error)) {
      ;
   }
    else if(range_compare(voltage, fwd_volt, error)) {
      forward();
    }
    else if(range_compare(voltage, rev_volt, error)) {
      reverse();
    }
    else if(range_compare(voltage, ffwd_volt, error)) {
      fforward();
    }
    else if(range_compare(voltage, rrev_volt, error)) {
      rreverse();
    }
  }
  clear_leds(number_of_leds);
}




//*************** SETUP *******************
//*****************************************

void setup() {
  for(int i=0; i<sizeof_Array(leds); i++) {
    pinMode(leds[i], OUTPUT);
  }
  for(int i=0; i<sizeof_Array(buts); i++) {
    pinMode(buts[i], INPUT);
  }
  /*pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  pinMode(but3, INPUT);*/

  pinMode(serial_mode_pin, INPUT);

  if(digitalRead(serial_mode_pin)==HIGH){
    serial_mode=true;
  }



  if(serial_mode) {
    Serial.begin(9600);
    while(!Serial) {
      ; //wait for serial to connect.
    }
    establishContact();
    //startup_test();
  }
}




//**************** MAIN ********************
//******************************************

void loop() {

  //****  manually control LEDs  *******
  for(int i=0; i<3; i++) {
    if (digitalRead(buts[i])==HIGH) {
      digitalWrite(leds[i], HIGH);
    }
    else {
      if(!serial_mode) {
        digitalWrite(leds[i], LOW);
      }
    }
  }


  //****** analog ref controls LEDs ********
  //mutually exclusive with serial mode to avoid confusion.
  if(!serial_mode) {
    ref_control();
  }

  //****** Serial control of LEDs *******
  if(serial_mode) {
    serial_control();
  }

}
