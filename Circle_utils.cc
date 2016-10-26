#include <geoc/geometry/Circle.h>
#include <geoc/scene/Point.h>
#include <geoc/math/Math.h>


using namespace geoc;
using namespace std;


void geoc::classify(const Circle& c, const Vector3& p, Colour3& colour, std::string& desc)
{
    //Exercise 3.
    //We calculate de orientation25D and depending if it is clockwise or not we inverse the result
    num sign = geoc::Math::orientation25D(c[0],c[1],c[2],p)*geoc::Math::orientation2D(c[0],c[1],c[2]);
    //If the result of the operator is positve it means that the point is inside the circle
    //Otherwise if it's negative it means that it is outside else it means the sign is 0
    //which means that is in the segment
    if (sign > 0) { colour = Colour3(0, 1, 0); desc = "Inside"; }
    else if (sign < 0) { colour = Colour3(1, 0, 0); desc = "Outside"; }
    else { colour = Colour3(1, 1, 0); desc = "In segment"; }
}

