#include <msp430.h>
#include "start5.h"




int8 newPacket = FALSE;
int16 packetData[48];
int8 packetIndex = 0;
int32 irPacket;

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;                 // stop the watchdog timer

	initMSP430();				// Setup MSP to process IR and buttons
	irPacket = 0;



	P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
	P2SEL |= BIT2;	// P2.2 is associated with TA1CCTL1


	P2DIR |= BIT0;
	P2OUT |= BIT0;

	P2DIR |= BIT1;
	P2OUT &= ~BIT1;


	P2DIR |= BIT4;							// P2.4 is associated with TA1CCR2
	P2SEL |= BIT4;							// P2.4 is associated with TA1CCTL2

	P2DIR |= BIT5;
	P2OUT |= BIT5;



	P2DIR |= BIT3;
	P2OUT &= ~BIT3;

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
	TA1CCR0 = 0x1388;						// set signal period

	TA1CCR1 = 0x0700;
	TA1CCTL1 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode

	TA1CCR2 = 0x0700;
	TA1CCTL2 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode
	motorControl(OFF, OFF);
	while (1) {

		if (newPacket) {
			newPacket = FALSE;
			if (irPacket == CH_DW) {
				motorControl(REVERSE, REVERSE);
				irPacket = 0;
			}
			if (irPacket == CH_UP) {
				motorControl(ON, ON);
				irPacket = 0;
			}
			if (irPacket == VOL_UP) {
				motorControl(ON, REVERSE);
				irPacket = 0;
			}
			if (irPacket == VOL_DW) {
				motorControl(REVERSE, ON);
				irPacket = 0;
			}
			if(irPacket==PWR){
				motorControl(OFF, OFF);
				irPacket = 0;
			}

		} // end if new IR packet arrived

	} // end loop

} // end main

void initMSP430() {

	IFG1 = 0; 					// clear interrupt flag1

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P1DIR = BIT6;
	P1DIR = BIT0;

	P2SEL &= ~BIT6;						// Setup P2.6 as GPIO not XIN
	P2SEL2 &= ~BIT6;
	P2DIR &= ~BIT6;
	P2IFG &= ~BIT6;						// Clear any interrupt flag
	P2IE |= BIT6;						// Enable PORT 2 interrupt on pin change

	HIGH_2_LOW;

	TA0CCR0 = 0xFFFF;					// create a 16mS roll-over period
	TACTL &= ~TAIFG;	// clear flag before enabling interrupts = good practice
	TACTL = ID_3 | TASSEL_2 | MC_1;	// Use 1:1 presclar off MCLK and enable interrupts

	_enable_interrupt();
}

#pragma vector = PORT2_VECTOR			// This is from the MSP430G2553.h file

__interrupt void pinChange(void) {

	int8 pin;
	int16 pulseDuration;			// The timer is 16-bits

	if (IR_PIN)
		pin = 1;
	else
		pin = 0;

	switch (pin) {					// read the current pin level
	case 0:						// !!!!!!!!!NEGATIVE EDGE!!!!!!!!!!
		pulseDuration = TAR;
		packetData[packetIndex++] = pulseDuration;

		if ((10000 >= pulseDuration) && (pulseDuration >= minLogic1Pulse)) {
			irPacket = (irPacket << 1) | 1;

		}
		if ((maxLogic0Pulse >= pulseDuration)
				&& (pulseDuration >= minLogic0Pulse)) {
			irPacket = (irPacket << 1) | 0;
		}

		TACTL = 0;
		LOW_2_HIGH; 				// Setup pin interrupr on positive edge
		break;

	case 1:							// !!!!!!!!POSITIVE EDGE!!!!!!!!!!!
		TAR = 0x0000;					// time measurements are based at time 0
		TA0CCR0 = 0x2000;
		TACTL = ID_3 | TASSEL_2 | MC_1 | TAIE;
		HIGH_2_LOW; 					// Setup pin interrupr on negative edge
		break;
	} // end switch

	P2IFG &= ~BIT6;	// Clear the interrupt flag to prevent immediate ISR re-entry

} // end pinChange ISR

// -----------------------------------------------------------------------
//			0 half-bit	1 half-bit		TIMER A COUNTS		TIMER A COUNTS
//	Logic 0	xxx
//	Logic 1
//	Start
//	End
//
// -----------------------------------------------------------------------
#pragma vector = TIMER0_A1_VECTOR			// This is from the MSP430G2553.h file
__interrupt void timerOverflow(void) {
	TACTL = 0;
	TAR = 0x0000;
	TACTL ^= TAIE;
	packetIndex = 0;
	newPacket = TRUE;
	TACTL &= ~TAIFG;
}

void motorControl( int8 leftMode,  int8 rightMode) {
	if (leftMode == OFF) {
		P2OUT &= ~BIT0;		//	P2DIR &= ~BIT0 ; turn off enable
	}
	if (leftMode == ON) {
		P2OUT |= BIT0;//enable
		P2OUT &= ~BIT1;//set ground
		TA1CCTL1 = OUTMOD_7;//move forward
	}
	if (leftMode == REVERSE) {
		P2OUT |= BIT0;//DIR HIGH, MOD_3
		P2OUT |= BIT1;
		TA1CCTL1 = OUTMOD_3;
	}

	if (rightMode == OFF) {
		P2OUT &= ~BIT5;//	P2DIR &= ~BIT0 ; turn off enable
	}
	if (rightMode == ON) {
		P2OUT |= BIT5;//DIR LOW, MOD_7
		P2OUT &= ~BIT3;
		TA1CCTL2 = OUTMOD_7;
	}
	if (rightMode == REVERSE) {
		P2OUT |= BIT5;//DIR HIGH, MOD3
		P2OUT |=BIT3;
		TA1CCTL2 = OUTMOD_3;
	}

}

