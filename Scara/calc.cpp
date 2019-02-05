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
		if (tmp >= GetObjData(AXIS_1_ANGLE_MIN) && *val <= GetObjData(AXIS_1_ANGLE_MAX))
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
		if (tmp >= GetObjData(AXIS_2_ANGLE_MIN) && *val <= GetObjData(AXIS_2_ANGLE_MAX))
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

/* Convert and store the given angles in rad for servo1 & servo2 in the objDir.
   return 0  - operation successful
   return -1 - operation failed */
uint8_t SetNewAngles(float *servo1, float *servo2)
{
	int16_t actAngle1 = GetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE);
		
	if (SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, round((degrees(*servo1)) * 10.0)) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, round((degrees(*servo2)) * 10.0)) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, actAngle1); /* try to set the old value of axis 1 to prevent inconsistent angle values */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

/* Convert and store the actual positions for x & y in the objDir.
   return 0  - operation successful
   return -1 - operation failed */
uint8_t SetNewPositions(float *xPos, float *yPos)
{
	int16_t actPosX = GetObjData(OBJ_IDX_X_ACTUAL_POS);

	if (SetObjData(OBJ_IDX_X_ACTUAL_POS, round(*xPos * 10.0)) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_ACTUAL_POS, round(*yPos * 10.0)) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_ACTUAL_POS, actPosX); /* try to set the old value of x to prevent inconsistent position values */
			return -1;
		}
	}
	else
	{
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

int8_t CalcAngle(int16_t *xPos, int16_t *yPos)
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
	float servo1A = 0;			// angle for servo motor 1
	float servo1B = 0;	
	float servo2A = 0;			// 
	float servo2B = 0;			// 

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
	alpha = AXIS_1_LENGTH + AXIS_2_LENGTH == b ? 0 : acos((b * b + AXIS_2_LENGTH * AXIS_2_LENGTH - AXIS_1_LENGTH * AXIS_1_LENGTH) / (2.0 * b * AXIS_2_LENGTH)); /* if both arms are in a straight line don't use law of cosine - !!!!! test with very small numbers !!!!! */
	//alpha1a = 2 * PI - alpha - alpha2; /* solution a */
	//alpha1b = 2 * PI - alpha2; /* solution b */

	beta = acos((AXIS_1_LENGTH * AXIS_1_LENGTH + AXIS_2_LENGTH * AXIS_2_LENGTH - b * b) / (2.0 * AXIS_1_LENGTH * AXIS_2_LENGTH));
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
	servo1A = 2 * PI - SERVO_1_OFFS - alpha - alpha2; /* point B is "left" from b */
	servo1B = 2 * PI - SERVO_1_OFFS - alpha2;			/* point B is "right" from b */
	servo2A = 2 * PI - beta - SERVO_2_OFFS;			/* point B is "left" from b */
	servo2B = beta - SERVO_2_OFFS;					/* point B is "right" from b */

	/* check which solutions are inside the allowed range of the dynamixel servos to reach the given position */
	cmp1A = ChkServoLmt(1, &servo1A);
	cmp1B = ChkServoLmt(1, &servo1B);
	cmp2A = ChkServoLmt(2, &servo2A);
	cmp2B = ChkServoLmt(2, &servo2B);

	if (cmp1A == 1)
	{
		if (cmp1B == 1) /* both solutions for servo1 are allowed */
		{
			/* check which solution can also be reached by servo2 */

			if (cmp2A == 1 && cmp2B == 1) /* both solutions for servo2 are allowed */
			{
				/* decition is based on the shortest way for servo2 */

				float actAngle = radians(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE) / 10);
				float diffA = actAngle < servo2A ? servo2A - actAngle : actAngle - servo2A;
				float diffB = actAngle < servo2B ? servo2B - actAngle : actAngle - servo2B;

				if (diffA < diffB)
				{
					return SetNewAngles(&servo1A, &servo2A) == 0 ? 0 : -1;
				}
				else
				{
					return SetNewAngles(&servo1B, &servo2B) == 0 ? 0 : -1;
				}
			}
			else if (cmp2A == 1) /* only solution A for servo2 is allowed */
			{
				return SetNewAngles(&servo1A, &servo2A) == 0 ? 0 : -1; /* A oder B nutzen ??? */
			}
			else if (cmp2B == 1) /* only solution B for servo2 is allowed */
			{
				return SetNewAngles(&servo1B, &servo2B) == 0 ? 0 : -1; /* A oder B nutzen ??? */
			}
			else
			{
				return -1;
			}
		}
		else if (cmp1B == 0) /* only solution A for servo1 is allowed */
		{
			if (cmp2A == 1)
			{
				return SetNewAngles(&servo1A, &servo2A) == 0 ? 0 : -1;
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
	else if (cmp1A == 0)
	{
		if (cmp1B == 1) /* only solution B for servo1 is allowed */
		{
			if (cmp2B == 1)
			{
				return SetNewAngles(&servo1B, &servo2B) == 0 ? 0 : -1;
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

int8_t CalcPosistion(int16_t *angleAxis1, int16_t *angleAxis2)
{
	float vecA[2] = { NULL };
	float vecB[2] = { NULL };
	float vecR[2] = { NULL };

	vecA[X] = AXIS_1_LENGTH * (float)cos(radians(*angleAxis1 / 10.0) + SERVO_1_OFFS);
	vecA[Y] = AXIS_1_LENGTH * (float)sin(radians(*angleAxis1 / 10.0) + SERVO_1_OFFS);
	vecB[X] = AXIS_2_LENGTH * (float)cos(radians(*angleAxis2 / 10.0) + SERVO_2_OFFS);
	vecB[Y] = AXIS_2_LENGTH * (float)sin(radians(*angleAxis2 / 10.0) + SERVO_2_OFFS);

	vecR[X] = vecA[X] + vecB[X];
	vecR[Y] = vecA[Y] + vecB[Y];

	return SetNewPositions(&vecR[X], &vecR[Y]) ? 0 : -1;
}

int8_t UpdateZPos(void)
{
	int16_t val = round((Z_AXIS_RESOLUTION / Z_AXIS_GRADIENT) * GetObjData(OBJ_IDX_Z_POS_COUNT) * 10);

	if (SetObjData(OBJ_IDX_Z_ACTUAL_POS, val) == 0)
	{
		return 0;
	}
	else
	{
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR);
#endif 
		return -1;
	}
}