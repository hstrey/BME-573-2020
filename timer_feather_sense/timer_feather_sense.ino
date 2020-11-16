int flag = 0;

void setup() {
  Serial.begin(115200);
 
  pinMode(LED_BUILTIN, OUTPUT);
 
  //NRF52 Timer setup
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER2->TASKS_CLEAR = 1;             //Clear the task first so it's usable later
  NRF_TIMER2->PRESCALER = 6;                //I'm not completely sure what this means.... devided by 6? I'm used to doing it with bytes....
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit; //Set 16 bit timer resolution
  NRF_TIMER2->CC[0] = 20;                    //Set these to the compare values
  NRF_TIMER2->CC[1] = 65535;

  //Enable interrupt on timer for both C[0] and C[1]
  NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) | (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);

  NRF_TIMER2->TASKS_START = 1;

}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(flag);

}

//Use extern C with some other stuff to make sure that the right stuff actually gets called
extern "C" {
  void timer2_IRQHandler() {
    if (NRF_TIMER2->EVENTS_COMPARE[0] !=0) {
      NRF_TIMER2->EVENTS_COMPARE[0] = 0;    //Clear compare register 0 event
      flag++;
    }

    if (NRF_TIMER2->EVENTS_COMPARE[1] !=0) {
      NRF_TIMER2->EVENTS_COMPARE[1] = 0;    //Clear compare register 1 event
      //digitalWrite(LED_BUILTIN,LOW);
    }
  }
}

void toggleLED(){
  digitalWrite(LED_BUILTIN,HIGH);
}
