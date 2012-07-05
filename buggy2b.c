//
// Project name: buggy2
// ====================
//
// A simple table driven interpreter to execute a sequence of vehicle commands
//
// Author:
//    Jim Herd, June 2012
//
// Description:
//    Program uses a simple language to describe commands.  Sequences of these commands are stored in the 
//    C structure (table) 'sequence'.  This table is then executed indefinitely, or until it reaches a
//    FINNISH command.  Each command has up to 3 parameters. 
//    Language commands are as follows :- (unused parameters are shown as ---)
//
//                                                      | PARAMETER_1     |   PARAMETER_2      |  PARAMETER_3
//  ------------------------------------------------------------------------------------------------------------
//   SETVAR      : Load variable with a 16-bit constant |  variable       |   constant         |     ---
//  ------------------------------------------------------------------------------------------------------------
//   LOAD_RAND   : Load variable random value           |  variable       |   bottom of range  | top of range
//  ------------------------------------------------------------------------------------------------------------
//   CALC        : do simple calculations               | Operation(ADD)  |   variable         |  constant
//  ------------------------------------------------------------------------------------------------------------
//   JUMP        : jump to a program position           | line number     |     ---            |    ---
//  ------------------------------------------------------------------------------------------------------------
//   DECSKIP     : decrement a variable and skip next   |                 |                    |
//                 command if variable is zero          |   variable      |     ---            |    ---
//  ------------------------------------------------------------------------------------------------------------
//   TESTSKIP    : test variable and constant and skip  |                 |                    | 
//                 next command if true                 |   variable      |     test           |  constant
//                                                      |                 |    GREATER_THAN    | 
//                                                      |                 |    EQUAL_TO        | 
//                                                      |                 |    LESS_THAN       | 
//  ------------------------------------------------------------------------------------------------------------
//   SETSPEED    : set speed to the two drive motors    |     Mode        | % full speed of    |  % full speed  
//                                                      |                 | vehicle RIGHT motor| ovehicle LEFT motor
//                                                      |                 |                    |
//                                                      |   IMMEDIATE     |   +/- constant     |  +/- constant
//                                                      |   REGISTER      |    variable        |    variable
//  ------------------------------------------------------------------------------------------------------------
//   START       : start motors                         |     ---         |     ---            |    ---
//  ------------------------------------------------------------------------------------------------------------
//   STOP        : stop motors                          |     ---         |     ---            |    ---  
//  ------------------------------------------------------------------------------------------------------------
//   WAIT        : delay  seconds/milliseconds          |    mode         |    nos seconds     | nos milliseconds
//                                                      |                 |                    |
//                                                      |   IMMEDIATE     |    constant        |    constant
//                                                      |   REGISTER      |    variable        |    variable 
//  ------------------------------------------------------------------------------------------------------------
//   FINISH      : exit the sequence                    |     ---         |     ---            |    ---  
//  ------------------------------------------------------------------------------------------------------------
//
// Language notes
//    Some of the commands use a modifer (IMMEDIATE or REGISTER) as the fist parameter.  The IMMEDIATE
//    modifier implies that the instruction values are held in PARAMETER_2 and PARAMETER_3. 
//    The REGISTER modifier indicates that PARAMETER_2 and PARAMETER_3 hold the names of variables
//    where the values are stored.  The variables are named V0 to V9.  You can do some simple arithmetic
//    operations on these variables.
//
// Target configuration:
//    MCU         :   P18F4585
//    Oscillator  :   HS, 40.0000MHz
//    Software    :   Microchip C18 V3.36 Student Version
//                :   MPLAB V7.31
//
// Buggy spec
//    1. WattBot I vehicle with PIC18F4585 device
//    2. Full details of the vehicle are available here http://.................
//    3. LEFT and RIGHT motors are defined looking from the back to the front of the vehicle.
//           The RIGHT motor is driven by PWM unit 1.
//           The LEFT motor is driven by PWM unit 2.
//    4. The processor on the vehicle can be either an 18F452 ro an 18F4585
//
// General notes
//    Initial undergraduate Buggy was developed in 2006. Code was called "buggy2".  As of 2012
//    the design has been refreshed with updated power and breakout boards.  The power board is
//    completely compatible with the original power board.  The breakout board has a 20*2 line
//    LCD display and 4 LED/switch units. These new features are connected to the PIC through
//    an I2C interface.
//
#include "defines.h"

#pragma  config  OSC=HS, FCMEN=OFF, IESO=OFF, PWRT=ON, BOREN=OFF,  WDT=OFF
#pragma  config  WDTPS=1, MCLRE=ON,  PBADEN=OFF, DEBUG=OFF, LVP=OFF, STVREN=OFF    

#if defined(__18F452)
#include   <p18f452.h>
#define RIGHT_MOTOR_DIR_TRIS   	TRISBbits.TRISB0 
#define RIGHT_MOTOR_DIR			PORTBbits.RB0
#define LEFT_MOTOR_DIR_TRIS   	TRISBbits.TRISB1 
#define LEFT_MOTOR_DIR  		PORTBbits.RB1
#endif

#if defined(__18F4585)
#include    <p18f4585.h>  
#define RIGHT_MOTOR_DIR_TRIS   	TRISBbits.TRISB5 
#define RIGHT_MOTOR_DIR			PORTBbits.RB5
#define LEFT_MOTOR_DIR_TRIS   	TRISBbits.TRISB4 
#define LEFT_MOTOR_DIR  		PORTBbits.RB4
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Constant declarations
//
#define		IO_OUTPUT    0
#define		IO_INPUT     1

#define     OFF_PWM      0     // base PWM value == stopped
#define     FULL_PWM     4  

#define     SET_FORWARD  0
#define     SET_REVERSE  1

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Sequence definitions
//
typedef enum {SETSPEED, FINISH, START, STOP, WAIT, JUMP, SETVAR, LOAD_RAND, 
              DECSKIP, CALC, TESTSKIP
} COMMAND;
typedef enum {FULL_SPEED=100, HALF_SPEED=50} SPEED;  // % value
typedef enum {RUNNING, STOPPED} SEQ_STATE;
typedef enum {FORWARD, BACKWARD} DIRECTION;
enum {V0, V1, V2, V3, V4, V5, V6, V7, V8, V9} var_names;
enum {IMMEDIATE, REGISTER} cmd_modes;
enum {ADD} arith_ops;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Storage structure for vehicle sequence program
//
rom static struct  {
    COMMAND   cmd;
    int       param1, param2, param3;
} sequence[] = {
//           command  |     param1 |     param2 |    param3
            {WAIT     ,          5 ,          0 ,         0 },  //  initial 5 second delay  
            {SETSPEED ,  IMMEDIATE , FULL_SPEED , FULL_SPEED},  // full speed ahead
            {START    ,          0 ,          0 ,         0 },  // go
            {WAIT     ,          4 ,          0 ,         0 },  // run for 4 seconds
            {SETSPEED ,  IMMEDIATE ,-FULL_SPEED ,-FULL_SPEED},  // full speed in reverse
            {START    ,          0 ,          0 ,         0 },  // go
            {WAIT     ,          4 ,          0 ,         0 },  // run for 4 seconds
            {SETSPEED ,  IMMEDIATE ,         50 ,       -50 },  // spin left at half speed
            {START    ,          0 ,          0 ,         0 },  // go
            {WAIT     ,          2 ,          0 ,         0 },  // run for 2 seconds
            {STOP     ,          0 ,          0 ,         0 },
            {FINISH   ,          0 ,          0 ,         0 },

            {START    ,          0 ,          0 ,         0 },
            {WAIT     ,          4 ,          0 ,         0 },  // run for 4 seconds
            {WAIT     ,          4 ,          0 ,         0 },
            {DECSKIP  ,         V0 ,          0 ,         0 },
            {JUMP     ,          1 ,          0 ,         0 },
            {SETVAR   ,         V2 ,         10 ,         0 },  // counter
            {SETVAR   ,         V1 ,         10 ,         0 },  // start at 10%
            {SETSPEED ,   REGISTER ,         V1 ,        V1 },
            {START    ,          0 ,          0 ,         0 },
            {WAIT     ,          2 ,          0 ,         0 },
            {STOP     ,          0 ,          0 ,         0 },
            {WAIT     ,          2 ,          0 ,         0 },
            {CALC     ,        ADD ,         V1 ,        10 },  // add 10%
            {DECSKIP  ,         V2 ,          0 ,         0 },
            {JUMP     ,         18 ,          0 ,         0 },
            {FINISH   ,          0 ,          0 ,         0 }
        };

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Function Prototypes
//
void main (void);
void OpenBoth_PWM( uint8_t period );
void init(void);
void SetDutyCyclePWM1(unsigned int dutycycle);
void SetDutyCyclePWM2(unsigned int dutycycle);
void exec_seq( uint8_t seq_start_no );

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Program variables
//
int    vars[10];                          // user variables : vars[0] to vars[9]
int    right_speed, left_speed;
uint8_t  left_direction, right_direction;
int    left_offset, right_offset;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// OpenBoth_PWM : configure two PWM channels
// ============
//
void OpenBoth_PWM( uint8_t period )
{
    CCP1CON=0b00001100;      // set capture/compare/pwm module 1 to PWM  mode
    T2CONbits.TMR2ON = 0;    // STOP TIMER2 registers to POR state
    PR2 = period;            // set period
    TRISCbits.TRISC2 = 0;    // configure pin 2 of PORTC as output

    ECCP1CON=0b00001100;     // set enhanced capture/compare/pwm module 1 to PWM mode

#if defined(__18F452)
    TRISCbits.TRISC1 = 0;    // configure pin 1 of PORTC as output
#endif
#if defined(__18F4585)
    TRISDbits.TRISD4 = 0;    // configure pin 4 of PORTD as output
#endif

    OpenTimer2( TIMER_INT_OFF & T2_PS_1_16 & T2_POST_1_1 );
    T2CONbits.TMR2ON = 1;    // Turn on PWM1  
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// SetDutyCyclePWM1 : 
// ================
//
void SetDutyCyclePWM1(uint16_t dutycycle)
{
union PWMDC DCycle;

    DCycle.lpwm = dutycycle << 6;       // Save the dutycycle value in the union
    CCPR1L = DCycle.bpwm[1];            // Write the high byte into CCPR1L
    CCP1CON = (CCP1CON & 0xCF) | ((DCycle.bpwm[0] >> 2) & 0x30);     // Write the low byte into CCP1CON5:4
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// SetDutyCyclePWM2 : 
// ================
//
void SetDutyCyclePWM2(uint16_t dutycycle)
{
union PWMDC DCycle;

    DCycle.lpwm = dutycycle << 6;       // Save the dutycycle value in the union

#if defined(__18F452)
    CCPR2L = DCycle.bpwm[1];            // Write the high byte into CCPR2L   
    CCP2CON = (CCP2CON & 0xCF) | ((DCycle.bpwm[0] >> 2) & 0x30);    // Write the low byte into CCP2CON5:4 
#endif
#if defined(__18F4585)
    ECCPR1L = DCycle.bpwm[1];           // Write the high byte into CCPR1L  
    ECCP1CON = (ECCP1CON & 0xCF) | ((DCycle.bpwm[0] >> 2) & 0x30);   // Write the low byte into CCP1CON5:4  
#endif
}

//----------------------------------------------------------------------------
// init : initialise system
// ====
//
void init(void)
{
//
// Configure 2 PWM hardware subsystems for driving motors.
//
    OpenBoth_PWM( 124 );
//
//    SetDCPWM2(0);        // 18f452
//
    SetDutyCyclePWM1(0);
    SetDutyCyclePWM2(0);
//
// Set motor direction control lines and initialise
//
	LEFT_MOTOR_DIR_TRIS = IO_OUTPUT;
 	RIGHT_MOTOR_DIR_TRIS = IO_OUTPUT;

 	LEFT_MOTOR_DIR = SET_FORWARD;
 	RIGHT_MOTOR_DIR = SET_FORWARD;
//
// initialise system variables.
//
    left_speed = 0;
    left_direction = FORWARD;
    left_offset = 0;
//
    right_speed = 0;
    right_direction = FORWARD;
    right_offset = 0;
//
// initialise random number generator
//
    srand(143);
//
// initialise the I2C interface
//
    DelayMs(1000);
    I2C_Open(MASTER, SLEW_OFF);
    SSPADD= I2C_100KHZ; 
    I2C_Idle();
    DelayMs(1);

    return;
}

//----------------------------------------------------------------------------
// exec_seq : execute a command sequence 
// ========
void exec_seq(uint8_t seq_start_no)
{
uint16_t      temp1, temp2;
uint8_t       seq_counter;
SEQ_STATE   state;

    seq_counter = seq_start_no;
    state = RUNNING;
    
    for (;;) {
        switch (sequence[seq_counter].cmd) {
            case FINISH :
                SetDutyCyclePWM1(0);
                SetDutyCyclePWM2(0);
                state = STOPPED;
                break;
            case START :
                if (left_direction == FORWARD) {
                    LEFT_MOTOR_DIR = SET_FORWARD;
                } else {
                    LEFT_MOTOR_DIR = SET_REVERSE;
                }
                SetDutyCyclePWM2(left_speed);
                if (right_direction == FORWARD) {
                    RIGHT_MOTOR_DIR = SET_FORWARD;
                } else {
                    RIGHT_MOTOR_DIR = SET_REVERSE;
                }
                SetDutyCyclePWM1(right_speed);
                seq_counter++;
                break;

            case STOP :
                SetDutyCyclePWM1(0);
                SetDutyCyclePWM2(0);
                seq_counter++;
                break;

            case WAIT :
                DelayS(sequence[seq_counter].param1);
                seq_counter++;
                break;

            case SETSPEED :
                if (sequence[seq_counter].param1 == IMMEDIATE) {
                    right_speed = sequence[seq_counter].param2;    // % of full speed
                    left_speed = sequence[seq_counter].param3;   // % of full speed
                } 
                else {  // must be REGISTER mode
                    right_speed = vars[sequence[seq_counter].param2];
                    left_speed = vars[sequence[seq_counter].param3];
                }
                if (left_speed < 0 ) {
                    left_direction = BACKWARD;
                    left_speed  = (FULL_PWM * left_speed * -1) - left_offset;
                } 
                else {
                    left_direction = FORWARD;
                    left_speed  = (FULL_PWM * left_speed) - left_offset;
                } 
                                       
                if (right_speed < 0 ) {
                    right_direction = BACKWARD;
                    right_speed  = (FULL_PWM * right_speed * -1) - right_offset;
                } 
                else {
                    right_direction = FORWARD;
                    right_speed  = (FULL_PWM * right_speed) - right_offset;
                } 
                seq_counter++;
                break;

            case SETVAR :
                vars[sequence[seq_counter].param1] = sequence[seq_counter].param2;
                seq_counter++;
                break;

            case JUMP :
                seq_counter = sequence[seq_counter].param1;
                break;

            case LOAD_RAND :
                temp1 = rand();
                temp2 = (temp1 % (sequence[seq_counter].param3 - sequence[seq_counter].param2 + 1)) + sequence[seq_counter].param2;
                vars[sequence[seq_counter].param1] = temp2;
                seq_counter++;
                break;

            case DECSKIP :
                vars[sequence[seq_counter].param1]--;
                if (vars[sequence[seq_counter].param1] == 0) {
                    seq_counter += 2;
                } 
                else {
                    seq_counter++;
                }
                break;

            case CALC :
                switch (sequence[seq_counter].param1) {
                    case ADD :
                        vars[sequence[seq_counter].param2] += sequence[seq_counter].param3;
                        break;
                }
                seq_counter++;
                break;

        }  // end of outer switch
        if (state == STOPPED) {
            break;    // exit execute loop
        }
    }
    return;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void main (void)
{

int speed_int, direction_int;

    init(); 
    exec_seq(0);
    for(;;) {
    }
}
