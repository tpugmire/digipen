/******************************************************************************/
/*!
\file		NodeData.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree node data.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           NodeData

Description:    Constructor.

Arguments:      btnode:		behavior node logic.

Returns:        void.
*---------------------------------------------------------------------------*/
NodeData::NodeData(BehaviorNode &btnode)
	:
	m_btnode_ptr(&btnode),
	m_agentdata_ptr(nullptr),
	m_status(Status::BT_READY),
	m_currentchildorder(0),
	m_stackindex(0),
	m_stayStack(false)
{
	// get all children status to BT_READY
	for (unsigned i = 0; i < btnode.GetChildren().size(); ++i)
		m_childstatus.push_back(Status::BT_READY);
}

/*--------------------------------------------------------------------------*
Name:           NodeData

Description:    Move constructor.

Arguments:      nodedata:		node data to be moved.

Returns:        None.
*---------------------------------------------------------------------------*/
NodeData::NodeData(NodeData &&nodedata)
	: m_btnode_ptr(nodedata.m_btnode_ptr),
	m_agentdata_ptr(nodedata.m_agentdata_ptr),
	m_status(nodedata.m_status),
	m_currentchildorder(nodedata.m_currentchildorder),
	m_stackindex(nodedata.m_stackindex),
	m_stayStack(nodedata.m_stayStack),
	m_childstatus(nodedata.m_childstatus),
	m_customdata(std::move(nodedata.m_customdata))
{
}

/*--------------------------------------------------------------------------*
Name:           GetParentNodeData

Description:    Get the node data of parent node.

Arguments:      None.

Returns:        NodeData&:	node data of parent node.
*---------------------------------------------------------------------------*/
NodeData &NodeData::GetParentNodeData(void)
{
	return m_agentdata_ptr->GetNodeData(m_btnode_ptr->GetParentIndex());
}

/*--------------------------------------------------------------------------*
Name:           SetChildStatus

Description:    Set child status.
				(set m_childstatus of current node, also update child's m_status)

Arguments:      childorder:		node order of the child.
				status:			status to set.

Returns:        None.
*---------------------------------------------------------------------------*/
void NodeData::SetChildStatus(int childorder, Status status)
{
	// update m_status variable from child node data
	AgentBTData &agentbtdata = GetAgentData();
	int child_index = GetNodeLogic().GetChildNodeIndex(childorder);
	NodeData &child_data = agentbtdata.GetNodeData(child_index);

	child_data.SetStatus(status);

	// update m_childstatus

	m_childstatus[childorder] = status;
}

/*--------------------------------------------------------------------------*
Name:           operator=

Description:    Move operator.

Arguments:      nodedata:		node data.

Returns:        NodeData&:		reference of the node data.
*---------------------------------------------------------------------------*/
NodeData &NodeData::operator=(NodeData &&nodedata)
{
	m_btnode_ptr = nodedata.m_btnode_ptr;
	m_agentdata_ptr = nodedata.m_agentdata_ptr;
	m_status = nodedata.m_status;
	m_currentchildorder = nodedata.m_currentchildorder;
	m_stackindex = nodedata.m_stackindex;
	m_stayStack = nodedata.m_stayStack;
	m_childstatus = nodedata.m_childstatus;
	m_customdata = std::move(nodedata.m_customdata);

	return *this;
}

/*--------------------------------------------------------------------------*
Name:           IncrementCurrentChildOrder

Description:    Increment current child order.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void NodeData::IncrementCurrentChildOrder(void)
{
	int children_size = static_cast<int>(m_childstatus.size());

	if (children_size > 0)
		++m_currentchildorder %= children_size;
}

/*--------------------------------------------------------------------------*
Name:           Reset

Description:    Reset node data.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void NodeData::Reset(void) 
{ 
	m_status = Status::BT_READY;
	m_currentchildorder = 0;

	for (unsigned i = 0; i < m_childstatus.size(); ++i)
		m_childstatus[i] = Status::BT_READY;
}

/*--------------------------------------------------------------------------*
Name:           RunLogic

Description:    Run node logic.

Arguments:      dt:			delta time.

Returns:        Status:		node status.
*---------------------------------------------------------------------------*/
Status NodeData::RunLogic(float dt) 
{ 
	return m_btnode_ptr->Tick(dt, this); 
}

/*--------------------------------------------------------------------------*
Name:           HasParent

Description:    Check if this node has parent.

Arguments:      None.

Returns:        true:		current node has parent.
				false:		current node has no parent (it is root).
*---------------------------------------------------------------------------*/
bool NodeData::HasParent(void)
{
	if (m_btnode_ptr->GetParentIndex() >= 0)
		return true;

	return false;
}