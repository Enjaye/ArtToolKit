#ifndef ENUMS_HPP
#define ENUMS_HPP

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))  
#define abs(x) ((x) > 0 ? (x) : -(x))
#define sign(x) ((x) > 0 ? 1 : -1)
 
// Step mooving for object min & max
#define STEP_MIN 5
#define STEP_MAX 100 

#define OBJ1_PATT_NAME "Data\\patt.lego"     /// Chemin du premier objet (Hiro)
#define OBJ2_PATT_NAME "Data\\patt.hiro"    /// Chemin du deuxieme patron (Kanji)
#define OBJ1_SIZE 80.0                      /// Taille du premier objet (Hiro)
#define OBJ2_SIZE 80.0                      /// Taille du deuxieme objet (Kanji)
#define OBJ1_MODEL_ID 0                     /// ID du premier objet (Hiro)
#define OBJ2_MODEL_ID 1                     /// ID du deuxieme objet (Kanji)

#define TAILLE_MAX 100      /// Taille maximale des chaines de caracteres
#define SH 460              /// Hauteur de la fenetre init (depend de la camera pr la suite)
#define SW 680              /// Largeur de la fenetre init (depend de la camera pr la suite)

enum {POUCE, INDEX, MAJEUR};

#endif