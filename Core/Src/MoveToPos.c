/*
 * MoveToPos.c
 *
 *  Created on: Sep 21, 2024
 *      Author: Wis
 */

#include "MoveToPos.h"

CNC_pos_t CNC_pos;
Inv_CNC_t CNC;

void initializeCNC_pos(CNC_pos_t *cnc)
{
	//switch
	cnc->Lsw1 = 0;
	cnc->Lsw2 = 0;
	cnc->Lsw3 = 0;
	cnc->Lsw4 = 0;
	cnc->Lsw5 = 0;
	cnc->Lsw6 = 0;
	//Set value acceleration - deceleration - speed
	cnc->accel1 = 0;
	cnc->accel2 = 0;
	cnc->accel3 = 0;
	cnc->jerk1 = 0;
	cnc->jerk2 = 0;
	cnc->jerk3 = 0;
	cnc->max_speedXY = 1000;
	cnc->max_speedZ = 1000;
	cnc->a_maxX = 1000;
	cnc->j_maxX = 1000;
	cnc->a_maxY = 1000;
	cnc->j_maxY = 1000;
	cnc->a_maxZ = 1000;
	cnc->j_maxZ = 1000;
}

void trans_to_posXY(float x,float y)
{
	CNC.set_posX = x - CNC.pos_x;
	CNC.set_posY = y - CNC.pos_y;
}

void trans_to_posZ(float z)
{
	CNC.set_posZ = z - CNC.pos_z;
}

signed int caculate_pos(float pos, float pwm)
{
	return pos*pwm;
}
