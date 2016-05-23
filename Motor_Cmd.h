


// Arduino pins for the shift register
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

// 8-bit bus after the 74HC595 shift register
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6

// Arduino pins for the PWM signals.
#define MOTOR1_PWM 11
#define MOTOR2_PWM 3
#define MOTOR3_PWM 6
#define MOTOR4_PWM 5
#define SERVO1_PWM 10
#define SERVO2_PWM 9

// Codes for the motor function.
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4
#define LEFT 0
#define RIGHT 1

#define FAST 255
#define SLOW 50
#define HALF_SPEED 125
#define TWO_THIRDS_SPEED 200

void shiftWrite(int output, int high_low);
void motor_output (int output, int high_low, int speed);
void motor(int nMotor, int command, int speed);
void motor_output (int output, int high_low, int speed);
int convertDegrees(int degrees);

 void Turn(int direction,int speed);
 void Forward(int speed);
 void Backward(int speed);
 void Stop();
void ReleaseMotors();
