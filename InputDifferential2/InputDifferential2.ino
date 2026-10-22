/*
// Voltage Reference: 2.56V, cap. on AREF
#define ADC_VREF_TYPE ((1<<REFS1) | (1<<REFS0) | (0<<ADLAR))
#define vref 2.56
*/
// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0 << REFS1) | (1 << REFS0) | (0 << ADLAR))
#define vref 5.0

#define adc_input1 0b00010000  // ici on définit ADC0 input positive et ADC1 input negative voir page 283 datasheet --> gain x1
// https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf
// #define adc_input10 0b00001000   //définit ADC1 positive & ADC0 negative --> gain x10

#define nb_echantillon 100

void setup() {
  initCAN();
  Serial.begin(9600);
}

float tabValueIn[nb_echantillon]; //tableau de 100 valeurs pour avoir les échantillons
float tabValueOut[nb_echantillon];

void loop() {
  int value;

  //FAIRE UNE BOUCLE FOR POUR REMPLIR UN TABLEAU DE 100 VALEURS
  for (int i = 0; i < nb_echantillon; i++) {
    ADMUX = (adc_input1 & 0x1f) | ADC_VREF_TYPE;
    tabValueIn[i] = read_adc();
    ADMUX = (adc_input1  & 0x1f) | ADC_VREF_TYPE;
    tabValueOut[i] = read_adc();
  }

  for (int i = 0; i < nb_echantillon; i++) {
    //Serial.print("Min:0,Max:5,Signal:");   // ~normalisation pour empêcher le graphique de zoomer/dézoomer tout seul
    Serial.println(tabValueOut[i]);
  }

  //_delay_ms(2000);
}

void initCAN(void) {
  // ADC initialization
  // ADC Clock frequency: 1000,000 kHz
  // ADC Voltage Reference: 2.56V or 5V, cap. on AREF
  // ADC Auto Trigger Source: ADC Stopped
  // Digital input buffers on ADC0: Off, ADC1: Off, ADC2: On, ADC3: On, ADC4: On, ADC5: On, ADC6: On, ADC7: On
  DIDR0 = (0 << ADC7D) | (0 << ADC6D) | (0 << ADC5D) | (0 << ADC4D) | (0 << ADC3D) | (0 << ADC2D) | (1 << ADC1D) | (1 << ADC0D);
  // Digital input buffers on ADC8: On, ADC9: On, ADC10: On, ADC11: On, ADC12: On, ADC13: On, ADC14: On, ADC15: On
  DIDR2 = (0 << ADC15D) | (0 << ADC14D) | (0 << ADC13D) | (0 << ADC12D) | (0 << ADC11D) | (0 << ADC10D) | (0 << ADC9D) | (0 << ADC8D);
  ADCSRA = (1 << ADEN) | (0 << ADSC) | (0 << ADATE) | (0 << ADIF) | (0 << ADIE) | (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0);
  ADCSRB = (0 << MUX5) | (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0);
  
  if (adc_input1 & 0x20) ADCSRB |= (1 << MUX5);
  else ADCSRB &= ~(1 << MUX5);
}


// Read the AD conversion result
unsigned int read_adc() {
  // Delay needed for the stabilization of the ADC input voltage
  _delay_us(10);
  // Start the AD conversion
  ADCSRA |= (1 << ADSC);
  // Wait for the AD conversion to complete
  while ((ADCSRA & (1 << ADIF)) == 0);
  ADCSRA |= (1 << ADIF);
  return ADCW;
}
