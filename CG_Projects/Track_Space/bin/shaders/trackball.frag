#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec2 tc;

// the only output variable
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform int solid_color_mod;

void main()
{
	if(solid_color_mod == 0) {
		fragColor = vec4(tc.xy,0,1);
	}
	else if(solid_color_mod == 1) {
		fragColor = vec4(tc.xxx,1);
	}
	else {
		fragColor = vec4(tc.yyy,1);
	}
}
