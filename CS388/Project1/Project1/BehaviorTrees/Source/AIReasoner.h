/******************************************************************************/
/*!
\file		AIReasoner.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	AI reasoner base class.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

namespace BT
{
	class AIReasoner : public ISystem
	{
	public:
		// Register the agent
		virtual bool Register(const std::string &agentname, const std::string &modelname) = 0;
		virtual bool Register(const std::string &agentname) = 0;
		// Unregister the agent
		virtual bool Unregister(const std::string &agentname) = 0;

		// Initialize model (behavior tree) for agent
		virtual void InitializeAgent(const std::string &agentname, const std::string &modelname) = 0;

		// Initilize AI reasoner class
		virtual bool Initialize(void)			{ return ISystem::Initialize(); }

		// Execute each agent's reasoner
		virtual void Update(float dt) = 0;

		// Shutdown reasoner
		virtual void Shutdown(void) = 0;
	};
}