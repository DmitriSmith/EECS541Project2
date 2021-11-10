//Hardware values
int rx_pin = 2;
int tx_pin = 4;

//Sensor input variables
byte sensor;                  //Phototransistor sensor value
byte prev[2] = {0,0};         //Previous 2 sensor readings

char print_buf[] = {'\0','\0','\0','\0','\0','\0','\0','\0', '\0'};

byte sample_ready = 0;
byte byte_buf = 0;

//Buffer counters
const byte BYTE_LENGTH = 8;       //Size of byte. Used for loop controls
byte bit_counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(2000000);
  pinMode(tx_pin, OUTPUT);
  pinMode(rx_pin, INPUT);

//  for(int i = 0; i < 8; i++) {
//    bit_buf = bit_buf << 1;
//    bitWrite(bit_buf, 0, 1);
////    Serial.println(bit_buf, HEX);
//  }

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

void write_bit(byte b){
  print_buf[bit_counter] = '0'+b;
  bit_counter++;
  if (bit_counter >= BYTE_LENGTH) {
    Serial.println(print_buf);
//    sprintf(print_buf, "%08x", byte_buf);
//    Serial.println(print_buf);
//    Serial.flush();
    bit_counter = 0;
  }
//  Serial.println(byte_buf, BIN);
  
  
}

void process_bit() {
  sensor = (sensor == HIGH ) ? 1 : 0;
//  Serial.println(sensor+64);
//  if(prev[0] != prev[1] && prev[1] != sensor) { //This is definitely a bit error
//    bit_errors++;
//    Serial.println(bit_errors);
//  }
  if(sample_ready == 1) {
    if(sensor == prev[1]) {
      write_bit(sensor);
    } else if (prev[1] == prev[0]) {
      write_bit(prev[1]);
    } else { //There's been a bit error. Prefer the most recent sample.
      write_bit(sensor);
    }
    sample_ready = 0;
  } else {
    sample_ready = 1;
  }
  prev[0] = prev[1];
  prev[1] = sensor;
}

ISR(TIMER1_COMPA_vect){
    //interrupt commands for TIMER 1 here
  sensor = !digitalRead(rx_pin);
  digitalWrite(tx_pin, sensor);
  process_bit();
//  sensor = digitalRead(rx_pin);
//  if(sensor == HIGH) {
//    if(sample_ready == 1) {
//      process_bit();
////        Serial.write("1");
//      sample_ready = 0;
//    } else {
//      sample_ready = 1;
//    }
//  }
//  else {
//    if(sample_ready == 1) {
//      process_bit();      
////      Serial.write("0");
//      sample_ready = 0;
//    } else {
//      sample_ready = 1;
//    }
//  }
//  
////  bit_counter++;
////  if(bit_counter == 2 * BYTE_LENGTH) {
////    Serial.print("\n");
////    bit_counter = 0;
////  }
  
}

void loop() {

}
