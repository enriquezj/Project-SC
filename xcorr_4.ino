#include "arduinoFFT.h"
 
#define SAMPLES 256             //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be less than 10000 due to ADC

//For FFT
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

//For corrrelation
int trueArraySize = 7; //test
int shiftSize = (trueArraySize*2) - 1; //for test ans = 13, change this for smaller shift 13 or less and remember to change the siren(less zero)
  
//Store the siren but with zero in front
double sumSirenSquare = 0.00;
double siren[7] = {0.1, 0.2, -0.1, 4.1, -2.0, 1.5, -0.1}; //test but not going to work with unsigned long
double newSiren[13];



//Fill the array that store the input and will be use for shifting
double input[7] = {0.1, 4.0, -2.2, 1.6, 0.1, 0.1, 0.2}; //test but not going to work with unsigned long
double newInput[13];
 
void setup() 
{
    Serial.begin(9600);

    //Fill the front of the siren with zero
    for (int i = 0; i < shiftSize;)
    {
      newSiren[i] = 0;
      i++;
    
      if (i == trueArraySize - 1)
      {
        for (int j =0; i < shiftSize; i++, j++)
        newSiren[i] = siren[j];
      }
    }

    //Sum of siren with ^2
    //For cross corr
    for(int m = 0; m < shiftSize; m++)
    {
      sumSirenSquare = sumSirenSquare + pow(newSiren[m], 2);
    }
 
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));

    Serial.print("Press 'y + enter' to start: ");
}
 
void loop() 
{ 
    int incomingChar = 'n';
    
    if (incomingChar == 'n')
    {
      //send data only when you receive data:
      if (Serial.available() > 0)
      {
        // read the incoming byte:
        incomingChar = Serial.read();
      }
    }
  
    if (incomingChar == 'y')
    {
      
    //collect input
    for (int i = 0; i < trueArraySize; i++)
    {   
      //SAMPLING
      for(int i=0; i<SAMPLES; i++)
      {
          microseconds = micros();    //Overflows after around 70 minutes!
       
          vReal[i] = analogRead(A0);
          vImag[i] = 0;
       
          while(micros() < (microseconds + sampling_period_us)){
          }
      }
   
      //FFT
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
      double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  
      Serial.println(peak);
  
      //input[i] = peak;
    }
  
    /*
    //Checking the input
    for(int i = 0; i < trueArraySize; i++)
    {
      Serial.print(input[i]);
      Serial.print(", ");
    }
    Serial.println(""); 
  */
  
      correlation();
      //while(1); //Run code once
  }
    
}

void getVolts()
{
  
}

void correlation() 
{
  double numTemp1 = 0.00;
  double numTemp2 = 0.00;

  //Fill the input array with zero
  for(int i = trueArraySize; i < shiftSize; i++)
  {
    if(i == trueArraySize)
    {
      for(int j = 0; j < trueArraySize; j++)
        newInput[j] = input[j];
    }

    newInput[i] = 0;
  }
/*
  //Checking the array
  for(int k = 0; k < shiftSize; k++)
  {
    Serial.print(newInput[k]);
    Serial.print(" ");
  }
  Serial.println(""); 
*/

  //Correlation code
  for(int i = 0; i < shiftSize; i++)
  { 
    //Sum of newInput with ^2
    double  sumNewInputSquare = 0.00;
    
    for(int l = 0; l < shiftSize; l++)
    {
      sumNewInputSquare = sumNewInputSquare + pow(newInput[l], 2);
    }
      
    //correlation
    double corr = 0.00;

    for(int k = 0; k < shiftSize; k++)
    {
      corr = corr + (newInput[k] * newSiren[k]);
    }

    //normalized correlation
    double normCorr = corr / sqrt(sumSirenSquare * sumNewInputSquare);

    //Print results
    Serial.print(corr);
    Serial.print("   ");
    Serial.println(normCorr);
    if (normCorr >= 0.94)
    {
      Serial.print("                                   ");
      Serial.println("Signal detected");
    }
  
    //Reset tempNum
    numTemp1 = 0.00;
    numTemp2 = 0.00;

    //Shifting
    for(int j = i; j < shiftSize; j++)
    {
      numTemp2 = newInput[j];
      newInput[j] = numTemp1;
      numTemp1 = numTemp2;
    }
/*
    //Checking the shifting
    for(int k = 0; k < shiftSize; k++)
    {
      Serial.print(newInput[k]);
      Serial.print(" ");
    }
    Serial.println("");   
*/
  } 
}
