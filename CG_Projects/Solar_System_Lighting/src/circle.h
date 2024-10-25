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
static const float moon_rad = 0.27f * 5.0f;
static const float ganymede_rad = 0.41f * 5.0f;
static const float titan_rad = 0.404f * 5.0f;
static const float callisto_rad = 0.38f * 5.0f;
static const float io_rad = 0.29f * 5.0f;
static const float europa_rad = 0.24f * 5.0f;
static const float triton_rad = 0.21f * 5.0f;
static const float titania_rad = 0.12f * 5.0f;
static const float rhea_rad = 0.12f * 5.0f;
static const float iapetus_rad = 0.115f * 5.0f;


static const float mercury_distance = 0.4f * 10.0f;
static const float venus_distance = 0.7f * 15.0f;
static const float earth_distance = 1.0f * 25.0f;
static const float mars_distance = 1.5f * 30.0f;
static const float jupiter_distance = 5.0f * 15.0f;
static const float saturn_distance = 10.0f * 15.0f;
static const float uranus_distance = 20.0f * 10.0f;
static const float neptune_distance = 30.0f * 7.5f;
static const float moon_distance = 0.003f * 200.0f;
static const float ganymede_distance = 0.007f * 200.0f;
static const float titan_distance = 0.008f * 200.0f;
static const float callisto_distance = 0.013f * 200.0f;
static const float io_distance = 0.003f * 200.0f;
static const float europa_distance = 0.004f * 200.0f;
static const float triton_distance = 0.002f * 200.0f;
static const float titania_distance = 0.003f * 200.0f;
static const float rhea_distance = 0.0035f * 200.0f;
static const float iapetus_distance = 0.023f * 200.0f;

static const float sun_rotation = 1.0f / 25.38f;
static const float mercury_rotation = 1.0f / 58.65f;
static const float venus_rotation = -1.0f / 243.02f;
static const float earth_rotation = 1.0f / 1.0f;
static const float mars_rotation = 1.0f / 1.03f;
static const float jupiter_rotation = 1.0f / 0.41f;
static const float saturn_rotation = 1.0f / 0.44f;
static const float uranus_rotation = 1.0f / 0.72f;
static const float neptune_rotation = 1.0f / 0.67f;
static const float moon_rotation = 1.0f / 27.3f * 365.26f / 6.0f;
static const float ganymede_rotation = 1.0f / 7.0f * 365.26f / 6.0f;
static const float titan_rotation = 1.0f / 15.9f * 365.26f / 6.0f;
static const float callisto_rotation = 1.0f / 16.7f * 365.26f / 6.0f;
static const float io_rotation = 1.0f / 1.77f * 365.26f / 20.0f;
static const float europa_rotation = 1.0f / 3.55f * 365.26f / 15.0f;
static const float triton_rotation = 1.0f / 5.9f * 365.26f / 10.0f;
static const float titania_rotation = 1.0f / 8.7f * 365.26f / 8.0f;
static const float rhea_rotation = 1.0f / 4.5f * 365.26f / 10.0f;
static const float iapetus_rotation = 1.0f / 79.3f * 365.26f / 5.0f;

static const float sun_revolution = 0.0f;
static const float mercury_revolution = 1.0f / 87.97f * 365.26f;
static const float venus_revolution = 1.0f / 224.7f * 365.26f;
static const float earth_revolution = 1.0f / 365.26f * 365.26f;
static const float mars_revolution = 1.0f / 686.96f * 365.26f;
static const float jupiter_revolution = 1.0f / 4333.29f * 365.26f * 5.0f;
static const float saturn_revolution = 1.0f / 10756.20f * 365.26f * 10.0f;
static const float uranus_revolution = 1.0f / 30707.49f * 365.26f * 10.0f;
static const float neptune_revolution = 1.0f / 60223.35f * 365.26f * 10.0f;
static const float moon_revolution = 1.0f / 27.3f * 365.26f / 6.0f;
static const float ganymede_revolution = 1.0f / 7.0f * 365.26f / 6.0f;
static const float titan_revolution = 1.0f / 15.9f * 365.26f / 6.0f;
static const float callisto_revolution = 1.0f / 16.7f * 365.26f / 6.0f;
static const float io_revolution = 1.0f / 1.77f * 365.26f / 20.0f;
static const float europa_revolution = 1.0f / 3.55f * 365.26f / 15.0f;
static const float triton_revolution = 1.0f / 5.9f * 365.26f / 10.0f;
static const float titania_revolution = 1.0f / 8.7f * 365.26f / 8.0f;
static const float rhea_revolution = 1.0f / 4.5f * 365.26f / 10.0f;
static const float iapetus_revolution = 1.0f / 79.3f * 365.26f / 5.0f;

struct sphere_t
{
	float	center_planet_revspeed = 0.0f;
	float	satellite_revolution_rad = 0.0f;
	bool	satellite = false;
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
	sun = { 0,0,false,vec3(0,0,0),sun_rad,sun_rotation, sun_revolution, vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(sun);

	sphere_t mercury;
	mercury = { 0,0,false, vec3(mercury_distance + sun_rad + mercury_rad,mercury_rad,0),mercury_rad,mercury_rotation,mercury_revolution,vec4(1.0f,0.5f,0.5f,1.0f)};
	sphere.emplace_back(mercury);

	sphere_t venus;
	venus = { 0,0,false, vec3(-(venus_distance + sun_rad + venus_rad),venus_rad,0),venus_rad,venus_rotation,venus_revolution,vec4(1.0f,0.5f,0.5f,1.0f)};
	sphere.emplace_back(venus);

	sphere_t earth;
	earth = { 0,0,false,vec3(earth_distance + sun_rad + earth_rad,-earth_rad,0),earth_rad,earth_rotation,earth_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(earth);

	sphere_t moon;
	moon = { earth_revolution,moon_distance + moon_rad + earth_rad,true,vec3(earth.center.x, earth.center.y, 0),moon_rad,moon_rotation,moon_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(moon);

	sphere_t mars;
	mars = { 0,0,false, vec3(-(mars_distance + sun_rad + mars_rad),-mars_rad,0),mars_rad,mars_rotation,mars_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(mars);

	sphere_t jupiter;
	jupiter = { 0,0,false, vec3(jupiter_distance + sun_rad + jupiter_rad,jupiter_rad,0),jupiter_rad,jupiter_rotation,jupiter_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(jupiter);

	sphere_t io;
	io = { jupiter_revolution,io_distance + io_rad + jupiter_rad,true,vec3(jupiter.center.x, jupiter.center.y, 0),io_rad,io_rotation,io_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(io);

	sphere_t europa;
	europa = { jupiter_revolution,europa_distance + europa_rad + jupiter_rad + 4.0f,true,vec3(jupiter.center.x, jupiter.center.y, 0),europa_rad,europa_rotation,europa_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(europa);

	sphere_t ganymede;
	ganymede = { jupiter_revolution,ganymede_distance + ganymede_rad + jupiter_rad + 8.0f,true,vec3(jupiter.center.x, jupiter.center.y, 0),ganymede_rad,ganymede_rotation,ganymede_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(ganymede);

	sphere_t callisto;
	callisto = { jupiter_revolution,callisto_distance + callisto_rad + jupiter_rad + 12.0f,true,vec3(jupiter.center.x, jupiter.center.y, 0),callisto_rad,callisto_rotation,callisto_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(callisto);

	sphere_t saturn;
	saturn = { 0,0,false, vec3(-(saturn_distance + sun_rad + saturn_rad),saturn_rad,0),saturn_rad,saturn_rotation,saturn_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(saturn);

	sphere_t rhea;
	rhea = { saturn_revolution,rhea_distance + rhea_rad + saturn_rad,true,vec3(saturn.center.x, saturn.center.y, 0),rhea_rad,rhea_rotation,rhea_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(rhea);

	sphere_t titan;
	titan = { saturn_revolution,titan_distance + titan_rad + saturn_rad + 1.0f,true,vec3(saturn.center.x, saturn.center.y, 0),titan_rad,titan_rotation,titan_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(titan);

	sphere_t iapetus;
	iapetus = { saturn_revolution,iapetus_distance + iapetus_rad + saturn_rad + 3.0f,true,vec3(saturn.center.x, saturn.center.y, 0),iapetus_rad,iapetus_rotation,iapetus_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(iapetus);

	sphere_t uranus;
	uranus = { 0,0,false, vec3(uranus_distance + sun_rad + uranus_rad,-uranus_rad,0),uranus_rad,uranus_rotation,uranus_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(uranus);

	sphere_t titania;
	titania = { uranus_revolution, titania_distance + titania_rad + uranus_rad,true,vec3(uranus.center.x, uranus.center.y, 0),titania_rad,titania_rotation,titania_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(titania);

	sphere_t neptune;
	neptune = { 0,0,false, vec3(-(neptune_distance + sun_rad + neptune_rad),-neptune_rad,0),neptune_rad,neptune_rotation,neptune_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(neptune);

	sphere_t triton;
	triton = { neptune_revolution,triton_distance + triton_rad + neptune_rad,true,vec3(neptune.center.x, neptune.center.y, 0),triton_rad,triton_rotation,triton_revolution,vec4(1.0f,0.5f,0.5f,1.0f) };
	sphere.emplace_back(triton);

	return sphere;
}

inline void sphere_t::update(float t)
{
	float c_rotation = cos(t * rotation_speed), s_rotation = sin(t * rotation_speed);
	float c_revolution = cos(t * revolution_speed), s_revolution = sin(t * revolution_speed);
	float center_c_revolution = cos(t * center_planet_revspeed), center_s_revolution = sin(t * center_planet_revspeed);
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

	mat4 revolution_matrix =
	{
		c_revolution,-s_revolution, 0, 0,
		s_revolution, c_revolution, 0, 0,
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
	if (satellite == false) model_matrix =  revolution_matrix * translate_matrix * rotation_matrix * scale_matrix;
	else {
		mat4 translate_matrix =
		{
			1, 0, 0, (center.x + satellite_revolution_rad * c_revolution),
			0, 1, 0, (center.y + satellite_revolution_rad * s_revolution),
			0, 0, 1, center.z,
			0, 0, 0, 1
		};
		revolution_matrix =
		{
			center_c_revolution,-center_s_revolution, 0, 0,
			center_s_revolution, center_c_revolution, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		model_matrix =  revolution_matrix * translate_matrix * rotation_matrix * scale_matrix;
	}
}

#endif
