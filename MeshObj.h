#ifndef  MESHOBJ_H
#define  MESHOBJ_H

#include <windows.h>
#include <GL/glut.h>
//#include <GLES2/gl2.h>
#include <string>
#include <vector>
#include <limits>

#include "Material.h"
#include "FloatVector.h"
#include <fstream>
#include <iostream>
#include <float.h>


class MeshObj
{
  
	public:
        MeshObj(std::string,MeshObj *first=NULL);
        /* MeshObj(std::string,MeshObj *first=NULL);
           Constructeur, prend en arguments le nom du mod�le � charger et le pointeur de la premi�re frame si le mod�le appartient � une animation (sinon laissez-le � NULL).
        */
        ~MeshObj();
        /* ~MeshObj();
           Destructeur, lib�re toute la m�moire qui lui a �t� allou�e.
        */
        void charger_obj(std::string,MeshObj *first=NULL);
        /* void charger_obj(std::string,MeshObj *first=NULL);
           Charge un fichier OBJ et son MTL, prend en arguments le nom du mod�le � charger et le pointeur de la premi�re frame si le mod�le appartient � une animation (sinon laissez-le � NULL). Cette fonction est appel�e par le constructeur.
           Aucune valeur de retour.
        */
        void charger_mtl(std::string);
        /* void charger_mtl(std::string);
           Charge un fichier MTL, prend en argument le nom du fichier � charger. Cette fonction est appel�e par charger_obj.
           Aucune valeur de retour.
        */
        void draw_model(bool nor=true,bool tex=false, bool color=true);
        /* void draw_model(bool nor=true,bool tex=false);
           Dessine le mod�le, prend en arguments deux bool�ens repr�sentant respectivement les normales et la texture. Si nor vaut true alors on prend en compte les normales, et si tex vaut true alors on applique la texture.
           Aucune valeur de retour.
        */

    private:
        GLuint texture;
        int n_data;
        float *vertice,*normals,*textures,*colours;
        std::vector<Material*> materiaux;

};

#endif //  MESHOBJ_H
