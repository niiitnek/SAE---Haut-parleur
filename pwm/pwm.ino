#define NECHANT 128
#define SHIFT_ACCUM 25

uint32_t icr;
uint32_t table_onde[NECHANT];
uint32_t accum1, accum2, accum3, increm;
uint16_t diviseur[6] = { 0, 1, 8, 64, 256, 1024 };

void init_pwm_timer1(uint32_t period) {
  char clockBits;
  TCCR1A = 0;
  TCCR1A |= (1 << COM1A1);  //Clear OCnA/OCnB/OCnC on compare match, set OCnA/OCnB/OCnC at BOTTOM (non-inverting mode)
  TCCR1A |= (1 << COM1B1);
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
  TCCR1A |= (1 << COM1C1);
#endif
  TCCR1B = 1 << WGM13;  // phase and frequency correct pwm mode, top = ICR1
  int d = 1;
  icr = (F_CPU / 1000000 * period / 2);
  while ((icr > 0xFFFF) && (d < 6)) {  // choix du diviseur d'horloge
    d++;
    icr = (F_CPU / 1000000 * period / 2 / diviseur[d]);
  }
  clockBits = d;
  ICR1 = icr;           // valeur maximale du compteur
  TIMSK1 = 1 << TOIE1;  // overflow interrupt enable
  sei();                // activation des interruptions
  TCNT1 = 0;            // mise à zéro du compteur
  TCCR1B |= clockBits;  // déclenchement du compteur
}

ISR(TIMER1_OVF_vect) {  // Timer 1 Overflow interrupt
  accum1 += increm;
  OCR1A = table_onde[accum1 >> SHIFT_ACCUM];
}

void set_sinus_table(float amp, float offset) {
  int i;
  float dt = 2 * 3.1415926 / NECHANT;
  for (i = 0; i < NECHANT; i++) {
    table_onde[i] = icr * 0.5 * (1.0 - offset + amp * sin(i * dt));
  }
}

void set_square_table(float amp, float offset) {
  float i0 = NECHANT / 2;
  int i;
  for (i = 0; i < i0; i++) table_onde[i] = icr * 0.5 * (1.0 - offset + amp);
  for (i = i0; i < NECHANT; i++) table_onde[i] = icr * 0.5 * (1.0 - offset - amp);
}

void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);  // commande ENABLE
#if defined(__AVR_ATmega2560__)
  pinMode(11, OUTPUT);
#elif defined(__AVR_ATmega32U4__)
  pinMode(9, OUTPUT);
#else
  pinMode(9, OUTPUT);
#endif
// modif de period_pwm de 100 à 20 pour correspondre au commentaire
  uint32_t period_pwm = 20;  // en microsecondes (50 kHz)
  uint32_t frequence = 440;    // en Hz
  accum1 = 0;
  increm = (uint32_t)(((float)(0xFFFFFFFF)) * ((float)(frequence)*1e-6 * (float)(period_pwm)));  // incrément de l'accumulateur de phase
  init_pwm_timer1(period_pwm);
  set_sinus_table(0.05, 0);    //réglage pour avoir max 200mV -> 2V au haut-parleur
}

void loop() {}