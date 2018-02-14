// ship.cpp


#include "ship.h"


void Ship::rotateCW( float deltaT )

{
  // YOUR CODE HERE
	orientation = quaternion(-SHIP_ROTATION_SPEED * deltaT, vec3(0, 0, 1)) * orientation;
}


void Ship::rotateCCW( float deltaT )

{
  orientation = quaternion( + SHIP_ROTATION_SPEED * deltaT, vec3(0,0,1) ) * orientation;
}


void Ship::addThrust( float deltaT )

{
  // Thrust is in the ship's +y direction.  Make sure to change the
  // velocity in that direction in the *world* coordinate system,
  // since the object velocity is in the world coordinate system.

  // YOUR CODE HERE

	velocity = velocity + (deltaT * SHIP_THRUST_ACCEL) * (orientation * vec3(0, 1, 0));
	//AR change velocity
	/*Modify Ship::addThrust() to change the velocity.  Use
   SHIP_THRUST_ACCEL, deltaT, and the current ship orientation.
*/





}


Shell * Ship::fireShell()

{
  // YOUR CODE HERE (below, find the correct position, velocity, and orientation for the shell)

	//position of the nose of the ship, velocity = ? orientation = ship orientation

	return new Shell(position, velocity + SHELL_SPEED * (orientation * vec3(0,1,0)), orientation );

  //return new Shell( vec3(0,0,0), vec3(0,0,0), quaternion(1,0,0,0) );
}


// Ship model consisting of line segments
//
// These are in a ARBITRARY coordinate system and get remapped to the
// world coordinate system (with the centre of the ship at (0,0) and
// width SHIP_WIDTH) when the VAO is set up.


float Ship::verts[] = {

   3,0,  0,9,
   0,9, -3,0, 
  -3,0,  0,1,
   0,1,  3,0,

  9999
};

