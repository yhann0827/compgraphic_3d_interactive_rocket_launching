#include <iostream>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.cpp"
#include "defaults.hpp"
#include "loadobj.hpp"
#include "texture.hpp"
#include "simple_mesh.hpp"
#include "cylinder.hpp"
#include "cone.hpp"
#include "cube.hpp"
#include "button.hpp"
namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";
	constexpr float kPi_ = 3.1415926f;

	constexpr float kMovementPerSecond_ = 5.f; // units per second
	constexpr float kMouseSensitivity_ = 0.01f; // radians per
	bool isAnimate = false;
	bool resetAnimation = false;
	bool splitScreen = false;

	struct State_
	{
		ShaderProgram* prog;
		ShaderProgram* pad;
		ShaderProgram* blinn;
		ShaderProgram* button;

		enum CameraMode
		{
			FIXED_DISTANCE_CAMERA,
			GROUND_CAMERA,
			DEFAULT_CAMERA
		} cameraMode;
		
		struct CamCtrl_
		{
			bool cameraActive, trackCameraActive;
			bool moveForward, moveBackward, moveLeft, moveRight, moveUp,moveDown, actionSpeedUp, actionSlowDown;

			float phi, theta;
			float radius;

			float speed;
			float lastX, lastY;
			Mat44f viewMatrix;
			Vec3f FirstPOVMovement = {0,0,0};
			Vec3f StartingPosition;

		} camControl;
	};
	void lightDirection(Vec3f& lightDir);
	void drawAssets(GLuint, GLuint, std::size_t , GLuint , const Mat44f& , const Mat33f& ,GLuint , GLuint , std::size_t , const Mat44f& , const Mat44f& );
	void glfw_callback_error_( int, char const* );
	void glfw_callback_motion_(GLFWwindow*, double, double);
	void glfw_callback_key_( GLFWwindow*, int, int, int, int );
	void glfw_callback_mouse_button_(GLFWwindow* , int , int , int);

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};

	float calculateMovementSpeed(float baseMovement, float multiplier, bool isSpeedUp, bool isSlowDown) {
		return baseMovement * (isSpeedUp ? multiplier : (isSlowDown ? 1.0f / multiplier : 1.0f));
	}	
}

int main() try
{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	State_ state{};

	// TODO: Additional event handling setup
	glfwSetWindowUserPointer(window, &state);
	glfwSetKeyCallback( window, &glfw_callback_key_ );
	glfwSetMouseButtonCallback(window, &glfw_callback_mouse_button_);
	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);
	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// Global GL Setup
	glEnable(GL_FRAMEBUFFER_SRGB);
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f); 
	OGL_CHECKPOINT_ALWAYS();

	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );
	glViewport( 0, 0, iwidth, iheight );

	// Load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
	});

	state.prog = &prog;
	state.camControl.radius = 10.f;

	ShaderProgram pad({
		{ GL_VERTEX_SHADER, "assets/pad.vert" },
		{ GL_FRAGMENT_SHADER, "assets/pad.frag" }
		});

	state.pad = &pad;

	ShaderProgram blinn({
		{ GL_VERTEX_SHADER, "assets/blinn.vert" },
		{ GL_FRAGMENT_SHADER, "assets/blinn.frag" }
		});

	state.blinn = &blinn;

	ShaderProgram button({
		{ GL_VERTEX_SHADER, "assets/button.vert" },
		{ GL_FRAGMENT_SHADER, "assets/button.frag" }
		});

	state.button = &button;

	//Create VBO and VAO
	//Loading in map and texture
	auto parlahtiMesh = load_wavefront_obj("assets/parlahti.obj");
	GLuint parlahtiVAO = create_vao(parlahtiMesh);
	std::size_t parlahtiVertex = parlahtiMesh.positions.size();

	GLuint textureID = load_texture_2d("assets/L4343A-4k.jpeg");

	//Load landing pad model
	auto landingpad = load_wavefront_obj("assets/landingpad.obj");
	GLuint landingpadVAO = create_vao(landingpad);
	std::size_t landingpadVertex = landingpad.positions.size();

	//Create the custom model
	auto maincylinder = make_cylinder(true, 16, {2.f, 2.f, 2.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(2.2f, 0.2f, 0.2f)* make_translation({0.f, 0.f, 0.f}));
	auto maincone = make_cone(true, 16, {1.f, 0.f, 0.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(1.5f, 0.2f, 0.2f) * make_translation({1.45f, 0.f, 0.f}));

	auto cylinderbehind = make_cylinder(true, 16, {1.f, 1.f, 1.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(0.8f, 0.1f, 0.1f)* make_translation({0.f, 2.8f, 0.f}));
	auto conebehind = make_cone(true, 16, {1.f, 0.f, 0.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(1.f, 0.1f, 0.1f) * make_translation({0.8f, 2.8f, 0.f}));;
	
	auto cylinderright = make_cylinder(true, 16, {1.f, 1.f, 1.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(0.8f, 0.1f, 0.1f)* make_translation({0.f, 0.f, 2.f}));
	auto coneright = make_cone(true, 16, {1.f, 0.f, 0.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(1.f, 0.1f, 0.1f) * make_translation({0.8f, 0.f, 2.f}));
	
	auto cylinderleft = make_cylinder(true, 16, {1.f, 1.f, 1.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(0.8f, 0.1f, 0.1f)* make_translation({0.f, 0.f, -2.f}));
	auto coneleft = make_cone(true, 16, {1.f, 0.f, 0.f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(1.f, 0.1f, 0.1f) * make_translation({0.8f, 0.f, -2.f}));
	
	auto engine = make_cube({1.f, 0.098f, 0.2f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(0.2f, 0.1f, 0.1f)* make_translation({1.f, -2.8f, 0.f}));
	auto engine2 = make_cube({1.f, 0.098f, 0.2f}, make_rotation_z(3.141592f  / 2.0f) * make_scaling(0.2f, 0.1f, 0.1f)* make_translation({3.2f, -2.8f, 0.f}));

	SimpleMeshDataWithoutTexture spaceship = maincylinder;
	spaceship = concatenate(std::move(spaceship), maincone);
	spaceship = concatenate(std::move(spaceship), cylinderbehind);
	spaceship = concatenate(std::move(spaceship), conebehind);
	spaceship = concatenate(std::move(spaceship), cylinderright);
	spaceship = concatenate(std::move(spaceship), coneright);
	spaceship = concatenate(std::move(spaceship), cylinderleft);
	spaceship = concatenate(std::move(spaceship), coneleft);
	spaceship = concatenate(std::move(spaceship), engine);
	spaceship = concatenate(std::move(spaceship), engine2);

	GLuint spaceshipVao = create_vao_without_texture(spaceship);
	std::size_t spaceshipVertex = spaceship.positions.size();

	//Create the launch and reset button
	GLuint buttonVAO1 = create_launch_button_vao();
	GLuint buttonVAO2 = create_reset_button_vao();

	// Animation state
	auto last = Clock::now();
	float angle = 0.f;
	float t = 0.005f;
    Vec3f p0 = {-20.f, -0.9f, -30.f};  
	Vec3f p1 = p0 + Vec3f({5.f, 25.f, -20.f});  
	Vec3f p2 = p0 + Vec3f({25.f, 20.f, -20.f});
	
	// // Other initialization & loading
	// OGL_CHECKPOINT_ALWAYS();
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		// Handle framebuffer incomplete error
		fprintf(stderr, "Framebuffer is incomplete: %x\n", status);
	}
		
	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		// https://en.cppreference.com/w/cpp/chrono/high_resolution_clock/now
		// glBeginQuery(GL_TIME_ELAPSED, queryID[frameIndex]);
		// auto startFrameTime = std::chrono::high_resolution_clock::now();
		Vec3f result = quadraticBezier(p0, p1, p2, t);
		// Let GLFW process events
		glfwPollEvents();
		
		float fbwidth, fbheight;
		// Check if window was resized.
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;


		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		// https://learnopengl.com/Getting-started/Camera
		// https://www.youtube.com/watch?v=MZuYmG1GBFk
		// https://en.wikipedia.org/wiki/Spherical_coordinate_system
		// We are using phi angle to control the movement of the user as phi angle is used for horizontal movement while theta angle is used for vertical movement.
		// Phi affects x and z components of the camera's movement, so it is used to maipulate the coordinates of firstPOVmovement.
		// Coordinate of y when moving up and down is adjusted using the movementSpeed, which defaul value is 1 and increase or decrease based on the speed of the user.
		float movementSpeed = calculateMovementSpeed(kMovementPerSecond_ * dt, 10.0f, state.camControl.actionSpeedUp, state.camControl.actionSlowDown);
		float phiSin = sin(state.camControl.phi);
		float phiCos = cos(state.camControl.phi);

		if (state.camControl.moveForward) {
			state.camControl.FirstPOVMovement.x -= movementSpeed * phiSin;
			state.camControl.FirstPOVMovement.z += movementSpeed * phiCos;
		} else if (state.camControl.moveBackward) {
			state.camControl.FirstPOVMovement.x += movementSpeed * phiSin;
			state.camControl.FirstPOVMovement.z -= movementSpeed * phiCos;
		} else if (state.camControl.moveRight) {
			state.camControl.FirstPOVMovement.x -= movementSpeed * phiCos;
			state.camControl.FirstPOVMovement.z -= movementSpeed * phiSin;
		} else if (state.camControl.moveLeft) {
			state.camControl.FirstPOVMovement.x += movementSpeed * phiCos;
			state.camControl.FirstPOVMovement.z += movementSpeed * phiSin;
		} else if (state.camControl.moveUp) {
			state.camControl.FirstPOVMovement.y -= movementSpeed;
		} else if (state.camControl.moveDown && state.camControl.FirstPOVMovement.y < 0) {
			state.camControl.FirstPOVMovement.y += movementSpeed;
		}

		//Fixed distance camera and ground camera
		if (state.camControl.trackCameraActive) {
			if (state.cameraMode == State_::FIXED_DISTANCE_CAMERA) {
				state.camControl.theta = 0.f;
				state.camControl.phi = 0.f;
				Vec3f offset = Vec3f({0.f, -0.9f, -10.f} );
				state.camControl.FirstPOVMovement = -result + offset;
			} 
		}
		if(state.camControl.trackCameraActive && state.cameraMode == State_::GROUND_CAMERA) {
			Vec3f offset = Vec3f({20.f, -0.9f, 15.f} );

			state.camControl.FirstPOVMovement = offset;
			state.camControl.theta = -0.1f + -result.y/45;
			if(result.x>-15.f){
				state.camControl.phi = 0.425f+ result.x/35;
			}
		}

		if (state.camControl.radius <= 0.1f)
			state.camControl.radius = 0.1f;

		
		// Update: compute matrices
		// Define and compute projCameraWorld matrix
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(kIdentity44f)));
		//rotate around the y-axis
		Mat44f model2world = make_rotation_y(0);
		//rotate around x-axis with angle specified
		Mat44f Rx = make_rotation_x(state.camControl.theta);
		//rotate around y-axis with angle specified
		Mat44f Ry = make_rotation_y(state.camControl.phi);
		//translate to move objects along the x and z axis
		//change here to for cam to move along x and z axis
		// Mat44f T = make_translation({ 0.f, 0.f, -state.camControl.radius });
		Mat44f T = make_translation({state.camControl.FirstPOVMovement.x, state.camControl.FirstPOVMovement.y, state.camControl.FirstPOVMovement.z});
		//rotations and translation to transform world to camera space which defines how the scene appears on the camera
		Mat44f world2camera = Rx * Ry * T;
		// displaying on 2D space
		Mat44f projection = make_perspective_projection(
			60.f * 3.1415926f / 180.f,
			fbwidth / float(fbheight),
			0.1f, 100.f
		);
		// rendering
		Mat44f projCameraWorld = projection * world2camera * model2world;


		// Draw scene
		OGL_CHECKPOINT_DEBUG();

		//Measruing Performance for Section 1.2
		// auto startSubmitCodeTimeforTask1_2 = std::chrono::high_resolution_clock::now();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!splitScreen){ 
		//Launch and Reset button
		glUseProgram(button.programId());
		static float const basicColor[] = { 0.2f, 1.f, 1.f };
		glUniform3fv(0, 1, basicColor);
		glBindVertexArray(buttonVAO1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(buttonVAO2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);	
		glUseProgram(0);	

		 //Load shader program for pahlati model
		glUseProgram(prog.programId());
		glBindVertexArray(parlahtiVAO);

		// auto endSubmitCodeTimeforTask1_2 = std::chrono::high_resolution_clock::now();

		//Bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, 0, parlahtiVertex);
		Vec3f lightDir = normalize({ 0.f, 1.f, -1.f });
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
		lightDirection(lightDir);
		glBindVertexArray(0);
		glUseProgram(0);

		// Program for the landing pad
		//Different shader program for lading pad
		glUseProgram(pad.programId());
		//Landing pad 1
		Mat44f model2worldPad = make_translation({10.f, -0.9f, 40.f});
		Mat44f projCameraWorldPad = projection * world2camera * model2worldPad;
		normalMatrix = mat44_to_mat33(transpose(invert(model2worldPad)));
		glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldPad.v);
		glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
		lightDirection(lightDir);

		//Landing pad 2
		Mat44f model2worldPad2 = make_translation({-20.f, -0.9f, -30.f});
		Mat44f projCameraWorldPad2 = projection * world2camera * model2worldPad2;
		normalMatrix = mat44_to_mat33(transpose(invert(model2worldPad2)));
		glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldPad2.v);
		glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
		lightDirection(lightDir);

		// Measuring Peformance for Task 1.4
		// auto startSubmitCodeTimeforTask1_4 = std::chrono::high_resolution_clock::now();
		//Bind VAO and Draw array
		glBindVertexArray(landingpadVAO);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVertex);
		glBindVertexArray(landingpadVAO);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVertex);
		glBindVertexArray(0);
		// auto endSubmitCodeTimeforTask1_4 = std::chrono::high_resolution_clock::now();
		//Vehicle
		// blinn phong lighting
		glUseProgram(blinn.programId());

		// Note 
		// diffuse color = color of the light
		// specular color = color of the specular highlights
		// position of the light = intensity of the lighting

		glUniform3f(3, 0.2f, 1.f, -1.f); // light position
		glUniform3f(6, 6.f, 0.9f, 0.5f); // diffuse
		glUniform3f(9, 6.f, 0.9f, 0.5f); // light specular
		glUniform3f(17, 0.05f, 0.05f, 0.05f); // ambient

		// Measuring performance for section1.5
		// auto startSubmitCodeTimeforTask1_5 = std::chrono::high_resolution_clock::now();
		//Vehicle
		if(isAnimate){
			if (resetAnimation) {
				t = 0.0f;
				resetAnimation = false;
        	}
			float animationSpeed = 0.05f;
			Vec3f tangent = normalize(quadraticBezierTangent(p0, p1, p2, t));
			float angleRadians = atan2(-tangent.x, -tangent.z);
			if (t < 1.0f) { 
				Mat44f model2worldVehicle = make_translation(result) * make_rotation_z(angleRadians) ;
				Mat44f projCameraWorldVehicle = projection * world2camera * model2worldVehicle;
				normalMatrix = mat44_to_mat33(transpose(invert(model2worldVehicle)));
				glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldVehicle.v);
				glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
				lightDirection(lightDir);
				glBindVertexArray(spaceshipVao);
				glDrawArrays(GL_TRIANGLES, 0, spaceshipVertex);
				t += dt * animationSpeed; 
			}
		}else{
			Mat44f model2worldVehicle = make_translation({-20.f, -0.9f, -30.f});
			Mat44f projCameraWorldVehicle = projection * world2camera * model2worldVehicle;
			normalMatrix = mat44_to_mat33(transpose(invert(model2worldVehicle)));
			glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldVehicle.v);
			glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
			lightDirection(lightDir);
			glBindVertexArray(spaceshipVao);
			glDrawArrays(GL_TRIANGLES, 0, spaceshipVertex);
		}
		// auto endSubmitCodeTimeforTask1_5 = std::chrono::high_resolution_clock::now();
		glBindVertexArray(0);
		glUseProgram(0);
		OGL_CHECKPOINT_DEBUG();	

		// CODE FOR QUES1.12: MEASURING PERFORMANCE
		// glEndQuery(GL_TIME_ELAPSED);
		// auto endFrameTime = std::chrono::high_resolution_clock::now();
		
		// // Swap frameIndex for the next iteration
		// frameIndex = 1 - frameIndex;


		// // Wait for the previous query to be available
		// GLuint resultAvailable = 0;
		// while (resultAvailable != GL_TRUE) {
		// 	glGetQueryObjectuiv(queryID[1 - frameIndex], GL_QUERY_RESULT_AVAILABLE, &resultAvailable);
		// }


		// // Get the result of the previous query
		// GLuint64 timeElapsed;
		// glGetQueryObjectui64v(queryID[1 - frameIndex], GL_QUERY_RESULT, &timeElapsed);
		// // auto frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(endFrameTime - startFrameTime).count();


		// //  Calculate frame duration in nanoseconds
		// auto frameDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(endFrameTime - startFrameTime);
		// std::cout << "Frame Duration in Nanoseconds: " << frameDuration.count() << "ms" << std::endl;
		// // Convert frame duration to milliseconds
		// auto frameDurationMillis = std::chrono::duration_cast<std::chrono::milliseconds>(frameDuration);
		// // auto frameDurationMillis = frameDuration*1000000;
		// std::cout << "Frame Duration in Milliseconds: " << frameDurationMillis.count() << "ms" << std::endl;


		// // Add the frame duration to the total in milliseconds
		// totalFrameDuration += frameDurationMillis;


		// // totalFrameDuration += std::chrono::nanoseconds(frameDuration);
		// totalRenderingTime += timeElapsed;
		// auto task12_code_submission = std::chrono::duration_cast<std::chrono::nanoseconds>(endSubmitCodeTimeforTask1_2 - startSubmitCodeTimeforTask1_2);
		// auto task14_code_submission = std::chrono::duration_cast<std::chrono::nanoseconds>(endSubmitCodeTimeforTask1_4 - startSubmitCodeTimeforTask1_4);
		// auto task15_code_submission = std::chrono::duration_cast<std::chrono::nanoseconds>(endSubmitCodeTimeforTask1_5 - startSubmitCodeTimeforTask1_5);


		// //print the time elapsed (in nanoseconds)
		// if (printCounter <= 500) {
		// 	fprintf(stdout, "Frame Duration: %lld ns\n", static_cast<long long>(frameDuration.count()));
		// 	fprintf(stdout, "Rendering Time: %lu ns\n", static_cast<unsigned long>(timeElapsed));
		// 	fprintf(stdout, "Total Rendering Time: %lu ns\n", static_cast<unsigned long>(totalRenderingTime));
		// }
		// if (printCounter > 500) {
		// 	break;
		// }
		// printCounter++;


		// OGL_CHECKPOINT_DEBUG();
		// glfwSwapBuffers( window );
		// std::cout << "time to submit code for 1.2: " << task12_code_submission.count() << "ns" << std::endl;
		// std::cout << "time to submit code for 1.4 " << task14_code_submission.count()<< "ms" << std::endl; 
		// std::cout << "time to submit code for 1.5 " << task15_code_submission.count() << "ns" << std::endl;
	// }


	// auto averageFrameDuration = totalFrameDuration*1000000 / printCounter;
	// auto averageRenderingTime = (totalRenderingTime / printCounter)/1000000;


	// std::cout << "Average from Duration: " << averageFrameDuration.count() << "ns" << std::endl;
	// std::cout << "Average Rendering Time: " << averageRenderingTime<< "ms" << std::endl;   

	} 
	else{
		//Split Screen View
		glViewport(0, 0, fbwidth / 2, fbheight);
		glScissor(0, 0, fbwidth / 2, fbheight);
		drawAssets(prog.programId(), parlahtiVAO, parlahtiVertex, textureID, projCameraWorld, normalMatrix, pad.programId(), landingpadVAO, landingpadVertex, 
		projection,  world2camera);
		// vehicle normals
		glUseProgram(blinn.programId());
		Vec3f lightDir = normalize({ 0.f, 1.f, -1.f });
		glUniform3f(3, 0.2f, 1.f, -1.f); 
		glUniform3f(6, 6.f, 0.9f, 0.5f); 
		glUniform3f(9, 6.f, 0.9f, 0.5f); 
		glUniform3f(17, 0.05f, 0.05f, 0.05f);
		//Vehicle
		if(isAnimate){
			if (resetAnimation) {
				t = 0.0f;
				resetAnimation = false;
        	}
			float animationSpeed = 0.05f;
			Vec3f tangent = normalize(quadraticBezierTangent(p0, p1, p2, t));
			float angleRadians = atan2(-tangent.x, -tangent.z);
			if (t < 1.0f) { 
				Mat44f model2worldVehicle = make_translation(result) * make_rotation_z(angleRadians) ;
				Mat44f projCameraWorldVehicle = projection * world2camera * model2worldVehicle;
				normalMatrix = mat44_to_mat33(transpose(invert(model2worldVehicle)));
				glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldVehicle.v);
				glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
				lightDirection(lightDir);
				glBindVertexArray(spaceshipVao);
				glDrawArrays(GL_TRIANGLES, 0, spaceshipVertex);
				t += dt * animationSpeed; 
			}
		}else{
			Mat44f model2worldVehicle = make_translation({-20.f, -0.9f, -30.f});
			Mat44f projCameraWorldVehicle = projection * world2camera * model2worldVehicle;
			normalMatrix = mat44_to_mat33(transpose(invert(model2worldVehicle)));
			glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldVehicle.v);
			glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
			lightDirection(lightDir);
			glBindVertexArray(spaceshipVao);
			glDrawArrays(GL_TRIANGLES, 0, spaceshipVertex);
		}


		glViewport(fbwidth / 2, 0, fbwidth / 2, fbheight);
		glScissor(fbwidth / 2, 0, fbwidth / 2, fbheight);
		drawAssets(prog.programId(), parlahtiVAO, parlahtiVertex, textureID, projCameraWorld, normalMatrix, pad.programId(), landingpadVAO, landingpadVertex, 
		projection,  world2camera);
		// vehicle normals
		glUseProgram(blinn.programId());
		glUniform3f(3, 0.2f, 1.f, -1.f); 
		glUniform3f(6, 6.f, 0.9f, 0.5f); 
		glUniform3f(9, 6.f, 0.9f, 0.5f); 
		glUniform3f(17, 0.05f, 0.05f, 0.05f); 

		//Vehicle
		if(isAnimate){
			if (resetAnimation) {
				t = 0.0f;
				resetAnimation = false;
        	}
			float animationSpeed = 0.05f;
			Vec3f tangent = normalize(quadraticBezierTangent(p0, p1, p2, t));
			float angleRadians = atan2(-tangent.x, -tangent.z);
			if (t < 1.0f) { 
				Mat44f model2worldVehicle = make_translation(result) * make_rotation_z(angleRadians) ;
				Mat44f projCameraWorldVehicle = projection * world2camera * model2worldVehicle;
				normalMatrix = mat44_to_mat33(transpose(invert(model2worldVehicle)));
				glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldVehicle.v);
				glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
				lightDirection(lightDir);
				glBindVertexArray(spaceshipVao);
				glDrawArrays(GL_TRIANGLES, 0, spaceshipVertex);
				t += dt * animationSpeed; 
			}
		}else{
			Mat44f model2worldVehicle = make_translation({-20.f, -0.9f, -30.f});
			Mat44f projCameraWorldVehicle = projection * world2camera * model2worldVehicle;
			normalMatrix = mat44_to_mat33(transpose(invert(model2worldVehicle)));
			glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldVehicle.v);
			glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
			lightDirection(lightDir);
			glBindVertexArray(spaceshipVao);
			glDrawArrays(GL_TRIANGLES, 0, spaceshipVertex);
		}
	}
		OGL_CHECKPOINT_DEBUG();	
		glfwSwapBuffers( window );
	}

	// Cleanup.
	//TODO: additional cleanup
	state.prog = nullptr;
	state.pad = nullptr;
	state.blinn = nullptr;
	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}

namespace
{
	void glfw_callback_error_(int aErrNum, char const* aErrDesc)
	{
		std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
	}

	void glfw_callback_mouse_button_(GLFWwindow* aWindow, int aButton, int aAction, int)
	{
		if (GLFW_MOUSE_BUTTON_LEFT == aButton && GLFW_PRESS == aAction)
		{
			double xpos, ypos;
			glfwGetCursorPos(aWindow, &xpos, &ypos);

			if (xpos >= 450.0 && xpos <= 580.0 && ypos >= 615.0 && ypos <= 690.0)
			{
				isAnimate = true;
			} else if (xpos >= 705.0 && xpos <= 835.0 && ypos >= 615.0 && ypos <= 690.0){
				isAnimate = false;
				resetAnimation = true;
			}
		}
	}
	void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			//Tracking Camera
			if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.trackCameraActive = true;
				if(state->cameraMode== State_::DEFAULT_CAMERA){
					state->camControl.trackCameraActive = true;
					state->cameraMode= State_::FIXED_DISTANCE_CAMERA;
				}
				else if(state->cameraMode== State_::FIXED_DISTANCE_CAMERA){
					state->camControl.trackCameraActive = true;
					state->cameraMode = State_::GROUND_CAMERA;
				}else{
					state->cameraMode= State_::	DEFAULT_CAMERA;
					state->camControl.cameraActive = true;
				}
			}
			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;
				state->camControl.trackCameraActive = false;
				state->cameraMode = State_::GROUND_CAMERA;
				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			//Split Screen
			if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction)
			{
				splitScreen = !splitScreen;
			}

		// Camera controls WSADEQ, shift and ctrl if camera is active
			if (state->camControl.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveForward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveForward = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveBackward= true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveBackward = false;
				}
				else if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveLeft = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveLeft = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveRight = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveRight = false;
				}
					else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveUp = false;
				}
				else if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.moveDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.moveDown = false;
				}
				else if (GLFW_KEY_LEFT_SHIFT== aKey || GLFW_KEY_RIGHT_SHIFT  == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionSpeedUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionSpeedUp = false;
				}

				else if (GLFW_KEY_LEFT_CONTROL == aKey|| GLFW_KEY_RIGHT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionSlowDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionSlowDown = false;
				}
			}

		//Animation State
		if (GLFW_KEY_F == aKey)
			{
				if (GLFW_PRESS == aAction)
					isAnimate = true;
			}
		else if (GLFW_KEY_R == aKey)
			{
				if (GLFW_PRESS == aAction){
					isAnimate = false;
					resetAnimation = true;
				}
					
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = float(aX - state->camControl.lastX);
				auto const dy = float(aY - state->camControl.lastY);

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}
			state->camControl.lastX = float(aX);
			state->camControl.lastY = float(aY);
		}
	}  

	void lightDirection(Vec3f& lightDir){
		glUniform3f(2, lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05, 0.05f);
	}
	void drawAssets(GLuint programId, GLuint parlahtiVAO, std::size_t parlahtiVertex, GLuint textureID, const Mat44f& projCameraWorld, const Mat33f& normalMatrix,
	GLuint padID, GLuint landingpadVAO, std::size_t landingpadVertex, const Mat44f& projection, const Mat44f& world2camera){
		glUseProgram(programId);
		glBindVertexArray(parlahtiVAO);

		//Bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, 0, parlahtiVertex);

		Vec3f lightDir = normalize({ 0.f, 1.f, -1.f });
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1,1, GL_TRUE, normalMatrix.v);
		lightDirection(lightDir);
		glBindVertexArray(0);
		glUseProgram(0);

		glUseProgram(padID);

		Mat44f model2worldPad = make_translation({10.f, -0.9f, 40.f});
		Mat44f projCameraWorldPad = projection * world2camera * model2worldPad;
		Mat33f padMatrix = mat44_to_mat33(transpose(invert(model2worldPad)));
		glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldPad.v);
		glUniformMatrix3fv(1,1, GL_TRUE, padMatrix.v);
		lightDirection(lightDir);

		//Landing pad 2
		Mat44f model2worldPad2 = make_translation({-20.f, -0.9f, -30.f});
		Mat44f projCameraWorldPad2 = projection * world2camera * model2worldPad2;
		Mat33f padMatrix2  = mat44_to_mat33(transpose(invert(model2worldPad2)));
		glUniformMatrix4fv(0,1, GL_TRUE, projCameraWorldPad2.v);
		glUniformMatrix3fv(1,1, GL_TRUE, padMatrix2.v);
		lightDirection(lightDir);

		// //Bind VAO and Draw array
		glBindVertexArray(landingpadVAO);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVertex);
		glBindVertexArray(landingpadVAO);
		glDrawArrays(GL_TRIANGLES, 0, landingpadVertex);
		glBindVertexArray(0);

		glBindVertexArray(0);
		glUseProgram(0);
	}
}


namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}
