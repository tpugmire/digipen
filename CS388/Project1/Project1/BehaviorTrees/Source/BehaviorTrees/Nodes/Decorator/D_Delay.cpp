/******************************************************************************/
/*!
\file		D_Delay.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Suspend Child For 1 To 2 Seconds.

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

Returns:        D_DelayData*:	custom node data pointer.
*---------------------------------------------------------------------------*/
D_DelayData *D_Delay::GetCustomData(NodeData *nodedata_ptr)
{
	return nodedata_ptr->GetCustomData<D_DelayData>();
}

/*--------------------------------------------------------------------------*
Name:           InitialCustomData

Description:    Initial custom data.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void D_Delay::InitialCustomData(NodeData *nodedata_ptr)
{
	nodedata_ptr->InitialCustomData<D_DelayData>();
}

/*--------------------------------------------------------------------------*
Name:           SetRandomTimer

Description:    Set random time on timer.

Arguments:      nodedata_ptr:	current node data pointer.
				min:			min number for timer.
				max:			max number for timer.

Returns:        None.
*---------------------------------------------------------------------------*/
void D_Delay::SetRandomTimer(NodeData *nodedata_ptr, float min, float max)
{
	D_DelayData *customdata = GetCustomData(nodedata_ptr);

	customdata->m_timer = g_random.RangeFloat(1.0f, 2.0f);
}

/*--------------------------------------------------------------------------*
Name:           IsTimeUp

Description:    If time is up.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        true:	time is up.
				false:	time is not up.
*---------------------------------------------------------------------------*/
bool D_Delay::IsTimeUp(float dt, NodeData *nodedata_ptr)
{
	D_DelayData *customdata = GetCustomData(nodedata_ptr);

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
void D_Delay::OnInitial(NodeData *nodedata_ptr)
{
	InterrupterNode::OnInitial(nodedata_ptr);

	InitialCustomData(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status D_Delay::OnEnter(NodeData *nodedata_ptr)
{
	// set timer

	SetRandomTimer(nodedata_ptr);

	// run child

	InterrupterNode::OnEnter(nodedata_ptr);
	
	// tiny idle

	AgentBTData &data = nodedata_ptr->GetAgentData();
	GameObject *self = data.GetGameObject();
	if (self)
	{
		self->SetSpeedStatus(TinySpeedStatus::TS_IDLE);
		SetTinySpeed(self);
	}

	// suspend child

	nodedata_ptr->SetChildStatus(0, Status::BT_SUSPEND);

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void D_Delay::OnExit(NodeData *nodedata_ptr)
{
	//InterrupterNode::OnExit(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status D_Delay::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	Status child_status = nodedata_ptr->GetChildStatus(0);

	if (child_status == Status::BT_SUSPEND)
	{
		if (IsTimeUp(dt, nodedata_ptr))
		{
			// resume child

			nodedata_ptr->SetChildStatus(0, Status::BT_READY);

			return Status::BT_SUSPEND;
		}

		return Status::BT_RUNNING;
	}
	else
		return child_status;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status D_Delay::OnSuspend(NodeData *nodedata_ptr)
{
	return InterrupterNode::OnSuspend(nodedata_ptr);
}