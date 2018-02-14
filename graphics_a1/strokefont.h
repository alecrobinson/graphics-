// strokefont.h


#include "headers.h"
#include <string>

typedef enum { LEFT, CENTRE, RIGHT } Alignment;

void drawStrokeString( string str, float x, float y, float height, GLint transformLocation, Alignment alignment );


