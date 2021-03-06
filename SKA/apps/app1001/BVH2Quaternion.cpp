//-----------------------------------------------------------------------------
// app1001 - Builds with SKA Version 4.0
//-----------------------------------------------------------------------------
// BVH2Quaternion.cpp
// Based on the DataInput code produced by COMP 259 students, fall 2014.
//
// Converts the Euler angles in a BVH motion file to quaternions.
// Stores results in a new file. New file only contains the frame data, not the BVH skeleton.

#include <Core/SystemConfiguration.h>
#include "BVH2Quaternion.h"
#include <Math/Vector3D.h>
#include <Math/Quaternion.h>
#include <vector>
using namespace std;

void convertBVH2Quaternion(string& inputfile, string& outputfile)
{
	cout << "DataInput::readFile" << inputfile << " to " << outputfile << endl;
	ifstream data(inputfile.c_str());
	ofstream output2(outputfile.c_str());
	string line;

	// get us to the euler angles, skip rest of file
	while (!data.eof())
	{
		getline(data, line);
		if (line[0] == 'M') // M for MOTION
		{
			getline(data, line); // skip the Frames: line
			getline(data, line); // skip the Frame Time: line
			break;
		}
	} 
	while (!data.eof())
	{
		Vector3D root_pos, root_rot;
		data >> root_pos.x;
		data >> root_pos.y;
		data >> root_pos.z;
		data >> root_rot.z;
		data >> root_rot.x;
		data >> root_rot.y;

		Quaternion tempQuat;
		tempQuat.fromEuler(root_rot);

		// FUTUREWORK (150618) - should the position be omitted?
		//   MotionGraph seems to assume that only quaterions are in this file.
		//output2 << root_pos.x << " " << root_pos.y << " " << root_pos.z << " ";
		output2 << tempQuat.w << " " << tempQuat.z << " " << tempQuat.x << " " << tempQuat.y << " ";
		
		// FUTUREWORK (150626) - 20 joint count needs to be parameterized.
		//   needs to be coordinated with MotionGraph::fileReader()
		//output2 << root_pos.x << " " << root_pos.y << " " << root_pos.z << " ";
		for (int j = 0; j < 20; j++)
		{
			Vector3D angles;
			data >> angles.z;
			data >> angles.x;
			data >> angles.y;
			Quaternion tempQuat2;
			tempQuat2.fromEuler(angles);
			output2 << tempQuat2.w << " " << tempQuat2.z << " " << tempQuat2.x << " " << tempQuat2.y << " ";
		}
		output2<<endl;
	}
}


