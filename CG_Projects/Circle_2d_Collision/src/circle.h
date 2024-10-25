#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec2	center = vec2(0);	// 2D position for translation
	vec2	velocity = vec2(0);	// 2D velocity by x y axis
	float	radius = 1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float frame_time);
};

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	return circles;
}

inline void circle_t::update(float frame_time)
{
	center = vec2(center.x + velocity.x * frame_time, center.y + velocity.y * frame_time);

	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
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
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

#endif