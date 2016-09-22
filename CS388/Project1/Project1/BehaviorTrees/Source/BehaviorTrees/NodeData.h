/******************************************************************************/
/*!
\file		NodeData.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree node data.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// store actual data for each node per agent
	class NodeData
	{
	public:

		/* constructors/destructor */

		// Constructor
		NodeData(BehaviorNode &btnode);
		// Move constructor
		NodeData(NodeData &&nodedata);

		/* getters/setters */

		BehaviorNode &GetNodeLogic(void)							{ return *m_btnode_ptr; }
		BehaviorNode const &GetNodeLogic(void) const				{ return *m_btnode_ptr; }
		AgentBTData &GetAgentData(void)								{ return *m_agentdata_ptr; }
		AgentBTData const &GetAgentData(void) const					{ return *m_agentdata_ptr; }
		Status GetStatus(void)										{ return m_status; }
		int GetCurrentChildOrder(void)								{ return m_currentchildorder; }
		int GetCurrentChildNodeIndex(void)							{ return m_btnode_ptr->GetChildNodeIndex(m_currentchildorder); }
		unsigned GetStackIndex(void)								{ return m_stackindex; }
		bool GetStayStackFlag(void)									{ return m_stayStack; }
		Status GetChildStatus(int childorder)						{ return m_childstatus[childorder]; }
		std::vector<Status> &GetAllChildStatus(void)				{ return m_childstatus; }
		std::vector<Status> const &GetAllChildStatus(void) const	{ return m_childstatus; }

		NodeData &GetParentNodeData(void);

		// Get custom node data
		template <typename T>
		T *GetCustomData(void);

		void SetAgentData(AgentBTData *agentdata)					{ m_agentdata_ptr = agentdata; }
		void SetStatus(Status status)								{ m_status = status; }
		void SetCurrentChildOrder(int childorder)					{ m_currentchildorder = childorder; }
		void SetStackIndex(unsigned stackindex)						{ m_stackindex = stackindex; }
		void SetStayStackFlag(bool flag)							{ m_stayStack = flag; }

		// Set child status (set m_childstatus of current node, also update child's m_status).
		void SetChildStatus(int childorder, Status status);

		// Initial custom node data 
		// (only accept derived struct from NodeAbstractData)
		template <typename T>
		void InitialCustomData(void);

		/* operators */

		// Move operator
		NodeData &operator=(NodeData &&nodedata);

		/* methods */

		// Increment current child order
		void IncrementCurrentChildOrder(void);

		// Reset node data
		void Reset(void);

		// Run node logic
		Status RunLogic(float dt);

		// Check if this node has parent
		bool HasParent(void);

	private:

		/* variables */

		BehaviorNode *m_btnode_ptr;						// node logic pointer
		AgentBTData *m_agentdata_ptr;					// agent's behavior tree data
		Status m_status;								// node status
		int m_currentchildorder;						// the order of the current child in m_children to execute
		unsigned m_stackindex;							// which stack the node is on
		bool m_stayStack;								// flag: do not pop the node from stack automatically (parallel or root nodes)
		std::vector<Status> m_childstatus;				// children status
		std::shared_ptr<NodeAbstractData> m_customdata;	// custom node data
	};

	/* Template Functions */

	/*--------------------------------------------------------------------------*
	Name:           GetCustomData

	Description:    Get custom node data.

	Arguments:      None.

	Returns:        T*:		Pointer of custom node data.
	*---------------------------------------------------------------------------*/
	template <typename T>
	T *NodeData::GetCustomData(void)
	{
		return static_cast<T *>(m_customdata.get());
	}

	/*--------------------------------------------------------------------------*
	Name:           InitialCustomData

	Description:    Initial custom node data.
					(only accept derived struct from NodeAbstractData)

	Arguments:      None.

	Returns:        void.
	*---------------------------------------------------------------------------*/
	template <typename T>
	void NodeData::InitialCustomData(void)
	{
		m_customdata = std::make_shared<T>();
	}

}