/* Copyright Steve Rabin, 2013. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2013"
 */

#pragma once

class Time;
class Database;
class MsgRoute;
class DebugLog;
class AnimationManager;
class CMultiAnim;
class CTiny;

class World
{
public:
	World();
	~World();

	void InitializeSingletons( void );
	void Initialize( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent );
	void PostInitialize();

	void Update(float dt);
	void Animate( double dTimeDelta );
	void AdvanceTimeAndDraw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye );
	void RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );

	void InvalidateDeviceObjects( void );

	inline AnimationManager& GetAnimationManager()	{ assert( m_animationManager ); return( *m_animationManager ); }

protected:

	bool m_initialized;
	Clock* m_clock;
	Database* m_database;
	MsgRoute* m_msgroute;
	DebugLog* m_debuglog;
	Random* m_random;

	AnimationManager* m_animationManager;

};

