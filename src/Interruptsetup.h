void interruptSetup(uint16_t prescaler, uint16_t comparevalue){
  cli(); // disable interrupts
  
  // reset timer 1
  TCCR1A = 0; // set TCCR1A register to 0
  TCCR1B = 0; // set TCCR1B register to 0
  TCNT1  = 0; // reset counter value

  OCR1A = comparevalue; // compare match register 

  switch (prescaler) {
  case 1:
    TCCR1B |= (1 << CS10);
    break;
  case 8:
    TCCR1B |= (1 << CS11);
    break;
  case 64:
    TCCR1B |= (1 << CS11) | (1 << CS10); 
    break;
  case 256:
    TCCR1B |= (1 << CS12);
    break;    
  case 1024:
    TCCR1B |= (1 << CS12) | (1 << CS10);  
    break;
  default:
    Serial.print((String)"[ERROR] Prescaler of:"+prescaler+" is not a proper value");
    //exit(0);
    break;
}
  
  TCCR1B |= (1 << WGM12); // turn on CTC mode and reser timer to 0
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  
  sei(); // allow interrupts
}
