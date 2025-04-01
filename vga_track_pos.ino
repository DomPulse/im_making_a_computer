//so the VGA has a clock cylce of 25MHz
//arduino is only 16MHz
//the easist thing to do is to basically give 1/16 the resolution of the VGA 
//so every 25 clock cycles on the vga correspond to 16 on the arduino, this should give them ample time to synch up on the h and vsyn lines
//of course that makes effecive resolution of 40x30 which uh yeah not great
//at 5x5 font (https://bitfume.com/tools/fonts/pixel-five/) that's only 8x6 characters so yeah that's pretty crappy
//let's not worry about that right now, let's just focus on the get something working thing
//almost all the timings convert easily except for the V_LOW, VBP, and VFP, these are 2, 33, and 10 clock cycles respectively this
//this corresponds to 1.28, 21.12, and 6.4 clock cylces on arduino respecitively
//to be frank, I really don't know how to best do that other than just round to the nearest clock cyle 

//I have a seeduino that can run at up to 48MHz aparently so that's cool, more than enough :) can be set to other values, see https://files.seeedstudio.com/wiki/Seeeduino-XIAO/res/ATSAMD21G18A-MU-Datasheet.pdf
//it is at only 3.3 volts but that might be logic level high for the 5V h/vsync
//if not I can just slap on some transistors I guess

//i think i might be profoundly dumb and thinking aobut this too digitally
//just get the bulk times/frequencies correct, don't worry about counting clock cycles
//yeah if i get the H_High a little wrong there will be crappy signal at the edges or something
//but like, just see what happens when h and vsynch ar right and have fun

//hey guess who's and idiot and has two thumbs, this guy!
//so there isn't a 2 clock cycle delay on the vsync, it's 2 whole lines
//i mean the drawing does show that but idk it's defintely really long

//need the synch total period to be 512 clock cycles on arduino, it is off for 3.84us which is 60 clock cycles and it needs to be on for 28.16us which is 450 clock clycles
//asstute readers may notice that this does not add to 512 clock cycles, we'll deal with that later i guess



volatile uint16_t toggleCount1 = 0;  // For Timer1
volatile uint16_t toggleCount2 = 0;  // For Timer2

const int width = 300;
const int height = 32;

bool oneRow[width] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int hsync_pos_track = 0;
int vsync_pos_track = 0;

int hsync_in = 2;
int vsync_in = 3;

int hsync = 9; //pin 13 on the connector, no tape
int vsync = 11; //pin 14 on the connector it has duck tape
int brightness = 10;
int time = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(hsync_in, INPUT);
  pinMode(vsync_in, INPUT);
  pinMode(hsync, OUTPUT);
  pinMode(vsync, OUTPUT);
  pinMode(brightness, OUTPUT);

  //sets the wgm (waveform generator) bits to 101 which is phase corrected (counts up and down) and sets the top value in OCR2A I think
  //idk why i can set WGM22 here, it should br in register TCCR2B but if I try and set it there it breaks
  TCCR2A = 0b10000001;
  //prescale of 1, ie no prescale
  TCCR2B = 0b00000001;
  //top value? or like where it is compared
  OCR2A = 225;
  
  //yippe this is hsync for sure
  //now I need a second counter for vsync

  //I'm going to be so fucking fr, IDK how I got this 
  //it runs at ~60Hz and I hope that's good enough for the vsync
  TCCR1A = 0b10000001;  // Toggle OC1A (pin 9) on Compare Match
  //prescale of 32
  TCCR1B = 0b00001101;  // CTC mode, No prescaler (16MHz clock)
  OCR1A = 254; 

  //gpt ahh code
  //TIMSK1 |= _BV(OCIE1A);
  // Enable Timer2 Output Compare Interrupt (for toggles)
  //TIMSK2 |= _BV(OCIE2A);  // Enable compare match interrupt for Timer2 (Pin 3)

  sei();  // Enable global interrupts

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(hsync_in), incr_hsync, RISING);
  //attachInterrupt(digitalPinToInterrupt(vsync_in), incr_vsync, RISING);
}

/*
ISR(TIMER1_COMPA_vect) {
  toggleCount1++;  // Increment every time PWM toggles
  if(toggleCount1 >= 800) toggleCount1 = 0;
}

ISR(TIMER2_COMPA_vect) {
  toggleCount2++;  // Increment every time PWM toggles
  if(toggleCount2 >= 525) toggleCount2 = 0;
  PORTB = 0b00000100 & (TCNT2);
}
*/

void incr_hsync(){
  drawOneRow();
  //hsync_pos_track++;
  //if (hsync_pos_track >= 100) hsync_pos_track = 0;
}

void incr_vsync(){
  vsync_pos_track++;
  if (vsync_pos_track >= height) vsync_pos_track = 0;
  //bool* oneRow = myArray[20];
  //drawOneRow();
}

void drawOneRow(){
  for (int i = 0; i < width; i++){
    digitalWrite(10, oneRow[i]);
  }
}

void loop(){
  //digitalWrite(10, HIGH);
  //PORTB = 0b00000100;
  //PORTD = (TCNT2 == 0b01011100);
  //if ((TCNT2 == 0b01011100)) digitalWrite(10, HIGH);
  //if ((TCNT2 == 0b11110110)) digitalWrite(10, LOW);
  //digitalWrite(brightness, (TCNT2 >> 7));
  //if((TCNT1 == 0x00)) Serial.println('v');
  //if((TCNT2 == 0x00)) drawOneRow();
  //Serial.println(vsync_pos_track);
  
}

