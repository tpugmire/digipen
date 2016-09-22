/******************************************************************************/
/*!
\file		L_Idle.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Action: Idle for 1 to 2 seconds.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           GetCustomData

Description:    Get custom data pointer.

Arguments:      None.

Returns:        L_IdleData*:	custom node data pointer.
*---------------------------------------------------------------------------*/
L_IdleData *L_Idle::GetCustomData(NodeData *nodedata_ptr)
{
	return nodedata_ptr->GetCustomData<L_IdleData>();
}

/*--------------------------------------------------------------------------*
Name:           InitialCustomData

Description:    Initial custom data.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_Idle::InitialCustomData(NodeData *nodedata_ptr)
{
	nodedata_ptr->InitialCustomData<L_IdleData>();
}

/*--------------------------------------------------------------------------*
Name:           SetRandomTimer

Description:    Set random time on timer.

Arguments:      nodedata_ptr:	current node data pointer.
				min:			min number for timer.
				max:			max number for timer.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_Idle::SetRandomTimer(NodeData *nodedata_ptr, float min, float max)
{
	L_IdleData *customdata = GetCustomData(nodedata_ptr);

	customdata->m_timer = g_random.RangeFloat(1.0f, 2.0f);
}

/*--------------------------------------------------------------------------*
Name:           IsTimeUp

Description:    If time is up.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        True:	time is up.
				False:	time is not up.
*---------------------------------------------------------------------------*/
bool L_Idle::IsTimeUp(float dt, NodeData *nodedata_ptr)
{
	L_IdleData *customdata = GetCustomData(nodedata_ptr);

	if (customdata->m_timer < 0.0f)
		return true;

	customdata->m_timer -= dt;

	return false;
}

/* protected methods */

/*--------------------------------------------------------------------------*
Name:           OnInitial

Description:    Only run when initializng the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_Idle::OnInitial(NodeData *nodedata_ptr)
{
	LeafNode::OnInitial(nodedata_ptr);

	InitialCustomData(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_Idle::OnEnter(NodeData *nodedata_ptr)
{
	LeafNode::OnEnter(nodedata_ptr);

	GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();

	self->SetSpeedStatus(TinySpeedStatus::TS_IDLE);
	SetTinySpeed(self);

	SetRandomTimer(nodedata_ptr);

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void L_Idle::OnExit(NodeData *nodedata_ptr)
{
	LeafNode::OnExit(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_Idle::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	LeafNode::OnUpdate(dt, nodedata_ptr);

	if (IsTimeUp(dt, nodedata_ptr))
		return Status::BT_SUCCESS;

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status L_Idle::OnSuspend(NodeData *nodedata_ptr)
{
	return LeafNode::OnSuspend(nodedata_ptr);
}