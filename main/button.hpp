#ifndef BUTTON_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
#define BUTTON_HPP_6874B39C_112D_4D34_BD85_AB81A730955B

// This defines the vertex data for a colored unit cube.
#include <glad.h>

constexpr float const launchButPos[] = {
	-0.3f,  -0.9f,
	-0.3f, -0.7f, 
	-0.1f, -0.7f, 
	-0.1f,  -0.7f,  
	-0.1f, -0.9f, 
	-0.3f,  -0.9f
};

constexpr float const launchButColors[] = {
	0.1f, 0.1f, 0.0f , 
	0.1f, 0.1f, 0.0f ,
	0.1f, 0.1f, 0.0f , 
	0.1f, 0.1f, 0.0f ,  
	0.1f, 0.1f, 0.0f, 
	0.1f, 0.1f, 0.0f 
};

constexpr float const resetButPos[] = {
	0.3f,  -0.9f, 
	0.3f, -0.7f, 
	0.1f, -0.7f, 
	0.1f,  -0.7f,  
	0.1f, -0.9f, 
	0.3f,  -0.9f
};

constexpr float const resetButColors[] = {
	1.0f, 0.0f, 0.0f, 
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 
	1.0f, 0.0f, 0.0f,  
	1.0f, 0.0f, 0.0f, 
	1.0f, 0.0f, 0.0f 
};

GLuint create_launch_button_vao();
GLuint create_reset_button_vao();

#endif // BUTTON_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
