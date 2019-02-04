// 
// 
// 

#include "calc.h"
#include "objdir.h"
#include "status.h"

int8_t CalcAngle(int16_t xPos, int16_t yPos)
{
	float *coordinates = NULL;	// converted coordinates
	float xTmp = 0;				// temp value for conversion
	float yTmp = 0;				// temp value for conversion
	float xVal = 0;				// 
	float yVal = 0;				// 

	const float a = 200.0;		// 
	float b =			0;		// 
	const float c = 195.0;		// 

	uint8_t cmp1A = 0;			/* temporary compare values */
	uint8_t cmp1B = 0;
	uint8_t cmp2A = 0;
	uint8_t cmp2B = 0;

	// all angles in radians & non negative!
	float alpha =	0;						// inner angle of triangle at point A (between b & c)
	//float alpha1a =	0;						// solution a for angle between c & x-axis (orientation of triangle)
	//float alpha1b = 0;						// solution a for angle between c & x-axis (orientation of triangle)
	float alpha2 =	0;						// angle between b & x-axis 
	float beta =	0;						// inner angle of triangle at point B (between a & c)
	//float beta1 =	0;						// ????????????????? outer angle between a & b (angle for servo 2)
	float gamma =	0;						// inner angle of triangle at point C (between b & c)
	float servo1A =	0;						// angle for servo motor 1
	float servo1B = 0;
	const float servo1offs = 2.35619449;	// conversion from field to servo orientation (165° offset to x-axis)
	//float servo2 =	0;						// angle for servo motor 2
	float servo2A = 0;						// 
	float servo2B = 0;						// 
	const float servo2offs = 0.52359877;	// conversion from triangle to servo orientation (30° offset to c)

	// convert the input position values based on the field coordinate system
	// to the coordinate system of the robot axis
	xTmp = xPos / 10.0;
	yTmp = yPos / 10.0;
#ifdef _DEBUG
	Serial.print("xTmp: ");
	Serial.println(xTmp, 8);
	Serial.print("yTmp: ");
	Serial.println(yTmp, 8);
#endif 
	
	coordinates = ConvertCoordinate(CONVERT_COORDINATE_TO_ROBOT, &xTmp, &yTmp);
	xVal = /*10.629;*/ coordinates[ID_AXIS_1];
	yVal = /*-313.209;*/ coordinates[ID_AXIS_2];
#ifdef _DEBUG
	Serial.print("xVal: ");
	Serial.println(xVal, 8);
	Serial.print("yVal: ");
	Serial.println(yVal, 8);
#endif 

	// calculate missing lengths and angles
	b = sqrt(xVal * xVal + yVal * yVal);
	alpha2 = atan(yVal / xVal) * (-1);
#ifdef _DEBUG
	Serial.print("b: ");
	Serial.println(b, 8);
	Serial.print("alpha2: ");
	Serial.println(alpha2 * 180 / PI, 8);
#endif 

	// zielfenster ?!?!!?!?!?!?!!
	alpha = a + c == b ? 0 : acos((b * b + c * c - a * a) / (2.0 * b * c)); /* if both arms are in a straight line don't use law of cosine - !!!!! test with very small numbers !!!!! */ 
	//alpha1a = 2 * PI - alpha - alpha2; /* solution a */
	//alpha1b = 2 * PI - alpha2; /* solution b */

	beta = acos((a * a + c * c - b * b) / (2.0 * a * c));
	gamma = PI - alpha - beta;
#ifdef _DEBUG
	Serial.print("alpha: ");
	Serial.println(alpha * 180 / PI, 8);
	Serial.print("alpha1: ");
	Serial.println(alpha1 * 180 / PI, 8);
	Serial.print("beta: ");
	Serial.println(beta * 180 / PI, 8);
	Serial.print("gamma: ");
	Serial.println(gamma * 180 / PI, 8);
#endif 
	
	/* Calculation of the new axis angles considering a minimum travel distance
	 * between the actual and new position of the robot arm. The servo motors have a
	 * operating angle of 300° ( 0° - 300°). Due to mechanical requirements this range 
	 * might be reduced and it is neccessary to check if servo1 & servo2 are able to
	 * reach the given coordinates. */

	 /* calculation of the angles based on the orientation of the triangle in the coordinate system */
	servo1A = 2 * PI - servo1offs - alpha - alpha2; /* point B is "left" from b */
	servo1B = 2 * PI - servo1offs - alpha2;			/* point B is "right" from b */
	servo2A = 2 * PI - beta - servo2offs;			/* point B is "left" from b */
	servo2B = beta - servo2offs;					/* point B is "right" from b */

	/* check which solutions are inside the allowed range of the dynamixel servos to reach the given position */
	cmp1A = chkServoLmt(1, servo1A);
	cmp1B = chkServoLmt(1, servo1B);
	cmp2A = chkServoLmt(2, servo2A);
	cmp2B = chkServoLmt(2, servo2B);

	if (cmp1A == 1)
	{
		if (cmp1B == 1) /* both solutions for servo1 are allowed */
		{
			/* check which solutions of servo1 can also be reached by servo2 */

			if (cmp2A == 1 && cmp2B == 1) /* both solutions for servo2 are allowed */
			{

			}
			else if (cmp2A == 1) /* only solution A for servo2 is allowed */
			{

			}
			else if (cmp2B == 1) /* only solution B for servo2 is allowed */
			{

			}
			else
			{
				//error
				return -1;
			}
		}
		else if (cmp1B == 0) /* only solution A for servo1 is allowed */
		{
			if (cmp2A == 1)
			{
				/* write angle values to objDir */
				// error handling !!!!!!!!!!!!!!
				// Wie sicherstellen das immer beide Werte oder keiner geschrieben wird? Evtl. muss es noch einen Parameter check/set geben?
				SetObjData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE, (int16_t)round(degrees(servo1A)) * 10);
				SetObjData(OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE, (int16_t)round(degrees(servo2A)) * 10);
				return 0;
			}
			else if (cmp2A == 0)
			{
				// error
				return -1;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if(cmp1A == 0)
	{
		if (cmp1B == 1) /* only solution B for servo1 is allowed */
		{
			if (cmp2B == 1)
			{
				/* write angle values to objDir */
				// error handling !!!!!!!!!!!!!!
				// Wie sicherstellen das immer beide Werte oder keiner geschrieben wird? Evtl. muss es noch einen Parameter check/set geben?
				SetObjData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE, (int16_t)round(degrees(servo1B)) * 10);
				SetObjData(OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE, (int16_t)round(degrees(servo2B)) * 10);
				return 0;
			}
			else if (cmp2B == 0)
			{
				// error
				return -1;
			}
			else
			{
				// error
				return -1;
			} 
		}
		else 
		{
			 /* no solution is allowed */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

/* Check if the given angle in rad is inside the allowed range of the dynamixel servo
 * return 1 = value is in range
 * return 0 = value is not in range
 * return -1 = error */
uint8_t chkServoLmt(uint8_t servo, float val)
{
	if (servo == 1)
	{
		int16_t tmp = (int16_t)round(val * RAD_TO_DEG * 10);
		if (tmp >= GetObjData(AXIS_1_ANGLE_MIN) && val <= GetObjData(AXIS_1_ANGLE_MAX))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (servo == 2)
	{
		int16_t tmp = (int16_t)round(val * RAD_TO_DEG * 10);
		if (tmp >= GetObjData(AXIS_2_ANGLE_MIN) && val <= GetObjData(AXIS_2_ANGLE_MAX))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else {
		//error message?
		return -1;
	}
}

int8_t CalcPosistion(int16_t valA, int16_t valB)
{
	float pointA[2] = { 0 };	// punkt durch servo id 1
	float pointB[2] = { 0 };	// punkt durch servo id 2 (z achse) -> GESUCHT <-
	float angleA = 0;			// umgerechneter winkel von dynamixel id 0
	float angleB = 0;			// umgerechneter winkel von dynamixel id 1
	float lenghtA = 0;			// gegenkathete von winkel A
	float lenghtB = 0;			// gegenkathete von winkel B

	lenghtA = asin(angleA) * AXIS_1_LENGTH; 
	pointA[ID_AXIS_2] = lenghtA * (-1);
	pointA[ID_AXIS_1] = sqrt(lenghtA * lenghtA + AXIS_1_LENGTH * AXIS_1_LENGTH);

	lenghtB = asin(angleB) * AXIS_2_LENGTH;
	pointB[ID_AXIS_2] = lenghtA * (-1);
	pointB[ID_AXIS_1] = sqrt(lenghtB * lenghtB + AXIS_2_LENGTH * AXIS_2_LENGTH);

	return 0;
}

int8_t UpdateZPos(void)
{
	int16_t val = round((Z_AXIS_RESOLUTION / Z_AXIS_GRADIENT) * GetObjData(OBJ_IDX_Z_POS_COUNT) * 10);

	if (SetObjData(OBJ_IDX_Z_ACTUAL_POS, val) == -1)
	{
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR);
#endif 
		return -1;
	}
	return 0;
}

float* ConvertCoordinate(uint8_t direction, float *xValue, float *yValue)
{
	static float val[2] = { 0 };

	switch (direction)
	{
	case CONVERT_COORDINATE_TO_ROBOT:
		val[ID_AXIS_1] = *xValue + MACHINE_ZERO_OFFS_X_FIELD - MACHINE_ZERO_OFFS_X_ROBOT;
		val[ID_AXIS_2] = *yValue + MACHINE_ZERO_OFFS_Y_FIELD - MACHINE_ZERO_OFFS_Y_ROBOT;
		/*Serial.print("val[ID_AXIS_1]: ");
		Serial.println(val[ID_AXIS_1], 8);
		Serial.print("val[ID_AXIS_2]: ");
		Serial.println(val[ID_AXIS_2], 8);*/
		return val;

	case CONVERT_COORDINATE_TO_FIELD:
		val[ID_AXIS_1] = *xValue + MACHINE_ZERO_OFFS_X_ROBOT - MACHINE_ZERO_OFFS_X_FIELD;
		val[ID_AXIS_2] = *yValue + MACHINE_ZERO_OFFS_Y_ROBOT - MACHINE_ZERO_OFFS_Y_FIELD;
		return val;

	default:
		return NULL;
	}
}