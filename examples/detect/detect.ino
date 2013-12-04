/*
  Blinks built in pin13 LED on a 16mhz Arduino when it detects a 700 hz tone
  which is inside of 1, 2, and 3 button on your phones keypad, but not the others.
  Othwerise google for a tone generator on the web or an app for your phone.
  I like this one http://onlinetonegenerator.com/
  
  Frequency sampling is .. squishy business. Be prepared to mess with the
  numbers below until you get something you

  This work is entirely based on the Kevin Banks code found at
  http://www.embedded.com/design/embedded/4024443/The-Goertzel-Algorithm
  so full credit to him for his generic implementation and breakdown. I've
  simply massaged it into an Arduino library. I recommend reading his article
  for a full description of whats going on behind the scenes.
  
  The Goertzel algorithm is long standing so see 
  http://en.wikipedia.org/wiki/Goertzel_algorithm for a full description.
  It is often used in DTMF tone detection as an alternative to the Fast 
  Fourier Transform because it is quick with low overheard because it
  is only searching for a single frequency rather than showing the 
  occurrence of all frequencies.
  
  If what you really want is DTMF tone detection see el_supremo's fork of
  this library at http://forum.arduino.cc/index.php/topic,121540.0.html

  To use, you'll want a microphone not unlike this electret from Sparkfun
  https://www.sparkfun.com/products/9964 with AVD plugged to A0, VCC to 5V, and GND to GND

  See Contributors.md and add yourself for pull requests
  Released into the public domain.
*/
#include <Goertzel.h>

int sensorPin = A0;
int led = 13;

// Since arduino is a relatively slow in terms of audio, we sample literally as fast as we can
// This is generally around ~8900hz for a 16mhz arduino and 4400hz for an 8mhz arduino. We estimate to 8900 here
float sampling_freq=8900; 

// ideally an integer of SAMPLING_FREQ/N to center the bins around your content so if you're
// looking for 700hz frequency below and above it equally contribute. Read up on Kevin's article for more info
// Nyquist says the highest frequency we can target is SAMPLING_FREQ/2 
float target_freq=700.0; 

// if you're trying to detect several different drump hits all within low frequency like
// ~100-200hz you'll need a small bin size like 25 or 50 to distinguish them.
// If however you're just trying to find ANY bass hit you might want something
// basically equal to frequency youre looking for like ~100
 
// If Im detecting a frequency much higher with no care about nearby tones, like 2000hz
// Ill set to a round divisor like 200 So 1900 to 2100 could trigger, but not less or more
// Max is 200 as we have limited ram in the arduino, and sampling longer would make us less 
// responsive anyway
float n=100.0; 			

Goertzel goertzel = Goertzel(target_freq,n,sampling_freq);

void setup(){
  pinMode(led, OUTPUT);     
  Serial.begin(9600); 
}

void loop()
{
  goertzel.sample(sensorPin); //Will take n samples
  
  float magnitude = goertzel.detect();  //check them for target_freq
  
  if(magnitude>1000) //if you're getting false hits or no hits adjust this
    digitalWrite(led, HIGH); //if found, enable led
  else
    digitalWrite(led, LOW); //if not found, or lost, disable led
    
  Serial.println(magnitude);
}