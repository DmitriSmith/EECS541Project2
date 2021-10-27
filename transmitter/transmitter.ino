int pin = 1;
int prev = 0;
String pattern = "0110100100111";
int len = pattern.length();
int counter = 0;
int flag = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  
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
  if(flag == 0) { flag = 1; } 
  else {
    flag = 0;
    if(pattern[counter] == '1') {
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
    }
    counter++;
    if(counter == len) { counter = 0; }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
