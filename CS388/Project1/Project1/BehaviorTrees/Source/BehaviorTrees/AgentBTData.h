/******************************************************************************/
/*!
\file		AgentBTData.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree data for each agent.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// each agent has one, where it stores data relates to tree 
	// (so tree itself can be static)
	class AgentBTData
	{
	public:
		/* constructors/destructor */

		// Constructor
		AgentBTData(const std::string &agentname);
		// Move constructor (class with unique_ptr members)
		AgentBTData(AgentBTData &&agentdata);

		/* getters/setters */

		// Get name of the agent
		std::string const &GetName(void) const		{ return m_name; }

		//Get behavior node data from behavior tree node index
		NodeData &GetNodeData(int node_index);
		NodeData const &GetNodeData(int node_index) const;

		// Get user defined data
		template <typename T>
		T *GetCustomData(void);

		// Get user defined data (AgentAbstractData)
		AgentAbstractData *GetAgentAbstractData(void);

		// Get game object pointer (TinyCustomData)
		GameObject *GetGameObject(void);

		// Initial user defined data 
		// (only accept derived struct from AgentAbstractData)
		template <typename T>
		void InitialCustomData(void);

		// Initialize user defined data (AgentAbstractData)
		void InitialAgentAbstractData(void);
		// Initialize user defined data (TinyCustomData)
		void InitialTinyCustomData(GameObject *npc);

		// Set user defined data 
		// (only accept derived struct from AgentAbstractData)
		template <typename T>
		T *SetCustomData(const T& data);

		/* operators */

		// Move operator (class with unique_ptr members)
		AgentBTData &operator=(AgentBTData &&agentdata);

		/* methods */

		// Initialize behavior node data
		void InitialNodeData(const std::string &treename);

		// Enable (Reset execution list)
		void Enable(void);
		// Disable (Clear execution list)
		void Disable(void);

		// Mark the agent for deletion
		void MarkForDeletion(void);
		
		// Add child to execution list and return the stack index of the child
		unsigned AddChildToExecutionList(int node_index, int child_index, bool on_same_stack, bool stay_stack);
		// Remove all children of the node from execution list
		void RemoveChildrenFromExecutionList(int node_index);

		// Send message
		void SendMsg(objectID id, MSG_Name name, MSG_Data& data = MSG_Data());
		void SendMsg(GameObject* object, MSG_Name name, MSG_Data& data = MSG_Data());
		void SendMsgToAll(MSG_Name name, MSG_Data& data = MSG_Data());
		// Receive message and put it into queue
		void ReceiveMsg(const MSG_Object &msg);

		// Execute each node on execution list (also handle messages)
		void Execute(float dt);

	private:
		/* variable */

		std::string m_name;										// agent name
		std::vector<NodeData> m_nodedata;						// behavior tree node data
		ExecutionList m_executionlist;							// execution list for running nodes
		std::unique_ptr<AgentAbstractData> m_customdata;		// user defined data for each agent

		/* methods */

		// Get behavior node data from top of the node stack
		NodeData &GetNodeDataFromNodeStack(unsigned stack_index);

		// Grow more stack from execution list if index is more than current size
		void GrowNodeStack(void);
		// Clear execution list
		void ClearExecutionList(void);
		// Find next empty node stack and return its index
		// (also initialze if needed)
		unsigned FindNextEmptyNodeStack(unsigned start_stackindex);
		// Clear certain node stack from execution list 
		// (also all other stacks associate to it)
		void ClearNodeStack(unsigned stack_index);
		// Push node onto execution list
		void PushNodeToExecutionList(int node_index, unsigned stack_index, bool stay_stack);
	};

	/* Template Functions */

	/*--------------------------------------------------------------------------*
	Name:           GetCustomData

	Description:    Get user defined data.

	Arguments:      None.

	Returns:        T*:		Pointer of user defined data.
	*---------------------------------------------------------------------------*/
	template <typename T>
	T *AgentBTData::GetCustomData(void)
	{
		return static_cast<T *>(m_customdata.get());
	}

	/*--------------------------------------------------------------------------*
	Name:           InitialCustomData

	Description:    Initial user defined data.
					(only accept derived struct from AgentAbstractData)

	Arguments:      None.

	Returns:        None.
	*---------------------------------------------------------------------------*/
	template <typename T>
	void AgentBTData::InitialCustomData(void)
	{
		m_customdata = std::make_unique<T>();
	}

	/*--------------------------------------------------------------------------*
	Name:           SetCustomData

	Description:    Set user defined data.
					(only accept derived struct from AgentAbstractData)

	Arguments:      customdata:		input user defined data.

	Returns:        T*:				Pointer of user defined data.
	*---------------------------------------------------------------------------*/
	template <typename T>
	T *AgentBTData::SetCustomData(const T& customdata)
	{
		if (m_customdata == nullptr)
			m_customdata = std::make_unique<T>();

		m_customdata = customdata;

		return m_customdata.get();
	}
}