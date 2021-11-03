int pin = 9;
byte prev = 0;
const byte DELIM_BYTE = 0x7e;
String pattern = "0110100100111";
byte tx_buf;
byte msg[] = {0x7e, 0x7a, 0x77, 0x2a, 0x00, DELIM_BYTE};
//byte msg[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
int byte_len = 8;
int tx_counter, msg_counter, msg_len;
int checksum = 0;
byte tx_flag = 0;
byte load_tx_buf = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  Serial.begin(115200);
  tx_buf = msg[0];
  msg_counter = 1;
  tx_counter = 7;
  msg_len = 6;
  for(int i = 7; i >= 0; i--) {
    Serial.print(bitRead(tx_buf,i));
  }
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
//  if(tx_flag == 0) {
//    tx_flag = 1;
//  } else {
//    if(prev == 0) {
//      digitalWrite(pin, HIGH);
//      prev = 1;
//    } else {
//      digitalWrite(pin, LOW);
//      prev = 0;
//    }
//    tx_flag = 0;
//  }
  if(tx_flag == 0) { 
    tx_flag = 1;
    if(load_tx_buf == 1) {
      load_tx_buf = 0;
      tx_buf = msg[msg_counter];
      msg_counter++;
      if (msg_counter == msg_len) {
        msg_counter = 0;
      }
    }
  } 
  else {
    tx_flag = 0;
    if(bitRead(tx_buf,tx_counter) == 1) {
      digitalWrite(pin, HIGH);
//      Serial.write("1\n");
    } else {
      digitalWrite(pin, LOW);
//      Serial.write("0\n");
    }
    if(tx_counter == 7) {
      tx_counter = 0;
      load_tx_buf = 1;
    } else {
      tx_counter++;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
