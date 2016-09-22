/******************************************************************************/
/*!
\file		BehaviorNode.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree node logic base class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           GetTreeLogic

Description:    Get behavior tree logic.

Arguments:      None.

Returns:        TreeLogic&:		behavior tree logic of this node.
*---------------------------------------------------------------------------*/
TreeLogic &BehaviorNode::GetTreeLogic(void)
{
	return g_trees.GetTreeLogic(m_treeindex);
}

TreeLogic const &BehaviorNode::GetTreeLogic(void) const
{
	return g_trees.GetTreeLogic(m_treeindex);
}

/*--------------------------------------------------------------------------*
Name:           GetChildNodeIndex

Description:    Get child node index.

Arguments:      child_order:	child order of m_children.

Returns:        int:			child node index.
*---------------------------------------------------------------------------*/
int BehaviorNode::GetChildNodeIndex(int child_order)
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		child_order >= 0 && child_order < static_cast<int>(m_children.size()),
		"Child index out of bounds\nchild index: %d\nchildren size: %d",
		child_order, m_children.size());
#endif

	return m_children[child_order];
}

int BehaviorNode::GetChildNodeIndex(int child_order) const
{
#ifdef _DEBUG
	MY_ASSERT_WITH_VALUES(
		child_order >= 0 && child_order < static_cast<int>(m_children.size()),
		"Child index out of bounds\nchild index: %d\nchildren size: %d",
		child_order, m_children.size());
#endif

	return m_children[child_order];
}

/*--------------------------------------------------------------------------*
Name:           GetChild

Description:    Get child node.

Arguments:      child_order:	child order of m_children.

Returns:        BehaviorNode&:	child node.
*---------------------------------------------------------------------------*/
BehaviorNode &BehaviorNode::GetChild(int child_order)
{
	return GetTreeLogic()[GetChildNodeIndex(child_order)];
}

BehaviorNode const &BehaviorNode::GetChild(int child_order) const
{
	return GetTreeLogic()[GetChildNodeIndex(child_order)];
}

/*--------------------------------------------------------------------------*
Name:           GetParent

Description:    Get parent node.

Arguments:      None.

Returns:        BehaviorNode&:	parent node.
*---------------------------------------------------------------------------*/
BehaviorNode &BehaviorNode::GetParent(void)
{
	return GetTreeLogic()[m_parentindex];
}

BehaviorNode const &BehaviorNode::GetParent(void) const
{
	return GetTreeLogic()[m_parentindex];
}

/*--------------------------------------------------------------------------*
Name:           Initialize

Description:    Initial node.

Arguments:      nodedata_ptr:	node data pointer.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorNode::Initialize(NodeData *nodedata_ptr)
{
	OnInitial(nodedata_ptr);
}

/*--------------------------------------------------------------------------*
Name:           Tick

Description:    Run every tick.

Arguments:      dt:				delta time.
				nodedata_ptr:	node data pointer.

Returns:        Status:			node status.
*---------------------------------------------------------------------------*/
Status BehaviorNode::Tick(float dt, NodeData *nodedata_ptr)
{
#if defined (_DEBUG) && defined (PRINT_BEHAVIORTREE)
	PrintStatus(*nodedata_ptr);
#endif

	switch (nodedata_ptr->GetStatus())
	{
	case Status::BT_READY:
	{
		return OnEnter(nodedata_ptr);
	}
		break;

	case Status::BT_RUNNING:
	{
		return OnUpdate(dt, nodedata_ptr);
	}
		break;

	case Status::BT_SUCCESS:
	case Status::BT_FAILURE:
	{
		OnExit(nodedata_ptr);

		return Status::BT_READY;
	}
		break;

	default:
	case Status::BT_SUSPEND:
	{
		return OnSuspend(nodedata_ptr);
	}
	}
}

#ifdef _DEBUG
/*--------------------------------------------------------------------------*
Name:           Visit

Description:    Visitor method (depth-first).

Arguments:      depth:	node depth.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorNode::Visit(unsigned depth)
{
	m_depth = depth;

	for (unsigned i = 0; i < depth; ++i)
		printf("\t");

	printf("Name: %s\n", m_name.c_str());

	for (unsigned i = 0; i < m_children.size(); ++i)
	{
		BehaviorNode &child = GetChild(i);

		child.Visit(depth + 1);
	}
}

/*--------------------------------------------------------------------------*
Name:           PrintStatus

Description:    Print node status.

Arguments:      nodedata:	node data.

Returns:        None.
*---------------------------------------------------------------------------*/
void BehaviorNode::PrintStatus(NodeData &nodedata)
{
	PrintNodeStatus(this, nodedata.GetStatus());
}
#endif