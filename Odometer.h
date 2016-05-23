
#include <stdint.h>

class Odometer
{

	int flexPin;

	long checkInTime;
	long lastCheck;
	long min,max;
	long movingTimeout;
	long movingElapsed;
	
	bool odometerClicked; 
	bool moving; 
	
	int rotateCount;
	float tireRadius;

	public:
	Odometer();
	void update(long currTime);
	void setup(int pSampleSize,
	int pCheckIn, 
	int pTireRadius,
	int pPin,
	int pTolerance);

	bool isMoving();
	void resetValues(long currTime);
	
	protected:
	void establishBase(int pSampleSize,int pPin,int pTolerance);
	
	
};
