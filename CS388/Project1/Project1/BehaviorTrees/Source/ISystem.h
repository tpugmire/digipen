/******************************************************************************/
/*!
\file		ISystem.h
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	System interface.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

// interface of system
class ISystem
{
public:

	/* getters/setters */

	unsigned GetFlag(void)					{ return m_flag; }
	std::string GetName(void)				{ return m_name; }
	std::string const GetName(void) const	{ return m_name; }

	void SetFlag(unsigned flag)				{ m_flag = flag; }
	// Set system name to the name of the class
	void SetName(void);

	/* methods */

	// Initialize the system
	virtual bool Initialize(void);
	// Update the system
	virtual void Update(float dt) = 0;
	// Close the system
	virtual void Shutdown(void) = 0;

protected:

	/* variables */

	unsigned m_flag;		// flag value (used for index)
	std::string m_name;		// name of the system
};