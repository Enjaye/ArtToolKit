#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>

class MeshObj;

class Object3D
{
protected:

	MeshObj *mesh; // mesh de l'instance
	float x, y, z; // position x y z du centre de l'objet
	float ox, oy, oz; //angle de rotation
    double trans[3][4]; // transformation appliquée par le marqueur
	double size; // taille de l'objet
	int doigt; // sur le principe de la souris, le pouce vaut 1, l'index 2 et le majeur 4, on fait l'addition pour savoir quel doigt sont à quel
	           // moment sur l'objet en question

public:

	Object3D();
	Object3D(MeshObj* mesh_id, double centre[2], double c_trans[3][4], double c_size);
	~Object3D();

	float	getPosX();
	float	getPosY();
	float	getPosZ();

	float	getRotationX();
	float	getRotationY();
	float	getRotationZ();

	int		getDoigt();
	double	getSize();

	void	setPosX(float nx);
	void	setPosY(float ny);
	void	setPosZ(float nz);

	void	setRotationX(float nx);
	void	setRotationY(float ny);
	void	setRotationZ(float nz);

	void	setDoigt(int d);
	void	reinitFlag();

	void	zoom(float nsize);

	void	draw(bool d_screen); //d_screen est comme pour dans display : texture ou bleu?
};

#endif