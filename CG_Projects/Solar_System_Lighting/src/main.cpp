#include "cgmath.h"		// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "circle.h"

//*************************************
// global constants
static const char*	window_name = "A3";
static const char*	vert_shader_path = "shaders/trackball.vert";
static const char*	frag_shader_path = "shaders/trackball.frag";
static const char*	sun_texture_path = "textures/sun.jpg";
static const char*	mercury_texture_path = "textures/mercury.jpg";
static const char*	venus_texture_path = "textures/venus.jpg";
static const char*	earth_texture_path = "textures/earth.jpg";
static const char*	mars_texture_path = "textures/mars.jpg";
static const char*	jupiter_texture_path = "textures/jupiter.jpg";
static const char*	saturn_texture_path = "textures/saturn.jpg";
static const char*	uranus_texture_path = "textures/uranus.jpg";
static const char*	neptune_texture_path = "textures/neptune.jpg";
static const char*	moon_texture_path = "textures/moon.jpg";
static const char*  saturn_ring_texture_path = "textures/saturn-ring.jpg";
static const char*  uranus_ring_texture_path = "textures/uranus-ring.jpg";
static const char*  saturn_ring_alpha_path = "textures/saturn-ring-alpha.jpg";
static const char*  uranus_ring_alpha_path = "textures/uranus-ring-alpha.jpg";
static const uint	longitude_edge = 72;
static const uint	latitude_edge = 36;

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices
std::vector<vertex>	unit_ring_vertices;
//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 0, -300, 30 );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );
		
	float	fovy = PI/4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 20000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4(0.0f, 0.0f, 0.0f, 1.0f);   // spot light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float	shininess = 1000.0f;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	vertex_array = 0;	// ID holder for vertex array object
GLuint	ring_vertex_array = 0;
GLuint	SUN = 0;
GLuint  MERCURY = 0;
GLuint  VENUS = 0;
GLuint  EARTH = 0;
GLuint  MARS = 0;
GLuint  JUPITER = 0;
GLuint  SATURN = 0;
GLuint  URANUS = 0;
GLuint  NEPTUNE = 0;
GLuint  MOON = 0;
GLuint	SATURN_RING = 0;
GLuint	URANUS_RING = 0;
GLuint	SATURN_ALPHA = 0;
GLuint	URANUS_ALPHA = 0;

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
float	t = 0.0f;						// current simulation parameter
bool	b_rotate = true;
float	prev_frame_time = 0.0f;
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	sphere = std::move(create_sphere());

//*************************************
// scene objects
camera		cam;
trackball	tb;
light_t		light;
material_t	material;

//*************************************
void update()
{
	// update global simulation parameter
	float cur_frame_time = float(glfwGetTime());
	float frame_time = cur_frame_time - prev_frame_time;
	prev_frame_time = cur_frame_time;
	if (b_rotate) t += frame_time;

	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	// build the model matrix for oscillating scale
	float scale = 1.0f;
	mat4 model_matrix = mat4::scale( scale, scale, scale );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	uloc = glGetUniformLocation( program, "model_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, model_matrix );

	// setup light properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray(vertex_array);

	int index = 0;
	for (auto& c : sphere)
	{
		// per-circle update
		c.update(t);

		if (index == 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SUN);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), true);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 1) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MERCURY);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 2) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, VENUS);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 3) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, EARTH);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 4 || index == 7 || index == 8 || index == 9 || index == 10 || index == 12 || index == 13 || index == 14 || index == 16 || index == 18) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MOON);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 5) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MARS);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 6) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, JUPITER);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}

		else if (index == 11) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SATURN);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(ring_vertex_array);
			glBindTexture(GL_TEXTURE_2D, SATURN_RING);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), true);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, SATURN_ALPHA);
			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * 3, GL_UNSIGNED_INT, nullptr);
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glBindVertexArray(vertex_array);
		}

		else if (index == 15) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, URANUS);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(ring_vertex_array);
			glBindTexture(GL_TEXTURE_2D, URANUS_RING);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), true);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, URANUS_ALPHA);
			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * 3, GL_UNSIGNED_INT, nullptr);
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glBindVertexArray(vertex_array);
		}

		else if (index == 17) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, NEPTUNE);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glUniform1i(glGetUniformLocation(program, "b_sun"), false);
			glUniform1i(glGetUniformLocation(program, "b_ring"), false);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 2 * (longitude_edge + 1) * latitude_edge * 3, GL_UNSIGNED_INT, nullptr);
		}
		index++;
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers(window);
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
#ifndef GL_ES_VERSION_2_0
	printf("- press 'w' to toggle wireframe\n");
#endif
	printf( "- press Home to reset camera\n" );
	printf("- press Space (or Pause) to pause the simulation\n");
	printf( "\n" );
}

std::vector<vertex> create_sphere_vertices()
{
	std::vector<vertex> v = { { vec3(0), vec3(0,0,-1.0f), vec2(0.5f) } }; // origin

	for (uint i = 0; i < latitude_edge + 1; i++) {
		for (uint j = 0; j < longitude_edge + 1; j++) {
			float theta = PI * i / float(latitude_edge), cos_theta = cos(theta), sin_theta = sin(theta);
			float phi = PI * 2.0f * j / float(longitude_edge), cos_phi = cos(phi), sin_phi = sin(phi);
			v.push_back({
				vec3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta), // position
				vec3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta), // normal vector
				vec2(phi / (2.0f * PI), 1.0f - theta / PI) // texture coordinate
				});
		}
	}
	return v;
}

std::vector<vertex> create_ring_vertices()
{
	std::vector<vertex> v = { { vec3(0,0,0), vec3(0,0,-1.0f), vec2(1.0f,1.0f) } }; // origin

	for (uint i = 0; i < 2; i++) {
		for (uint j = 0; j < longitude_edge + 1; j++) {
			float theta = PI / 2, cos_theta = cos(theta), sin_theta = sin(theta);
			float phi = PI * 2.0f * j / float(longitude_edge), cos_phi = cos(phi), sin_phi = sin(phi);
			v.push_back({ 
				vec3((1.5f + 0.8f * i) * sin_theta * cos_phi, (1.5f + 0.8f * i) * sin_theta * sin_phi, cos_theta), // position
				vec3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta), // normal vector
				vec2(1.0f - i * 1.0f, 1.0f - i * 1.0f) }); // texture coordinate
		}
	}
	return v;
}

void update_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices;
	for (uint i = 0; i < latitude_edge + 1; i++) {
		uint k1 = i * (longitude_edge + 1);
		uint k2 = k1 + longitude_edge + 1;

		for (uint j = 0; j < longitude_edge + 1; j++) {
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != latitude_edge - 1) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
			k1++;
			k2++;
		}
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

void update_ring_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices;
	for (uint i = 0; i < longitude_edge + 1; i++) {
		uint k = i + longitude_edge + 1;

		indices.push_back(i);
		indices.push_back(k);
		indices.push_back(i + 1);

		indices.push_back(i + 1);
		indices.push_back(k);
		indices.push_back(k + 1);

	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (ring_vertex_array) glDeleteVertexArrays(1, &ring_vertex_array);
	ring_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!ring_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_HOME)					cam=camera();
		else if (key == GLFW_KEY_R || key == GLFW_KEY_PAUSE || key == GLFW_KEY_SPACE)
		{
			b_rotate = !b_rotate;
		}
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
#endif
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if(action==GLFW_PRESS)			tb.begin_track( cam.view_matrix, npos );
		else if (action == GLFW_RELEASE) {
			tb.end_track();
			if (tb.is_zooming()) tb.end_zoom();
			if (tb.is_panning()) tb.end_pan();
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT || (button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_SHIFT)))
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin_zoom(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end_zoom();
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE || (button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_CONTROL)))
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin_pan(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end_pan();
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if((!tb.is_tracking() && !tb.is_zooming() && !tb.is_panning())) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	if (tb.is_tracking()) cam.view_matrix = tb.update_track(npos);
	if (tb.is_zooming()) cam.view_matrix = tb.update_zoom(npos);
	if (tb.is_panning()) cam.view_matrix = tb.update_pan(npos);
}

// this function will be avaialble as cg_create_texture() in other samples
GLuint create_texture(const char* image_path, bool mipmap = true, GLenum wrap = GL_CLAMP_TO_EDGE, GLenum filter = GL_LINEAR)
{
	// load image
	image* i = cg_load_image(image_path); if (!i) return 0; // return null texture; 0 is reserved as a null texture
	int		w = i->width, h = i->height, c = i->channels;

	// induce internal format and format from image
	GLint	internal_format = c == 1 ? GL_R8 : c == 2 ? GL_RG8 : c == 3 ? GL_RGB8 : GL_RGBA8;
	GLenum	format = c == 1 ? GL_RED : c == 2 ? GL_RG : c == 3 ? GL_RGB : GL_RGBA;

	// create a src texture (lena texture)
	GLuint texture;
	glGenTextures(1, &texture); if (texture == 0) { printf("%s(): failed in glGenTextures()\n", __func__); return 0; }
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, i->ptr);
	if (i) { delete i; i = nullptr; } // release image

	// build mipmap
	if (mipmap)
	{
		int mip_levels = 0; for (int k = w > h ? w : h; k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, internal_format, (w >> l) == 0 ? 1 : (w >> l), (h >> l) == 0 ? 1 : (h >> l), 0, format, GL_UNSIGNED_BYTE, nullptr);
		if (glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, !mipmap ? filter : filter == GL_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

	return texture;
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_TEXTURE_2D);			// enable texturing
	glActiveTexture(GL_TEXTURE0);		// notify GL the current texture slot is 0

	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices());
	unit_ring_vertices = std::move(create_ring_vertices());

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer(unit_sphere_vertices);
	update_ring_vertex_buffer(unit_ring_vertices);

	SUN = create_texture(sun_texture_path, true);   if (!SUN) return false;
	MERCURY = create_texture(mercury_texture_path, true);   if (!MERCURY) return false;
	VENUS = create_texture(venus_texture_path, true);   if (!VENUS) return false;
	EARTH = create_texture(earth_texture_path, true);   if (!EARTH) return false;
	MARS = create_texture(mars_texture_path, true);   if (!MARS) return false;
	JUPITER = create_texture(jupiter_texture_path, true);   if (!JUPITER) return false;
	SATURN = create_texture(saturn_texture_path, true);   if (!SATURN) return false;
	URANUS = create_texture(uranus_texture_path, true);   if (!URANUS) return false;
	NEPTUNE = create_texture(neptune_texture_path, true);   if (!NEPTUNE) return false;
	MOON = create_texture(moon_texture_path, true);   if (!MOON) return false;
	SATURN_RING = create_texture(saturn_ring_texture_path, true);   if (!SATURN_RING) return false;
	URANUS_RING = create_texture(uranus_ring_texture_path, true);   if (!URANUS_RING) return false;
	SATURN_ALPHA = create_texture(saturn_ring_alpha_path, true);   if (!SATURN_ALPHA) return false;
	URANUS_ALPHA = create_texture(uranus_ring_alpha_path, true);   if (!URANUS_ALPHA) return false;

	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
