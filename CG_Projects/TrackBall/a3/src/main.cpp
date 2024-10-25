#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "sphere.h"
#include "box.h"

//*************************************
// global constants
static const char*	window_name = "a3";
static const char*	vert_shader_path = "shaders/a4.vert";
static const char*	frag_shader_path = "shaders/a4.frag";
static const uint	LONG_NUM = 40;
static const uint	LATI_NUM = 20;

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;					// ID holder for GPU program
GLuint	vertex_array = 0;		// ID holder for vertex array object
GLuint	test_vertex_array = 0;		// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
float	t = 0.0f;				// current simulation parameter
float	prev_time = 0.0f;
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	spheres = std::move(create_spheres()); //unit

//*************************************
// scene objects
//mesh*		p_mesh = nullptr;
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices
std::vector<std::vector<vertex>> box_vertices;

camera		cam=camera(vec3(278,273,-800)), home=cam;
trackball	tb;

//*************************************
void update()
{
	//update global simulation parameter
	float curr_time = float(glfwGetTime());
	float elapsed_time = curr_time - prev_time;
	prev_time = curr_time;

	///t += elapsed_time;
	 
	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	float scale = 1.0f;
	mat4 model_matrix = mat4::scale( scale, scale, scale );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	uloc = glGetUniformLocation( program, "model_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, model_matrix );
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray(vertex_array);

	// render vertices: trigger shader programs to process vertex data
	for (auto& c : spheres)
	{
		// per-circle update
		c.update(t);
		
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		
		// draw calls
		uint num_indices = 2 * (LONG_NUM + 1) * LATI_NUM * 3;
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
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
	printf( "- press Home to reset camera\n" );
#ifndef GL_ES_VERSION_2_0
	printf("- press 'w' to toggle wireframe\n");
#endif
	printf("\n");
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_HOME)					cam=home;
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
		if(action==GLFW_PRESS)			tb.begin( cam, npos );
		else if(action==GLFW_RELEASE)	tb.end();
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	cam = tb.update( npos );
}

std::vector<vertex> create_sphere_vertices()
{
	std::vector<vertex> v;

	// create sphere vertices 
	for (uint i = 0; i <= LATI_NUM; i++) {
		for (uint j = 0; j <= LONG_NUM; j++) {
			float theta = PI * i / float(LATI_NUM);
			float phi = PI * 2.0f * j / float(LONG_NUM);

			float ct = cos(theta), st = sin(theta);
			float cp = cos(phi), sp = sin(phi);

			v.push_back({
				vec3(1.0f * st * cp, 1.0f * st * sp, 1.0f * ct),			// position coordinate
				vec3(st * cp, st * sp, ct),									// normal vector
				vec2(phi / (2.0f * PI), 1.0f - theta / PI)					// texture coordinates
			});
		}
	}

	return v;
}

//std::vector<vertex> create_box_vertices()
//{
//	std::vector<vertex> v;
//
//	//create box vertices
//	
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(0.0f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(0.0, 0.0, 559.2), vec3(0), vec2(0) });
//	v.push_back({ vec3(549.6, 0.0, 559.2), vec3(0), vec2(0) });
//
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	v.push_back({ vec3(552.8f, 0.0f, 0.0f), vec3(0), vec2(0) });
//	return v;
//}

void update_sphere_vertex_buffer(const std::vector<vertex>& vertices)
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
	for (uint i = 0; i <= LATI_NUM; i++) {
		uint v1 = i * (LONG_NUM + 1);
		uint v2 = v1 + LONG_NUM + 1;

		for (uint j = 0; j <= LONG_NUM; j++) {
			if (i != 0) { // uppder
				indices.push_back(v1);
				indices.push_back(v2);
				indices.push_back(v1 + 1);
			}

			if (i != LATI_NUM - 1) {  // lower
				indices.push_back(v1 + 1);
				indices.push_back(v2);
				indices.push_back(v2 + 1);
			}
			v1++;
			v2++;
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

//void update_box_vertex_buffer(const std::vector<vertex>& vertices)
//{
//
//}


bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests

	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices());
	// create vertex buffer; called again when index buffering mode is toggled
	update_sphere_vertex_buffer(unit_sphere_vertices);

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
