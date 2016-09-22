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

class GameObject;


class Body
{
public:
	Body( int health, D3DXVECTOR3& pos, GameObject& owner );
	~Body( void );

	inline int GetHealth( void )					{ return( m_health ); }
	inline void SetHealth( int health )				{ if( health > 0 ) { m_health = health; } else { m_health = 0; } }
	inline bool IsAlive( void )						{ return( m_health > 0 ); }

	inline void SetSpeed( float speed )				{ m_speed = speed; }
	inline float GetSpeed( void )					{ return( m_speed ); }

	inline void SetPos( D3DXVECTOR3& pos )			{ m_pos = pos; }
	inline D3DXVECTOR3& GetPos( void )				{ return( m_pos ); }

	inline void SetDir( D3DXVECTOR3& dir )			{ m_dir = dir; }
	inline D3DXVECTOR3& GetDir( void )				{ return( m_dir ); }

	inline void SetRadius( float radius )			{ m_radius = radius; }
	inline float GetRadius( void )					{ return( m_radius ); }

	inline void SetScale(float scale)				{ m_scale = scale; }
	inline float GetScale(void)						{ return m_scale; }
	
protected:

	GameObject* m_owner;

	int m_health;

	D3DXVECTOR3 m_pos;		//Current position
	D3DXVECTOR3 m_dir;		//Current facing direction
	float m_speed;			//Current movement speed

	float m_radius;			//Personal bounding radius
	float m_scale;			// Scale
};