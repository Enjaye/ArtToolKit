#ifndef MATERIAL_H
#define MATERIAL_H

#include "FloatVector.h"
#include <string>

using namespace std;


class Material
{
    /*
    Classe Material : d�finition d'un mat�riau, compos� d'une couleur et d'un nom sp�cifique.
    */
    public:
        Material(float r,float g,float b,std::string n);
        /* Material(float r,float g,float b,std::string n);
           Constructeur, les trois premiers arguments repr�sentent la couleur RGB du mat�riau et n est son nom.
        */
        Material(Material *mat);
        /* Material(Material *mat);
           Constructeur alternatif, affecte au mat�riau courant le contenu du mat�riau pass� en argument.
        */
        ~Material();
        /* ~Material();
           Destructeur, totalement inutile.
        */

        FloatVector coul;
        std::string name;
};

#endif // MATERIAL_H
