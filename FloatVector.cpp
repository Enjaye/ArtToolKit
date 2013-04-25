#include "FloatVector.h"

FloatVector::FloatVector(float px,float py,float pz,float pa):x(px),y(py),z(pz),a(pa)
{
}
FloatVector::~FloatVector()
{
}
FloatVector FloatVector::operator=(const FloatVector &fv)
{
    x=fv.x;
    y=fv.y;
    z=fv.z;
    a=fv.a;

    return *this;
}
