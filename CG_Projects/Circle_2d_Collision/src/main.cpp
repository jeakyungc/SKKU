#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "circle.h"		// circle class definition
#include <random>		/// for random generation
#include <math.h>


//*************************************
// global constants
static const char*	window_name = "moving-circles";
static const char*	vert_shader_path = "shaders/a1-moving-circles.vert";
static const char*	frag_shader_path = "shaders/a1-moving-circles.frag";

static const uint		NUM_TESS = 30;	/// initial tessellation factor of the circle as a polygon

static const int	MIN_CRIC = 17;
static const int	MAX_CRIC = 100;
int					NUM_CIRC = 17;		/// initial number of numbers

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
float	prev_time = 0.0f;
float   elapsed_time = 0.0f;
bool	b_solid_color = true;			// use circle's color?
bool	b_index_buffer = true;			// use index buffering?
bool	b_restart = false;
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	circles = std::move(create_circles()); /// created only one circle verteices. (not an VAO)

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_circle_vertices;	// host-side vertices

///calculate squared_distance
vec2 cal_squared_distance(const circle_t& a, const circle_t& b) {
	float dx = a.center.x - b.center.x;
	float dy = a.center.y - b.center.y;
	float dr = a.radius + b.radius;
	return vec2((dx * dx) + (dy * dy), dr * dr);
}

//*************************************
void update()
{
	// update global simulation parameter
	float curr_time = float(glfwGetTime());
	t = curr_time*0.4f;
	elapsed_time = curr_time - prev_time;
	prev_time = curr_time;
	
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
	uloc = glGetUniformLocation( program, "b_solid_color" );	if(uloc>-1) glUniform1i( uloc, b_solid_color );
	uloc = glGetUniformLocation( program, "aspect_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, aspect_matrix );

	/// forward declaration of update functions
	void update_population(int n, float aspect);
	void update_velocity(circle_t& c, float e, float aspect);

	/// populate if keyboard event comes
	if (circles.size() != NUM_CIRC || b_restart) {
		b_restart = false;
		update_population(NUM_CIRC, aspect);
	}
	/// per circle update
	for (auto& c : circles) {
		update_velocity(c, elapsed_time, aspect);
		c.update(elapsed_time);
	}
}

void update_population(int NUM_CIRC, float aspect)
{
	circles.clear(); // clear all circle instances to generate new.
	
	// range of raduis
	float r_min = 0.2f * 1 / (float)sqrt(NUM_CIRC);
	float r_max = 0.8f * 1 / (float)sqrt(NUM_CIRC);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis_p(-1.0f*aspect, 1.0f*aspect);
	std::uniform_real_distribution<float> dis_v(-0.3f, 0.3f);
	std::uniform_real_distribution<float> dis_r(r_min, r_max);
	std::uniform_real_distribution<float> dis_c(0.01f, 1.0f);
	
	// generate instances
	for (int i = 0; i < NUM_CIRC; i++) {
		circle_t c;
		c = { vec2(0), vec2(0), 1.0f, vec4(0.0f,0.0f,0.0f,0.0f) };
		
		while (1) { // if wrong x,y,r has genertated, generate loop goes until the proper x,y,r appearse
			bool placed = true;
			c.center = vec2(dis_p(gen), dis_p(gen));
			c.radius = dis_r(gen);
			
			if ((c.center.x - c.radius < -1.0f * aspect) ||
				(c.center.x + c.radius > 1.0f * aspect) ||
				(c.center.y - c.radius < -1.0f) ||
				(c.center.y + c.radius > 1.0f)) continue;

			for (auto& c2 : circles) {
				vec2 xy = cal_squared_distance(c, c2);
				if (xy.x < xy.y) placed = false;
			}

			if (placed) {
				c.velocity = vec2(dis_v(gen), dis_v(gen));
				c.velocity.x += 0.001f; //slight addition for 0.0 speed
				c.velocity.y += 0.001f;
				c.color = vec4(dis_c(gen), dis_c(gen), dis_c(gen), dis_c(gen));
				circles.emplace_back(c);
				break;
			}
		}
	}
}

void update_velocity(circle_t& c1, float e, float aspect) {
	float x1 = c1.center.x, y1 = c1.center.y, u1x = c1.velocity.x, u1y = c1.velocity.y, r1 = c1.radius;
	circle_t next_c1, next_c2;

	for (auto& c2 : circles) {
		if (c1.center == c2.center) continue;
		
		float x2 = c2.center.x, y2 = c2.center.y, u2x = c2.velocity.x, u2y = c2.velocity.y, r2 = c2.radius;
		
		next_c1.center = vec2(x1 + u1x * e, y1 + u1y * e);
		next_c2.center = vec2(x2 + u2x * e, y2 + u2y * e);
		
		float m1 = 3.14f * r1 * r1;
		float m2 = 3.14f * r2 * r2;

		float d = sqrt(cal_squared_distance(c1, c2).x); 
		float next_d = sqrt(cal_squared_distance(next_c1, next_c2).x);

		if ((d < (r1 + r2)) && d > next_d) { // if collides and gets closer
			
			///elastic collision
			float v1x = (u1x * (m1 - m2) + 2 * m2 * u2x) / (m1 + m2);
			float v1y = (u1y * (m1 - m2) + 2 * m2 * u2y) / (m1 + m2);
			float v2x = (u2x * (m2 - m1) + 2 * m1 * u1y) / (m1 + m2);
			float v2y = (u2y * (m2 - m1) + 2 * m1 * u1y) / (m1 + m2);

			c1.velocity = vec2(v1x, v1y);
			c2.velocity = vec2(v2x, v2y);
		}
	}
	
	/// for boarder collision
	if ((next_c1.center.x - r1 < -1.0f * aspect && u1x < 0) ||
		(next_c1.center.x + r1 > 1.0f * aspect && u1x > 0)) c1.velocity.x *= -1;
	if ((next_c1.center.y - r1 < -1.0f && u1y < 0) ||
		(next_c1.center.y + r1 > 1.0f && u1y > 0)) c1.velocity.y *= -1;
};

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray( vertex_array );

	
	int i = 0;
	for( auto& c : circles )
	{
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation( program, "solid_color" );		if(uloc>-1) glUniform4fv( uloc, 1, c.color );	// pointer version
		uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, c.model_matrix );

		// per-circle draw calls
		glDrawElements( GL_TRIANGLES, NUM_TESS*3, GL_UNSIGNED_INT, nullptr );
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
	//printf( "- press 'd' to toggle between solid color and texture coordinates\n" );
	printf("- press '+/-' to increase/decrease the number of circle(min = 20, max = 512)\n"); //, MIN_TESS, MAX_TESS );
	printf("press 'r' to reset circles");
	
#ifndef GL_ES_VERSION_2_0
	printf( "- press 'w' to toggle wireframe\n" );
#endif
	printf( "\n" );
}

std::vector<vertex> create_circle_vertices( uint N ) 
{
	std::vector<vertex> v = {{ vec3(0), vec3(0,0,-1.0f), vec2(0.5f) }}; // origin
	for( uint k=0; k <= N; k++ )
	{
		float t=PI*2.0f*k/float(N), c=cos(t), s=sin(t); 
		v.push_back( { vec3(c,s,0), vec3(0,0,-1.0f), vec2(c,s)*0.5f+0.5f } );
	}
	return v;
}

void update_vertex_buffer( const std::vector<vertex>& vertices, uint N ) /// generates VAO
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if(vertex_buffer)	glDeleteBuffers( 1, &vertex_buffer );	vertex_buffer = 0;
	if(index_buffer)	glDeleteBuffers( 1, &index_buffer );	index_buffer = 0;

	// check exceptions
	if(vertices.empty()){ printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if(b_index_buffer) //index buffer »ç¿ë
	{
		std::vector<uint> indices;
		for( uint k=0; k < N; k++ )
		{
			indices.push_back(0);	// the origin
			indices.push_back(k+1);
			indices.push_back(k+2);
		}

		// generation of vertex buffer: use vertices as it is
		glGenBuffers( 1, &vertex_buffer );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers( 1, &index_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW );
	}

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if(vertex_array) glDeleteVertexArrays(1,&vertex_array);
	vertex_array = cg_create_vertex_array( vertex_buffer, index_buffer );
	if(!vertex_array){ printf("%s(): failed to create vertex aray\n",__func__); return; }
}


void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	bool flag = false;
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_R)							{ printf("> reset circles\n"); b_restart = true; }
		
#ifndef GL_ES_VERSION_2_0
		else if(key==GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode( GL_FRONT_AND_BACK, b_wireframe ? GL_LINE:GL_FILL );
			printf( "> using %s mode\n", b_wireframe ? "wireframe" : "solid" );
		}
#endif
	}
	else if(action==GLFW_RELEASE)
	{
		int n = NUM_CIRC;
		if (key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT))) NUM_CIRC++;
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) NUM_CIRC--;
		
		if (NUM_CIRC > MAX_CRIC) NUM_CIRC = MAX_CRIC;
		else if (NUM_CIRC < MIN_CRIC) NUM_CIRC = MIN_CRIC;
		
		if (n != NUM_CIRC) printf("\r> number of circles = %d", NUM_CIRC);
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
	unit_circle_vertices = std::move(create_circle_vertices( NUM_TESS ));

	// create vertex buffer; 
	update_vertex_buffer( unit_circle_vertices, NUM_TESS );

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
