// segment.h


class Segment {
 public:

  vec3 tail, head;

  Segment( vec3 t, vec3 h )
    { tail = t; head = h; }

  bool intersects( Segment const& s2 ) {

    // YOUR CODE HERE

	  vec3 cross1;

	  bool intersects;
	  intersects = false;
	  vec3 H1 = head;         //S1 head
	  vec3 T1 = tail;           //S1 tail
	  vec3 H2 = s2.head;        //S2 head
	  vec3 T2 = s2.tail;        //S2 tail

	

	  vec3 A = H1 - T1;
	  //vec3 B = H2 - T1;
	  vec3 B = T2 - T1;
	  vec3 C = H2 - T1;

	  vec3 D = H2 - T2;
	  //vec3 E = H1 - T2;
	  vec3 E = T1 - T2;
	  vec3 F = H1 - T2;

	  
	  if (((A^B)*(A^C)<0 )&&(D^E)*(D^F)<0)
	  {

		  intersects = true;

	  }


	

    return intersects;
  }
};


