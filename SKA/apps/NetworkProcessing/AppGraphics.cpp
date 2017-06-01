//-----------------------------------------------------------------------------
// NetworkProcessing project - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// AppGraphics.cpp
//-----------------------------------------------------------------------------
// SKA configuration.
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <cstdlib> // just here to prevent exit() redefinition in glut.h
// openGL library
#include <GL/glut.h>
// SKA modules
#include <Core/Utilities.h>
#include <Graphics/GraphicsInterface.h>
#include <Graphics/Lights.h>
#include <Graphics/Textures.h>
#include <Graphics/Graphics2D.h>
#include <Models/SphereModels.h>
#include <Input/InputManager.h>

// local application
#include "AppConfig.h"
#include "AppGraphics.h"
#include "InputProcessing.h"
#include "CameraControl.h"

//#define LARGER_SCREEN

HudData hud_data;
AnalysisObjects analysis_objects;

// pointers to animated objects that need to be drawn (bones)
list<Object*> character_render_list;

// pointers to background world objects that need to be drawn
static list<Object*> world_render_list;
// pointers to analysis objects that need to be drawn
static list<Object*> analysis_render_list;

// pointers to transparent objects that need to be drawn (drawn last)
static list<Object*> transparent_render_list;

static Color white(1.0f, 1.0f, 1.0f, 1.0f);
static Color black(0.0f, 0.0f, 0.0f, 1.0f);
static Color grey(0.5f, 0.5f, 0.5f, 1.0f);
static Color yellow(1.0f, 1.0f, 0.0f, 1.0f);
static Color red(1.0f, 0.0f, 0.0f, 1.0f);
static Color green(0.0f, 1.0f, 0.0f, 1.0f);
static Color blue(0.0f, 0.0f, 1.0f, 1.0f);

// default window size
#ifdef LARGER_SCREEN
static int window_height = 800;
static int window_width = 800;
#else
static int window_height = 600;
static int window_width = 600;
#endif

// which objects background objects do we want to see?
static bool SHOW_GROUND = false;
static bool SHOW_COORD_AXIS = true;

//  background color (black)
static float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

// heads-up display = 2D text on screen
void drawHUD()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Color base_color(1.0f, 1.0f, 0.0f, 1.0f);
	Color abduction_color = grey;
	Color extension_color = grey;
	Color flexion_color = grey;
	if (hud_data.focus_abduction) abduction_color = white;	
	if (hud_data.focus_extension) extension_color = white;
	if (hud_data.focus_flexion)   flexion_color   = white;

#ifdef LARGER_SCREEN
	float ydiff = -0.05f;
#else
	float ydiff = -0.07f;
#endif
	float y = 0.9f;

	float x = 0.4f;
	string s;

	s = "Time:      " + toString(hud_data.animation_time);
	renderString(x, y, 0.0f, base_color, s.c_str());
	y -= 0.05f;
	s = "Frame:     " + toString(hud_data.animation_frame);
	renderString(x, y, 0.0f, base_color, s.c_str());
	y -= 0.05f;
	if (hud_data.recording)
		s = "RECORDING";
	else
		s = "NOT RECORDING";
	renderString(x, y, 0.0f, base_color, s.c_str());
	y -= 0.05f;

	//s = "Time Warp: " + toString(hud_data.animation_time_warp);
	//renderString(0.5f, y, 0.0f, color, s.c_str());
	//y -= 0.05f;

	if (hud_data.animation_paused)
	{
		s = "Animation Frozen";
		renderString(0.5f, y, 0.0f, base_color, s.c_str());
	}
	y -= 0.05f;


	x = -0.9f;
	y = 0.9f;
	s = "analysis mode: ";
	renderString(x, y, 0.0f, base_color, s.c_str());
	y -= 0.05f;
	if (hud_data.focus_abduction) {
		s = "abduction: ";
		renderString(x, y, 0.0f, abduction_color, s.c_str());
		y -= 0.05f;
		s = "max abduction: ";
		renderString(x, y, 0.0f, abduction_color, s.c_str());
		y -= 0.05f;
	}
	if (hud_data.focus_extension) {
		s = "extension: ";
		renderString(x, y, 0.0f, extension_color, s.c_str());
		y -= 0.05f;
		s = "max extension: ";
		renderString(x, y, 0.0f, extension_color, s.c_str());
		y -= 0.05f;
	}
	if (hud_data.focus_flexion) {
		s = "flexion: ";
		renderString(x, y, 0.0f, flexion_color, s.c_str());
		y -= 0.05f;
		s = "max flexion: ";
		renderString(x, y, 0.0f, flexion_color, s.c_str());
		y -= 0.05f;
	}

#ifdef LARGER_SCREEN
	x = -0.5f;
#else
	x = -0.4f;
#endif
	y = 0.9f;
	renderString(x, y, 0.0f, base_color, hud_data.analysis_mode.c_str());
	y -= 0.05f;
	if (hud_data.recording)
	{
		if (hud_data.focus_abduction) {
			s = toString(hud_data.abduction);
			renderString(x, y, 0.0f, abduction_color, s.c_str());
			y -= 0.05f;
			s = toString(hud_data.max_abduction);
			renderString(x, y, 0.0f, abduction_color, s.c_str());
			y -= 0.05f;
		}
		if (hud_data.focus_extension) {
			s = toString(hud_data.extension);
			renderString(x, y, 0.0f, extension_color, s.c_str());
			y -= 0.05f;
			s = toString(hud_data.max_extension);
			renderString(x, y, 0.0f, extension_color, s.c_str());
			y -= 0.05f;
		}
		if (hud_data.focus_flexion) {
			s = toString(hud_data.flexion);
			renderString(x, y, 0.0f, flexion_color, s.c_str());
			y -= 0.05f;
			s = toString(hud_data.max_flexion);
			renderString(x, y, 0.0f, flexion_color, s.c_str());
			y -= 0.05f;
		}
	}
/*
		s = toString(hud_data.misc[0]) 
			+ string(" ") + toString(hud_data.misc[1])
			+ string(" ") + toString(hud_data.misc[2]);
		renderString(x, y, 0.0f, red, s.c_str());
		y -= 0.05f;
*/
	x = -0.9f;
	y = -0.9f;
	renderString(x, y, 0.0f, base_color, hud_data.take_label.c_str());
	y -= 0.05f;
}

typedef void(*void_callback)(void);
void_callback anim_callback;

// display() is the call back function from the openGL rendering loop.
// All recurring processing is initiated from this function.
void display(void)
{
	// call back to the animation module
	anim_callback();

	// Set up openGL to draw next frame.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.setSceneView();
	glMatrixMode(GL_MODELVIEW);

	// identity transform 
	Matrix4x4 world_xform;

	// draw background objects
	list<Object*>::iterator witer = world_render_list.begin();
	while (witer != world_render_list.end())
	{
		if ((*witer)->isVisible()) (*witer)->render(world_xform);
		witer++;
	}

	// draw background objects
	list<Object*>::iterator aiter = analysis_render_list.begin();
	while (aiter != analysis_render_list.end())
	{
		if ((*aiter)->isVisible()) (*aiter)->render(world_xform);
		aiter++;
	}

	// draw character objects
	if (analysis_objects.show_skeleton)
	{
		list<Object*>::iterator aiter = character_render_list.begin();
		while (aiter != character_render_list.end())
		{
			if ((*aiter)->isVisible()) (*aiter)->render(world_xform);
			aiter++;
		}
	}

	// draw transparent objects
	list<Object*>::iterator tpiter = transparent_render_list.begin();
	while (tpiter != transparent_render_list.end())
	{
		if ((*tpiter)->isVisible())	(*tpiter)->render(world_xform);
		tpiter++;
	}

	// draw the heads-up display
	drawHUD();

	// Activate the new frame.
	glutSwapBuffers();

	// Record any redering errors.
	checkOpenGLError(203);
}

void buildWorldObjects()
{
	if (SHOW_GROUND)
	{
		ModelSpecification groundspec("Ground");
		world_render_list.push_back(new Object(groundspec,
			Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), Vector3D(1.0f, 1.0f, 1.0f)));
	}
	if (SHOW_COORD_AXIS)
	{
		ModelSpecification caxisspec("CoordinateAxis");
		caxisspec.addSpec("length", "100");
		world_render_list.push_back(new Object(caxisspec,
			Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f)));
	}
}

void buildAnalysisObjects()
{
	// PTData display objects
	ModelSpecification yellow_box("Box", Color(1.0f, 1.0f, 0.0f));
	yellow_box.addSpec("length", "1"); yellow_box.addSpec("width", "1"); yellow_box.addSpec("height", "1");
	ModelSpecification purple_box("Box", Color(1.0f, 0.0f, 1.0f));
	purple_box.addSpec("length", "1"); purple_box.addSpec("width", "1"); purple_box.addSpec("height", "1");
	ModelSpecification aqua_box("Box", Color(0.0f, 1.0f, 1.0f));
	aqua_box.addSpec("length", "1"); aqua_box.addSpec("width", "1"); aqua_box.addSpec("height", "1");
	ModelSpecification red_box("Box", Color(1.0f, 0.0f, 0.0f));
	red_box.addSpec("length", "1"); red_box.addSpec("width", "1"); red_box.addSpec("height", "1");
	ModelSpecification blue_box("Box", Color(0.0f, 0.0f, 1.0f));
	blue_box.addSpec("length", "1"); blue_box.addSpec("width", "1"); blue_box.addSpec("height", "1");
	ModelSpecification green_box("Box", Color(0.0f, 1.0f, 0.0f));
	green_box.addSpec("length", "1"); green_box.addSpec("width", "1"); green_box.addSpec("height", "1");

	analysis_render_list.push_back(analysis_objects.root_marker = new Object(aqua_box));
	analysis_render_list.push_back(analysis_objects.upperarm_marker = new Object(yellow_box));
	analysis_render_list.push_back(analysis_objects.elbow_marker = new Object(yellow_box));
	analysis_render_list.push_back(analysis_objects.upperarm_cp_marker = new Object(red_box));
	analysis_render_list.push_back(analysis_objects.elbow_cp_marker = new Object(red_box));
	analysis_render_list.push_back(analysis_objects.upperarm_sp_marker = new Object(green_box));
	analysis_render_list.push_back(analysis_objects.elbow_sp_marker = new Object(green_box));
	analysis_render_list.push_back(analysis_objects.spine_cp_marker = new Object(blue_box));
	analysis_render_list.push_back(analysis_objects.spine_sp_marker = new Object(blue_box));

	analysis_objects.coronal_plane_image = new Object(new XYPlaneModel(40, 10, Color(0.7f, 0.7f, 0.0f, 0.5f)),
		Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f));
	analysis_objects.sagittal_plane_image = new Object(new YZPlaneModel(40, 10, Color(0.0f, 0.7f, 0.7f, 0.5f)),
		Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f));
	analysis_objects.transverse_plane_image = new Object(new ZXPlaneModel(40, 10, Color(0.7f, 0.0f, 0.7f, 0.5f)),
		Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f));
	transparent_render_list.push_back(analysis_objects.coronal_plane_image);
	transparent_render_list.push_back(analysis_objects.sagittal_plane_image);
	transparent_render_list.push_back(analysis_objects.transverse_plane_image);

	ModelSpecification bonespec("Bone", Color(1.0f, 0.0f, 0.0f));
	analysis_objects.arm_cp_bone = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 5.0f);
	analysis_objects.arm_cp_bone->drawAsMesh();
	analysis_objects.arm_cp_bone->setEndpoints(Vector3D(5.0f, 20.0f, 5.0f), Vector3D(10.0f, 20.0f, 10.0f));
	analysis_objects.arm_cp_bone->drawAsLine();
	analysis_objects.arm_sp_bone = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 5.0f);
	analysis_objects.arm_sp_bone->drawAsMesh();
	analysis_objects.arm_sp_bone->setEndpoints(Vector3D(5.0f, 21.0f, 5.0f), Vector3D(10.0f, 20.0f, 10.0f));
	analysis_objects.arm_sp_bone->drawAsLine();
	analysis_objects.spine_cp_bone = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 5.0f);
	analysis_objects.spine_cp_bone->drawAsMesh();
	analysis_objects.spine_cp_bone->setEndpoints(Vector3D(5.0f, 22.0f, 5.0f), Vector3D(10.0f, 20.0f, 10.0f));
	analysis_objects.spine_cp_bone->drawAsLine();
	analysis_objects.spine_sp_bone = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 5.0f);
	analysis_objects.spine_sp_bone->drawAsMesh();
	analysis_objects.spine_sp_bone->setEndpoints(Vector3D(5.0f, 23.0f, 5.0f), Vector3D(10.0f, 20.0f, 10.0f));
	analysis_objects.spine_sp_bone->drawAsLine();

	analysis_objects.right_dir_marker = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 20.0f);
	analysis_objects.right_dir_marker->drawAsLine();
	analysis_objects.forward_dir_marker = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 20.0f);
	analysis_objects.forward_dir_marker->drawAsLine();
	analysis_objects.up_dir_marker = new BoneObject(bonespec, Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f), 20.0f);
	analysis_objects.up_dir_marker->drawAsLine();

	analysis_render_list.push_back(analysis_objects.arm_cp_bone);
	analysis_render_list.push_back(analysis_objects.arm_sp_bone);
	analysis_render_list.push_back(analysis_objects.spine_cp_bone);
	analysis_render_list.push_back(analysis_objects.spine_sp_bone);
	analysis_render_list.push_back(analysis_objects.right_dir_marker);
	analysis_render_list.push_back(analysis_objects.forward_dir_marker);
	analysis_render_list.push_back(analysis_objects.up_dir_marker);
}

void buildObjects()
{
	texture_manager.addTextureFilepath((char*)TEXTURE_FILE_PATH);
	buildWorldObjects();
	buildAnalysisObjects();
}

// reshape() is a call back from openGL to indicate that the window has 
// changed size. Adjust the camera and view parameters accordingly.
static void reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	window_width = w; window_height = h;
	glViewport(0, 0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.projectionParameters(
		40.0,									// vertical FOV
		float(window_width) / window_height,	// aspect ratio
		1.0,									// near plane
		1000.0);								// far plane
}

// launchRenderer() is called one time during initialization.
void launchRenderer(int argc, char **argv, void(*animation_callback)(void))
{
	anim_callback = animation_callback;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Animation");

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

	// experiments 161223
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// The input manager needs to have register call back function with GLUT
	// to receive keyboard and mouse actions. This needs to happen after GLUT
	// initialization, so it is implemented in a member functions, rather than
	// the InputManager constructor.
	input_manager.registerGlutHandlers();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	// Call into the SKA Graphics module to select the default lights.
	initializeDefaultLighting();

	// Application specific initialization of the camera/viewpoint.
	camera.initializeCamera(window_width, window_height);

	// construct background objects
	buildObjects();

	// Record any openGL errors.
	checkOpenGLError(202);

	glutMainLoop();
}
