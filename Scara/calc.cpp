// 
// 
// 

#include "calc.h"
#include "objdir.h"
#include "status.h"

uint8_t CalcAngle(int16_t xPos, int16_t yPos) {

	float *coordinates = NULL;		// converted coordinates
	float xTmp = 0;			// temp value for conversion
	float yTmp = 0;			// temp value for conversion
	float xVal = 0;
	float yVal = 0;

	float a = 200.0;		// 
	float b = 0;			// 
	float c = 195.0;		// 

	// all angles in radians!
	float angleAlpha =	0;	// inner angle of triangle at point A (between a & c)
	float angleAlpha1 = 0;	// angle between a & x-axis (angle for servo 1)
	float angleAlpha2 = 0;	// angle between c & x-axis (orientation of triangle)
	float angleBeta =	0;	// inner angle of triangle at point B (between a & b)
	float angleBeta1 =	0;	// outer angle between a & b (angle for servo 2)
	float angleGamma =	0;	// inner angle of triangle at point C (between b & c)
	float angleServo1 = 0;	// angle for servo motor 1 (alpha1)
	float angleServo2 = 0;	// angle for servo motor 2 (beta1)

	// convert the input position values based on the field coordinate system
	// to the coordinate system of the robot axis
	xTmp = xPos / (float)10.0;
	yTmp = yPos / (float)10.0;
	/*Serial.print("xTmp: ");
	Serial.println(xTmp, 8);
	Serial.print("yTmp: ");
	Serial.println(yTmp, 8);*/

	//coordinates = ConvertCoordinate(CONVERT_COORDINATE_TO_ROBOT, &xTmp, &yTmp);
	xVal = 10.629;//coordinates[x];
	yVal = -313.209;//coordinates[y];
	/*Serial.print("xVal: ");
	Serial.println(xVal, 8);
	Serial.print("yVal: ");
	Serial.println(yVal, 8);*/

	// calculate b, alpha, beta & gamma
	b = sqrt(xVal * xVal + yVal * yVal);
	angleAlpha = acos((b * b + c * c - a * a) / (2.0 * b * c)); // law of cosine
	angleBeta = acos((a * a + c * c - b * b) / (2.0 * a * c));
	angleGamma = PI - angleAlpha - angleBeta;
	/*Serial.print("b: ");
	Serial.println(b, 8);
	Serial.print("angleAlpha: ");
	Serial.println(angleAlpha * 180 / PI,8);
	Serial.print("angleBeta: ");
	Serial.println(angleBeta * 180 / PI,8);
	Serial.print("angleGamma: ");
	Serial.println(angleGamma * 180 / PI,8);*/

	// calculate alpha 2
	angleAlpha2 = atan(yVal / xVal);
	/*Serial.print("angleAlpha2: ");
	Serial.println(angleAlpha2 * 180 / PI, 8);*/

	// calculate angles alpha1 & beta1
	angleAlpha1 = angleAlpha2 - angleAlpha;
	angleBeta1 = PI - angleBeta;
	/*Serial.print("angleAlpha1: ");
	Serial.println(angleAlpha1 * 180 / PI, 8);
	Serial.print("angleBeta1: ");
	Serial.println(angleBeta1 * 180 / PI, 8);*/

	
	/* Calculation of the new axis angle considering a minimum travel distance
	 * between actual and new position of the robot arm. The servo motors have a
	 * operating angle of 300° ( +/- 180°). Due to the alignment of the robot axis,
	 * a target angle of 0° for the servos corresponds to an angle of -45° at the field.
	 * 
	 * The coordinate origin runs through the axis of rotation of servo 1 with a
	 * standard orientation of the x and y-axis (operating in fourth quadrant). 
	 */


	// propper calculation of axis angles
	

	// set new target angles for axis 1 & 2
	// Wie sicherstellen das immer beide Werte oder keiner geschrieben wird? Evtl. muss es noch einen Parameter check/set geben?
	SetObjStructData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE, (int16_t)round(degrees(angleServo1)) * 10);
	SetObjStructData(OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE, (int16_t)round(degrees(angleServo2)) * 10);


}

uint8_t CalcPosistion(int16_t angle1, int16_t angle2) {

}

int16_t CalcZPos(void) {


}

float* ConvertCoordinate(uint8_t direction, float *xValue, float *yValue) {

	static float val[2] = { NULL };
	
	if(direction == CONVERT_COORDINATE_TO_ROBOT) {
		val[x] = *xValue + MACHINE_ZERO_OFFS_X_FIELD - MACHINE_ZERO_OFFS_X_ROBOT;
		val[y] = *yValue + MACHINE_ZERO_OFFS_Y_FIELD - MACHINE_ZERO_OFFS_Y_ROBOT;
		/*Serial.print("val[x]: ");
		Serial.println(val[x], 8);
		Serial.print("val[y]: ");
		Serial.println(val[y], 8);*/
		return val;
	}
	else if (direction == CONVERT_COORDINATE_TO_FIELD) {
		val[x] = *xValue + MACHINE_ZERO_OFFS_X_ROBOT - MACHINE_ZERO_OFFS_X_FIELD;
		val[y] = *yValue + MACHINE_ZERO_OFFS_Y_ROBOT - MACHINE_ZERO_OFFS_Y_FIELD;
		return val;
	}
	else {
		return NULL;
	}
}