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
           Constructeur, prend en arguments le nom du modèle à charger et le pointeur de la première frame si le modèle appartient à une animation (sinon laissez-le à NULL).
        */
        ~MeshObj();
        /* ~MeshObj();
           Destructeur, libère toute la mémoire qui lui a été allouée.
        */
        void charger_obj(std::string,MeshObj *first=NULL);
        /* void charger_obj(std::string,MeshObj *first=NULL);
           Charge un fichier OBJ et son MTL, prend en arguments le nom du modèle à charger et le pointeur de la première frame si le modèle appartient à une animation (sinon laissez-le à NULL). Cette fonction est appelée par le constructeur.
           Aucune valeur de retour.
        */
        void charger_mtl(std::string);
        /* void charger_mtl(std::string);
           Charge un fichier MTL, prend en argument le nom du fichier à charger. Cette fonction est appelée par charger_obj.
           Aucune valeur de retour.
        */
        void draw_model(bool nor=true,bool tex=false, bool color=true);
        /* void draw_model(bool nor=true,bool tex=false);
           Dessine le modèle, prend en arguments deux booléens représentant respectivement les normales et la texture. Si nor vaut true alors on prend en compte les normales, et si tex vaut true alors on applique la texture.
           Aucune valeur de retour.
        */

    private:
        GLuint texture;
        int n_data;
        float *vertice,*normals,*textures,*colours;
        std::vector<Material*> materiaux;

};

#endif //  MESHOBJ_H
