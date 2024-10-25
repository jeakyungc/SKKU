#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "sphere.h"		// circle class definition

//*************************************
// global constants
static const char*	window_name = "a2";
static const char*	vert_shader_path = "shaders/sphere.vert";
static const char*	frag_shader_path = "shaders/sphere.frag";
static const uint	LONG_NUM = 72;
static const uint	LATI_NUM = 36;

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // initial window size

//*************************************
// OpenGL objects
GLuint	program = 0;		// ID holder for GPU program
GLuint	vertex_array = 0;	// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// current simulation parameter
int		soild_color_state = 0;
bool	b_rotate = true;
float	prev_time = 0.0f;
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	spheres = std::move(create_spheres()); //unit

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices

//*************************************
void update()
{
	// update global simulation parameter
	float curr_time = float(glfwGetTime());
	float elapsed_time = curr_time - prev_time;
	prev_time = curr_time;

	if (b_rotate) t += elapsed_time;


	// tricky aspect correction matrix for non-square window
	float aspect = window_size.x/float(window_size.y);
	mat4 aspect_matrix = 
	{
		std::min(1/aspect,1.0f), 0, 0, 0,
		0, std::min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// update common uniform variables in vertex/fragment shaders
	GLint uloc;
	mat4 view_projection_matrix = aspect_matrix * mat4{ 0,1,0,0,0,0,1,0,-1,0,0,1,0,0,0,1 };
	
	uloc = glGetUniformLocation(program, "soild_color_state");
	if (uloc > -1) glUniform1i(uloc, soild_color_state);

	uloc = glGetUniformLocation(program, "view_projection_matrix");
	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, view_projection_matrix);
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray( vertex_array );

	// render two circles: trigger shader program to process vertex data
	for( auto& c : spheres )
	{
		// per-circle update
		c.update(t);

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation( program, "solid_color" );		if(uloc>-1) glUniform4fv( uloc, 1, c.color );	// pointer version
		uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, c.model_matrix );

		// draw calls
		uint num_indices = 2 * (LONG_NUM + 1) * LATI_NUM *3;
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
	printf( "- press 'd' to toggle (tc.xy,0) > (tc.xxx) > (tc.yyy)\n" );
	printf("- press 'r' to rotate the sphere\n");
#ifndef GL_ES_VERSION_2_0
	printf( "- press 'w' to toggle wireframe\n" );
#endif
	printf( "\n" );
}

std::vector<vertex> create_sphere_vertices()
{
	std::vector<vertex> v = {{ vec3(0), vec3(0,0,-1.0f), vec2(0.5f) }}; // origin
	
	// create sphere vertices (VAO)
	for (uint i = 0; i <= LATI_NUM; i++) {
		for (uint j = 0; j <= LONG_NUM; j++) {
			float theta = PI * i / float(LATI_NUM);
			float phi = PI * 2.0f * j / float(LONG_NUM);
			
			float ct = cos(theta), st = sin(theta);
			float cp = cos(phi), sp = sin(phi);

			vertex tmp =
			{
				vec3(1.0f*st*cp, 1.0f*st*sp, 1.0f*ct),			// position coordinate
				vec3(st*cp, st*sp, ct),							// normal vector
				vec2(phi / (2.0f * PI), 1.0f - theta / PI)		// texture coordinates
			};
			v.push_back(tmp);
		}
	}

	return v;
}

void update_vertex_buffer( const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if(vertex_buffer)	glDeleteBuffers( 1, &vertex_buffer );	vertex_buffer = 0;
	if(index_buffer)	glDeleteBuffers( 1, &index_buffer );	index_buffer = 0;

	// check exceptions
	if(vertices.empty()){ printf("[error] vertices is empty.\n"); return; }

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
	if(vertex_array) glDeleteVertexArrays(1,&vertex_array);
	vertex_array = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool flag = false;
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_R)							b_rotate = !b_rotate;
		else if (key == GLFW_KEY_D)
		{
			soild_color_state += 1;
			if (soild_color_state > 2) soild_color_state = 0;
			
			if (soild_color_state == 0) printf("> using (texcoord.xy,0) as color\n");
			else if (soild_color_state == 1) printf("> using (texcoord.xxx,1) as color\n");
			else if (soild_color_state == 2) printf("> using (texcoord.yyy,1) as color\n");
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
	if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_PRESS )
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		printf( "> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y) );
	}
}

void motion( GLFWwindow* window, double x, double y )
{
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth( 1.0f );
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	
	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices());

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer(unit_sphere_vertices);

	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movements

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
