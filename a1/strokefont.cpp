// strokefont.h
//
// Draw characters using GLUT's stroke characters
//
// Draw 'string' at position (x,y) in viewing coordinates
// [-1,1]x[-1,1].  Scale the string to have height 'height'.
//
// We need 'transformLocation' to be the location at which to store
// the transformation matrix for the GPU shader.  This is found by the
// calling program using glGetUniformLocation().


#include "strokefont.h"


void drawStrokeString( string str, float x, float y, float height, GLint transformLocation, Alignment alignment )

{
  void *font = GLUT_STROKE_MONO_ROMAN;
  int fontHeight = glutStrokeHeight( font );
  float s = height / fontHeight;

  // Find width

  float width = 0;
  for (unsigned int i=0; i<str.size(); i++)
    width += glutStrokeWidth( font, str[i] );
  width *= s;

  // Find start position

  float xPos;

  switch (alignment) {
  case LEFT:
    xPos = x;
    break;
  case CENTRE:
    xPos = x - width/2.0;
    break;
  case RIGHT:
    xPos = x - width;
    break;
  }

  for (unsigned int i=0; i<str.size(); i++) {
 
    mat4 transform
      = translate( xPos, y, 0.0f )
      * scale( s, s, 1.0f );

    glUniformMatrix4fv( transformLocation, 1, GL_TRUE, &transform[0][0] );
    glutStrokeCharacter( font, str[i] );
    xPos += s * glutStrokeWidth( font, str[i] );
  }
}


