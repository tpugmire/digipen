/******************************************************************************/
/*!
\file		C_ParallelSequencer.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Parallel sequencer node logic.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* protected methods */

/*--------------------------------------------------------------------------*
Name:           OnInitial

Description:    Only run when initializng the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void C_ParallelSequencer::OnInitial(NodeData *nodedata_ptr)
{
	ParallelNode::OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_ParallelSequencer::OnEnter(NodeData *nodedata_ptr)
{
	return ParallelNode::OnEnter(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void C_ParallelSequencer::OnExit(NodeData *nodedata_ptr)
{
	ParallelNode::OnExit(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_ParallelSequencer::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);

	// return SUCCESS when all children return SUCCESS
	// if any of children returns FAILURE, return FAILURE

	bool allsucceed = true;

	for (unsigned i = 0; i < m_children.size(); ++i)
	{
		Status childstatus = nodedata_ptr->GetChildStatus(i);

		if (childstatus == Status::BT_FAILURE)
			return Status::BT_FAILURE;
		if (childstatus != Status::BT_SUCCESS)
		{
			allsucceed = false;

			break;
		}
	}

	if (allsucceed)
		return Status::BT_SUCCESS;
	else
		return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_ParallelSequencer::OnSuspend(NodeData *nodedata_ptr)
{
	return ParallelNode::OnSuspend(nodedata_ptr);
}