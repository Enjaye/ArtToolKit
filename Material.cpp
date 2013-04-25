#include "Material.h"
#include <string>


Material::Material(float r,float g,float b,string n):name(n)
{
    coul.x=r;
    coul.y=g;
    coul.z=b;
}
Material::Material(Material *mat)
{
    coul=mat->coul;
    name=mat->name;
}

Material::~Material(){}
