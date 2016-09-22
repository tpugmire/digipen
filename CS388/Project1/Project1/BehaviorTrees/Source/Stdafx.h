#pragma once

#pragma warning(disable: 4005)
#pragma warning(disable: 4995) 

#include <DXUT/DXUT.h>
#include <DXUT/DXUTcamera.h>
#include <DXUT/DXUTenum.h>
#include <DXUT/DXUTgui.h>
#include <DXUT/DXUTmisc.h>
#include <DXUT/DXUTres.h>
#include <DXUT/DXUTsettingsdlg.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <xinput.h>

#include <io.h>
#include <fcntl.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_map>

#include <global.h>
#include <singleton.h>
#include <MyAssert.h>
#include <Random.h>

#include <MultiAnimation.h>

#include <Clock.h>

// Resource
#include <resource.h>

// Animation
#include <Tiny.h>

// Game Object
#include <body.h>
#include <gameobject.h>
#include <movement.h>

// State Machine Language
#include <msg.h>
#include <debuglog.h>
#include <msgroute.h>
#include <statemch.h>

// Game Engine
#include <ISystem.h>
#include <AIReasoner.h>
#include <database.h>
#include <world.h>

// Behavior Tree				// project 1 only
#include <BehaviorTrees/BehaviorTreesDef.h>

// Unit test state machines
//#include <example.h>
//#include <UnitTests/unittest1.h>
//#include <UnitTests/unittest2a.h>
//#include <UnitTests/unittest2b.h>
//#include <UnitTests/unittest2c.h>
//#include <UnitTests/unittest3a.h>
//#include <UnitTests/unittest3b.h>
//#include <UnitTests/unittest4.h>
//#include <UnitTests/unittest5.h>
//#include <UnitTests/unittest6.h>

#pragma warning(default: 4005)
#pragma warning(default: 4995)