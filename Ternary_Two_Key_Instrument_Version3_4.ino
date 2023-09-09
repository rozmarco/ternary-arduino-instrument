#include <Servo.h>

int ldrPin = A0;
int ldrValue = 0;

int piezzoPin=7;

int servo_potent_pin = A1;
int servo_pin =4;
Servo myservo;

int noteValue[]= {8,0};
int playing =0;
int current_note;
int whammy_note;
int current_bit =0;
int last_reading;

int buttonPin = 5;
int bitFlag=0;

int resetButtonPin=6;
int resetFlag =0;

int switchPin =2;
int switchState = 0;

int noteSwitchPin = 3;
int noteSwitchState =0;

// Default Settings
int open_read = 439;
int open_read_1;
int open_read_2;
int open_read_3;
int white_read = 226;
int white_read_1;
int white_read_2;
int white_read_3;
int black_read = 47;
int black_read_1;
int black_read_2;
int black_read_3;
int furthest_distance =9;

int trigPin =9;
int echoPin=10;

long duration;
int distance=24;

int noteKeys[] = {262, 294, 330, 349, 392, 440, 494, 523, 587};

int possibleNotes[] = {185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698,740,784,831, 880};

bool buttonVal;
bool resetVal; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(piezzoPin, OUTPUT);
  pinMode(servo_potent_pin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(switchPin, INPUT);
  myservo.attach(servo_pin);
  pinMode(noteSwitchPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  switchState = digitalRead(switchPin);
  noteSwitchState = digitalRead(noteSwitchPin);
  

  while (switchState) {
    Serial.println("Entering calibration mode");
    Serial.print("Collecting trit 0 reading: ");
    calibrate();
  }
  
  while (noteSwitchState) {
    Serial.println("Entering note selection mode");
    //Serial.println("Beginning note selection");
    calibrate_notes();
    
  }
  // int servo_potent_val = analogRead(servo_potent_pin);
  // servo_potent_val = map(servo_potent_val, 0 , 1023, 0, 180);
  // myservo.write(servo_potent_val);
  //Serial.println("Out of calibration mode");  
  // put your main code here, to run repeatedly:
 // setColour(255,0,0);
  //digitalWrite(ledPin, HIGH);
  
  update_motor();
  evaluate_buttons();
  
  digitalWrite(trigPin, LOW);
  update_motor();
  evaluate_buttons();
  delayMicroseconds(2);
  // update_motor();
  // evaluate_buttons();
  

  digitalWrite(trigPin,HIGH);
  update_motor();
  evaluate_buttons();
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  update_motor();
  evaluate_buttons();

  duration = pulseIn(echoPin, HIGH);
  update_motor();
  evaluate_buttons();
  last_reading = distance;
  distance = duration*0.034/2;
  update_motor();
  evaluate_buttons();

  

  //Serial.print("Distance: ");update_motor();
  //Serial.println(distance);
  

  if (distance <furthest_distance && distance>0 && playing!=0) {
   // Serial.println(current_note-distance*10);
    if (last_reading > distance) {
      whammy_note -=3;
    } else if (last_reading <distance){
      whammy_note +=3;
    }
    tone(piezzoPin, whammy_note);
  } else if (distance>furthest_distance && playing!=0) {
    tone(piezzoPin, current_note);
    whammy_note = current_note;
  }

  update_motor();
  evaluate_buttons();
  delay(5);
  // buttonVal = digitalRead(buttonPin);
  // resetVal = digitalRead(resetButtonPin);
  // if (resetVal) {
  //   reset_pressed();
  // }
  // if (buttonVal) {
  //   note_pressed();
  // }
 


}

int play(int note0, int note1) {
  int note = note0*10 +note1;
  switch (note) {
    case 0: 
      tone(piezzoPin, noteKeys[0]);
      Serial.println("Playing note: 00");
      return noteKeys[0];
      break;
    case 1:
      tone(piezzoPin, noteKeys[1]);
      Serial.println("Playing note: 01");
      return noteKeys[1];
      break;
    case 2:
      tone(piezzoPin, noteKeys[2]);
      Serial.println("Playing note: 02");
      return noteKeys[2];
      break;
    case 10: 
      tone(piezzoPin, noteKeys[3]);
      Serial.println("Playing note: 10");
      return noteKeys[3];
      break;
    case 11:
      tone(piezzoPin, noteKeys[4]);
      Serial.println("Playing note: 11");
      return noteKeys[4];
      break;
    case 12:
      tone(piezzoPin, noteKeys[5]);
      Serial.println("Playing note: 12");
      return noteKeys[5];
      break;
    case 20: 
      tone(piezzoPin, noteKeys[6]);
      Serial.println("Playing note: 20");
      return noteKeys[6];
      break;
    case 21: 
      tone(piezzoPin, noteKeys[7]);
      Serial.println("Playing note: 21");
      return noteKeys[7];
      break;
    case 22:
      tone(piezzoPin, noteKeys[8]);
      Serial.println("Playing note: 22");
      return noteKeys[8];
      break;
  }
}

int capture(int ldrVal) {
  if (ldrVal > (open_read+white_read)/2) {
    return 0;
  } else if (ldrVal >(white_read+black_read)/2 && ldrVal<(open_read+white_read/2)) {
    return 1;
  } else if (ldrVal <(white_read+black_read)/2) {
    return 2;
  }
}

void calibrate(){
  if (playing) {
    noTone(piezzoPin);
    playing =0;
  }
  current_bit=0;
  for (int calibration_state = 0; calibration_state <5; calibration_state++) {
    int buttonVal = digitalRead(buttonPin);
    while (!buttonVal) {
      buttonVal = digitalRead(buttonPin);
      switchState = digitalRead(switchPin);
      int resetVal = digitalRead(resetButtonPin);

      update_motor();
      if (!switchState){
        Serial.println("Leaving calibration mode");
        return;
      }
      if (resetVal) {
        calibration_state =0;
        delay(300);
        Serial.println("Reset calibration mode");
        Serial.print("Collecting trit 0 reading: ");
      }
    }
    switch(calibration_state){
      case 0:
        open_read_1 = analogRead(ldrPin);
        open_read_2 = analogRead(ldrPin);
        open_read_3 = analogRead(ldrPin);
        open_read = (open_read_1 + open_read_2 +open_read_3)/3;
        //Serial.print("Captured trit 0 reading: ");
        Serial.println(open_read);
        Serial.print("Collecting trit 1 reading: ");
        break;
      case 1:
        white_read_1 = analogRead(ldrPin);
        white_read_2 = analogRead(ldrPin);
        white_read_3 = analogRead(ldrPin);
        white_read = (white_read_1 + white_read_2 +white_read_3)/3;
        //Serial.print("captured white reading: ");
        Serial.println(white_read);
        Serial.print("Collecting trit 2 reading: ");
        break;
      case 2:
        black_read_1 = analogRead(ldrPin);
        black_read_2 = analogRead(ldrPin);
        black_read_3 = analogRead(ldrPin);
        black_read = (black_read_1 + black_read_2 +black_read_3)/3;
        //Serial.print("captured black reading: ");
        Serial.println(black_read);
        Serial.print("Collecting base ultrasonic sensor reading: ");
        break;
      case 3: 
        furthest_distance = distance-3;
        //Serial.print("Captured furthest ultrasonic sensor reading: ");
        Serial.println(furthest_distance);
        Serial.println("Calibration complete, please exit calibration mode or press the key to restart");
        break;
      case 4: 
        calibration_state = -1;
        Serial.println("Restarting calibration");
        Serial.print("Collecting trit 0 reading: ");

    }
    delay(500);
  }
}

void update_motor() {
  int servo_potent_val = analogRead(servo_potent_pin);
  servo_potent_val = map(servo_potent_val, 0 , 1023, 0, 180);
  myservo.write(servo_potent_val);
}

void calibrate_notes() {
  if (playing) {
    noTone(piezzoPin);
    playing =0;
  }
  current_bit=0;

  int current_note = 0;

 // while (current_note <9) {

  // Serial.print("Now selecting note ");
  // Serial.println(current_note);
  for (int current_possible_note=0; current_possible_note<28; current_possible_note++) {
    Serial.print("Now playing note ");
    print_key_name(possibleNotes[current_possible_note]);
    Serial.println("");
    int setNote = digitalRead(buttonPin);
    int nextNote = digitalRead(resetButtonPin);

    tone(piezzoPin, possibleNotes[current_possible_note]);
    while (!nextNote) {
      setNote = digitalRead(buttonPin);
      nextNote = digitalRead(resetButtonPin);
      noteSwitchState = digitalRead(noteSwitchPin);
      
      
      if (!noteSwitchState) {
        noTone(piezzoPin);
        Serial.println("Leaving note selection mode, the current note values are: ");
        for (int i =0; i<8; i++) {
          print_key_name(noteKeys[i]);
          Serial.print(", ");
        }
        print_key_name(noteKeys[8]);
        
        Serial.println("");
       // delay(1000);
        return;
      }

      if (setNote) {
        noteKeys[current_note] = possibleNotes[current_possible_note];
        Serial.print("Set note ");
        //Serial.print(current_note);
        print_trit_equiv(current_note);
        Serial.print(" to ");
        print_key_name(possibleNotes[current_possible_note]);
        Serial.println(" ");
        current_note++;
        if (current_note == 9) {
          Serial.println("All notes have been set, please exit selection mode or press the key to restart");
          //delay(500)
          
        } else if (current_note ==10){
          current_note =0;
          Serial.println("Note selection restarted");
          Serial.print("Selecting note ");
          Serial.println(current_note);
        } else if (current_note > 9) {
          Serial.print("Now selecting note ");
          Serial.println(current_note);
        }
        delay(500);
      }
      
    }
    //continue;
    delay(500);
    
  }
  
  //delay(4000);
  //return;
}

void print_key_name(int note_freq) {
  switch (note_freq) {
    case 185: 
      Serial.print("F3♯/G3♭");
      break;
    case 196: 
      Serial.print("G3");
      break;
    case 208: 
      Serial.print("G3♯/A3♭");
      break;
    case 220: 
      Serial.print("A3");
      break;
    case 233: 
      Serial.print("A3♯/B3♭");
      break;
    case 247: 
      Serial.print("B3");
      break;
    case 262: 
      Serial.print("C4");
      break;
    case 277: 
      Serial.print("C4♯/D4♭");
      break;
    case 294: 
      Serial.print("D4");
      break;
    case 311: 
      Serial.print("D4♯/E4♭");
      break;
    case 330: 
      Serial.print("E4");
      break;
    case 349: 
      Serial.print("F4");
      break;
    case 370: 
      Serial.print("F4♯/G4♭");
      break;
    case 392: 
      Serial.print("G4");
      break;
    case 415: 
      Serial.print("G4♯/A4♭");
      break;
    case 440: 
      Serial.print("A4");
      break;
    case 466: 
      Serial.print("A4♯/B4♭");
      break;
    case 494: 
      Serial.print("B4");
      break;
    case 523: 
      Serial.print("C5");
      break;
    case 554: 
      Serial.print("C5♯/D5♭");
      break;
    case 587: 
      Serial.print("D5");
      break;
    case 622: 
      Serial.print("D5♯/E5♭");
      break;
    case 659: 
      Serial.print("E5");
      break;
    case 698: 
      Serial.print("F5");
      break;
    case 740:
      Serial.print("F5♯/G5♭");
      break;
    case 784:
      Serial.print("G5");
      break;
    case 831:
      Serial.print("G5♯/A5♭");
      break;
    case 880:
      Serial.print("A5");
      break;
    
  }
}

void reset_pressed() {
  if (resetFlag) {
    noTone(piezzoPin);
    Serial.println("Music paused");
    playing =0;
  } else {
    current_bit = 0;
    Serial.println("Trits cleared");
  }
  resetFlag = 1;
  delay(100);
  update_motor();
  delay(100);
  update_motor();
  delay(100);
  update_motor();
}

void note_pressed() {
  ldrValue = analogRead(ldrPin);
  resetFlag=0;
  // Serial.println(ldrValue);
  switch (current_bit) {
    case 0:
      noteValue[0]=capture(ldrValue);
      current_bit++;
      Serial.print("Collected trit: ");
      Serial.println(noteValue[0]);
      break;
    case 1:
      noteValue[1]=capture(ldrValue);
      Serial.print("Collected trit: ");
      Serial.println(noteValue[1]);
      if (playing) {
        noTone(piezzoPin);
        delay(100);
      }
      current_note = play(noteValue[0], noteValue[1]);
      whammy_note = current_note;
      playing = 1;
      current_bit =0;
      break;
  }
  delay(100);
  update_motor();
  delay(100);
  update_motor();
  delay(100);
  update_motor();
}

void evaluate_buttons() {
  buttonVal = digitalRead(buttonPin);
  resetVal = digitalRead(resetButtonPin);
  if (resetVal) {
    reset_pressed();
  }
  if (buttonVal) {
    note_pressed();
  }
}

void whammy() {

}

void print_trit_equiv(int decimal_val) {
  switch (decimal_val){
    case 0:
      Serial.print("00");
      break;
    case 1:
      Serial.print("01");
      break;
    case 2:
      Serial.print("02");
      break;
    case 3:
      Serial.print("10");
      break;
    case 4:
      Serial.print("11");
      break;
    case 5:
      Serial.print("12");
      break;
    case 6:
      Serial.print("20");
      break;
    case 7:
      Serial.print("21");
      break;
    case 8:
      Serial.print("22");
      break;
  }
}