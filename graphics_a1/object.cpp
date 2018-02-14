// object.cpp
//
// Set up and move an object.


#include "headers.h"
#include "object.h"
#include "world.h"
#include "gpuProgram.h"
#include "main.h"


// Set up an object by creating a VAO and rewriting the object
// vertices so that it is centred at (0,0).

void Object::setupVAO( float objectVerts[], float objectWidth )

{
  // ---- Rewrite the object vertices ----

  // Find the bounding box of the object

  vec3 min = vec3( objectVerts[0], objectVerts[1], 0 );
  vec3 max = vec3( objectVerts[0], objectVerts[1], 0 );

  int i;
  for (i=0; objectVerts[i] != 9999; i+=2) {
    vec3 v( objectVerts[i], objectVerts[i+1], 0 );
    if (v.x < min.x) min.x = v.x;
    if (v.x > max.x) max.x = v.x;
    if (v.y < min.y) min.y = v.y;
    if (v.y > max.y) max.y = v.y;
  }

  // Rewrite the model vertices so that the object is centred at (0,0)
  // and has width objectWidth

  float s = objectWidth / (max.x - min.x);
 
  mat4 modelToOriginTransform
    = scale( s, s, 1 )
    * translate( -0.5*(min.x+max.x), -0.5*(min.y+max.y), 0 );

  for (int i=0; objectVerts[i] != 9999; i+=2) {
    vec4 newV = modelToOriginTransform * vec4( objectVerts[i], objectVerts[i+1], 0.0, 1.0 );
    objectVerts[i]   = newV.x / newV.w;
    objectVerts[i+1] = newV.y / newV.w;
  }

  // Store segments in the object model for later

  for (int i=0; objectVerts[i] != 9999; i+=4)
    segments.push_back( Segment( vec3( objectVerts[i],   objectVerts[i+1], 0 ),
				 vec3( objectVerts[i+2], objectVerts[i+3], 0 ) ) );

  // ---- Create a VAO for this object ----

  //AR do the GLuint thing

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // YOUR CODE HERE
  GLuint segmentbuffer;
  glGenBuffers(1, &segmentbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, segmentbuffer);

  glBufferData( GL_ARRAY_BUFFER, 2*segments.size()*3*sizeof(float), &segments[0], GL_STATIC_DRAW );

  glEnableVertexAttribArray(0);

  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

 
}


// Draw the object


void Object::draw( mat4 &worldToViewTransform )

{
  mat4 modelToViewTransform;

  // YOUR CODE HERE (set the transform)

  float theta = 0;
  float eyeDistance = 5;

  vec3 eyePosition(1, 2, eyeDistance);
  //AR model-view transform
  mat4 model_view = translate(-1 * eyePosition);

  //AR model-view-projection transform:
  //mat4 perspective(float fovy, float aspect, float n, float f);
  //mat4 modelToViewTransform = perspective(100.0*M_PI / 180.0, 1.333, 0.1, 100) * model_view;

  modelToViewTransform = worldToViewTransform * modelToWorldTransform();




  //AR need to make an equivalent to this
  /* static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};*/
  
  // Tell the shaders about the model-to-view transform.  (See MVP in asteroids.vert.)
 
  glUniformMatrix4fv( glGetUniformLocation( myGPUProgram->id(), "MVP"), 1, GL_TRUE, &modelToViewTransform[0][0] );

  // YOUR CODE HERE (call OpenGL to draw the VAO of this object)

  //AR bind array
  glBindVertexArray(VAO);
  // AR draw
  glDrawArrays(GL_LINES,0 , 2*segments.size());




}


mat4 Object::modelToWorldTransform() const

{
  mat4 M;

  // YOUR CODE HERE

  mat4 scale_matrix = scale(scaleFactor,scaleFactor,scaleFactor);

  //AR quaternion orientation
  mat4 rotation_matrix = orientation.toMatrix();

  //AR translation - each object has a position in world coordinates position
  mat4 translation_matrix = translate(position);

  M = translation_matrix * (rotation_matrix * scale_matrix);
	

  return M;
}



// Update the pose (position and orientation)


void Object::updatePose( float deltaT )

{
  // Update position

  position = position + deltaT * velocity;

  // Update orientation

  float angularSpeed = angularVelocity.length();
  vec3 rotationAxis;

  if (angularSpeed > 0.0001)
    rotationAxis = angularVelocity.normalize();
  else
    rotationAxis = vec3(0,0,1);

  quaternion q = quaternion( deltaT * angularSpeed, rotationAxis );
  orientation =  q * orientation;

  // wrap around screen

  if (position.x > world->worldMax.x)
    position.x -= (world->worldMax.x - world->worldMin.x);
  if (position.x < world->worldMin.x)
    position.x += (world->worldMax.x - world->worldMin.x);
  if (position.y > world->worldMax.y)
    position.y -= (world->worldMax.y- world->worldMin.y);
  if (position.y < world->worldMin.y)
    position.y += (world->worldMax.y - world->worldMin.y);
}



bool Object::intersects( Object const& obj ) const

{
  mat4 M = modelToWorldTransform();

  for (unsigned int i=0; i<segments.size(); i++) {

    vec3 t = (M * vec4( segments[i].tail )).toVec3();
    vec3 h = (M * vec4( segments[i].head )).toVec3();
    Segment seg(t,h);

    if (obj.intersects( seg ))
	return true;
  }

  return false;
}


bool Object::intersects( Segment const &seg ) const

{
  mat4 M = modelToWorldTransform();

  for (unsigned int i=0; i<segments.size(); i++) {

    vec3 t = (M * vec4( segments[i].tail )).toVec3();
    vec3 h = (M * vec4( segments[i].head )).toVec3();
    Segment s(t,h);

    if (s.intersects( seg ))
	return true;
  }

  return false;
}

