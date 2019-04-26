/*  */

#include "calc.h"
#include "objdir.h"
#include "status.h"
#include "dynamixel.h"

/* 
	Check if the given angle in rad is inside the allowed range of the dynamixel servo
	return 1  - value is in range
	return 0  - value is not in range
	return -1 - error
 */
static uint8_t ChkServoLmt(uint8_t servo, float *val)
{
	if (val != NULL)
	{
		int16_t tmp = (int16_t)round(*val * RAD_TO_DEG * 10);

#ifdef _DEBUG
		Serial.print("ChkServoLmt() -> tmp: ");
		Serial.println(tmp, DEC);
#endif

		if (servo == 1)
		{
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
			if (tmp >= AXIS_2_ANGLE_MIN && tmp <= AXIS_2_ANGLE_MAX)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			/* error message? */
			return -1;
		}
	}
	else
	{
		
		return -1;
	}
}

/* 
	ConvertCoordinates converts x and y coordinates between the different coordinate systems (machine & field).
	return #    - pointer to array
	return NULL - error
 */
static float* ConvertCoordinates(uint8_t direction, float *xVal, float *yVal)
{
	static float val[2] = { 0 };

	switch (direction)
	{
	case CONVERT_COORDINATE_TO_ROBOT:
		val[DXL_ID_AXIS_1] = *xVal + MACHINE_ZERO_OFFS_X_FIELD - MACHINE_ZERO_OFFS_X_ROBOT;
		val[DXL_ID_AXIS_2] = *yVal + MACHINE_ZERO_OFFS_Y_FIELD - MACHINE_ZERO_OFFS_Y_ROBOT;
#ifdef _DEBUG
		Serial.println("ConvertCoordinates()");
		Serial.print("val[DXL_ID_AXIS_1]: ");
		Serial.println(val[DXL_ID_AXIS_1], 8);
		Serial.print("val[DXL_ID_AXIS_2]: ");
		Serial.println(val[DXL_ID_AXIS_2], 8);
#endif 
		return val;

	case CONVERT_COORDINATE_TO_FIELD:
		val[DXL_ID_AXIS_1] = *xVal + MACHINE_ZERO_OFFS_X_ROBOT - MACHINE_ZERO_OFFS_X_FIELD;
		val[DXL_ID_AXIS_2] = *yVal + MACHINE_ZERO_OFFS_Y_ROBOT - MACHINE_ZERO_OFFS_Y_FIELD;
		return val;

	default:
		return NULL;
	}
}

float* CalcAngle(int16_t *xPos, int16_t *yPos)
{
	float *coordinates = NULL;	/* converted coordinates */
	static float result[2];		/* result of calculation */
	float x_tmp = 0;				/* temp value for conversion */
	float y_tmp = 0;				/* temp value for conversion */
	float x_val = 0;				/*  */
	float y_val = 0;				/*  */

	float b = 0;				/* length between c & a */

	uint8_t cmp_1A = 0;			/* temporary compare values */
	uint8_t cmp_1B = 0;
	uint8_t cmp_2A = 0;
	uint8_t cmp_2B = 0;

	/* all angles in non negative radians! */
	float alpha = 0;			/* inner angle of triangle at point A (between b & c) */
	float alpha_2 = 0;			/* angle between b & x-axis  */
	float beta = 0;				/* inner angle of triangle at point B (between a & c) */
	float gamma = 0;			/* inner angle of triangle at point C (between b & c) */
	static float servo_1A = 0;	/* angle for servo motor 1 */
	static float servo_1B = 0;	/*  */
	static float servo_2A = 0;	/*  */
	static float servo_2B = 0;	/*  */

	/* convert the input position values based on the field coordinate system to the coordinate system of the robot axis */
	x_tmp = *xPos / 10.0;
	y_tmp = *yPos / 10.0;
#ifdef _DEBUG
	Serial.println("CalcAngle()");
	Serial.print("x_tmp: ");
	Serial.println(x_tmp, 8);
	Serial.print("y_tmp: ");
	Serial.println(y_tmp, 8);
#endif 
	coordinates = ConvertCoordinates(CONVERT_COORDINATE_TO_ROBOT, &x_tmp, &y_tmp);
	x_val = x_tmp; /* coordinates[DXL_ID_AXIS_1]; */
	y_val = y_tmp; /* coordinates[DXL_ID_AXIS_2]; */
#ifdef _DEBUG
	Serial.print("x_val: ");
	Serial.println(x_val, 8);
	Serial.print("y_val: ");
	Serial.println(y_val, 8);
#endif 

	/* calculate missing lengths and angles */
	b = (float)sqrt(x_val * x_val + y_val * y_val);
	alpha_2 = atan(y_val / x_val) * (-1);
#ifdef _DEBUG
	Serial.print("b: ");
	Serial.println(b, 8);
	Serial.print("alpha_2: ");
	Serial.println(alpha_2 * 180 / PI, 8);

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

	/* 
		Die berechneten Winkel und die in GeoGebra stimmen nicht überein. Alpha & gamma haben bis zu 2° Abweichung, wobei die Fehlerursache sehr wahrscheinlich
		bei GeoGebra liegt. Dies führt zu Abweichung der X- und Y-Positionen von mehreren Millimetern !!!
	*/
#ifdef _DEBUG
	Serial.print("alpha: ");
	Serial.println(alpha * 180 / PI, 8);
	Serial.print("beta: ");
	Serial.println(beta * 180 / PI, 8);
	Serial.print("gamma: ");
	Serial.println(gamma * 180 / PI, 8);
#endif 

	/* 
		Calculation of the new axis angles considering a minimum travel distance
		between the actual and new position of the robot arm. The servo motors have a
		operating angle of 300° ( 0° - 300°). Due to mechanical requirements this range
		might be reduced so it is necessary to check if servo1 & servo2 are able to
		reach the given coordinates.
	*/

	 /* calculation of the servo angles based on the orientation of the triangle in the coordinate system */
	servo_1A = 2 * PI - SERVO_1_OFFS - (alpha - alpha_2);	/* point B is "left" from b */
	servo_1B = 2 * PI - SERVO_1_OFFS - (alpha_2 - alpha);	/* point B is "right" from b */
	servo_2A = 2 * PI - SERVO_2_OFFS - beta;				/* point B is "left" from b */
	servo_2B = beta - SERVO_2_OFFS;							/* point B is "right" from b */
#ifdef _DEBUG
	Serial.print("servo_1A: ");
	Serial.println(degrees(servo_1A), DEC);
	Serial.print("servo_1B: ");
	Serial.println(degrees(servo_1B), DEC);
	Serial.print("servo_2A: ");
	Serial.println(degrees(servo_2A), DEC);
	Serial.print("servo_2B: ");
	Serial.println(degrees(servo_2B), DEC);
#endif

	/* check which solutions are inside the allowed range of the dynamixel servos to reach the given position */
	cmp_1A = ChkServoLmt(1, &servo_1A);
	cmp_1B = ChkServoLmt(1, &servo_1B);
	cmp_2A = ChkServoLmt(2, &servo_2A);
	cmp_2B = ChkServoLmt(2, &servo_2B);
#ifdef _DEBUG
	Serial.print("cmp_1A: ");
	Serial.println(cmp_1A, DEC);
	Serial.print("cmp_1B: ");
	Serial.println(cmp_1B, DEC);
	Serial.print("cmp_2A: ");
	Serial.println(cmp_2A, DEC);
	Serial.print("cmp_2B: ");
	Serial.println(cmp_2B, DEC);
#endif 

	if (cmp_1A)
	{
		if (cmp_1B) /* both solutions for servo1 are allowed */
		{
			/* check which solution can also be reached by servo2 */

			if (cmp_2A && cmp_2B) /* both solutions for servo2 are allowed */
			{
				/* decision is based on the shortest way for servo2 */
				float actAngle = radians(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE) / 10);
				float diffA = actAngle < servo_2A ? servo_2A - actAngle : actAngle - servo_2A;
				float diffB = actAngle < servo_2B ? servo_2B - actAngle : actAngle - servo_2B;

				if (diffA < diffB)
				{
					result[0] = servo_1A;
					result[1] = servo_2A;
					return result;
				}
				else
				{
					result[0] = servo_1B;
					result[1] = servo_2B;
					return result;
				}
			}
			else if (cmp_2A) /* only solution A for servo2 is allowed */
			{
				result[0] = servo_1A;
				result[1] = servo_2A;
				return result;
			}
			else if (cmp_2B) /* only solution B for servo2 is allowed */
			{
				result[0] = servo_1B;
				result[1] = servo_2B;
				return result;
			}
			else
			{
				return NULL;
			}
		}
		else if (!cmp_1B) /* only solution A for servo1 is allowed */
		{
			if (cmp_2A)
			{
				result[0] = servo_1A;
				result[1] = servo_2A;
				return result;
			}
			else if (cmp_2B) /* stimmt das? war vorher auch cmp_2A  */
			{
				/* error */
				return NULL;
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else if (!cmp_1A)
	{
		if (cmp_1B) /* only solution B for servo1 is allowed */
		{
			if (cmp_2B)
			{
				result[0] = servo_1B;
				result[1] = servo_2B;
				return result;
			}
			else if (!cmp_2B)
			{
				/* error */
				return NULL;
			}
			else
			{
				/* error */
				return NULL;
			}
		}
		else
		{
			/* no solution is allowed */
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

float* CalcPosistion(int16_t *angleAxis1, int16_t *angleAxis2)
{
	float vec_a[2] = { 0 };
	float vec_b[2] = { 0 };
	float vec_r[2] = { 0 };
	float alpha_1 = 0;			/* outer angle between x-axis and c */
	float beta = 0;				/* inner angle between a & c */
	float beta_1 = 0;			/* inner angle between x-axis & a */
	
	enum axis
	{
		X,
		Y,
		Z,
		V_SIZE
	};

	vec_a[X] = AXIS_1_LENGTH * cos(radians(*angleAxis1 / 10.0) + SERVO_1_OFFS);
	vec_a[Y] = AXIS_1_LENGTH * sin(radians(*angleAxis1 / 10.0) + SERVO_1_OFFS);

	alpha_1 =           radians(*angleAxis1 / 10.0) + SERVO_1_OFFS;
	beta    = 2 * PI - (radians(*angleAxis2 / 10.0) + SERVO_2_OFFS);
	beta_1  = 2 * PI - (beta - (alpha_1 - PI));

	vec_b[X] = AXIS_2_LENGTH * cos(beta_1);
	vec_b[Y] = AXIS_2_LENGTH * sin(beta_1);

	vec_r[X] = vec_a[X] + vec_b[X];
	vec_r[Y] = vec_a[Y] + vec_b[Y]; /* bis hier hin funktioniert sie :D */

//#ifdef _DEBUG
//	Serial.println("CalcPosition()");
//	Serial.print("alpha_1: ");
//	Serial.println(alpha_1 * 180 / PI, DEC);
//	Serial.print("beta: ");
//	Serial.println(beta * 180 / PI, DEC);
//	Serial.print("beta_1: ");
//	Serial.println(beta_1 * 180 / PI, DEC);
//	Serial.print("anlgeAxis1: ");
//	Serial.println(*angleAxis1 / 10, DEC);
//	Serial.print("vec_a[X]: ");
//	Serial.println(vec_a[X], DEC);
//	Serial.print("vec_a[Y]: ");
//	Serial.println(vec_a[Y], DEC);
//	Serial.print("vec_b[X]: ");
//	Serial.println(vec_b[X], DEC);
//	Serial.print("vec_b[Y]: ");
//	Serial.println(vec_b[Y], DEC);
//	Serial.print("vec_r[X]: ");
//	Serial.println(vec_r[X], DEC);
//	Serial.print("vec_r[Y]: ");
//	Serial.println(vec_r[Y], DEC);
//#endif

	return ConvertCoordinates(CONVERT_COORDINATE_TO_FIELD, &vec_r[X], &vec_r[Y]);
}

int16_t UpdateZPos(void)
{
	int16_t val = round((Z_RESOLUTION / Z_GRADIENT) * GetObjData(OBJ_IDX_Z_POS_COUNT) * 10.0);

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