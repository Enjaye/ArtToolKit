#ifndef FLOATVECTOR_H
#define FLOATVECTOR_H


class FloatVector
{
    /*
    Classe FloatVector : simple vecteur XYZ ou XYZA (dans le cas de couleurs).
    */
    public:
        FloatVector(float px=0,float py=0,float pz=0,float pa=0);
        /* FloatVector(float px=0,float py=0,float pz=0,float pa=0);
           Constructeur, prend en param�tres des flottants correspondant respectivement � x, y, z et a.
        */
        ~FloatVector();
        /* ~FloatVector();
           Destructeur, totalement inutile.
        */
        FloatVector operator=(const FloatVector &fv);
        /* FloatVector operator=(const FloatVector &fv);
           Affecte au vecteur courant le contenu du vecteur pass� en argument.
           Retourne le vecteur courant ainsi modifi�.
        */
        float x,y,z,a;
};

#endif // FLOATVECTOR_H
