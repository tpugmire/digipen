/******************************************************************************/
/*!
\file		ISystem.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	System interface.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

// public functions

/*--------------------------------------------------------------------------*
Name:           Initialize

Description:    Initialize the system.

Arguments:      None.

Returns:        true:		finish initialization.
				false:		error during initialization.
*---------------------------------------------------------------------------*/
bool ISystem::Initialize(void)
{
	return true;
}

/*--------------------------------------------------------------------------*
Name:           SetName

Description:    Set system name to the name of the class.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void ISystem::SetName(void)
{
	// if the type name is "<NAMESPACE>::<CLASS>" or "<SOMETHING> <CLASS>"
	// only keep <CLASS> part

	std::string classname = typeid(*this).name();
	size_t found_colon = classname.find_last_of(':') + 1;

	if (found_colon > 0)
		m_name = classname.substr(found_colon, classname.size() - found_colon);
	else
	{
		size_t found_space = classname.find_last_of(' ') + 1;

		if (found_space > 0)
			m_name = classname.substr(found_space, classname.size() - found_space);
	}
}
