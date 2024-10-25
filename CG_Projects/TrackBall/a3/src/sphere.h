#pragma once
#ifndef __SPHERE_H__
#define __SPHERE_H__

struct sphere_t
{
	vec3	center=vec3(0);		// 3D position for translation
	float	radius=1.0f;		// radius
	//float	theta=0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t );
};

inline std::vector<sphere_t> create_spheres()
{
	std::vector<sphere_t> spheres;
	sphere_t c;
	
	///여기에서 scaling된 공 9개 random 위치로 생성

	for (int i = 0; i < 1; i++) {
		c = { vec3(0.0f,0.0f,0.0f),100.0f,vec4(1.0f,0.5f,0.5f,1.0f) }; // c, r, th(x), color
		spheres.emplace_back(c);
	}
	

	return spheres;
}

inline void sphere_t::update( float t )
{
	
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

#endif
