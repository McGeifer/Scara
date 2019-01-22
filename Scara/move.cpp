// 
// 
// 

#include "move.h"
#include "dynamixel.h"
#include "objdir.h"

int8_t UpdatePos(void) {

	uint8_t id = 0;
	int16_t angle[3] = { 0 };
	uint8_t erroBit = 0;

	for (id = 0; id < 3; id++)	{
		angle[id] = Dynamixel.readPosition(id);

		if (angle[id] < 0) {
			erroBit = angle[id] * (-1); // errorBit is negative by dynamixel library
			DynamixelError(erroBit, id); 
			return -1;
		}
		else {
			
			switch (id)
			{
			case 0:

			default:
				break;
			}
			
			
			
			SetObjStructData(OBJ_IDX)

		}

	}
}

void HandleMove(void) {


}