/*
  The Goertzel algorithm is long standing so see 
  http://en.wikipedia.org/wiki/Goertzel_algorithm for a full description.
  It is often used in DTMF tone detection as an alternative to the Fast 
  Fourier Transform because it is quick with low overheard because it
  is only searching for a single frequency rather than showing the 
  occurrence of all frequencies.
  
  This work is entirely based on the Kevin Banks code found at
  http://www.eetimes.com/design/embedded/4024443/The-Goertzel-Algorithm 
  so full credit to him for his generic implementation and breakdown. I've
  simply massaged it into an Arduino library. I recommend reading his article
  for a full description of whats going on behind the scenes.

  Created by Jacob Rosenthal, June 20, 2012.
  Released into the public domain.
*/
// include core Wiring API
#include "Arduino.h"

// include this library's description file
#include "Goertzel.h"

float SAMPLING_RATE;
float TARGET;
int N;
float coeff;
float Q1;
float Q2;
float sine;
float cosine;

byte testData[160];

Goertzel::Goertzel(float TARGET_FREQUENCY, float BLOCK)
{
	#if F_CPU == 16000000L
		Goertzel(TARGET_FREQUENCY, BLOCK, 9000.0);
	#else 
		Goertzel(TARGET_FREQUENCY, BLOCK, 4400.0);
	#endif
}

Goertzel::Goertzel(float TARGET_FREQUENCY,float BLOCK,float SAMPLING_FREQ)
{
  
  SAMPLING_RATE=SAMPLING_FREQ;	//on 16mhz, ~8928.57142857143, on 8mhz ~44444
  TARGET=TARGET_FREQUENCY; //must be integer of SAMPLING_RATE/N
  N=BLOCK;	//Block size
  int	k;
  float	floatN;
  float	omega;

  floatN = (float) N;
  k = (int) (0.5 + ((floatN * TARGET_FREQUENCY) / SAMPLING_RATE));
  omega = (2.0 * PI * k) / floatN;
  sine = sin(omega);
  cosine = cos(omega);
  coeff = 2.0 * cosine;

  ResetGoertzel();
}


/* Call this routine before every "block" (size=N) of samples. */
void Goertzel::ResetGoertzel(void)
{
  Q2 = 0;
  Q1 = 0;
}


/* Call this routine for every sample. */
void Goertzel::ProcessSample(byte sample)
{
  float Q0;
  Q0 = coeff * Q1 - Q2 + (float) sample;
  Q2 = Q1;
  Q1 = Q0;
}


/* Basic Goertzel */
/* Call this routine after every block to get the complex result. */
void Goertzel::GetRealImag(float *realPart, float *imagPart)
{
  *realPart = (Q1 - Q2 * cosine);
  *imagPart = (Q2 * sine);
}


/* Sample some test data. */
void Goertzel::sample(int sensorPin)
{
  for (int index = 0; index < N; index++)
  {
    testData[index] = (byte) analogRead(sensorPin);
  }
}


float Goertzel::detect()
{

  int	index;

  float	magnitudeSquared;
  float	magnitude;
  float	real;
  float	imag;

  /* Process the samples. */
  for (index = 0; index < N; index++)
  {
    ProcessSample(testData[index]);
  }

  /* Do the "standard Goertzel" processing. */
  GetRealImag(&real, &imag);

  magnitudeSquared = real*real + imag*imag;
  magnitude = sqrt(magnitudeSquared);

  ResetGoertzel();
  return magnitude;
}

