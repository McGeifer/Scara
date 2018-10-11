// 
// 
// 

#include "objdir.h"
#include "handler.h"

// Initial definition of the object dictionary entries.
static ObjStruct objStruct_data[] =
{
	{OBJ_IDX_ACK,				OBJ_PROP_RW, 0, &Hdl_ACK},
	{OBJ_IDX_START_MOVE,		OBJ_PROP_RW, 0, &Hdl_StartMove},

	{OBJ_IDX_X_NEW_TARGET,		OBJ_PROP__W, 0, &SetNewTargetPos},
	{OBJ_IDX_X_ACT_TARGET,		OBJ_PROP_R_, 0, &GetActualTargetPos},
	{OBJ_IDX_X_ACT,				OBJ_PROP_R_, 0, &GetActualPos},

	{OBJ_IDX_Y_NEW_TARGET,		OBJ_PROP__W, 0, &SetNewTargetPos},
	{OBJ_IDX_Y_ACT_TARGET,		OBJ_PROP_R_, 0, &GetActualTargetPos},
	{OBJ_IDX_Y_ACT,				OBJ_PROP_R_, 0, &GetActualPos},

	{OBJ_IDX_Z_NEW_TARGET,		OBJ_PROP__W, 0, &SetNewTargetPos},
	{OBJ_IDX_Z_ACT_TARGET,		OBJ_PROP_R_, 0, &GetActualTargetPos},
	{OBJ_IDX_Z_ACT,				OBJ_PROP_R_, 0, &GetActualPos},

	{OBJ_IDX_AX1_NEW_TARGET,	OBJ_PROP__W, 0, &SetNewTargetAngle},
	{OBJ_IDX_AX1_ACT_TARGET,	OBJ_PROP_R_, 0, &GetActualTargetAngle},
	{OBJ_IDX_AX1_ACT_,			OBJ_PROP_R_, 0, &GetActualAngle},

	{OBJ_IDX_AX2_NEW_TARGET,	OBJ_PROP__W, 0, &SetNewTargetAngle},
	{OBJ_IDX_AX2_ACT_TARGET,	OBJ_PROP_R_, 0, &GetActualTargetAngle},
	{OBJ_IDX_AX2_ACT_,			OBJ_PROP_R_, 0, &GetActualAngle},
};
 
// Function to search the object dictionary. If a given entry is found a pointer to it will be returned by the function.  
ObjStruct* LocateObjDir(uint8_t index, uint8_t props, uint16_t data) {
	
	ObjStruct *p;
	p = objStruct_data;

	for (int i = 0; i < (sizeof(objStruct_data) / sizeof(objStruct_data[0])); i++, p++)
	{
		if (p->index == index)
			return p;
	}
	return NULL;
}