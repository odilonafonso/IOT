/*
 * http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
 * 
 * T = timer period, f = clock frequency
 * T = 1 / f
 * T = 1 / 1 MHz = 1 / 10^6 Hz
 * T = (1 * 10^-6) s

Timer0
Timer0 is an 8-bit timer, meaning its counter register can record a maximum value of 255
(the same as an unsigned 8-bit byte).
Timer0 is used by native Arduino timing functions such as delay() and millis(),
so you Arduino users shouldn’t mess with it unless you’re comfortable with the consequences.

Timer1
Timer1 is a 16-bit timer, with a maximum counter value of 65535 (an unsigned 16-bit integer).
The Arduino Servo library uses this timer, so be aware if you use it in your projects.

No playground Arduino há uma série de libraries de Timer, optei por esta:
http://playground.arduino.cc/Code/Timer1

Timer2
Timer2 is an 8-bit timer that is very similar to Timer0.
It is utilized by the Arduino tone() function


Configuring and running the timer

In order to use these timers, we need to set them up, then make them start running.
To do this, we’ll use built-in registers on the AVR chip that store timer settings.
Each timer has a number of registers that do various things.
Two of these registers hold setup values, and are called TCCRxA and TCCRxB,
where x is the timer number (TCCR1A and TCCR1B, etc.).
TCCR stands for Timer/Counter Control Register.
Each register holds 8 bits, and each bit stores a configuration value.
Here are the details, taken from the ATmega328 datasheet:

TCCR1A - Timer/Counter Control Register A
  Bit           7       6       5       4     3   2     1     0
  (0x80)      COM1A1  COM1A0  COM1B1  COM1B0  -   -   WGM11 WGM10 
  Read/Write   R/W     R/W     R/W     R/W    R   R    R/W   R/W
  InitialValue  0       0       0       0     0   0     0     0

TCCR1B - Timer/Counter Control Register B
  Bit           7       6       5       4     3    2    1     0
  (0x81)      ICNC1   ICES1     -     WGM13 WGM12 CS12 CS11  CS10
  Read/Write   R/W     R/W     R/W     R/W    R    R    R/W  R/W
  InitialValue  0       0       0       0     0    0     0    0
 
 To start using our timer, the most important settings are the last three bits in
 TCCR1B, CS12, CS11, and CS10.
 These dictate the timer clock setting.
 By setting these bits in various combinations, we can tell the timer to run at different speeds.
 Here’s the relevant table from the datasheet:

CS12  CS11  CS10
 0     0     0     No clock source (Timer/Counter stopped)
 0     0     1     CLKio/1 (No prescaling)
 0     1     0     CLKio/8 (From prescaler) 
 0     1     1     CLKio/64 (From prescaler)
 1     0     0     CLKio/256 (From prescaler)
 1     0     1     CLKio/1024 (From prescaler)
 1     1     0     External clock source on T1 pin. Clock on falling edge.
 1     1     1     External clock source on T1 pin. Clock on rising edge.
 
By default, these bits are set to zero.  Let’s use a simple example, and say that we
want to have Timer1 run at clock speed, with one count per clock cycle. When it overflows,
we’ll run an Interrupt Service Routine (ISR) that toggles a LED tied to pin 2 on or off.

/*
void setup()
{
    pinMode(LEDPIN, OUTPUT);
 
    // initialize Timer1
    cli();             // disable global interrupts
    TCCR1A = 0;        // set entire TCCR1A register to 0
    TCCR1B = 0;

    // enable Timer1 overflow interrupt:
    TIMSK1 = (1 << TOIE1);
    // Set CS10 bit so timer runs at clock speed:
    TCCR1B |= (1 << CS10);
    // enable global interrupts:
    sei();
} 

You’ll notice that we used a new register, TIMSK1. This is the Timer/Counter1 Interrupt 
Mask Register. It controls which interrupts the timer can trigger. Setting the TOIE1 bit
tells the timer to trigger an interrupt when the timer overflows.
We can also set other bits to trigger other interrupts. 

Once we set the CS10 bit, the timer is running, and since we’ve enabled an overflow 
interrupt, it will call the ISR(TIMER1_OVF_vect) whenever the timer overflows.

Next, we can define the ISR:

ISR(TIMER1_OVF_vect) {
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
}

Now we’re free to define our loop() and our LED will toggle on and off regardless of what’s 
happening in the main program. To turn the timer off, we can set TCCR1B = 0 at any time.

However, let’s think about how this will work. Using the code we’ve written, how fast will our LED blink?

We’ve set Timer1 to interrupt on an overflow, and let’s assume we’re using an ATmega328 
with a 16MHz clock. Since Timer1 is 16 bits, it can hold a maximum value of (2^16 – 1), or 65535.
At 16MHz, we’ll go through one clock cycle every 1/(16*10^6) seconds, or 6.25e-8 s. 
That means 65535 timer counts will elapse in (65535 * 6.25e-8s) and our ISR will trigger in,oh… 
about 0.0041 seconds. Then again and again, every four thousandths of a second after that. Oops. 
At this rate, we probably won’t even be able to detect blinking. If anything, we’ve created an extremely
fast PWM signal for the LED that’s running at a 50% duty cycle, so it may appear to be constantly
on but dimmer than normal. An experiment like this shows the amazing power of microprocessors
– even an inexpensive 8-bit chip can process information far faster than we can detect.

Timer prescaling and CTC

Luckily, the good engineers at Atmel thought of this problem, and included some options. 
It turns out you can also set the timer to use a prescaler, which allows you to divide your 
clock signal by various powers of two, thereby increasing your timer period.
For example, let’s say we’d rather have our LED blink at one second intervals.
Going back to the TCCR1B register, we can use the three CS bits to set a better timer resolution. 
If we set CS10 and CS12 using TCCR1B |= (1 << CS10); and TCCR1B |= (1 << CS12);,
we divide our clock source by 1024. This gives us a timer resolution of 1/(16*10^6 / 1024), 
or 6.4e-5 seconds. Now the timer will overflow every (65535 * 6.4e-5s), or 4.194s.
Hm, too long. What can we do?

It turns out there’s another mode of operation for AVR timers. This mode is called 
Clear Timer on Compare Match, or CTC. Instead of counting until an overflow occurs, 
the timer compares its count to a value that was previously stored in a register. 
When the count matches that value, the timer can either set a flag or trigger 
an interrupt, just like the overflow case.

To use CTC, let’s start by figuring out how many counts we need to get to our one second 
interval. Assuming we keep the 1024 prescaler as before, we’ll calculate as follows:

(target time) = (timer resolution) * (# timer counts + 1)

and rearrange to get

(# timer counts + 1) = (target time) / (timer resolution)
(# timer counts + 1) = (1 s) / (6.4e-5 s)
(# timer counts + 1) = 15625
(# timer counts) = 15625 - 1 = 15624

Why did we add the extra +1 to our number of timer counts? In CTC mode, when the timer
matches our desired count it will reset itself to zero. This takes one clock cycle to 
perform, so we need to factor that into our calculations. In many cases, one timer tick
isn’t a huge deal, but if you have a time-critical application it can make all the
difference in the world.

Now we can rewrite our setup() function to configure the timer for these settings:

*/

// avr-libc library includes
#include <avr/io.h>
#include <avr/interrupt.h>
 
#define LEDPIN 13

void setup()
{
    pinMode(LEDPIN, OUTPUT);
 
    // initialize Timer1
    cli();          // disable global interrupts
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 15624/5;  //15624 = 1seg;
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    sei();          // enable global interrupts
}
void loop()
{
  //loop principal. a manipulação do led é feita na ISR
}

/*
 * And we’ll need to replace our overflow ISR with a compare match version:

ISR(TIMER1_COMPA_vect)
{
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
}

Keep in mind that you can use the built-in ISRs to extend timer functionality. For example,
if you wanted to read a sensor every 10 seconds, there’s no timer setup that can go
this long without overflowing. However, you can use the ISR to increment a counter variable
in your program once per second, then read the sensor when the variable hits 10.
Using the same CTC setup as in our previous example, our ISR would look something like this:

*/

volatile byte seconds=0;
volatile byte i;
volatile byte pause = 0;
#define TIMEOUT_SECONDS 10  //10 interrupções blink (5 high, 5 low)
#define TIMEOUT_PAUSE 50    //5 interrupçoes de pause
ISR(TIMER1_COMPA_vect)
{
    if (seconds == TIMEOUT_SECONDS){
      pause++;
      if (pause == TIMEOUT_PAUSE) {
        seconds = 0;
        pause = 0;
      }
//        digitalWrite(LEDPIN, !digitalRead(LEDPIN));
//        for (i=0; i<11; i++){
//          digitalWrite(LEDPIN, HIGH); delay(300);
//          digitalWrite(LEDPIN, LOW); //delay(100);
//        }
//        readMySensor();
    }
    else {
      digitalWrite(LEDPIN, !digitalRead(LEDPIN));
      seconds++;
    }
}

/*
Note that in order for a variable to be modified within an ISR, it must be declared as
volatile. In this case, we’d need to declare volatile byte seconds; or similar at the
beginning of our program.

This tutorial covers the basics of timers. As you start to understand the underlying 
concepts, you’ll want to check the datasheet for more information on your particular chip.
Datasheets are readily available on Atmel’s website. To find them, navigate to the page for
your device (8-bit AVRs found here):
http://www.atmel.com/products/AVR/default.asp?category_id=163&family_id=607
or do a search for your chip model. There’s a lot of information to wade through, but the
documentation is surprisingly readable if you have the patience.

Otherwise, experiment and have fun!  Check out our other tutorials 
http://www.engblaze.com/category/tutorials/
if you’re looking for more knowledge, or sign up for our email newsletter for future AVR 
and Arduino updates.
 */


/*
 * Abaixo, outra forma:
 * Led pisca de 1 em 1 segundo
 *
void setup()
{
  pinMode(ledPin, OUTPUT);
 
  // Configuração do timer1 
  TCCR1A = 0;                        //confira timer para operação normal pinos OC1A e OC1B desconectados
  TCCR1B = 0;                        //limpa registrador
  TCCR1B |= (1<<CS10)|(1 << CS12);   // configura prescaler para 1024: CS12 = 1 e CS10 = 1
 
  TCNT1 = 0xC2F7;                    // incia timer com valor para que estouro ocorra em 1 segundo
                                     // 65536-(16MHz/1024/1Hz) = 49911 = 0xC2F7
  
  TIMSK1 |= (1 << TOIE1);           // habilita a interrupção do TIMER1
}

void loop()
{
  //loop principal. a manipulação do led é feita na ISR
}
 
 
ISR(TIMER1_OVF_vect)                              //interrupção do TIMER1 
{
  TCNT1 = 0xC2F7;                                 // Renicia TIMER
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);  //inverte estado do led
}
*/

