#include "Object3D.h"
#include "MeshObj.h"
#include "Enums.hpp"

#include <stdlib.h>

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>


Object3D::Object3D(){}
Object3D::Object3D(MeshObj *mesh_id, double centre[2], double c_trans[3][4], double c_size)
{
	mesh = mesh_id;

	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 4; j++)
			trans[i][j] = c_trans[i][j];

	size = 5;

	x = centre[0];
	y = centre[1];
	z = -1 /c_size; //application du repère du marqueur
	ox = oy = oz = 0.0;
}

Object3D::~Object3D(){}


float	Object3D::getPosX()		{ return x; }
float	Object3D::getPosY()		{ return y; }
float	Object3D::getPosZ()		{ return z; }
float	Object3D::getRotationX()	{ return ox; }
float	Object3D::getRotationY()	{ return oy; }
float	Object3D::getRotationZ()  { return oz; }
int		Object3D::getDoigt()	{ return doigt; }
double	Object3D::getSize()		{ return size; }


void	Object3D::setPosX(float nx)		{ x += nx; }
void	Object3D::setPosY(float ny)		{ y += ny; }
void	Object3D::setPosZ(float nz)       { z += nz; }
void	Object3D::setRotationX(float nx)	{ ox = nx;}
void	Object3D::setRotationY(float ny)	{ oy = ny;}
void	Object3D::setRotationZ(float nz)	{ oz = nz;}
void    Object3D::zoom(float nsize)		{ size += nsize;}
void	Object3D::setDoigt(int d)	{  doigt += d;}
void	Object3D::reinitFlag()		{  doigt = 0; }


void	Object3D::draw(bool d_screen)
{
	double gl_para[16];

	glPushMatrix();
		
        glTranslated(x,y,z); //placement du modele sur le patron
        glRotated(ox, 1, 0, 0);//rotation si besoin
		glRotated(oy, 0, 1, 0);
		glRotated(oz, 0, 0, 1);
		
		argConvGlpara(trans, gl_para);
		glMultMatrixd(gl_para);
		glScalef(size,size,size);


		if(d_screen)
			mesh->draw_model();
		else
			mesh->draw_model(true, false, false);

	glPopMatrix();
}

