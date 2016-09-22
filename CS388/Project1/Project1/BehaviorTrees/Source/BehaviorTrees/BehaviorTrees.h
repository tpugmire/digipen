/******************************************************************************/
/*!
\file		BehaviorTrees.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Behavior tree class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
	// behavior tree class to store all trees (only one instance)
	class BehaviorTrees : public AIReasoner
	{
	public:

		/* constructor */

		BehaviorTrees();

		/* methods */

		// Get agent data
		AgentBTData &GetAgentData(const std::string &agentname);
		AgentBTData const &GetAgentData(const std::string &agentname) const;
		// Get tree logic instance from name
		TreeLogic &GetTreeLogic(const std::string &treename);
		TreeLogic const &GetTreeLogic(const std::string &treename) const;
		// Get tree logic instance from index
		TreeLogic &GetTreeLogic(int treeindex);
		TreeLogic const &GetTreeLogic(int treeindex) const;
		// Get tree root
		BehaviorNode &GetTreeRoot(const std::string &treename);
		BehaviorNode const &GetTreeRoot(const std::string &treename) const;

		// Get all agents
		AgentBTDataList &GetAllAgentsBTData(void)						{ return m_agentdata; }
		const AgentBTDataList &GetAllAgentsBTData(void) const			{ return m_agentdata; }

		// Get all trees
		std::vector<TreeLogic> &GetAllTreeLogics(void)					{ return m_trees; }
		const std::vector<TreeLogic> &GetAllTreeLogics(void) const		{ return m_trees; }

		// Register the agent
		virtual bool Register(const std::string &agentname, const std::string &modelname) override;
		virtual bool Register(const std::string &agentname) override;
		// Unregister the agent
		virtual bool Unregister(const std::string &agentname) override;

		// Initialize model (behavior tree) for agent
		virtual void InitializeAgent(const std::string &agentname, const std::string &modelname) override;

		// Initialize behavior trees class
		virtual bool Initialize(void) override;
		// Execute each agent's behavior tree
		virtual void Update(float dt) override;
		virtual void Shutdown(void) override {}

	private:

		/* variables */

		std::vector<TreeLogic> m_trees;							// tree logic list
		std::unordered_map<std::string, int> m_treeindextable;	// tree logic lookup table (get tree logic ndex from tree name)
		std::vector<std::unique_ptr<AgentBTData>> m_agentdata;	// tree data for each agent
		std::unordered_map<std::string, int> m_agentindextable;	// look up table (get agent data index from agent name)

		/* methods */

		// Load trees from file
		void LoadTrees(void);
		// Create tree logic
		void CreateTreeLogic(const std::string &treename);
	};
}