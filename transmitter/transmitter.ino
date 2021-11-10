int pin = 2;
byte tx_buf;
unsigned long timer = 0;

//byte frame[] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f};
//byte frame[] = {START_BYTE, 0x65, 0x65, 0x65, 0x65, END_BYTE};
byte frame[] = {0x55,0x7e,0x0a};


int byte_len = 8;
int tx_counter, frame_len, frame_counter;
byte checksum = 0;
byte tx_flag = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  Serial.begin(115200);
  timer = millis();
  tx_counter = 0;
  frame_len = 3;
  frame_counter = 0;
  tx_buf = frame[frame_counter];

  // TIMER 1 for interrupt frequency 3000.1875117194822 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 3000.1875117194822 Hz increments
  OCR1A = 5332; // = 16000000 / (1 * 3000.1875117194822) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts


  
}

ISR(TIMER1_COMPA_vect){
  if(tx_flag == 0) { 
    tx_flag = 1;
  } 
  else {
    tx_flag = 0;
    if(bitRead(tx_buf,tx_counter) == 1) {
      digitalWrite(pin, HIGH);
//      Serial.write("1");
    } else {
      digitalWrite(pin, LOW);
//      Serial.write("0");
    }
    if(tx_counter == 7) {
      tx_counter = 0;
//      Serial.write("\n");
    } else {
      tx_counter++;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - timer >= 5000) {
    frame_counter++;
    if (frame_counter == frame_len) {
      frame_counter = 0;
    }
    Serial.println(frame_counter);
    tx_buf = frame[frame_counter];
    timer = millis();
  }
}
