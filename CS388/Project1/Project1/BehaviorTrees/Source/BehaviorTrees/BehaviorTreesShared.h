/******************************************************************************/
/*!
\file		BehaviorTreesShared.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior trees global definition data.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

namespace BT
{

// uncomment the following to display tree execution debug info on console
//#define PRINT_BEHAVIORTREE

// display node name on debug info
#define DRAWNODENAME	nodedata_ptr->GetAgentData().GetAgentAbstractData()->AddToRunningNodesString(m_name);

#define g_trees			Singleton<BT::BehaviorTrees>::GetInstance()
#define g_trees_ptr		(&Singleton<BT::BehaviorTrees>::GetInstance())

	// How many stacks to be initialized at once from ExecutionList if more stacks are needed
	static const unsigned STACK_INCREASE_NUM = 4;

	// Node status
	enum Status
	{
		BT_READY,		// ready to run
		BT_SUCCESS,		// success
		BT_FAILURE,		// failure
		BT_RUNNING,		// running
		BT_SUSPEND,		// stop running the node temporarily
	};

	struct AgentAbstractData;
	struct NodeAbstractData;

	class BehaviorNode;
	class ControlFlowNode;
	class CompositeNode;
	class ParallelNode;
	class DecoratorNode;
	class InterrupterNode;
	class RepeaterNode;
	class LeafNode;

	class NodeData;
	class TreeLogic;
	class AgentBTData;

	class AIReasoner;
	class BehaviorTrees;

	typedef std::stack<int> NodeStack;											// stack of behavior tree node data
	typedef std::vector<NodeStack> ExecutionList;								// array of stacks of behavior tree node data
	typedef std::vector<std::unique_ptr<BT::AgentBTData>> AgentBTDataList;		// array of agent behavior tree data

	// Add nodes to enum
	typedef enum
	{
#define REGISTER_CONTROLFLOW(Name, Summary)	Name,
#define REGISTER_DECORATOR(Name, Summary)	Name,
#define REGISTER_LEAF(Name, Summary)		Name,
#include "../../BTResources/Nodes.def"
		NODES_NUM,
#undef REGISTER_CONTROLFLOW
#undef REGISTER_DECORATOR
#undef REGISTER_LEAF
	} NodeEnum;

	// Add nodes to strings
	static const char* NodeNames[] =
	{
#define REGISTER_CONTROLFLOW(Name, Summary)	#Name,
#define REGISTER_DECORATOR(Name, Summary)	#Name,
#define REGISTER_LEAF(Name, Summary)		#Name,
#include "../../BTResources/Nodes.def"
		"Invalid",
#undef REGISTER_CONTROLFLOW
#undef REGISTER_DECORATOR
#undef REGISTER_LEAF
	};

	// Helper functions declarations

	// Find item pointer from a hashmap using key, return nullptr if no item in hashmap.
	template <typename T1, typename T2>
	T2 *HashmapFind(std::unordered_map<T1, T2> &hashmap, const T1 &key);
	template <typename T1, typename T2>
	T2 const *HashmapFind(const std::unordered_map<T1, T2> &hashmap, const T1 &key);

	// Get class name.
	template <typename T>
	std::string GetMyClassName(T *obj);

	// Print out node status info.
	template <typename T>
	void PrintNodeStatus(T *node, Status status);

	/* Template Functions */

	/*--------------------------------------------------------------------------*
	Name:           HashmapFind

	Description:    Find item from a hashmap using key, return nullptr if no item in hashmap.

	Arguments:      hashmap:	hashmap that may contain item.
					key:		hashkey used to find item.

	Returns:        T2*:		pointer of the item, nullptr if item can't be found.
	*---------------------------------------------------------------------------*/
	template <typename T1, typename T2>
	T2 *HashmapFind(std::unordered_map<T1, T2> &hashmap, const T1 &key)
	{
		std::unordered_map<T1, T2>::iterator it = hashmap.find(key);

		if (it == hashmap.end())
			return nullptr;

		return &(it->second);
	}

	template <typename T1, typename T2>
	T2 const *HashmapFind(const std::unordered_map<T1, T2> &hashmap, const T1 &key)
	{
		std::unordered_map<T1, T2>::const_iterator it = hashmap.find(key);

		if (it == hashmap.end())
			return nullptr;

		return &(it->second);
	}

	/*--------------------------------------------------------------------------*
	Name:           GetMyClassName

	Description:    Get class name from object.

	Arguments:      obj:		pointer of the object.

	Returns:        string:		name of the class.
	*---------------------------------------------------------------------------*/
	template <typename T>
	std::string GetMyClassName(T *obj)
	{
		std::string classname = typeid(*obj).name();
		size_t found = classname.find_last_of(':') + 1;

		return classname.substr(found, classname.size() - found);
	}

	/*--------------------------------------------------------------------------*
	Name:           PrintNodeStatus

	Description:    Print out node status info.

	Arguments:      node:		pointer of the node.

	Returns:        status:		node status.
	*---------------------------------------------------------------------------*/
	template <typename T>
	void PrintNodeStatus(T *node, Status status)
	{
		for (unsigned i = 0; i < node->GetDepth(); ++i)
			printf("\t");

		printf("%s(%s): ", GetMyClassName(node).c_str(), node->GetName().c_str());

		switch (status)
		{
		case Status::BT_READY:
			printf("Enter\n");
			break;

		case Status::BT_RUNNING:
			printf("Running\n");
			break;

		case Status::BT_SUCCESS:
			printf("Exit (Success)\n");
			break;

		case Status::BT_FAILURE:
			printf("Exit (Failure)\n");
			break;

		default:
		case Status::BT_SUSPEND:
			printf("Suspend\n");
			break;

		}
	}
}