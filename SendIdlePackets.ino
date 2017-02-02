/*
Example Timer1 Interrupt
Flash LED every second
*/

int timer1_counter;
const int TIMETRUE = 34286;
const int TIMEFALSE = 6200;

boolean BITPART = false;
unsigned long TIMERSLOW = 0; //Langzame timer voor zaken die niet 10 miljoen x per seconde hoeven
boolean KNOPSTATUS = false;
boolean STOP = false;
// voor bitverwering
boolean BITREADY = true; //bit is nu KLAAR >> nieuw bit
boolean BIT = false; // holder voor het huidige bit wat wordt verzonden
int COUNTBIT =0 ;


boolean PACKET[45]; //array voor een packett binair 

void setup()
{
	//packett maken voor begin
	IDLE();

	pinMode(13, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	
	
	
	// initialize timer1 
	noInterrupts();           // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;

	// Set timer1_counter to the correct value for our interrupt interval
	//timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
	//timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
	timer1_counter = 34286; //65532;   // preload timer 65536-16MHz/256/2Hz   34286


	TCNT1 = TIMETRUE;   // preload timer
	TCCR1B |= (1 << CS12);    // 256 prescaler 
	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
	interrupts();             // enable all interrupts
}
void IDLE() { // Maak een idle PACKET
	int i = 0;
	while (i < 45) {
		PACKET[i] = true;
		i++;
	}
}
void GETBIT() {

}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{

	
	if (BITPART == false) { //2e deel van bit, bit nu klaar nieuw bit ophalen uit packet
		BITPART = true;
		BIT = PACKET[COUNTBIT];
		}
		else
		{
			BITPART = false;
		}

		
	if (BIT == true) { // timer interrupt weer instellen.
		TCNT1 = TIMETRUE;   // preload timer
	}
		else {
		TCNT1 = TIMEFALSE;   // preload timer
		}
		if (STOP==false) SETOUTPUTS(BITPART); // outputs aansturen
		digitalWrite(13, digitalRead(13) ^ 1);

		COUNTBIT++; //teller naar volgend bit 
		if (COUNTBIT > 45) COUNTBIT = 0;
}

void HALT() { //stopt de controller
	STOP = true;
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);
	
}
void START() {// start de controller	
	STOP = false;

}


void SETOUTPUTS(boolean bitpart) {
	digitalWrite(3, bitpart);
	digitalWrite(4, ! bitpart);




}
void KNOP() {

	if (digitalRead(6) != KNOPSTATUS) { //dus knop stand veranderd
		if (KNOPSTATUS == true) {
			KNOPSTATUS = false;
		}
		else {
			KNOPSTATUS = true;

			if (STOP == true)
			{
				START();
			}
			else
			{
				HALT();
			}
		}
	}
}


void loop()
{
	// if (digitalRead(5) == LOW && STOP == false) HALT(); werkt niet ...! 


	if (millis() - TIMERSLOW > 100) {
		KNOP();
		TIMERSLOW = millis(); 
		
		 
	}

}

