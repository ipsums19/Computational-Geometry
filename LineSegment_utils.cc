#include <geoc/geometry/LineSegment.h>
#include <geoc/math/Math.h>


using namespace geoc;
using namespace std;

int diff(float f1, float f2)
{
    //Function to calculate possible cases and make it easier
    //to compare the segments
    if(f1 > 0 and f2 > 0) return 1;
    else if( f1 < 0 and f2 < 0) return 1;
    else if(f1 == 0 and f2 == 0) return 0;
    else if(f1 == 0 or f2 == 0) return 2;
    else return 3;
}

bool between(const LineSegment& s, const LineSegment& t)
{
    //eix x
    //Compare all the X axis to see if they are inside
    //another line
    if(s[0][0] > t[0][0] and s[1][0] < t[0][0]) return true;
    if(s[0][0] < t[0][0] and s[1][0] > t[0][0]) return true;
    if(s[0][0] > t[1][0] and s[1][0] < t[1][0]) return true;
    if(s[0][0] < t[1][0] and s[1][0] > t[1][0]) return true;

    if(t[0][0] > s[0][0] and t[1][0] < s[0][0]) return true;
    if(t[0][0] < s[0][0] and t[1][0] > s[0][0]) return true;
    if(t[0][0] > s[1][0] and t[1][0] < s[1][0]) return true;
    if(t[0][0] < s[1][0] and t[1][0] > s[1][0]) return true;

    //eix y
    //If the X were all on the same X, to make sure we have
    //to compare all the Y axis also
    if(s[0][1] > t[0][1] and s[1][1] < t[0][1]) return true;
    if(s[0][1] < t[0][1] and s[1][1] > t[0][1]) return true;
    if(s[0][1] > t[1][1] and s[1][1] < t[1][1]) return true;
    if(s[0][1] < t[1][1] and s[1][1] > t[1][1]) return true;

    if(t[0][1] > s[0][1] and t[1][1] < s[0][1]) return true;
    if(t[0][1] < s[0][1] and t[1][1] > s[0][1]) return true;
    if(t[0][1] > s[1][1] and t[1][1] < s[1][1]) return true;
    if(t[0][1] < s[1][1] and t[1][1] > s[1][1]) return true;

    return false;
}

bool colinear(const LineSegment& s, const LineSegment& t)
{

    //colinear
    //At this point we now that the lines are in the same direction and they are no "linear" so
    //we only left of one vertex be in the same position of another or that the lines don't
    //touch each other, so we can compare if two vertex are on the same position, if no we only
    //left that they don't cross
    if((s[0][0]==t[0][0] and s[0][1]==t[0][1]) or (s[0][0]==t[0][0] and s[0][1]==t[0][1])) return true;
    if((s[1][0]==t[0][0] and s[1][1]==t[0][1]) or (s[1][0]==t[0][0] and s[1][1]==t[0][1])) return true;
    return false;
}

void geoc::classifyIntersection(const LineSegment& s, const LineSegment& t, Colour3& colour, std::string& desc)
{
    //Exercise 1.
    //Calculate the orientations
    float o1 = Math::orientation2D(s[0],s[1],t[0]);
    float o2 = Math::orientation2D(s[0],s[1],t[1]);
    float o3 = Math::orientation2D(t[0],t[1],s[0]);
    float o4 = Math::orientation2D(t[0],t[1],s[1]);

    //Use the function diff to make it easier to compare the lines
    if(diff(o1,o2) == 1 or diff(o3,o4) == 1)      { colour = Colour3(0,1,0); desc = "not cross";}
    else if(diff(o1,o2) == 3 and diff(o3,o4) == 3){ colour = Colour3(1,0,0); desc = "cross";}
    //we only have to compare if one of them are on the same direction cause we also have
    //check the case that if they don't cross
    else if(diff(o1,o2) == 2 or diff(o3,o4) == 2){ colour = Colour3(1,1,1); desc = "colinear";}
    else if(between(s,t)) { colour = Colour3(0,1,1); desc = "linear";}
    //if the lines are no between each other we have to compare if two vertex are on the
    //same position, otherwise that will mean that they don't cross
    else if(colinear(s,t)) { colour = Colour3(1,1,1); desc = "colinear";}
    else { colour = Colour3(0,1,0); desc = "not cross";}
}
