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


/* global variables */
skeleton skele;
camera cam;
Vector3f initialPosition(0.0, 0.0, -2.0);


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
  setup(argv[1]);
  glutMainLoop();
  return 0;
}


void setup(char* fileName)
{
  skele.readBvhFile(fileName);
  // skele.normalize();
  // skele.glCreateDisplayList();
  cam.initialize(persp, -1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
}


void drawScene(void)
{
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // cam.glVolume(); // camera view volume

  // glMatrixMode(GL_MODELVIEW);
  // glLoadIdentity();
  // cam.glPosition(); // camera transformation
  // skele.glPosition(initialPosition); // object transformation

  // if (fog) { // set fog
  //   glEnable(GL_FOG);
  //   glHint(GL_FOG_HINT, GL_NICEST);
  //   glFogfv(GL_FOG_COLOR, fogColor);
  //   glFogi(GL_FOG_MODE, GL_LINEAR);
  //   glFogi(GL_FOG_START, 1.0);
  //   glFogi(GL_FOG_END, 5.0);
  // } else
  //   glDisable(GL_FOG);

  // // draw model
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // skele.glColor();
  // skele.glCallDisplayList();
  // glutSwapBuffers();
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
    case 'p': skele.playAnim(); break;
    case 'P': skele.pauseAnim(); break;
    case 'd': cam.xTransl(0.1);   break;
    case 'D': cam.xTransl(-0.1);  break;
    case 'c': cam.yTransl(0.1);   break;
    case 'C': cam.yTransl(-0.1);  break;
    case 'z': cam.zTransl(0.1);   break;
    case 'Z': cam.zTransl(-0.1);  break;
    case 't': cam.xRotate(10.0);  break;
    case 'T': cam.xRotate(-10.0); break;
    case 'a': cam.yRotate(10.0);  break;
    case 'A': cam.yRotate(-10.0); break;
    case 'l': cam.zRotate(10.0);  break;
    case 'L': cam.zRotate(-10.0); break;
    case 'x': // reset
      skele.reset();
      cam.initialize(persp, -1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
      break;
    case '-': skele.decreaseSpeed(10.0); break;
    case '+': skele.increaseSpeed(10.0); break;
    default: break;
  }
  glutPostRedisplay();
}

