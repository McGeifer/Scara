// 
// 
// 

#include "calc.h"
#include "objdir.h"
#include "status.h"

/* Check if the given angle in rad is inside the allowed range of the dynamixel servo
   return 1  - value is in range
   return 0  - value is not in range
   return -1 - error */
uint8_t ChkServoLmt(uint8_t servo, float *val)
{
	if (servo == 1)
	{
		int16_t tmp = (int16_t)round(degrees(*val) * 10);
#ifdef _DEBUG
		Serial.print("ChkServoLmt() -> tmp: ");
		Serial.println(tmp, DEC);
#endif
		if (tmp >= AXIS_1_ANGLE_MIN && tmp <= AXIS_1_ANGLE_MAX)
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
		int16_t tmp = (int16_t)round(*val * RAD_TO_DEG * 10);
#ifdef _DEBUG
		Serial.print("ChkServoLmt() -> tmp: ");
		Serial.println(tmp, DEC);
#endif
		if (tmp >= AXIS_2_ANGLE_MIN && tmp <= AXIS_2_ANGLE_MAX)
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

/* ConvertCoordinates converts x and y coordiantes between the different coordinate systems (machine & field).
   return #    - pointer to array
   return NULL - error */
float* ConvertCoordinates(uint8_t direction, float *xVal, float *yVal)
{
	static float val[2] = { 0 };

	switch (direction)
	{
	case CONVERT_COORDINATE_TO_ROBOT:
		val[ID_AXIS_1] = *xVal + MACHINE_ZERO_OFFS_X_FIELD - MACHINE_ZERO_OFFS_X_ROBOT;
		val[ID_AXIS_2] = *yVal + MACHINE_ZERO_OFFS_Y_FIELD - MACHINE_ZERO_OFFS_Y_ROBOT;
#ifdef _DEBUG
		Serial.println("ConvertCoordinates()");
		Serial.print("val[ID_AXIS_1]: ");
		Serial.println(val[ID_AXIS_1], 8);
		Serial.print("val[ID_AXIS_2]: ");
		Serial.println(val[ID_AXIS_2], 8);
#endif 
		return val;

	case CONVERT_COORDINATE_TO_FIELD:
		val[ID_AXIS_1] = *xVal + MACHINE_ZERO_OFFS_X_ROBOT - MACHINE_ZERO_OFFS_X_FIELD;
		val[ID_AXIS_2] = *yVal + MACHINE_ZERO_OFFS_Y_ROBOT - MACHINE_ZERO_OFFS_Y_FIELD;
		return val;

	default:
		return NULL;
	}
}

float* CalcAngle(int16_t *xPos, int16_t *yPos)
{
	float *coordinates = NULL;	// converted coordinates
	float xTmp = 0;				// temp value for conversion
	float yTmp = 0;				// temp value for conversion
	float xVal = 0;				// 
	float yVal = 0;				// 

	float b = 0;				// length between c & a

	uint8_t cmp1A = 0;			/* temporary compare values */
	uint8_t cmp1B = 0;
	uint8_t cmp2A = 0;
	uint8_t cmp2B = 0;

	/* all angles in radians & non negative! */
	float alpha = 0;			// inner angle of triangle at point A (between b & c)
	float alpha2 = 0;			// angle between b & x-axis 
	float beta = 0;				// inner angle of triangle at point B (between a & c)
	float gamma = 0;			// inner angle of triangle at point C (between b & c)
	static float servo1A = 0;	// angle for servo motor 1
	static float servo1B = 0;	// 
	static float servo2A = 0;	// 
	static float servo2B = 0;	// 

	// convert the input position values based on the field coordinate system
	// to the coordinate system of the robot axis

	xTmp = *xPos / 10.0;
	yTmp = *yPos / 10.0;
#ifdef _DEBUG
	Serial.println("CalcAngle()");
	Serial.print("xTmp: ");
	Serial.println(xTmp, 8);
	Serial.print("yTmp: ");
	Serial.println(yTmp, 8);
#endif 

	coordinates = ConvertCoordinates(CONVERT_COORDINATE_TO_ROBOT, &xTmp, &yTmp);
	xVal = xTmp; //coordinates[ID_AXIS_1];
	yVal = yTmp; //coordinates[ID_AXIS_2];
#ifdef _DEBUG
	Serial.print("xVal: ");
	Serial.println(xVal, 8);
	Serial.print("yVal: ");
	Serial.println(yVal, 8);
#endif 

	// calculate missing lengths and angles
	b = (float)sqrt(xVal * xVal + yVal * yVal);
	alpha2 = atan(yVal / xVal) * (-1);
#ifdef _DEBUG
	Serial.print("b: ");
	Serial.println(b, 8);
	Serial.print("alpha2: ");
	Serial.println(alpha2 * 180 / PI, 8);

	/*Serial.print("Teil1: ");
	Serial.println(b * b + AXIS_2_LENGTH * AXIS_2_LENGTH - AXIS_1_LENGTH * AXIS_1_LENGTH, 8);
	Serial.print("Teil2: ");
	Serial.println(2.0 * b * AXIS_2_LENGTH, 8);
	Serial.print("Teil3: ");
	Serial.println((b * b + AXIS_2_LENGTH * AXIS_2_LENGTH - AXIS_1_LENGTH * AXIS_1_LENGTH) / (2.0 * b * AXIS_2_LENGTH), 8);*/
#endif 

	/* if both arms are in a straight line don't use law of cosine - !!!!! test with very small numbers! what about the target window? */
	alpha = AXIS_1_LENGTH + AXIS_2_LENGTH == b ?  0 : (float)acos((b * b + AXIS_2_LENGTH * AXIS_2_LENGTH - AXIS_1_LENGTH * AXIS_1_LENGTH) / (2.0 * b * AXIS_2_LENGTH)); 
	beta =  AXIS_1_LENGTH + AXIS_2_LENGTH == b ? PI : (float)acos((AXIS_1_LENGTH * AXIS_1_LENGTH + AXIS_2_LENGTH * AXIS_2_LENGTH - b * b) / (2.0 * AXIS_1_LENGTH * AXIS_2_LENGTH));
	gamma = AXIS_1_LENGTH + AXIS_2_LENGTH == b ?  0 : PI - alpha - beta;

	// berechnete Winkel und Winkel in GeoGebra stimmen nicht überein. Alpha & gamma haben bis zu 2° Abweichung wobei die Fehlerursache möglicherweise bei GeoGebra liegt!!!!!!!!!!!!!!!!!!
#ifdef _DEBUG
	Serial.print("alpha: ");
	Serial.println(alpha * 180 / PI, 8);
	Serial.print("beta: ");
	Serial.println(beta * 180 / PI, 8);
	Serial.print("gamma: ");
	Serial.println(gamma * 180 / PI, 8);
#endif 

	/* Calculation of the new axis angles considering a minimum travel distance
	 * between the actual and new position of the robot arm. The servo motors have a
	 * operating angle of 300° ( 0° - 300°). Due to mechanical requirements this range
	 * might be reduced so it is neccessary to check if servo1 & servo2 are able to
	 * reach the given coordinates. */

	 /* calculation of the servo angles based on the orientation of the triangle in the coordinate system */
	servo1A = 2 * PI - SERVO_1_OFFS - alpha - alpha2;	/* point B is "left" from b */
	servo1B = 2 * PI - SERVO_1_OFFS - (alpha2 - alpha); /* point B is "right" from b */
	servo2A = 2 * PI - SERVO_2_OFFS - beta;				/* point B is "left" from b */
	servo2B = beta - SERVO_2_OFFS;						/* point B is "right" from b */
#ifdef _DEBUG
	Serial.print("servo1A: ");
	Serial.println(degrees(servo1A), DEC);
	Serial.print("servo1B: ");
	Serial.println(degrees(servo1B), DEC);
	Serial.print("servo2A: ");
	Serial.println(degrees(servo2A), DEC);
	Serial.print("servo2B: ");
	Serial.println(degrees(servo2B), DEC);
#endif

	/* check which solutions are inside the allowed range of the dynamixel servos to reach the given position */
	cmp1A = ChkServoLmt(1, &servo1A);
	cmp1B = ChkServoLmt(1, &servo1B);
	cmp2A = ChkServoLmt(2, &servo2A);
	cmp2B = ChkServoLmt(2, &servo2B);
#ifdef _DEBUG
	Serial.print("cmp1A: ");
	Serial.println(cmp1A, DEC);
	Serial.print("cmp1B: ");
	Serial.println(cmp1B, DEC);
	Serial.print("cmp2A: ");
	Serial.println(cmp2A, DEC);
	Serial.print("cmp2B: ");
	Serial.println(cmp2B, DEC);
#endif 

	if (cmp1A)
	{
		if (cmp1B) /* both solutions for servo1 are allowed */
		{
			/* check which solution can also be reached by servo2 */

			if (cmp2A && cmp2B) /* both solutions for servo2 are allowed */
			{
				/* decition is based on the shortest way for servo2 */
				float actAngle = radians(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE) / 10);
				float diffA = actAngle < servo2A ? servo2A - actAngle : actAngle - servo2A;
				float diffB = actAngle < servo2B ? servo2B - actAngle : actAngle - servo2B;

				if (diffA < diffB)
				{
					return SetActualAngles(&servo1A, &servo2A);
				}
				else
				{
					return SetActualAngles(&servo1B, &servo2B);
				}
			}
			else if (cmp2A) /* only solution A for servo2 is allowed */
			{
				return SetActualAngles(&servo1A, &servo2A); /* A oder B nutzen ??? */
			}
			else if (cmp2B) /* only solution B for servo2 is allowed */
			{
				return SetActualAngles(&servo1B, &servo2B); /* A oder B nutzen ??? */
			}
			else
			{
				return -1;
			}
		}
		else if (!cmp1B) /* only solution A for servo1 is allowed */
		{
			if (cmp2A)
			{
				return SetActualAngles(&servo1A, &servo2A);
			}
			else if (cmp2A)
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
	else if (!cmp1A)
	{
		if (cmp1B) /* only solution B for servo1 is allowed */
		{
			if (cmp2B)
			{
				return SetActualAngles(&servo1B, &servo2B);
			}
			else if (!cmp2B)
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

float* CalcPosistion(int16_t *angleAxis1, int16_t *angleAxis2)
{
	float vecA[2] = { 0, 0 };
	float vecB[2] = { 0, 0 };
	float vecR[2] = { 0, 0 };
	float *tmp = NULL;
	float alpha1 = 0;			/* outer angle between x-axis and c */
	float beta = 0;				/* inner angle between a & c */
	float beta1 = 0;			/* inner angle between x-axis & a */

	vecA[X] = AXIS_1_LENGTH * cos(radians(*angleAxis1 / 10.0) + SERVO_1_OFFS);
	vecA[Y] = AXIS_1_LENGTH * sin(radians(*angleAxis1 / 10.0) + SERVO_1_OFFS);

	alpha1 =           radians(*angleAxis1 / 10.0) + SERVO_1_OFFS;
	beta   = 2 * PI - (radians(*angleAxis2 / 10.0) + SERVO_2_OFFS);
	beta1  = 2 * PI - (beta - (alpha1 - PI));

	vecB[X] = AXIS_2_LENGTH * cos(beta1);
	vecB[Y] = AXIS_2_LENGTH * sin(beta1);

	vecR[X] = vecA[X] + vecB[X];
	vecR[Y] = vecA[Y] + vecB[Y]; // bis hier hin funktioniert sie :D


	// Was wenn die Funktion zum umrechnen der actTargetPos o. newTargetPos aufgerufen wurde? Hier fehlt noch ein Mechanismus oder Parameter
	// evtl über __func__ ?!?!
	tmp = ConvertCoordinates(CONVERT_COORDINATE_TO_FIELD, &vecR[X], &vecR[Y]);
#ifdef _DEBUG
	Serial.println("CalcPosition()");
	Serial.print("alpha1: ");
	Serial.println(alpha1 * 180 / PI, DEC);
	Serial.print("beta: ");
	Serial.println(beta * 180 / PI, DEC);
	Serial.print("beta1: ");
	Serial.println(beta1 * 180 / PI, DEC);
	Serial.print("anlgeAxis1: ");
	Serial.println(*angleAxis1 /10, DEC);
	Serial.print("vecA[X]: ");
	Serial.println(vecA[X], DEC);
	Serial.print("vecA[Y]: ");
	Serial.println(vecA[Y], DEC);
	Serial.print("vecB[X]: ");
	Serial.println(vecB[X], DEC);
	Serial.print("vecB[Y]: ");
	Serial.println(vecB[Y], DEC);
	Serial.print("vecR[X]: ");
	Serial.println(vecR[X], DEC);
	Serial.print("vecR[Y]: ");
	Serial.println(vecR[Y], DEC);
#endif 
	return SetNewPositions(&tmp[X], &tmp[Y]);
}

int8_t UpdateZPos(void)
{
	int16_t val = round((Z_AXIS_RESOLUTION / Z_AXIS_GRADIENT) * GetObjData(OBJ_IDX_Z_POS_COUNT) * 10);

	if (SetObjData(OBJ_IDX_Z_ACTUAL_POS, val, true) == 0)
	{
		return 0;
	}
	else
	{
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR, false);
#endif 
		return -1;
	}
}