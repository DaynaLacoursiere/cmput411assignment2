/*
  Much of this code is taken from Dale Schuurmans CMPUT 411 Assignment 1 solution
  Also some from http://www.gamedev.net/page/resources/_/technical/game-programming/bvh-file-loading-and-displaying-r3295
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cfloat>
#include <cmath>
#include <vector>
#include <Eigen/Dense>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifndef __APPLE__
#  include <GL/glew.h>
#endif
#include <GL/freeglut.h>

#ifndef SKELETON_H
#define SKELETON_H

#define DEG2RAD_SKEL 0.017453292519943 /* rad = deg*PI/180 = deg*DEG2RAD */
#define RAD2DEG_SKEL 57.29577951308232 /* deg = rad*180/PI = rad*RAD2DEG */

enum Channel { XPOS, YPOS, ZPOS, XROT, YROT, ZROT};

using namespace std;
using namespace Eigen;

typedef struct JOINT JOINT;

struct JOINT
{
  const char* name = NULL;
  JOINT* parent = NULL;
  int joint_type; // 0 = root, 1 = node, 2 = leaf
  unsigned int num_channels = 0;
  int * channels_order = NULL;
  std::vector<JOINT*> children;
  Vector3f offset;
  glm::mat4 matrix;
  unsigned int channel_start = 0;
};

typedef struct
{
  JOINT* rootJoint;
  unsigned int num_channels;
} HIERARCHY;

typedef struct
{
  unsigned int num_frames;
  float frame_time;
  unsigned int num_motion_channels = 0;
  float* data = NULL;
  unsigned* joint_channel_offsets;
} MOTION;


struct skeleton
{
  JOINT* rootJoint;
  MOTION motionData;

  vector<glm::vec4> vertices;
  vector<GLshort> indices;
  // member functions


  void reset()
  {
    restartAnim();
    pauseAnim();
    resetSpeed();
  }

  // plays animation
  void playAnim()
  {

  }

  // pauses animation
  void pauseAnim()
  {

  }

  // restarts animation (and coninues playing)
  void restartAnim()
  {

  }

  // increase speed of animation
  void increaseSpeed(float inc)
  {
    // speed += inc
  }

  // decrease speed of animation
  void decreaseSpeed(float dec)
  {
    // speed -= dec
  }

  // reset speed of animation
  void resetSpeed()
  {

  }



  // other member functions defined in skeleton.cpp
  void readBvhFile(char*);
  JOINT* loadJoint(istream & str, JOINT* parent, int type);
  void loadMotion(istream & str);
  void writeBvhFile(char*);
  int writeJoint(ofstream & str, JOINT* joint, int tabs);
  void writeMotion(ofstream & str);

  void moveJoint (JOINT * joint, MOTION * motionData, unsigned int frame_starts_index);
  void moveTo(unsigned int frame);

  void setVertices(JOINT * joint, GLshort parenIndex = 0);

};

#endif