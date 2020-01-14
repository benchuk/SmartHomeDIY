//***************** 8x8 -> pins 567 **********************//
//***************** Color strip -> pin 8 **********************//
//***************** speaker pin 9 **********************//
//***************** S1 pin 10 **********************//
//***************** S2 pin A3 **********************//
//***************** S3 pin 2 **********************//
//***************** S4 pin 3 **********************//

#include <Arduino.h>
#include <FastLED.h>
#include <MaxMatrix.h>
#include <PinChangeInt.h>

// TONES  ==========================================
// Start by defining the relationship between
//       note, period, &  frequency.
#define c 3830 // 261 Hz
#define d 3400 // 294 Hz
#define e 3038 // 329 Hz
#define f 2864 // 349 Hz
#define g 2550 // 392 Hz
#define a 2272 // 440 Hz
#define b 2028 // 493 Hz
#define C 1912 // 523 Hz
// Define a special note, 'R', to represent a rest
#define R 0
// pin change interrupts groups are:
// A0 to A5
// D0 to D7
// D8 to D13
// So using 2 groups pin 7 on one group and pin A3 on the other group
// This way I do not have to ready pin state to make sure which one was changed.
#define SWITCH3_PIN_CHANGE_INTERRUPT 10
#define SWITCH4_PIN_CHANGE_INTERRUPT A3
// interrupts from touch button
#define interruptTouch1Pin 2
#define interruptTouch2Pin 3

int DIN = 7; // DIN pin of MAX7219 module
int CLK = 6; // CLK pin of MAX7219 module
int CS = 5;  // CS pin of MAX7219 module
int maxInUse = 1;
MaxMatrix m(DIN, CS, CLK, maxInUse);


 char smile02[] = { 4, 8, B00000010, B01011001, B00001001, B00000110, B00000000, // ?
};

char ONE[] = {
    3, 8, B01000010, B01111111, B01000000, B00000000, B00000000, // 1
};

char TWO[] = {
    4, 8, B01100010, B01010001, B01001001, B01000110, B00000000, // 2
};

char THREE[] = {
    4, 8, B00100010, B01000001, B01001001, B00110110, B00000000, // 3
};


int gameEnded = 0;
int counter = 1;

#define LED_PIN 8
#define NUM_LEDS 12
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

int speakerOut = 9;
int melody1[] = {C, b, g, C, b, e, R, C, c, g, a, C};
int melody2[] = {a, b, C, g, b, R, e, g, C, e, f, a};
int beats[] = {16, 16, 16, 8, 8, 16, 32, 16, 16, 16, 8, 8};
int MAX_COUNT = sizeof(melody1) / 2; // Melody length, for looping.

#define c  261
#define d 294
#define e  329
#define f 349
#define g 391
#define gS 415
#define a  440
#define aS  455
#define b  466
#define cH 523
#define cSH  554
#define dH  587
#define dSH 622
#define eH  659
#define fH 698
#define fSH  740
#define gH  784
#define gSH 830
#define aH 880

void beep(int note, int duration)
{
  //Play tone on buzzerPin
    tone(speakerOut, note, duration);

 //Play different LED depending on value of 'counter'
  if(counter % 2 == 0)
  {
   // digitalWrite(ledPin1, HIGH);
    delay(duration);
   // digitalWrite(ledPin1, LOW);
  }else
  {
    //digitalWrite(ledPin2, HIGH);
    delay(duration);
    //digitalWrite(ledPin2, LOW);
  }

  //Stop tone on buzzerPin
  noTone(speakerOut);

  delay(50);

  //Increment counter
  counter++;

}
//C C C C A#  C  C C C
void ninja()
{

  beep(cH, 200);
  beep(cH, 200);
  beep(cH, 200);
  beep(cH, 200);
  //delay(100);
  beep(aS, 200);
  delay(270);
  beep(cH, 200);
  delay(600);
 beep(cH, 200);
  beep(cH, 200);
  beep(cH, 200);

  delay(500);
}
void brawstars()
{
    beep(cH, 150);
  beep(aS, 150);
  beep(cH, 150);
  beep(dSH, 150);

  delay(220);

  beep(g, 160);
  beep(aS,160);
  beep(cH, 160);

  delay(500);


}
  void firstSection()
{
  beep(a, 500);
  beep(a, 500);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);

  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);
}

void secondSection()
{
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);
  beep(fSH, 250);

  delay(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);
  beep(cSH, 175);
  beep(cH, 125);
  beep(b, 125);
  beep(cH, 250);

  delay(350);
}

// change this to make the song slower or faster
int tempo = 108;
// Set overall tempo
//long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
    long elapsed_time = 0;
    if (tone_ > 0) { // if this isn't a Rest beat, while the tone has
        //  played less long than 'duration', pulse speaker HIGH and LOW
        while (elapsed_time < duration) {

            digitalWrite(speakerOut, HIGH);
            delayMicroseconds(tone_ / 2);

            // DOWN
            digitalWrite(speakerOut, LOW);
            delayMicroseconds(tone_ / 2);

            // Keep track of how long we pulsed
            elapsed_time += (tone_);
        }
    } else {                                   // Rest beat; loop times delay
        for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
            delayMicroseconds(duration);
        }
    }
}
int playingMusic = 0;
void playMusic1() {
    playingMusic = 1;
    for (int i = 0; i < MAX_COUNT; i++) {
        tone_ = melody1[i];
        beat = beats[i];

        duration = beat * tempo; // Set up timing

        playTone();
        // A pause between notes...
        delayMicroseconds(pause);
    }
    playingMusic = 0;
}
void playMusic2() {
    playingMusic = 1;
    for (int i = 0; i < MAX_COUNT; i++) {
        tone_ = melody2[i];
        beat = beats[i];

        duration = beat * tempo; // Set up timing

        playTone();
        // A pause between notes...
        delayMicroseconds(pause);
    }
    playingMusic = 0;
}
// #define rfCE 9
// #define rfCS 10
// #define rfMISO 12
// #define rfMOSI 11
// #define rfCLK 13

// pin change interrupts groups are:
// A0 to A5
// D0 to D7
// D8 to D13
// So using 2 groups pin 7 on one group and pin A3 on the other group
// This way I do not have to ready pin state to make sure which one was changed.

void setAllColor(CRGB::HTMLColorCode color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
    FastLED.show();
}
void FillLEDsFromPaletteColors(uint8_t colorIndex) {
    uint8_t brightness = 255;

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness,
                                   currentBlending);
        colorIndex += 3;
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
    for (int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV(random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette() {
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
    CRGB purple = CHSV(HUE_PURPLE, 255, 255);
    CRGB green = CHSV(HUE_GREEN, 255, 255);
    CRGB black = CRGB::Black;

    currentPalette =
        CRGBPalette16(green, green, black, black, purple, purple, black, black,
                      green, green, black, black, purple, purple, black, black);
}
// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p,
// RainbowStripeColors_p, OceanColors_p, CloudColors_p, LavaColors_p,
// ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can
// write code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically() {
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if (lastSecond != secondHand) {
        lastSecond = secondHand;
        if (secondHand == 0) {
            currentPalette = RainbowColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 10) {
            currentPalette = RainbowStripeColors_p;
            currentBlending = NOBLEND;
        }
        if (secondHand == 15) {
            currentPalette = RainbowStripeColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 20) {
            SetupPurpleAndGreenPalette();
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 25) {
            SetupTotallyRandomPalette();
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 30) {
            SetupBlackAndWhiteStripedPalette();
            currentBlending = NOBLEND;
        }
        if (secondHand == 35) {
            SetupBlackAndWhiteStripedPalette();
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 40) {
            currentPalette = CloudColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 45) {
            currentPalette = PartyColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 50) {
            currentPalette = myRedWhiteBluePalette_p;
            currentBlending = NOBLEND;
        }
        if (secondHand == 55) {
            currentPalette = myRedWhiteBluePalette_p;
            currentBlending = LINEARBLEND;
        }
    }
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue, CRGB::Black,

    CRGB::Red,  CRGB::Gray,  CRGB::Blue,  CRGB::Black,

    CRGB::Red,  CRGB::Red,   CRGB::Gray,  CRGB::Gray,
    CRGB::Blue, CRGB::Blue,  CRGB::Black, CRGB::Black};

// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.

// char A[] = {
//     4, 8, B01111110, B00010001, B00010001, B01111110,
// };
int first = 1;
int s1 = 0;
unsigned long time1 = 0;
void onS1() {
    if(first)
    {
        first = 0;
        return;
    }
    if(counter<4)
    {
        return;
    }
      if(s1 == 1)
    {
        return;
    }
   time1 = millis();
    s1=1;
}


int first2 = 1;
int s2 = 0;
unsigned long time2 = 0;
void onS2() {
    if(first2)
    {
        first2 = 0;
        return;
    }
    if(counter<4)
    {
        return;
    }
    if(s2 == 1)
    {
        return;
    }
    time2 = millis();
    s2=1;

}

int first4 = 1;
int s4 = 0;
void onS4() {
    if(first4)
    {
        first4 = 0;
        return;
    }
    if(gameEnded)
    {
        Serial.println("onS4");
        s4=1;
    }
}
void onS3() { Serial.println("onS3"); }


void initInterrupts() {
    noInterrupts();
    pinMode(interruptTouch1Pin, INPUT_PULLUP);
    pinMode(interruptTouch2Pin, INPUT_PULLUP);
    pinMode(SWITCH3_PIN_CHANGE_INTERRUPT, INPUT_PULLUP);
    pinMode(SWITCH4_PIN_CHANGE_INTERRUPT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptTouch1Pin), onS1, RISING);
    attachInterrupt(digitalPinToInterrupt(interruptTouch2Pin), onS2, RISING);
    // attachPinChangeInterrupt(SWITCH3_PIN_CHANGE_INTERRUPT, onS3, CHANGE);
    // attachPinChangeInterrupt(SWITCH4_PIN_CHANGE_INTERRUPT, onS4, CHANGE);
    attachPinChangeInterrupt(SWITCH3_PIN_CHANGE_INTERRUPT, onS3, RISING);
    attachPinChangeInterrupt(SWITCH4_PIN_CHANGE_INTERRUPT, onS4, RISING);
    interrupts();
}

void animate()
{
     ChangePalettePeriodically();

    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors(startIndex);

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void initState()
{

    s1=0;
    s2=0;
    time1 =0;
    time2= 0;
    //s3=0;
    s4=0;
    counter = 1;
     gameEnded = 0;
}



/*
  Star Wars theme
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs

                                              Robson Couto, 2019
*/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


// // change this to make the song slower or faster
// int tempo = 108;

// change this to whichever pin you want to use
int buzzer = 11;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // Dart Vader theme (Imperial March) - Star wars
  // Score available at https://musescore.com/user/202909/scores/1141521
  // The tenor saxophone part was used

  NOTE_AS4,8, NOTE_AS4,8, NOTE_AS4,8,//1
  NOTE_F5,2, NOTE_C6,2,
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4,
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4,
  NOTE_AS5,8, NOTE_A5,8, NOTE_AS5,8, NOTE_G5,2, NOTE_C5,8, NOTE_C5,8, NOTE_C5,8,
  NOTE_F5,2, NOTE_C6,2,
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4,

  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4, //8
  NOTE_AS5,8, NOTE_A5,8, NOTE_AS5,8, NOTE_G5,2, NOTE_C5,-8, NOTE_C5,16,
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  NOTE_F5,8, NOTE_G5,8, NOTE_A5,8, NOTE_G5,4, NOTE_D5,8, NOTE_E5,4,NOTE_C5,-8, NOTE_C5,16,
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,

  NOTE_C6,-8, NOTE_G5,16, NOTE_G5,2, REST,8, NOTE_C5,8,//13
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  NOTE_F5,8, NOTE_G5,8, NOTE_A5,8, NOTE_G5,4, NOTE_D5,8, NOTE_E5,4,NOTE_C6,-8, NOTE_C6,16,
  NOTE_F6,4, NOTE_DS6,8, NOTE_CS6,4, NOTE_C6,8, NOTE_AS5,4, NOTE_GS5,8, NOTE_G5,4, NOTE_F5,8,
  NOTE_C6,1

};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;


void play3()
{
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(speakerOut, melody[thisNote], noteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(speakerOut);
  }
}
void setup() {
      Serial.begin(9600);
      pinMode(speakerOut, OUTPUT);
    initInterrupts();
    m.init();          // MAX7219 initialization
    m.setIntensity(8); // initial led matrix intensity, 0-15

    //delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
     setAllColor(CRGB::White);
     m.clear();
     m.writeSprite(2, 0, ONE);

     initState();
}

void loop() {
//play3();
//brawstars();
//ninja();
    if(playingMusic)
{
    return;
}
    if(gameEnded > 100)
    {
        if(s4)
        {

            initState();
              s4 = 0;
            return;
        }
        animate();
        return;
    }
     if(gameEnded > 0)
     {
         gameEnded++;
         return;
     }
if(counter<=4)
{
    if(counter ==1)
    {
         m.clear();
        m.writeSprite(2, 0, THREE);
        setAllColor(CRGB::White);
    }
      if(counter ==2)
    {
         m.clear();
        m.writeSprite(2, 0, TWO);
    }
     if(counter ==3)
    {
         m.clear();
        m.writeSprite(2, 0, ONE);
    }
     if(counter ==4)
    {
         m.clear();
        m.writeSprite(2, 0, smile02);
         setAllColor(CRGB::Green);
    }
    delay(1000);
    counter++;
    return;
}


if(time1 > time2)
{
     //Serial.println("onS1");
     setAllColor(CRGB::Red);
        m.clear();
   // playMusic1();
     //firstSection();
     ninja();
    gameEnded = 1;
    delay(1000);
     return;
}
else if(time1 < time2)
{
     // Serial.println("onS1");
     setAllColor(CRGB::Blue);
        m.clear();
    //playMusic2();
   // secondSection();
   //Variant 1
//   beep(f, 250);
//   beep(gS, 500);
//   beep(f, 350);
//   beep(a, 125);
//   beep(cH, 500);
//   beep(a, 375);
//   beep(cH, 125);
//   beep(eH, 650);

//   delay(500);

  //Repeat second section
  //secondSection();
  brawstars();

//   //Variant 2
//   beep(f, 250);
//   beep(gS, 500);
//   beep(f, 375);
//   beep(cH, 125);
//   beep(a, 500);
//   beep(f, 375);
//   beep(cH, 125);
//   beep(a, 650);

//   delay(650);

    gameEnded = 1;
    delay(1000);
    return;
}
else {
    // Serial.println("equal");
    //  Serial.println(time1);
    //   Serial.println(time2);
}

}
