#include <Arduino.h>
#include "Odometer.h"

Odometer::Odometer(){
	 checkInTime =0;
	 lastCheck=0;
	 rotateCount =0;
	 flexPin = 0;
	 min = 0;
	 max = 0;
	 moving = false;
	 movingElapsed = 0;
	 odometerClicked = false;
}


void Odometer::setup(int pSampleSize,
	int pCheckIn, 
	int pTireRadius,
	int pPin,
	int pTolerance)
{
	lastCheck = 0;
	checkInTime = pCheckIn;
	tireRadius = pTireRadius;
	flexPin = pPin;
	movingTimeout = 3000;
	establishBase(pSampleSize,pPin,pTolerance);
}

void Odometer::establishBase(int pSampleSize,int pPin,
int pTolerance)
{
	long sampleTotal=0;
	long flexAverage;
	Serial.print("establishing odometer base...");
	if(pSampleSize>0)
	{
		for(int x=0;x<pSampleSize;x++)
		{
			sampleTotal +=analogRead(pPin);
			delay(10);
		}
		flexAverage = sampleTotal/pSampleSize;
		min = flexAverage - pTolerance;
		max = flexAverage +pTolerance;
	}
	Serial.print("completed base line = ");
		Serial.println(flexAverage);
	Serial.print("min ");
		Serial.println(min);
	Serial.print("max ");
		Serial.println(max);
		
		if(movingElapsed ==0){
		movingElapsed = millis();
	}

}

void Odometer::update(long currTime)
{
	if(min == max){
		Serial.println("Odometer setup not run...exiting");
		return ;
	}

	if( (currTime - lastCheck) > checkInTime)
	{
		int currValue =analogRead(flexPin);
		bool clicked = (currValue<min || currValue >max);
		movingElapsed = clicked ?currTime:movingElapsed;
		moving = odometerClicked 
			&& !clicked 
			|| (currTime - movingElapsed)< movingTimeout; 
	
		odometerClicked = clicked;
		lastCheck = currTime;
		rotateCount += (int)moving;
		
	}
}

bool Odometer::isMoving()
{
		return moving;
}


void Odometer::resetValues(long pCurrTime)
{
	movingElapsed  =pCurrTime;
	lastCheck = pCurrTime;
}