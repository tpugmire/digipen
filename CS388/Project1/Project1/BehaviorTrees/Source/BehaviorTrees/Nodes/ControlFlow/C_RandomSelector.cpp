/******************************************************************************/
/*!
\file		C_RandomSelector.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Random selector node logic.

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
void C_RandomSelector::OnInitial(NodeData *nodedata_ptr)
{
	CompositeNode::OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnEnter

Description:    Only run when entering the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_RandomSelector::OnEnter(NodeData *nodedata_ptr)
{
	// randomly pick one from children to run

	int childorder = g_random.RangeInt() % m_children.size();
	int childindex = m_children[childorder];

	nodedata_ptr->SetCurrentChildOrder(childorder);
	RunChild(childindex, true, false, nodedata_ptr);

	return CompositeNode::OnEnter(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnExit

Description:    Only run when exiting the node.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void C_RandomSelector::OnExit(NodeData *nodedata_ptr)
{
	CompositeNode::OnExit(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           OnUpdate

Description:    Run every frame.

Arguments:      dt:				delta time.
				nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_RandomSelector::OnUpdate(float dt, NodeData *nodedata_ptr)
{
	UNREFERENCED_PARAMETER(dt);

	// randomly pick one from children, if it returns SUCCESS, return SUCCESS
	// if it returns FAILURE, return FAILURE

	Status child_status =
		nodedata_ptr->GetChildStatus(nodedata_ptr->GetCurrentChildOrder());

	if ((child_status == Status::BT_SUCCESS) || (child_status == Status::BT_FAILURE))
		return child_status;

	return Status::BT_RUNNING;
}

/*--------------------------------------------------------------------------*
Name:           OnSuspend

Description:    Only run when node is in suspended.

Arguments:      nodedata_ptr:	current node data pointer.

Returns:        Status:			return status.
*---------------------------------------------------------------------------*/
Status C_RandomSelector::OnSuspend(NodeData *nodedata_ptr)
{
	return CompositeNode::OnSuspend(nodedata_ptr);
}