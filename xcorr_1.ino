void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  int trueArraySize = 7;
  int shiftSize = (trueArraySize*2) - 1; //ans = 13
  float numTemp1 = 0.0;
  float numTemp2 = 0.0;
  
  //Store the siren but with zero in front and back
  float siren[13] = {0, 0, 0, 0, 0, 0, 0.1, 0.2, -0.1, 4.1, -2.0, 1.5, -0.1};

  //Fill the array that store the input and will be use for shifting
  float input[7] = {0.1, 4.0, -2.2, 1.6, 0.1, 0.1, 0.2};
  float newInput[13];

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
  //Shift the input array
  for(int i = 0; i < shiftSize; i++)
  {
    //Sum of siren with ^2
    float sumSirenSquare = 0.0;
    
    for(int m = 0; m < shiftSize; m++)
    {
      sumSirenSquare = sumSirenSquare + pow(siren[m], 2);
    }
    
    //Sum of newInput with ^2
    float sumNewInputSquare = 0.0;
    
    for(int l = 0; l < shiftSize; l++)
    {
      sumNewInputSquare = sumNewInputSquare + pow(newInput[l], 2);
    }
      
    //corr
    float corr = 0.0;

    for(int k = 0; k < shiftSize; k++)
    {
      corr = corr + (newInput[k] * siren[k]);
    }

    float normCorr = corr / sqrt(sumSirenSquare * sumNewInputSquare);
    Serial.print(corr);
    Serial.print("   ");
    Serial.println(normCorr);
  
    //Reset tempNum
    numTemp1 = 0.0;
    numTemp2 = 0.0;
  
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
