// CMPUT 411 Assignment 2
// Usage: ./motionViewer <skeleton.bvh>

/*
  Much of this code is taken from Dale Schuurmans CMPUT 411 Assignment 1 solution
*/

#ifndef __APPLE__
#  include <GL/glew.h>
#endif
#include <GL/freeglut.h>

#include "camera.h"
#include "skeleton.h"


int main(int, char**);
void setup(char*);
void drawScene(void);
void resize(int, int);
void keyInput(unsigned char, int, int);
void specialKeyInput(int, int, int);
static void timerCallback(int);
void playAnim();
void pauseAnim();
void increaseSpeed(float);
void decreaseSpeed(float);
void resetSpeed();

/* global variables */
skeleton skele;
camera cam;
Vector3f initialPosition(0.0, 0.0, -50.0);
int frame = 0;
int initfps = 120;
int updatedfps = 120;
int fps = 120;
bool revAnim = false;


int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitContextVersion(4, 2);
  glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("motionViewer");
  
  glutDisplayFunc(drawScene);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyInput);
  glutTimerFunc(1000/fps, timerCallback,1);
  setup(argv[1]);
  glutMainLoop();
  return 0;
}


void setup(char* fileName)
{
  skele.color[0] = skele.color[1] = skele.color[2] = skele.color[3] = 1.0;
  skele.readBvhFile(fileName);
  skele.setVertices(skele.rootJoint);
  // skele.normalize();
  // skele.glCreateDisplayList();
  cam.initialize(persp, -1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
}


void drawScene(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skele.glColor();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  cam.glVolume(); // camera view volume

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  cam.glPosition(); // camera transformation
  
  glTranslatef(initialPosition[0],initialPosition[1],initialPosition[2]);

  skele.setVertices(skele.rootJoint);
  skele.moveTo(frame);
  if (revAnim){
    frame--;
    if (frame == 0){
      frame = skele.motionData.num_frames;
    }
  } else {
    frame++;
    if (frame == skele.motionData.num_frames){
      frame = 0;
    }
  }
  

  skele.drawBone(skele.rootJoint);



  // // draw model
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // skele.glCallDisplayList();
  glutSwapBuffers();
}

static void timerCallback(int value)
{
  if (fps != 0){
    glutPostRedisplay();
    glutTimerFunc(1000/fps, timerCallback,1);
  }
}

void playAnim()
{
  fps = updatedfps;
  glutTimerFunc(1000/fps, timerCallback,1);
}

void pauseAnim()
{
  fps = 0;
}

void increaseSpeed(float inc)
{
  fps += inc;
  if (fps == 10.0){
    // turn timer back on
    glutTimerFunc(1000/fps, timerCallback,1);
  }
  cout << "updating speed: " << fps << endl;
  updatedfps = fps;
}

void decreaseSpeed(float dec)
{
  if (revAnim)
    fps = -fps;
  fps -= dec;
  cout << "updating speed: " << fps << endl;
  if (fps <= 0){
    fps = -fps;
    revAnim = true;
    if (fps == 10.0) {
      // turn timer back on
      glutTimerFunc(1000/fps, timerCallback,1);
    }
  } else {
    revAnim = false;
  }
  updatedfps = fps;
}

void resetSpeed()
{
  fps = initfps;
  updatedfps = fps;
}


void resize(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}


void keyInput(unsigned char key, int x, int y)
{
  switch(key) 
  {
    case 'q': exit(0);            break; // quit
    case 'w': skele.writeBvhFile("output.bvh"); break;
    case 'p': playAnim(); break;
    case 'P': pauseAnim(); break;
    case 'd': cam.xTransl(1.0);   break;
    case 'D': cam.xTransl(-1.0);  break;
    case 'c': cam.yTransl(1.0);   break;
    case 'C': cam.yTransl(-1.0);  break;
    case 'z': cam.zTransl(1.0);   break;
    case 'Z': cam.zTransl(-1.0);  break;
    case 't': cam.xRotate(10.0);  break;
    case 'T': cam.xRotate(-10.0); break;
    case 'a': cam.yRotate(10.0);  break;
    case 'A': cam.yRotate(-10.0); break;
    case 'l': cam.zRotate(10.0);  break;
    case 'L': cam.zRotate(-10.0); break;
    case 'x': // reset
      resetSpeed();
      frame = 0; // reset frame
      cam.initialize(persp, -1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
      break;
    case '-': decreaseSpeed(10.0); break;
    case '+': increaseSpeed(10.0); break;
    default: break;
  }
  glutPostRedisplay();
}

