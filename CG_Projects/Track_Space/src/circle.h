#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

static const float sun_rad = 109.25f / 3.0f;
static const float mercury_rad = 0.383f * 5.0f;
static const float venus_rad = 0.950f * 5.0f;
static const float earth_rad = 1.0f * 5.0f;
static const float mars_rad = 0.532f * 5.0f;
static const float jupiter_rad = 10.97f * 1.2f;
static const float saturn_rad = 9.14f * 1.2f;
static const float uranus_rad = 3.98f * 2.0f;
static const float neptune_rad = 3.87f * 2.0f;

static const float mercury_distance = 0.4f * 10.0f;
static const float venus_distance = 0.7f * 15.0f;
static const float earth_distance = 1.0f * 23.0f;
static const float mars_distance = 1.5f * 25.0f;
static const float jupiter_distance = 5.0f * 10.0f;
static const float saturn_distance = 10.0f * 10.0f;
static const float uranus_distance = 20.0f * 7.0f;
static const float neptune_distance = 30.0f * 6.0f;

static const float sun_rotation = 1.0f / 25.38f;
static const float mercury_rotation = 1.0f / 58.65f;
static const float venus_rotation = -1.0f / 243.02f;
static const float earth_rotation = 1.0f / 1.0f;
static const float mars_rotation = 1.0f / 1.03f;
static const float jupiter_rotation = 1.0f / 0.41f;
static const float saturn_rotation = 1.0f / 0.44f;
static const float uranus_rotation = 1.0f / 0.72f;
static const float neptune_rotation = 1.0f / 0.67f;

static const float sun_revolution = 0.0f;
static const float mercury_revolution = 1.0f / 87.97f * 365.26f;
static const float venus_revolution = 1.0f / 224.7f * 365.26f;
static const float earth_revolution = 1.0f / 365.26f * 365.26f;
static const float mars_revolution = 1.0f / 686.96f * 365.26f;
static const float jupiter_revolution = 1.0f / 4333.29f * 365.26f * 5.0f;
static const float saturn_revolution = 1.0f / 10756.20f * 365.26f * 10.0f;
static const float uranus_revolution = 1.0f / 30707.49f * 365.26f * 10.0f;
static const float neptune_revolution = 1.0f / 60223.35f * 365.26f * 10.0f;

struct sphere_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 1.0f;		// radius
	float	rotation_speed = 1.0f;
	float	revolution_speed = 1.0f;
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t);
};

inline std::vector<sphere_t> create_sphere()
{
	std::vector<sphere_t> sphere;

	sphere_t sun;
	sun = { vec3(0,0,0),sun_rad,sun_rotation, sun_revolution, vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(sun);

	sphere_t mercury;
	mercury = { vec3(mercury_distance + sun_rad + mercury_rad,mercury_rad + 100.0f,0),mercury_rad,mercury_rotation,mercury_revolution,vec4(1.0f,0.5f,0.5f,1.0f)};
	sphere.emplace_back(mercury);

	sphere_t venus;
	venus = { vec3(-(venus_distance + sun_rad + venus_rad),venus_rad + 100.0f,0),venus_rad,venus_rotation,venus_revolution,vec4(1.0f,0.5f,0.5f,1.0f)};
	sphere.emplace_back(venus);

	sphere_t earth;
	earth = { vec3(earth_distance + sun_rad + earth_rad,-earth_rad - 150.0f,0),earth_rad,earth_rotation,earth_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(earth);

	sphere_t mars;
	mars = { vec3(-(mars_distance + sun_rad + mars_rad),-mars_rad - 150.0f,0),mars_rad,mars_rotation,mars_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(mars);

	sphere_t jupiter;
	jupiter = { vec3(jupiter_distance + sun_rad + jupiter_rad,jupiter_rad + 200.0f,0),jupiter_rad,jupiter_rotation,jupiter_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(jupiter);

	sphere_t saturn;
	saturn = { vec3(-(saturn_distance + sun_rad + saturn_rad),saturn_rad + 200.0f,0),saturn_rad,saturn_rotation,saturn_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(saturn);

	sphere_t uranus;
	uranus = { vec3(uranus_distance + sun_rad + uranus_rad,-uranus_rad - 250.0f,0),uranus_rad,uranus_rotation,uranus_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(uranus);

	sphere_t neptune;
	neptune = { vec3(-(neptune_distance + sun_rad + neptune_rad),-neptune_rad - 250.0f,0),neptune_rad,neptune_rotation,neptune_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(neptune);

	return sphere;
}

inline void sphere_t::update(float t)
{
	float c_rotation = cos(t * rotation_speed), s_rotation = sin(t * rotation_speed);
	float c_revolution = cos(t * revolution_speed), s_revolution = sin(t * revolution_speed);
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
		c_rotation,-s_rotation, 0, 0,
		s_rotation, c_rotation, 0, 0,
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

	mat4 revolution_matrix =
	{
		c_revolution,-s_revolution, 0, 0,
		s_revolution, c_revolution, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	model_matrix = revolution_matrix * translate_matrix * rotation_matrix * scale_matrix;
}

#endif
