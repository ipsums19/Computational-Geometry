#include <geoc/math/Math.h>
#include <geoc/math/Vector.h>


using namespace geoc;


num Math::orientation2D(const Vector3& p, const Vector3& q, const Vector3& r)
{
    float orientation = ((q[1] - p[1])*(r[0] - q[0])) - ((q[0] - p[0])*(r[1] - q[1]));
    if (orientation == 0) return 0;
    else return orientation > 0 ? 1 : -1;
}
