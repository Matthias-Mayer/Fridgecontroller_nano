class ADCReadings {
  // class to read voltage from analog inputs

  private:
    uint8_t sensorpin;
    uint16_t numbMeasur;
    float vOff;
    float vGain;
    float vFinal;
    bool debug = false;
  
  public:
    ADCReadings(uint8_t pin, uint16_t readings, float offset, float gain){
      // Constructor
      sensorpin = pin;
      numbMeasur = readings;
      vOff = offset;
      vGain = gain;

      pinMode(sensorpin, INPUT); 
      }

    long readVcc() {
      // Read 1.1V reference against AVcc
      long result;
      
      #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
      #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV(MUX5) | _BV(MUX0);
      #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV(MUX3) | _BV(MUX2);
      #else
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
      #endif

      delay(2); // Wait for Vref to settle
      ADCSRA |= _BV(ADSC); // Convert
      while (bit_is_set(ADCSRA, ADSC));
      result = ADCL;
      result |= ADCH << 8;
      result = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
      return result;
    }

    float avgMeasur(){
      // Read averaged ADC value
      long sum = 0;
      
      for(uint8_t n =0; n<numbMeasur; n++)
        {
          sum += analogRead(sensorpin);
        }

      float avgVal = sum/numbMeasur;
      
      if (debug) {
        Serial.print((String)"avgVal:"+avgVal+"  ");
      }
      
      return avgVal; 
    }

    float readVoltageADC(){

      // read normal Arduino value
      float adcAvgVal = avgMeasur();

      // calculate voltage from adc value
      float vAvgVal = adcAvgVal * 5.0 / 1024.0;
      
      // read correct supply voltage
      float supply = readVcc() / 1000.0;
      
      // correct voltage reading
      float vCor = supply / 5 * vAvgVal;
      
      // correct sensor error
      vFinal = vCor * vGain + vOff;

      if (debug) {
        Serial.print((String)"vCor:"+vCor+"  ");
        Serial.print((String)"supply:"+supply+"  ");
        Serial.print((String)"vAvgVal:"+vAvgVal+"  ");
        Serial.print((String)"vFinal:"+vFinal+"  ");
      }    
      
      return vFinal;
    }

};
 
