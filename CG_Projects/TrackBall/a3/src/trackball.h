#pragma once
#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__

struct camera
{
	vec3	eye = vec3(0,0,1);
	vec3	at = vec3(0);
	vec3	up = vec3(0,1,0);
	mat4	view_matrix;

	float	fovy = PI/4.0f; // must be in radian
	float	aspect = 16.0f/9.0f; //1?
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;

	camera(){ view_matrix=mat4::look_at(eye,at,up); }
	camera( vec3 Eye, vec3 At=vec3(0), vec3 Up=vec3(0,0,1) ):eye(Eye),at(At),up(Up){ view_matrix=mat4::look_at(eye,at,up); }
};

struct trackball
{
	bool	b_tracking = false;
	float	scale = 1.0f;	// controls how much rotation is applied
	camera	cam0;			// initial camera when an action begins
	vec2	m0;				// the last mouse position

	trackball( float rot_scale=1.0f ) : scale(rot_scale){}
	bool is_tracking() const { return b_tracking; }
	void begin( const camera& cam, vec2 m );
	void end() { b_tracking = false; }
	camera update( vec2 m ) const;
};

inline void trackball::begin( const camera& cam, vec2 m )
{
	b_tracking = true;	// enable trackball tracking
	m0 = m;				// save current mouse position
	cam0 = cam;			// save current camera
}

inline camera trackball::update( vec2 m ) const
{
	// project a 2D mouse position to a unit sphere
	static const vec3 p0 = vec3(0,0,1.0f); // reference position on sphere
	vec3 p1 = vec3(m-m0,0); // displacement
	if( !b_tracking || length(p1)<0.0001f ) return cam0; // ignore subtle movement
	p1 *= scale; // apply rotation scale
	p1.z = sqrtf(std::max(0.0f,1.0f-length2(p1))); // back-project z=0 onto the unit sphere
	p1 = p1.normalize();

	// find rotation axis and angle in world space
	// - trackball self-rotation should be done at first in the world space
	// - mat3(cam0.view_matrix): rotation-only view matrix
	// - mat3(cam0.view_matrix).transpose(): inverse view-to-world matrix
	vec3 v = mat3(cam0.view_matrix).transpose()*p0.cross(p1);
	float theta = asin( std::min(v.length(),1.0f) );

	// build matrices to transform eye and up
	mat4 T = mat4::translate( cam0.at );
	mat4 R = mat4::rotate(v.normalize(),-theta); // use -theta, since we're rotating camera directly
	
	camera c = cam0;
	c.eye	= T*R*(c.eye-c.at);	// rotate view vector and translate by cam0.at
	c.up	= R*c.up;			// rotate up vector
	c.view_matrix = mat4::look_at( c.eye, c.at, c.up );

	return c;
}

// utility function
inline vec2 cursor_to_ndc( dvec2 cursor, ivec2 window_size )
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2( float(cursor.x)/float(window_size.x-1),
					  float(cursor.y)/float(window_size.y-1) );
	
	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x*2.0f-1.0f,1.0f-npos.y*2.0f);
}

#endif // __TRACKBALL_H__
