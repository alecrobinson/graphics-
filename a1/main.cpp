// Asteroids game


#include <sys/timeb.h>

#include "headers.h"
#include "gpuProgram.h"
#include "world.h"
#include "main.h"


GPUProgram *myGPUProgram;	// pointer to GPU program object

World *world;			// the world, including ship, asteroids, other ships, missiles

KeyState upKey    = UP;		// state of the arrow keys
KeyState downKey  = UP;
KeyState leftKey  = UP;
KeyState rightKey = UP;


// Display the world


void display()

{
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT );

  world->draw();

  glutSwapBuffers();  
}


// Idle function to update the world state
//
// We keep track of the last time this function was called so that we
// know the elapsed time between calls.


void idle()

{
  static bool firstTime = true;
  static timeb prevTime;

  if (firstTime) {

    // Record the time of the first call

    ftime( &prevTime );
    firstTime = false;

  } else {

    // Find the time different from the previous call

    struct timeb thisTime;
    ftime( &thisTime );

    float elapsedSeconds = (thisTime.time + thisTime.millitm / 1000.0) - (prevTime.time + prevTime.millitm / 1000.0);

    prevTime = thisTime;

    // Update the world state

    world->updateState( elapsedSeconds );

    glutPostRedisplay();
  }

  // Do nothing for a bit

#ifndef _WIN32
  usleep( 1000 );
#endif
}


// Handle a keypress and record state of arrows


void keyPress( unsigned char key, int x, int y )

{
  if (key == 27)		// quit upon ESC
    exit(0);
  else if (key == 's')
    world->start();
  else if (key == 'p')
    world->togglePause();
  else if (key == ' ')
    world->shipFires();
}


void specialKeyPress( int key, int x, int y )

{
  switch (key) {
  case GLUT_KEY_UP:    upKey    = DOWN; break;
  case GLUT_KEY_DOWN:  downKey  = DOWN; break;
  case GLUT_KEY_LEFT:  leftKey  = DOWN; break;
  case GLUT_KEY_RIGHT: rightKey = DOWN; break;
  }
}

  
void specialKeyRelease( int key, int x, int y )

{
  switch (key) {
  case GLUT_KEY_UP:    upKey    = UP; break;
  case GLUT_KEY_DOWN:  downKey  = UP; break;
  case GLUT_KEY_LEFT:  leftKey  = UP; break;
  case GLUT_KEY_RIGHT: rightKey = UP; break;
  }
}

  
// Main program


int main( int argc, char **argv )

{
  // Set up GLUT and GLEW

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize( SCREEN_ASPECT * SCREEN_WIDTH, SCREEN_WIDTH );
  glutInitWindowPosition( 0, 0 );
  glutCreateWindow( "demo" );

  GLenum status = glewInit();
  if (status != GLEW_OK) {
    cerr << "Error: " << glewGetErrorString(status) << endl;
    return 1;
  }

  glutDisplayFunc( display );
  glutIdleFunc( idle );
  glutKeyboardFunc( keyPress );
  glutSpecialFunc( specialKeyPress );
  glutSpecialUpFunc( specialKeyRelease );
  glutIgnoreKeyRepeat( 1 );

  // Set up shaders

  myGPUProgram = new GPUProgram( "asteroids.vert", "asteroids.frag" );
  myGPUProgram->activate();

  // Seed the random number generator

  timeb time;
  ftime( &time );
  srand( time.time );

  // Set up world

  world = new World( WORLD_MIN, WORLD_MAX );

  // Run

  glutMainLoop();

  return 1;
}


// A bug in some GL* library in Ubuntu 14.04 requires that libpthread
// be forced to load.  The code below accomplishes this (from MikeMx7f
// http://stackoverflow.com/questions/31579243/segmentation-fault-before-main-when-using-glut-and-stdstring).

#ifdef LINUX

#include <pthread.h>

void* simpleFunc(void*) { return NULL; }

void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }

#endif
