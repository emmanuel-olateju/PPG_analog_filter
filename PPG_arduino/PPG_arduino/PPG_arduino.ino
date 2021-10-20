#include <LiquidCrystal.h>
LiquidCrystal lcd(11,10,9,8,7,6,5);
#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); // Create FFT object We then define the variables specific to the signal.
const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2
//const double signalFrequency = 1000;
double samplingFrequency;
//const uint8_t amplitude = 100;

double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

double bpm=0;
double bpmFrequency=0;
double x;
//int ppgAnalogData[64];
//float f_peaks[5];
void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("...BPM-COUNTER..");
  delay(3000);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Ready");
}

void(* resetFunc)(void)=0;

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("....COMPUTING...");
  int j=1;
  while(j<=4){
    Serial.println("fetching data");
    int s1=millis();
    for(int i=0;i<=63;i++){
      switch(i){
        case 0:
          vReal[i]=analogRead(A0);
          break;
        case 1:
          vReal[i]=analogRead(A0);
          break;
        default:
          vReal[i]=(analogRead(A0)+vReal[i-1]+vReal[i-2])/3;
          break;
      }
      vImag[i]=0;
      delay(40);
    }
    s1=(millis()-s1)/63;
    samplingFrequency=1000/s1;
    if(samplingFrequency!=25){
     Serial.println("resetting");
     resetFunc();
    }
    Serial.print("samplingFrequency:");
    Serial.println(samplingFrequency);
    Serial.println("computing..........");
    delay(250);
  
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); //Compute FFT
    FFT.ComplexToMagnitude(vReal, vImag, samples); // Compute magnitudes
    x = FFT.MajorPeak(vReal, samples, samplingFrequency);
    Serial.print("frequency:");
    Serial.println(x, 6);
    delay(750);    /* Repeat after delay */
    if((x>3.5)or(x<=0))j--;
    else{
      bpmFrequency+=x;
      Serial.println(j);
      lcd.setCursor(j+5 ,1);
      lcd.print("*");
      j++;
    }
    if(j<=0)j=1;
  }
  lcd.clear();
  bpmFrequency/=4;
  Serial.print("BPM FREQUENCY:");
  Serial.println(bpmFrequency);
  bpm=bpmFrequency*60;
  Serial.print("BPM:");
  Serial.println(bpm);
  lcd.setCursor(0,0);
  lcd.print("...BPM-COUNTER..");
  lcd.setCursor(0,1);
  lcd.print(String(bpm,2));
  lcd.print("bpm");
  x=0;
  bpmFrequency=0;
  bpm=0;
  Serial.println();Serial.println();Serial.println();
  delay(3000);
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
   for (uint16_t i = 0; i < bufferSize; i++)
   {
      double abscissa;
      /* Print abscissa value */
      switch (scaleType){
        case SCL_INDEX:
          abscissa = (i * 1.0);
          break;
        case SCL_TIME:
          abscissa = ((i * 1.0) / samplingFrequency);
          break;
        case SCL_FREQUENCY:
          abscissa = ((i * 1.0 * samplingFrequency) / samples);
          break;
      }
   Serial.print(abscissa, 6);
   if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
      Serial.print(" ");
      Serial.println(vData[i], 4);
   }
   Serial.println();
}
