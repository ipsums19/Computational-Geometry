#include <geoc/math/Math.h>
#include <geoc/math/Vector.h>


using namespace geoc;


num Math::orientation2D(const Vector3& p, const Vector3& q, const Vector3& r)
{
    //Exercise 1.
    float orientation = ((q[1] - p[1])*(r[0] - q[0])) - ((q[0] - p[0])*(r[1] - q[1]));
    if (orientation == 0) return 0;
    else return orientation > 0 ? 1 : -1;
}


num Math::orientation25D(const Vector2& p, const Vector2& q, const Vector2& r, const Vector2& t)
{
    //Exercise 3.
    num n13 = (q[X]-p[X])*(q[X]+p[X])+(q[Y]-p[Y])*(q[Y]+p[Y]);
    num n23 = (r[X]-p[X])*(r[X]+p[X])+(r[Y]-p[Y])*(r[Y]+p[Y]);
    num n33 = (t[X]-p[X])*(t[X]+p[X])+(t[Y]-p[Y])*(t[Y]+p[Y]);

    //We calculate de volume using the formula explained in the diapos
    num volume = (q[X]-p[X])*(((r[Y]-p[Y])*n33)-(n23*(t[Y]-p[Y])))
              -(q[Y]-p[Y])*(((r[X]-p[X])*n33)-(n23*(t[X]-p[X])))
	      +(n13)*(((r[X]-p[X])*(t[Y]-p[Y]))-((r[Y]-p[Y]*(t[X]-p[X]))));
    //We return depending if the volume is negative or not or equal
    if(volume > 0) return  1;
    if(volume < 0) return -1;
    return 0;
}
