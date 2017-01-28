//-----------------------------------------------------------------------------
// app2001 - Builds with SKA Version 3.1 - Dec 6, 2016 - Michael Doherty
//-----------------------------------------------------------------------------
// AnimationControl.h
//    Animation controller for a Physical Therapy data analysis.
//    This controller is limited to playback of one motion file (one character)
//    at a time.
//-----------------------------------------------------------------------------
#ifndef ANIMATIONCONTROL_DOT_H
#define ANIMATIONCONTROL_DOT_H
// SKA configuration
#include <Core/SystemConfiguration.h>
// C/C++ libraries
#include <list>
using namespace std;
// SKA modules
#include <Objects/Object.h>

class Skeleton;

struct AnimationControl
{
private:
	bool ready;
	float run_time;
	Skeleton* character;
	// flags to control animation
	bool single_step;
	bool freeze;
	float time_warp;
	
	// time control for frame step mode
	float frame_duration; // milliseconds
	long num_frames;
	long current_frame;
	long loop_count;

	// configuration control - not involved in animation
	bool file_path_defined;

public:
	AnimationControl();
	virtual ~AnimationControl();
	// reset() undoes everything that was setup by loadCharacter().
	//   It's not necessary to call it externally, since it is automatically called
	//   when needed by loadCharacter() and ~AnimationControl().
	void reset();

	// loadCharacter() creates a skeleton, attaches the motion in the BVH file and 
	//   sets up the animation control.
	//   Returns false if character creation fails.
	bool loadCharacter(string& BVH_filename);
	// getRenderList() places all the bone objects for the character's skeleton into 
	//   the render list, so that they can be drawn by the graphics subsystem.
	//   Returns false if no character is loaded.
	bool getRenderList(list<Object*>& render_list);

	// updateAnimation() should be called every frame to update all characters.
	// _elapsed_time should be the time (in seconds) since the last frame/update.
	bool updateAnimation(float _elapsed_time);

	// framestepAnimation() is similar to updateAnimation(), except that
	// it assumes that the time step is the recorded frame rate.
	bool framestepAnimation();

	bool isReady()      { return ready; }
	void togglePause()  { freeze = !freeze; }
	void singleStep()   { single_step = true; }
	void slowDown()     { time_warp /= 2.0f; }
	void speedUp()      { time_warp *= 2.0f; }
	void normalSpeed()  { time_warp = 1.0f; }
	bool isFrozen()     { return freeze; }
	float getTimeWarp() { return time_warp; }
	float getAnimationTime() { return run_time; }
	long getAnimationFrame() { return current_frame; }

	bool looped() { return loop_count > 0; }
	long numFrames() { return num_frames;  }

	bool getBonePosition(const char* bonename, Vector3D& start, Vector3D& end);
	bool getBoneOrientation(const char* bonename, Vector3D& orientation);
};

// global single instance of the animation controller
extern AnimationControl anim_ctrl;

#endif // ANIMATIONCONTROL_DOT_H