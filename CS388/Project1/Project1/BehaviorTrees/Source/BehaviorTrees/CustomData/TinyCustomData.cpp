/******************************************************************************/
/*!
\file		TinyCustomData.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Custom data for tiny.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           TinyCustomData

Description:    Default constructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
TinyCustomData::TinyCustomData()
	: m_npc(nullptr)
{
	m_mouseClick = false;
}

/*--------------------------------------------------------------------------*
Name:           OnMessage

Description:    Handle messages.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void TinyCustomData::OnMessage(void)
{
	// default behavior is to drop all messages

	if (m_msgqueue.size())
	{
		MSG_Object &msg = m_msgqueue.front();
		m_msgqueue.pop();

		switch (msg.GetName())
		{
		case MSG_MouseClick:
		{
			m_mouseClick = true;
			m_mousePos = msg.GetVector3Data();
		}
			break;

		default:
			break;
		}
	}
}