#ifndef MATERIAL_H
#define MATERIAL_H

#include "FloatVector.h"
#include <string>

using namespace std;


class Material
{
    /*
    Classe Material : définition d'un matériau, composé d'une couleur et d'un nom spécifique.
    */
    public:
        Material(float r,float g,float b,std::string n);
        /* Material(float r,float g,float b,std::string n);
           Constructeur, les trois premiers arguments représentent la couleur RGB du matériau et n est son nom.
        */
        Material(Material *mat);
        /* Material(Material *mat);
           Constructeur alternatif, affecte au matériau courant le contenu du matériau passé en argument.
        */
        ~Material();
        /* ~Material();
           Destructeur, totalement inutile.
        */

        FloatVector coul;
        std::string name;
};

#endif // MATERIAL_H
