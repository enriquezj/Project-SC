#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

//For FFT
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

//For corrrelation
//int trueArraySize = 7; //test
//int shiftSize = (trueArraySize*2) - 1; //for test ans = 13, change this for smaller shift 13 or less and remember to change the siren(less zero)
//int trueArraySize = 34;
//int shiftSize = (trueArraySize*2) - 1; // ans = 67
int trueArraySize = 36;
int shiftSize = (trueArraySize*2) - 1; // ans = 67
  
//Store the siren but with zero in front
unsigned long sumSirenSquare = 0.00;
//double siren[13] = {0, 0, 0, 0, 0, 0, 0.1, 0.2, -0.1, 4.1, -2.0, 1.5, -0.1}; //test but not going to work with unsigned long
/*double siren[67] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        
                    0, 0, 0,
                    31.68, 77.24, 120.06, 162.64, 203.85, 244.21, 284.29, 213.54, 29.40, 93.65,
                    144.34, 173.78, 195.09, 208.98, 216.20, 222.02, 226.53, 229.42, 230.96, 234.42,
                    182.00, 121.52, 70.51, 19.73, 28.26, 73.38, 118.47, 162.73, 202.98, 242.87,
                    282.48, 258.78, 29.01, 88.07
};*/

double siren[71] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        
                    0, 0, 0, 0, 0,
                    20.46, 29.49, 21.30, 337.50, 188.66, 337.12, 326.33, 28.20, 335.59, 27.94, 
                    324.19, 341.60, 190.75, 477.71, 477.77, 52.91, 478.50, 192.78, 477.68, 29.68,
                    53.24, 477.78, 28.00, 338.39, 22.44, 329.38, 340.31, 337.59, 333.39, 21.70, 
                    326.00, 335.43, 29.39, 20.15, 137.12, 138.23
};


//Fill the array that store the input and will be use for shifting
//double input[7] = {0.1, 4.0, -2.2, 1.6, 0.1, 0.1, 0.2}; //test but not going to work with unsigned long
//double newInput[13]; //test
//double input[34];
//double newInput[67];
double input[36];
double newInput[71];
 
void setup() 
{
    Serial.begin(115200);

    //Sum of siren with ^2
    //For cross corr
    for(int m = 0; m < shiftSize; m++)
    {
      sumSirenSquare = sumSirenSquare + pow(siren[m], 2);
    }
 
    sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}
 
void loop() 
{
  int incomingByte = 0;
  
  //send data only when you receive data:
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    incomingByte = Serial.read();
  }
  
  if (incomingByte == 'y')
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

    input[i] = peak;
  }

    correlation();
    //while(1); //Run code once
  }
    
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
    unsigned long corr = 0.00;

    for(int k = 0; k < shiftSize; k++)
    {
      corr = corr + (newInput[k] * siren[k]);
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
