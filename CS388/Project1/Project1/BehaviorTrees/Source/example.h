/* Copyright Steve Rabin, 2013. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#pragma once

#include "statemch.h"


class Example : public StateMachine
{
public:

	Example( GameObject & object )
		: StateMachine( object ) {}
	~Example( void ) {}


private:

	virtual bool States( State_Machine_Event event, MSG_Object * msg, int state, int substate );

	//Put member variables or functions here
	objectID m_curTarget;
	objectID GetFarthestAgent( void );

};
