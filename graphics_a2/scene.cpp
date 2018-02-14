/* scene.C
 */


#include "headers.h"
#ifndef WIN32
  #include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scene.h"
#include "rtWindow.h"
#include "sphere.h"
#include "triangle.h"
#include "volume.h"
#include "light.h"
#include "font.h"
#include "main.h"


#ifndef MAXFLOAT
  #define MAXFLOAT 9999999
#endif

vector backgroundColour(1,1,1);
vector blackColour(0,0,0);


#define NUM_SOFT_SHADOW_RAYS 50


// Find the first object intersected

bool Scene::findFirstObjectInt( vector rayStart, vector rayDir, int thisObjIndex,
				vector &P, vector &N, float &param, int &objIndex )

{
  if (storingRays)
    storedRays.add( rayStart );

  param = MAXFLOAT;

  for (int i=0; i<objects.size(); i++)
    if (i != thisObjIndex) {	// don't check for int with the originating object
      vector point, normal;
      float t;
      if (objects[i]->rayInt( rayStart, rayDir, point, normal, t ))
	if (0 < t && t < param) {
	  param = t;
	  P = point;
	  N = normal;
	  objIndex = i;
	}
    }
    
  if (storingRays) {

    // Is this ray aiming for a light?

    bool foundLight = false;
    
    for (int j=0; j<lights.size(); j++) {
      float lightToRayDist = ((lights[j]->position - rayStart) - ((lights[j]->position - rayStart) * rayDir) * rayDir).length();
      if (lightToRayDist < 0.001) {
	foundLight = true;
	break;
      }
    }

    if (param != MAXFLOAT) {
      storedRays.add( P );
      if (foundLight)
	storedRayColours.add( vector(1,0,0) ); // RED: shadow ray toward a light
      else
	storedRayColours.add( vector(0,1,1) ); // CYAN: normal ray that hits something
    } else {
      storedRays.add( rayStart+2*rayDir );
      if (foundLight)
	storedRayColours.add( vector(1,0,0) ); // RED: shadow ray toward a light
      else
	storedRayColours.add( vector(1,1,0) ); // YELLOW: normal ray that misses
    }
  }

  return (param != MAXFLOAT);
}

// Raytrace: This is the main raytracing routine which finds the first
// object intersected, performs the lighting calculation, and does
// recursive calls.
//
// This returns the colour received on the ray.

vector Scene::raytrace(vector &rayStart, vector &rayDir, int depth, int thisObjIndex)

{
	// Terminate the ray?

	depth++;

	if (depth > maxDepth)
		return blackColour;

	// Find the closest object intersected

	vector P, N;
	float  t;
	int    objIndex;

	bool found = findFirstObjectInt(rayStart, rayDir, thisObjIndex, P, N, t, objIndex);

	// No intersection: Return background colour

	if (!found)
		if (depth == 1)
			return backgroundColour;
		else
			return blackColour;

	// Find incoming illumination (Iin) from the ideal reflection direction

	Object &obj = *objects[objIndex];

	vector Iin(0, 0, 0);

	// --- YOUR CODE HERE (Part A1) ---

	//AR basic equation for finding the reflection direction
	/*
	//need to reverse rayDir first
	vector reverse_rayDir = -1 * rayDir;


	vector reflection_direction = (2 * (reverse_rayDir * N)) * N - reverse_rayDir;*/

	vector rayDir_reflection = rayDir - (2 * (rayDir * N)) * N;


	//normalize reflection_direction
	rayDir_reflection = rayDir_reflection.normalize();




	//AR recursively call RayTrace
	//PROBLEM IS HERE why is the recursive call to raytrace returning (0,0,0)

	Iin = raytrace(P, rayDir_reflection, depth, objIndex);




	float  alpha;
	vector colour = obj.textureColour(P, alpha);
	vector kd = vector(colour.x*obj.mat->kd.x, colour.y*obj.mat->kd.y, colour.z*obj.mat->kd.z);

	// Calculate the contribution of Iin to the illumination returning to the eye (Iout)

	vector Iout = obj.mat->Ie + vector(obj.mat->ka.x * Ia.x, obj.mat->ka.y * Ia.y, obj.mat->ka.z * Ia.z);
	//AR           emissive   + ambient illumination ------------------------------------------------------
	// --- YOUR CODE HERE (Part A2) ---


	//need specular reflection and diffuse reflection
	//Iin = vector(0.5, 0.5, 0.5);

	vector specular_diffuse_reflection = calcIout(N, rayDir_reflection, -1 * rayDir, -1 * rayDir, kd, obj.mat->ks, obj.mat->n, Iin);

	Iout = Iout + specular_diffuse_reflection;
	//AR why doesn't the code break here?
	/*
	vector Scene::calcIout( vector N, vector L, vector E, vector R,
	vector Kd, vector Ks, float ns,
	vector In )
	L - direction of incoming light
	E - light exit direction (reflected to?) it is V in the notes
	R - reflection direction
	Kd - diffuse reflection coefficient
	Ks - specular coefficient
	ns - material property
	In - Illumination in - total light from other sources, how to get?
	*/
	// --- YOUR CODE HERE (Part B for point lights) ---

	// Add contributions from lights

	vector point_light_contribution;
	vector sum_point_light_contribution;
	vector towards_eye = eye->position - P;
	towards_eye = towards_eye.normalize();


	for (int i = 0; i < lights.size(); i++) {

		// --- YOUR CODE HERE (Part B for point lights) ---


		// direction to light
		vector lightDir = (light.position - P).normalize();


		vector lightReflectDir = lightDir - ((N * lightDir) * 2) * N;

		// vector lightIin = raytrace(P, lightReflectDir, depth, objIndex);

		float t1;
		if (findFirstObjectInt(rayStart, rayDir, thisObjIndex, P, N, t1, objIndex)) {}

		float t2;
		if (objects[i]->rayInt(rayStart, lightDir, P, N, t2)) {}


		vector lightIin;

		if (t1 < t2)
			lightIin = vector(0, 0, 0);
		else
			lightIin = light.colour;

		Iout = Iout + calcIout(N, lightDir, rayDir, lightReflectDir, obj.mat->kd, obj.mat->ks, obj.mat->n, lightIin);
	}
		

	}

	//Iout = Iout + sum_point_light_contribution;

	// --- YOUR CODE HERE (Part B for area lights) ---
	//...
	//

	return Iout;
	//return vector(1, 0, 0);
}


// Calculate the outgoing intensity due to light Iin entering from
// direction L and exiting to direction E, with normal N.  Reflection
// direction R is provided, along with the material properties Kd, 
// Ks, and n.
//
//       Iout = Iin * ( Kd (N.L) + Ks (R.V)^n )

vector Scene::calcIout( vector N, vector L, vector E, vector R,
			vector Kd, vector Ks, float ns,
			vector In )

{
  // Don't illuminate from the back of the surface

  if (N * L <= 0)
    return blackColour;

  // Both E and L are in front:

  vector Id = (L*N) * In;

  vector Is;

  if (R*E < 0)
    Is = blackColour;		// Is = 0 from behind the
  else				// reflection direction
    Is = pow( R*E, ns ) * In;

  return vector( Is.x*Ks.x+Id.x*Kd.x, Is.y*Ks.y+Id.y*Kd.y, Is.z*Ks.z+Id.z*Kd.z );
}


// Determine the colour of one pixel.  This is where
// the raytracing actually starts.

vector Scene::pixelColour( int x, int y )

{
  vector result;

#if 0
  if (win->tracePixel && win->pixelToTrace.x == x && win->pixelToTrace.y == y) {
    storedRays.clear();
    storedRayColours.clear();
    storingRays = true;
  }
#endif

#if 0

  const int n = 2;
  int count = 0;
  vector sum(0,0,0);

  for (float dx=0; dx<0.999; dx += 1/(float)n)
    for (float dy=0; dy<0.999; dy += 1/(float)n) {
      vector dir = (llCorner + (x+dx+0.5/(float)n) * right + (y+dy+0.5/(float)n) * up).normalize();
      sum = sum + raytrace( eye->position, dir, 0, -1 );
      count++;
    }

  result = (1.0/(float)count) * sum;

#else

  vector dir = (llCorner + x * right + y * up).normalize();
  result = raytrace( eye->position, dir, 0, -1 );

#endif

  if (storingRays) {
    storingRays = false;
    // win->tracePixel = false;
  }

  return result;
}


// Read the scene from an input stream

void Scene::read( istream &in )

{
  char command[1000];

  while (in) {

    skipComments( in );
    in >> command;
    if (!in || command[0] == '\0')
      break;
    
    skipComments( in );

    if (strcmp(command,"sphere") == 0) {

      Sphere *o = new Sphere();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"triangle") == 0) {

      Triangle *o = new Triangle();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"volume") == 0) {

      Volume *o = new Volume();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"material") == 0) {

      Material *m = new Material();
      in >> *m;
      materials.add( m );
      
    } else if (strcmp(command,"light") == 0) {

      Light *o = new Light();
      in >> *o;
      lights.add( o );
      
    } else if (strcmp(command,"eye") == 0) {

      eye = new Eye();
      in >> *eye;

      win->eye = eye->position;
      win->lookat = eye->lookAt;
      win->updir = eye->upDir;
      win->fovy = eye->fovy * 180.0/3.1415926;
      
    } else {
      
      cerr << "Command '" << command << "' not recognized" << endl;
      exit(-1);
    }
  }
  
  sceneLoaded = true;
}



// Output the whole scene (mainly for debugging the reader)


void Scene::write( ostream &out )

{
  out << *eye << endl;

  for (int i=0; i<lights.size(); i++)
    out << *lights[i] << endl;

  for (int i=0; i<materials.size(); i++)
    out << *materials[i] << endl;

  for (int i=0; i<objects.size(); i++)
    out << *objects[i] << endl;
}


// Draw the scene.  This sets things up and simply
// calls pixelColour() for each pixel.

void Scene::renderRT( bool restart )

{
  if (!sceneLoaded)
    return;

  static bool stop = true;
  static float nextDot;
  static int nextx, nexty;

  if (restart) {

    // Copy the window eye into the scene eye

    eye->position = win->eye;
    eye->lookAt = win->lookat;
    eye->upDir = win->updir;
    eye->fovy = win->fovy * 3.1415926/180.0 ;

    // Set up the window

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_COLOR_MATERIAL );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, win->xdim-0.99, 0, win->ydim-0.99, 0, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Compute the image plane coordinate system

    up = (2 * tan(eye->fovy/2.0)) * eye->upDir.normalize();

    right = (2 * tan(eye->fovy/2.0) * (float) win->xdim / (float) win->ydim)
      * ((eye->lookAt - eye->position) ^ eye->upDir).normalize();
  
    llCorner = (eye->lookAt - eye->position).normalize()
      - 0.5 * up - 0.5 * right;

    up = (1.0 / (float) (win->ydim-1)) * up;
    right = (1.0 / (float) (win->xdim-1)) * right;

    if (nextDot != 0) {
      cout << "\r           \r";
      cout.flush();
    }

    nextx = 0;
    nexty = 0;
    nextDot = 0;

    stop = false;
  }

  if (stop) {
#ifndef WIN32
    usleep(1000);
#endif
    return;
  }

  // Draw the next pixel

  vector colour = pixelColour( nextx, nexty );

  glBegin( GL_POINTS );
  glColor3fv( (GLfloat *) &colour.x );
  glVertex2i( nextx, nexty );
  glEnd();

  // Move (nextx,nexty) to the next pixel

  nexty++;
  if (nexty >= win->ydim) {
    nexty = 0;
    nextx++;
    if ((float)nextx/(float)win->xdim >= nextDot) {
      cout << "."; cout.flush(); nextDot += 0.1;
    }
    if (nextx >= win->xdim) {
      char buffer[1000];
      sprintf( buffer, "depth %d", maxDepth );
      glColor3f(1,1,1);
      printString( buffer, 10, 10, win->xdim, win->ydim );

      // Draw any stored rays (for debugging)

      if (storedRays.size() > 0) {
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	win->setupProjection();
	glColor3f( 0, 1, 1 );
	glBegin( GL_LINES );
	for (int i=0; i<storedRays.size(); i+=2) {
	  glColor3fv( (GLfloat*) &storedRayColours[i/2].x );
	  glVertex3fv( (GLfloat*) &storedRays[i].x );
	  glVertex3fv( (GLfloat*) &storedRays[i+1].x );
	}
	glEnd();
      }

      glFlush();
      glutSwapBuffers();
      nextx = 0;
      stop = true;
      cout << "\r           \r";
      cout.flush();
    }
  }
}


// Render the scene with OpenGL


void Scene::renderGL()

{
  if (!sceneLoaded)
    return;

  // Copy the window eye into the scene eye

  eye->position = win->eye;
  eye->lookAt = win->lookat;
  eye->upDir = win->updir;
  eye->fovy = win->fovy * 3.1415926/180.0 ;

  // Set up the framebuffer

  glClearColor( backgroundColour.x, backgroundColour.y, backgroundColour.z, 1 );
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );
  glPolygonMode( GL_FRONT, GL_FILL );
  glPolygonMode( GL_BACK, GL_LINE );


  glDisable( GL_LIGHTING );

  // Draw any stored points (for debugging)

  glPointSize( 4.0 );
  glBegin( GL_POINTS );
  for (int i=0; i<storedPoints.size(); i++)
    glVertex3fv( (GLfloat*) &storedPoints[i].x );
  glEnd();

  // Draw any stored rays (for debugging)

  if (storedRays.size() > 0) {

    glColor3f( 0, 1, 1 );
    glBegin( GL_LINES );
    for (int i=0; i<storedRays.size(); i+=2) {
      glColor3fv( (GLfloat*) &storedRayColours[i/2].x );
      glVertex3fv( (GLfloat*) &storedRays[i].x );
      glVertex3fv( (GLfloat*) &storedRays[i+1].x );
    }
    glEnd();
  }

  // Set up the lights

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  //glLoadIdentity();

  for (int i=0; i<lights.size(); i++) {

    GLfloat p[4];

    p[0] = lights[i]->position.x;
    p[1] = lights[i]->position.y;
    p[2] = lights[i]->position.z;
    p[3] = 0;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_POSITION, &p[0] );

    p[0] = lights[i]->colour.x;
    p[1] = lights[i]->colour.y;
    p[2] = lights[i]->colour.z;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_DIFFUSE,  &p[0] );

    p[0] = p[1] = p[2] = 0;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_AMBIENT,  &p[0] );

    p[0] = p[1] = p[2] = 1;
    glLightfv( (GLenum) (GL_LIGHT0+i), GL_SPECULAR, &p[0] );

    glEnable( (GLenum) (GL_LIGHT0+i) );
  }

  glPopMatrix();

  glShadeModel( GL_SMOOTH );

  GLint flag = 1;
  glLightModeliv( GL_LIGHT_MODEL_LOCAL_VIEWER, &flag );

  GLfloat amb[4] = {0.0,0.0,0.0,0.0};
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, &amb[0] );

  glEnable( GL_LIGHTING );

  // Draw the objects

  for (int i=0; i<objects.size(); i++) {
    objects[i]->mat->setMaterialForOpenGL();
    objects[i]->renderGL();
  }
}

