unsigned int rx_pin = 9;
unsigned int tx_pin = 1;
byte prev[2] = {0,0};
byte output_buf[24];
uint8_t len = 24;
const byte DELIM_BYTE = 0x7e;
uint8_t output_counter = 0;
byte val = 0;
unsigned long checksum = 0;
unsigned long bit_errors = 0;
byte rx_buf, rx_counter;
byte byte_buf;
uint8_t sample_flag = 0;
uint8_t record_flag = 0;
uint8_t byte_ready = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(tx_pin, OUTPUT);
  pinMode(rx_pin, INPUT);

  rx_counter = 0;
  
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
    //interrupt commands for TIMER 1 here
//   if(prev == 0) {
//     digitalWrite(pin, HIGH);
//     prev = 1;
//   } else {
//     digitalWrite(pin, LOW);
//     prev = 0;
//   }
  val = digitalRead(rx_pin);
  if(val == HIGH) {
    digitalWrite(tx_pin, HIGH);
  } else {
    digitalWrite(tx_pin, LOW);
  }
  if (prev[0] != prev[1] && prev[1] != val) {
    //This is definitely a bit error
    bit_errors++;
    //In this case, we assume the measured val is correct 
  }
  if(sample_flag == 0) {
    sample_flag = 1;
  } else {
    sample_flag = 0;
    bitWrite(rx_buf, rx_counter, val);

    if(rx_counter == 7) {
      byte_ready = 1;
      byte_buf = rx_buf;
      rx_counter = 0;
    }
  }
  

  prev[0] = prev[1];
  prev[1] = val;  
  
}

void check_byte() {
  if (record_flag == 0) {
    if (byte_buf == DELIM_BYTE) {
      record_flag = 1;
    }
  } else {
    if (byte_buf == DELIM_BYTE) {
      record_flag = 0;
    } else {
      output_buf[output_counter];
      if (output_counter == len - 1) {
        output_counter = 0;
      }
    }
  }
}

void loop() {
  if(byte_ready == 1) {
    byte_ready = 0;
    check_byte();
  }

}
