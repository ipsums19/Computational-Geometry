#include <geoc/geometry/Triangle.h>
#include <geoc/geometry/LineSegment.h>
#include <geoc/scene/Point.h>
#include <geoc/math/Math.h>
#include <geoc/GeocException.h>


using namespace geoc;
using namespace std;


void geoc::classify(const Triangle& t, const Vector3& p, Colour3& colour, std::string& desc)
{
    //Exercise 2.
    //If the point is in the same position of one vertex
    if(t[0] == p or t[1] == p or t[2] == p){ colour = Colour3(0,0,1); desc = "In vertex";return;}

    //The commented code is using an auxiliar point and seing how many triangles segments intersect
    //the segment between the new point and the point given 

/*  //Using auxiliar point
    LineSegment l1(p,Vector3(-10,-2,0));
    LineSegment triangleSegment1(t[0], t[1]);
    LineSegment triangleSegment2(t[0], t[2]);
    LineSegment triangleSegment3(t[1], t[2]);
    string desc1, desc2, desc3;
    Colour3 c;
    classifyIntersection(l1, triangleSegment1, c, desc1);
    classifyIntersection(l1, triangleSegment2, c, desc2);
    classifyIntersection(l1, triangleSegment3, c, desc3);
    if(desc1 == "colinear" or desc2 == "colinear" or desc3 == "colinear")
        {colour = Colour3(1,1,0); desc = "In segment";}
    else if(desc1 == "cross" and desc2 == "cross"){ colour = Colour3(1,0,0); desc = "Outsite";}
    else if(desc1 == "cross" and desc3 == "cross"){ colour = Colour3(1,0,0); desc = "Outsite";}
    else if(desc2 == "cross" and desc3 == "cross"){ colour = Colour3(1,0,0); desc = "Outsite";} 
    else if(desc1 == "cross" or desc2 == "cross" or desc3 == "cross"){ colour = Colour3(0,1,0); desc = "Inside";}
    else {colour = Colour3(1, 0, 0); desc = "Outsite";}
*/


    //In this case I calculate the orientation of the segments between the point given
    int n1 = geoc::Math::orientation2D(t[0], t[1], p);
    int n2 = geoc::Math::orientation2D(t[1], t[2], p);
    int n3 = geoc::Math::orientation2D(t[2], t[0], p);
    //If on orientation is 0 and the other are the same it mean it is inside a segment
    //else if all the orientations are the same it mean it is inside the triangle
    //else it is outside the triangle
    if(n1 == 0 and n2 == n3){ colour = Colour3(1,1,0); desc = "In segment"; }
    else if(n2 == 0 and n1 == n3){ colour = Colour3(1,1,0); desc = "In segment"; }
    else if(n3 == 0 and n2 == n1){ colour = Colour3(1,1,0); desc = "In segment"; }
    else if(n1 == n2 and n2 == n3){ colour = Colour3(0,1,0); desc = "Inside"; }
    else{ colour = Colour3(1,0,0); desc = "Outside"; }
}
