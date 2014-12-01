//-----------------------------------------------------------------
// Name:	Coulston
// File:	lab5.h
// Date:	Fall 2014
// Purp:	Include file for the MSP430
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Page 76 : MSP430 Optimizing C/C++ Compiler v 4.3 User's Guide
//-----------------------------------------------------------------
typedef		unsigned char		int8;
typedef		unsigned short		int16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define		TRUE				1
#define		FALSE				0

#define		OFF					0
#define		ON					1
#define		REVERSE				2

//-----------------------------------------------------------------
// Function prototypes found in lab5.c
//-----------------------------------------------------------------
void initMSP430();
void motorControl( int8 leftMode,  int8 rightMode);


__interrupt void pinChange (void);
__interrupt void timerOverflow (void);


//-----------------------------------------------------------------
// Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
//	Bit = 0: The PxIFGx flag is set with a low-to-high transition
//	Bit = 1: The PxIFGx flag is set with a high-to-low transition
//-----------------------------------------------------------------

#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6


#define		averageLogic0Pulse	550
#define		averageLogic1Pulse	1650
#define		averageStartPulse	4000
#define		minLogic0Pulse		averageLogic0Pulse - 100
#define		maxLogic0Pulse		averageLogic0Pulse + 100
#define		minLogic1Pulse		averageLogic1Pulse - 100
#define		maxLogic1Pulse		averageLogic1Pulse + 100
#define		minStartPulse		averageStartPulse - 100
#define		maxStartPulse		averageStartPulse + 100

#define		PWR		0xE0E040BF
#define		ONE		0xE144A25D
#define		TWO		0xE144629D
#define		THR		0xE144E21D

#define		VOL_UP	0xE0E0E01F //turn right
#define		VOL_DW	0xE0E0D02F //turn left
#define		CH_UP	0xE0E048B7 // forward
#define		CH_DW	0xE0E008F7 //backward

