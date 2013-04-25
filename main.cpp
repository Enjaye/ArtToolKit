/**===================================================================================================================
    Nom du projet: ARTOOLKIT_move
    But: faire bouger une sphere, cube, ours, autre, selectionner avec la souris en realite augmentee

    Derniere maj: 25/04/2013 
    Par: Angélique

    Erreurs de compilation : 0

    TODO list par ordre de priorité:
		- Affiner le redimensionnement via les couleurs
		- Intéraction couleur : Suppression d'un objet selectionné
		- Rajout d'élément IHM : Bouton pour lecture d'un code en RA
		- Rajout d'élément IHM : Bouton "d'aide" affichant des pictogrammes pour les mouvements possibles, en RA
		(Pour les deux derniers à partir du moment où on peut detecter la colision sur un objet, c'est bon, sinon, pour éviter des conflits
		entre les boutons ihm et les objets, on peut juste définir des zones correspondant et mettre un timer 'si l'index et le majeur sont plus de 2sc
		sur telle zone, alors on ouvre l'aide, etc)
		- Intéraction couleur : Rotation de l'objet (Un algortihme devrait nous être fourni par un ami à moi)
		- Intéraction couleur : Detection de collision entre nos objets (juste une seconde fonction de colision, sauf qu'on fait objet/objet)
		- Intéraction couleur : Rajout d'une inertie gravitationnelle

    Dernieres modifications :
        - Ajout du déplacement via suivi des couleur
		- Début du redimmensionnement via suivi des couleurs => à affiner
		- Code à trou pour les rotations
		Tout ce beau monde se trouve dans la fonction interactions ligne 839

    Autres :
        Fonctions du code actuel :
           - Affichage d'un objet via un qr code, selection de l'objet via la souris ou les couleurs liées au gant.
           - Calibrage des couleurs de ce gant.

        Interactions :
            En mode Calibrage :
				- Entrée pour selectionner une couleur et passer à la suivante
				- Retour pour modifier la couleur précédente.

			En mode Lego :
				- touche p pour scanner un nouveau code
				- touche + pour aggrandir l'objet 0 de onscreen_object
				- touche - pour réduire l'objet 0 de onscreen_object
				- touche s pour supprimer tout les objets à l'écran

		Idée de mouvement :
			- Pour déplacer : on doit avoir pouce et index||majeur sur l'objet, pas nécessairement de chaques côtés, 
			  car trop complexe à calculer pour le moment
			- Pour agrandir/rétrécir : Pouce et index sur l'objet. On calcule sur plusieurs frame l'écart entre les deux doigts
			  Si il s'élargit, on incrémente size dans Object3D, sinon l'inverse, ou alors un ratio taille/écart des doigts (si l'écart est de
			  9 puis de 5, on amène l'objet à taille 5
			- Suppression d'un objet : deux possibilités, on peut implémenter les deux je pense, qui sont juste une continuité des deux
			  précédentes : si on rétrécit l'objet et que size atteint 1, 0 ou devient négatif : on supprime l'objet,
			  Si on déplace l'objet hors de l'écran, on le supprime. J'ai découvert par hasard qu'on peut l'envoyer au delà de notre espace de vue.
			  Par contre une couleur hors de l'écran ne sera plus detectée, on peut alors partir sur un "si on à l'objet à moitié hors de l'écran, on continue de le pousser
			  avec qu'un seul doigt (genre l'index)

===================================================================================================================**/


///===================================================================================================================
/// Pretraitement : fichiers d'en-tete et macros
///===================================================================================================================

#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#include <OPENCV/highgui.h>
#include <OPENCV/cv.h>

#include "MeshObj.h"
#include "Enums.hpp"
#include "Color.hpp"
#include "Object3D.h"


///===================================================================================================================
/// Definition de structures et variables globales
///===================================================================================================================

/// Creation d'une structure OBJECT_T contenant info de base d'un marker
typedef struct
{
    char *patt_name;
    int patt_id, model_id, visible;// identifiant, model, visibilité 
    double width;//taille
    double center[2]; 
    double trans[3][4];
} OBJECT_T;


/// Configuration de la camera et du marqueur
char *vconf = "Data\\WDM_camera_flipV.xml"; 
/// Chemin de base pour trouver la camera par defaut dans windows
char *cparam_name = "Data\\camera_para.dat";     /// Chemin parametre de la camera par defaut
int xsize, ysize;                               /// Taille x et y
ARParam cparam;                                 /// parametre camera (initiliser lors de arinit()) 


/// Tableau des objets
OBJECT_T   object[2] = {{OBJ1_PATT_NAME, -1, OBJ1_MODEL_ID, 0, OBJ1_SIZE, {0.0,0.0}},
						{OBJ2_PATT_NAME, -1, OBJ2_MODEL_ID, 0, OBJ2_SIZE, {0.0,0.0}}};

vector<Object3D> onscreen_object; //tableau des objets à l'écran
vector<MeshObj*> mesh; //tableau des mesh possible, liée à leur code

/// Parametres du marqueur
double patt_width = 80.0;               /// Largeur du marqueur
double patt_center[2] = {0.0, 0.0};     /// Centre du marqueur
double patt_trans[3][4];                /// matrice de transformation


/// Coordonnees des objets et de la souris
float objet1_x = 0, objet1_y = 0;       /// Deplacement horizontal et vertical du premier objet
float objet2_x = 0, objet2_y = 0;       /// Deplacement horizontal et vertical du deuxieme objet
int previous_x = 0, previous_y = 0;     /// Position x et y precedente de la souris
bool left_button_state = false;         /// Etat du bouton de la souris : renvoie vrai si le bouton est enfonce et si on est sur l'objet a reconnaitre


//// Autres parametres
int thresh = 100;   /// Valeur de seuil de l'image capturee (pour trouver un marqueur)
int cpt = 0;      /// Temps ecoule ???
int k;              /// Indique si le marqueur est affiche (peut-etre a supprimer plus tard)
bool visible = false; // variable qui permet de savoir si on a vu le patron ou pas
int alterne1 = 0; //variable  permettant d'afficher 1 des 3 texte pour un effet d'animation 
int alterne2 = 0; //variable  permettant d'afficher 1 des 3 texte pour un effet d'animation 
int id_obj =0; // peut prendre valeur 1 2 ou 3   1= patron 1; 2=patron 2 ; 3= patron + patronn 2 -> permet de bouger les bons objets. utiliser pouir le deplacement/selection objet
int shift =0; //savoir si shift pressé ou non, utiliser pouir le deplacement/selection objet

//patron 1 = hiro 
//patron 2 = kanji


static bool calib = true, colide = false;
int couleur = 0, nbPixels;

IplImage *image;
Color pouce(POUCE), index(INDEX), majeur(MAJEUR);
CvPoint poucePos, indexPos, majeurPos, pouceNextPos, indexNextPos, majeurNextPos;


///===================================================================================================================
/// Declaration des fonctions
///===================================================================================================================

/// Fonctions de dessin de base 
void texte( void *font, char* texte,int x, int y);		/// Dessine un texte et place le caractere de debut de la chaine a la position (x,y)


/// Fonctions ARToolKit
void arInit();                                          /// Initialisation d'ARToolKit et de la camera
static void cleanup();                                  /// Nettoyage et arret du programme
void mainLoop();                                        /// Boucle principale


/// Fonctions glut
static void resize(int width, int height);              /// Redimensionnement de la fenetre
static void key(unsigned char key, int x, int y);       /// Gestion des evenements clavier
static void idle();                                     /// Procedure d'attente


/// Autres affichage + gestion evenements ( souris )
void calibrage();
void changeObjectColor(int x, int y);
static void display(bool d_screen);    /// Fonction d'affichage ( fait les transformations, les lumieres, + appel draw
void list_hits(GLint hits, GLuint *names);              /// Affichage des informations des objets cliques
void mouseClick();                                      /// Clic de la souris
void mousedw(int x, int y, int but);                    /// Bouton de la souris enfonce
void mouseMove(int x, int y);                           /// Mouvement de la souris
int gl_select2(int x, int y);						/// Procedure de selection (marche presque :( )
void detectColision();									/// Procédure de détection des colisions
void updateColor();									// Mise à jour des barycentres
void	interactions();								// Exécute les intéractions en fonction des couleurs sur l'objet

///===================================================================================================================
/// Fonctions de dessin :
///     - texte(void *font, char* texte, int x, int y)
///===================================================================================================================


void texte( void *font, char* texte,int x, int y)
{
	int taille=0;
	int i=0; 

	glRasterPos2f(x,y); // Position du texte
	taille = (int) strlen(texte); 

	for (i = 0; i < taille; i++) 
		glutBitmapCharacter(font,texte[i]); 

}

///===================================================================================================================
/// Fonctions ARToolKit :
///     - void arInit();
///     - static void cleanup();
///     - void mainLoop();
///===================================================================================================================

//initialisation artoolkit
void arInit()
{
    printf("Debut initialisation AR\n");
	cout << vconf << endl;
    ARParam wparam;

    ///ouverture fenetre de dialogue  de parametrage
    if( arVideoOpen( vconf ) < 0 ) /*exit(0)*/ printf("Connexion a la webcam impossible\n");
    if( arVideoInqSize(&xsize, &ysize) < 0 ) /*exit(0)*/printf("Impossible de recuperer la taille de la video\n");
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    ///parametrage de la camera via la fentre de dialogue precedente
   if( arParamLoad(cparam_name, 1, &wparam) < 0 )
    {
        printf("Camera parameter load error !!\n");
        //exit(0);
    }
	//changement des param de base par ceux choisi lors de l'affichage de la fenetre de configuration
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam ); ///affichage parametres

    ///fin init camera

    ///debut init marker de detection
    for( int i = 0; i < 2; i++ )
    {
        if( (object[i].patt_id=arLoadPatt(object[i].patt_name)) < 0 )
        {
            printf("pattern load error: %s\n", object[i].patt_name);
            //exit(0);
        }
    }

	try
	{
		 argInit( &cparam, 1.0, 0, 0, 0, 0 );
	}
	catch(...){}
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    printf("Fin initialisation AR\n");
}


///===================================================================================================================
/// fin du prog ( touche esc)
///===================================================================================================================
static void cleanup()
{
	cvReleaseImage(&image);
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}


///===================================================================================================================
/// boucle principale executé ppur la reconnaissance,affichae,...
///===================================================================================================================

void mainLoop()
{
    ARMarkerInfo *marker_info;
    ARUint8 *dataPtr;
    int marker_num;

	cvReleaseImage(&image);

	if(!calib)
		detectColision();

    // Recuperation du flux video
    if ( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL )
    {
        arUtilSleep(2);
        return;
    }

	// Passage en mode 2D pour analyse de l'image capturee
    argDrawMode2D();

	// Récupération de l'image openCV puis conversion en ARImage
	//IplImage* imgTest;
	image = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4);	
	image->imageData = (char *)dataPtr;

	//sinon l'image est à l'envers
	cvFlip(image, image, 1);
	

	//test si les couleurs ont déjà été calibrée
	// si oui on teste si y a collision, sinon on calibre
    if(calib)
		calibrage();
	else
	{
		updateColor();
		interactions();
	}


    // affichage image à l'ecran
    argDispImage( (unsigned char *)image->imageData, 0,0 );
	// Recuperation d'une autre image car on a fini avec la precedente
	arVideoCapNext();

		if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0)
		{
			printf("impossible de detecter le marqueur\n");
			cleanup();
		}


		if(visible == false && !calib) //element IHM : procedure qui permet de savoir si on recherche ou pas + réinit mouvemment des objets précédement affiché
		{
			objet1_x =0;objet1_y =0;objet2_x =0;objet2_y =0;


			glColor3ub(255,0,0);
			texte(GLUT_BITMAP_HELVETICA_18,(char*)"Recherche en cours",cparam.xsize-200,cparam.ysize-30);
			if(alterne1==0 && alterne2 > 20)
			{
				texte(GLUT_BITMAP_HELVETICA_18,(char*)"Recherche en cours .",cparam.xsize-200,cparam.ysize-30);
				if(alterne2 > 30){alterne2=0;alterne1=(alterne1+1)%3;}
			}
			if(alterne1==1 && alterne2 > 20 )
			{	
				texte(GLUT_BITMAP_HELVETICA_18,(char*)"Recherche en cours ..",cparam.xsize-200,cparam.ysize-30);
				if(alterne2 > 30){alterne2=0;alterne1=(alterne1+1)%3;}
			}
			if(alterne1==2 && alterne2 > 20)
			{	
				texte(GLUT_BITMAP_HELVETICA_18,(char*)"Recherche en cours ...",cparam.xsize-200,cparam.ysize-30);
				if(alterne2 > 30){alterne2=0;alterne1=(alterne1+1)%3;}
			}

			alterne2+=1;
		}
		else if(calib)
		{
			if(couleur == 0)
			{
				texte(GLUT_BITMAP_HELVETICA_18,(char*)"Cliquez sur la couleur du pouce",cparam.xsize-300,cparam.ysize-30);
			}
			else if(couleur == 1)
			{
				texte(GLUT_BITMAP_HELVETICA_18,(char*)"Cliquez sur la couleur de l'index",cparam.xsize-300,cparam.ysize-30);
			}
			else
			{
				texte(GLUT_BITMAP_HELVETICA_18,(char*)"Cliquez sur la couleur du majeur",cparam.xsize-300,cparam.ysize-30);
			}
		}
		else //passage mode 3d + init profondeur
		{
			argDrawMode3D();
			argDraw3dCamera(0, 0);
			glClearDepth(1.0);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		/// Visibilite de l'objet
		if(visible == false ) //si on a jms vu de patron ou qu'on a demandé une recapture des patrons faire
		{
			//recherche objet visible 
			for (int i=0; i<2; i++) //pour chaque patron initialisé faire 
			{
				k = -1; //k renseigne sur la visibilité du marker et son id
				for (int j=0; j<marker_num; j++) // pour chaque marqueur trouver avec arDetectMarker 
				{
					if (object[i].patt_id == marker_info[j].id) 
					{
						if (k == -1)
						{
							k = j;
						}
						else if (marker_info[k].cf < marker_info[j].cf)
						{
							k = j;
						}
					}
				}

				object[i].visible = k;

				if (k >= 0)
				{
					visible = true;
					arGetTransMat(&marker_info[k], object[i].center, object[i].width,object[i].trans);
					printf("object[%d] center[%f, %f]\n", i, marker_info->pos[0], marker_info->pos[1]);
					printf("object[%d] hg[%f, %f]\n", i, marker_info->vertex[0][0], marker_info->vertex[0][1]);
					printf("object[%d] hd[%f, %f]\n", i, marker_info->vertex[1][0], marker_info->vertex[1][1]);
					printf("object[%d] bg[%f, %f]\n", i, marker_info->vertex[2][0], marker_info->vertex[2][1]);
					printf("object[%d] bd[%f, %f]\n", i, marker_info->vertex[3][0], marker_info->vertex[3][1]);
					
					//si on a vu un patron, on créé une nouvelle instance de l'objet créé par le patron, qu'on stocke dans les objets à l'écran.
					onscreen_object.push_back(Object3D(mesh.at(object[i].model_id), object[i].center, object[i].trans, object[i].width));
				}
			}
		}
		
		//vu qu'on ne gère plus à partir de la variable "visible" d'un patron, on display, dans tout les cas, soit le vecteur est vide, soit 
		//on a un ou plusieurs objets à afficher
		display(true);
			
	
    argSwapBuffers(); /// Affichage de l'image sur l'interface graphique
}


///===================================================================================================================
/// Fonctions glut :
///     - static void resize(width, height);
///     - static void key(key, x, y);
///     - static void idle();
///===================================================================================================================

/// Redimensionnement de la fenetre
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

/// Gestion des evenements clavier
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 : /// Touche Echap (Echap = 27) pour quitter l'application
        cleanup();
		printf("Goodbye deary !\n");
        exit(0);
        break;

		case 'p' : /// Touche + pour demander a artoolkit de faire une recuperation de patron
			visible = false;
        break;

		//fonction qui était un test : pour le moment, sans selection, tu peux changer la taille de l'objet 0, dans le futuru,
		//quand l'objet sera selctionné au doigt, on changera sa taille en fonction du mouvement des doigts
		case '+':
			if(onscreen_object.size() > 0)
				onscreen_object.at(0).zoom(1);
		break;

		case '-':
			if(onscreen_object.size() > 0)
				onscreen_object.at(0).zoom(-1);
		break;

		//fonction nettoyage, après on implémentera un suivi du doigt pour supprimer un objet
		case 's':
			onscreen_object.clear();
		break;


	case 8: /// Touche retour
		if(couleur > -1)
		{
			if(couleur == 2)
				majeur.changeColor(255,255,255);
			else if(couleur == 1)
				index.changeColor(255,255,255);
			else
				pouce.changeColor(255,255,255);

			couleur--;
		}
		break;
			
	case 13 : //touche entrée
		if(couleur < 2)
			couleur++;
		else
		{	calib = !calib;	//Parce que le calibrage est fini patate, donc comme ça on passe plus dans la fonction en question 
		printf("fin du calibrage\n");}
		break;
    }

    glutPostRedisplay();
}

/// Procedure d'attente
static void idle()
{
    glutPostRedisplay();
}


///===================================================================================================================
/// Autres fonctions :
///     - static void display(object, trans);
///     - static void draw(cas, choix);
///     - void list_hits(hits, names);
///     - void mouseClick(button, state, x, y);
///     - void mousedw(x, y, but);
///     - void mouseMove(x, y);
///     - GLuint* gl_select2(x, y);
///===================================================================================================================

// Fonction de calibrage : selon le nombre de couleur déjà choisie, on avec une nouvelle couleur à un certain doigt. L'utilisateur
// est guidé sur le doigt à selectionner par un élément d'ihm
void calibrage()
{
		if(couleur == 0)
		{
			pouceNextPos = pouce.binarisation(image, &nbPixels, true);
			pouce.addObjectToVideo(image, pouceNextPos, nbPixels, poucePos);
		}
		else if(couleur == 1)
		{
			 pouceNextPos = pouce.binarisation(image, &nbPixels, false);
			 indexNextPos = index.binarisation(image, &nbPixels, true);
			 pouce.addObjectToVideo(image, pouceNextPos, nbPixels, poucePos);
			 index.addObjectToVideo(image, indexNextPos, nbPixels, indexPos);
		}
		else if(couleur == 2)
		{
			pouceNextPos = pouce.binarisation(image, &nbPixels, false);
			indexNextPos = index.binarisation(image, &nbPixels, false);
			majeurNextPos = majeur.binarisation(image, &nbPixels, true);
			pouce.addObjectToVideo(image, pouceNextPos, nbPixels, poucePos);
			index.addObjectToVideo(image, indexNextPos, nbPixels, indexPos);
			majeur.addObjectToVideo(image, majeurNextPos, nbPixels, majeurPos);
		}	   
}

//Pour changer la couleur d'un doigt, rencontre un soucis par moment, mais qui ne crash pas le programme, donc non prioritaire
void changeObjectColor(int x, int y)
{
	// Vars
	CvScalar pixel, rgb_pix; //Tableau contenant les valeurs des HSV, RGB et RGBA du pixel[x,y]
	IplImage *hsv=NULL, *rgb=NULL;//image pour la converion
	int h, s, v;
	int r, g, b;

		// On créer une image rgb à partir de l'image RGBA obtenue via artoolkit
		rgb = cvCreateImage(cvGetSize(image), image->depth, 3);
		cvCvtColor(image, rgb, CV_RGBA2RGB);
		
		
		hsv = cvCloneImage(rgb);
		cvCvtColor(rgb, hsv, CV_RGB2HSV);
		

		rgb_pix = cvGet2D(rgb, y, x);
		// Get the selected pixel
		
		r = (int)rgb_pix.val[0];
		g = (int)rgb_pix.val[1];
		b = (int)rgb_pix.val[2];

		pixel = cvGet2D(hsv, y, x); 
		h = (int)pixel.val[0];
		s = (int)pixel.val[1];
		v = (int)pixel.val[2];

		if(couleur == 0 && majeur.compareCouleur(h, s) && index.compareCouleur(h, s))
		{
			pouce.changeColor(h, s, v);
		}
		else if(couleur == 1 && pouce.compareCouleur(h, s) && index.compareCouleur(h, s))
		{
			index.changeColor(h, s, v);
		}
		else if(couleur == 2 && pouce.compareCouleur(h, s) && majeur.compareCouleur(h, s))
		{	
			majeur.changeColor(h, s, v);
		}

		printf("nombre couleur : %d\n", couleur);
		
 
		// Release the memory of the hsv image*/
    		cvReleaseImage(&hsv);
			cvReleaseImage(&rgb);
}

// Fonction d'affichage (d_screen => affichage pour l'écran ou pour la detection de la colision)
static void display(bool d_screen)
{
	glClearColor(0,0,0,0);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

   // Parametres lumiere et matrice
		
	GLfloat mat_ambient[]     = {0.5, 0.1, 0.1, 1.0};
    GLfloat mat_flash[]       = {0.7, 0.7, 0.7, 1.0};
    GLfloat mat_flash_shiny[] = {50.0};

    GLfloat light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
	

    // Passage en mode 3D et activation de la profondeur
    argDrawMode3D();
    argDraw3dCamera(0, 0);

    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

   //alumer Lumieres : il ne faut pas les activer en mode Picking, car on se base sur la couleur de l'objet dans ce mode, et l'activation
   // des lumières ajouterais des ombres et fausserait la sélection
   if(d_screen)
   {
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
   }
   else
   {
	   glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
   }


    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

	//dessin de la scene en fonction de l'objet vu 
   
	//C'est là que c'est tout nouveau. Au moment du dessin, si on est en picking, pour chaques objet, on lui calcule une valeur de bleu.
	//On fait i+1, car sinon on a un objet qui sera à 0 0 0, soit noir, et du coup, bah il sera pas reconnu, il sera noir
	//sur fond noir
	//si on est en affichage classique, on met les couleurs et texture
   
	for(unsigned int i = 0; i < onscreen_object.size(); i++)
	{
		if(!d_screen)//si on est en mode picking
			{
				glColor3f(0.0, 0.0, (float)(i+1)/onscreen_object.size());
				onscreen_object.at(i).draw(false);
			}
			else
				onscreen_object.at(i).draw(true);
	}

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

/// Clic de la souris
void mouseClick(int button, int state, int x, int y)
{
    /// si boutton souris 1 enfonce alors
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP) && calib)
	{
		//Ici par moment j'ai moi aussi l'erreur, la seule chose que ça fait c'est que ça ne rentre pas dans changeObject.
		//On avait une piste, qui était qu'un flux vidéo n'est peut être pas constant, car en castant DANS changeObject, on arrive des fois
		//a des images qui sont vide. Donc dataPtr pourrait être vide par moment, mais en faisant ce test ça ne change rien.
		//cela dit, ce n'est pas notre priorité pour le moment vu qu'il n'empêche pas le programme de tourner comme il faut
		try
		{	changeObjectColor(x, y);}
		catch( ... )
		{ printf("Erreur inconnue.\n");}
	}
    else if ( (button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) && !calib )
    {
		 mousedw(x, y, button);
    }
    else
    {
        left_button_state = false;
    }
	printf("fin manip souris\n");
}


/// Bouton de la souris enfoncé
void mousedw(int x, int y, int but)
{
    id_obj=0;
    previous_x = x;
    previous_y = y;
    left_button_state = true;
	
		id_obj = gl_select2(x,y); //test pour savoir si on a cliquer ou non sur un mesh

		if(id_obj == -1)
		{ 
			printf("on a clique dans le vide\n");
		}
		else
			printf("Click sur l'objet[%d]\n", id_obj);
}


//ne marche plus suite à la refonte du code : on ne manipule plus object[0] et object[1], mais un certain objet de onscreen_object, à réadapter donc
void mouseMove(int x, int y)
{
     if (left_button_state==true && id_obj == 1)
    {
        objet1_x = objet1_x + ( x - previous_x )*0.5;
        objet1_y = objet1_y + ( previous_y - y )*0.5;
        previous_x = x;
        previous_y = y;
    }

	if (left_button_state==true && id_obj == 2)
    {
        objet2_x = objet2_x + ( x - previous_x )*0.5;
        objet2_y = objet2_y + ( previous_y - y )*0.5;
        previous_x = x;
        previous_y = y;
    }

	if (left_button_state==true && id_obj == 3 && shift==1)
    {
        objet1_x = objet1_x + ( x - previous_x )*0.5;
        objet1_y = objet1_y + ( previous_y - y )*0.5;
        previous_x = x;
        previous_y = y;
    }

	if (left_button_state==true && id_obj == 3 && shift==0 )
    {
        objet2_x = objet2_x + ( x - previous_x )*0.5;
        objet2_y = objet2_y + ( previous_y - y )*0.5;
        previous_x = x;
        previous_y = y;
    }
}


//Brand new selection procédure, qui a perdu comme tu peux le voir une bonne 30aine de ligne.

int gl_select2(int x, int y)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE);

	unsigned char pixel[3] = {0, 0, 0};
	GLint viewport[4];

	//tout d'abord, on affiche tout les objets en mode nuance de bleu. Enfin affiche, on ne le voit pas apparaitre comme ça à l'écran, mais pourtant,
	//il y a une fraction de seconde ou s'est bleu, c'estj uste qu'on affiche après =x
	display(false);

	glGetIntegerv(GL_VIEWPORT, viewport);

	for(int w = -2; w < 3; w++)
		for(int h = -2; h < 3; h++)
		{
			if(pixel[2] == 0)
				glReadPixels((x + w), viewport[3] - (y + h), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
				
		}
	//glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glClear(GL_COLOR_BUFFER_BIT);

	//et vu qu'on ne s'occupe que de la valeur bleu, on inverse l'opération qui a été faite pendant le display, et HOP on obtient l'indice
	//dans le tableau onscreen qui a été selectionné, c'est tout simple =3
	return ((int)(pixel[2] * onscreen_object.size())/255)-1;
}

//===================================================================================================================
// détection des collisions couleurs/objet
//===================================================================================================================
void detectColision()
{
	int object_pouce = -1, object_index = -1, object_majeur=-1;

	for(unsigned int i = 0; i < onscreen_object.size(); i++)
		onscreen_object.at(i).reinitFlag();

	if(onscreen_object.size()>0)
	{
		//ici du coup, grâce au gl_select, à chaques boucle, on sait quel doigt est sur quel objet. Pour la manipulation, on rajoutera
		//trois boolean dans Object3D pour savoir quel doigt sont à quel moment sur l'objet
		object_pouce = gl_select2(pouce.getBarycentreX(), pouce.getBarycentreY()); //test pour savoir si on a cliquer ou non sur un mesh
		object_index = gl_select2(index.getBarycentreX(), index.getBarycentreY());
		object_majeur = gl_select2(majeur.getBarycentreX(), majeur.getBarycentreY());
	}


	if(object_pouce > -1)
		onscreen_object.at(object_pouce).setDoigt(1);

	if(object_index >-1)
		onscreen_object.at(object_index).setDoigt(2);

	if(object_majeur >-1)
		onscreen_object.at(object_majeur).setDoigt(4);
}


//simple mise à jour de la position des doigts
void updateColor()
{
	pouceNextPos = pouce.binarisation(image, &nbPixels, false);
	indexNextPos = index.binarisation(image, &nbPixels, false);
	majeurNextPos = majeur.binarisation(image, &nbPixels, false);
	
	pouce.addObjectToVideo(image, pouceNextPos, nbPixels, poucePos);
	index.addObjectToVideo(image, indexNextPos, nbPixels, indexPos);
	majeur.addObjectToVideo(image, majeurNextPos, nbPixels, majeurPos);
}

void	interactions()
{
	for(unsigned int i = 0; i < onscreen_object.size(); i++)
	{
		switch(onscreen_object.at(i).getDoigt())
		{
		case 1: // pouce sur l'objet
		break;

		case 2: // index sur l'objet
		break;

		case 3: // pouce et index sur l'objet : déplacement de l'objet
			{
					float ecart_p[2], ecart_i[2];

					ecart_p[0] = pouce.getBarycentreX() - pouce.getPBarycentreX();
					ecart_p[1] = pouce.getBarycentreY() - pouce.getPBarycentreY();

					ecart_i[0] = index.getBarycentreX() - index.getPBarycentreX();
					ecart_i[1] = index.getBarycentreY() - index.getPBarycentreY();

					onscreen_object.at(i).setPosX(ecart_p[0]);
					onscreen_object.at(i).setPosY(ecart_p[1]);
			}
		break;

		case 4: // majeur sur l'objet
		break;

		case 5: // majeur et pouce sur l'objet : redimensionnement de l'objet
			{
				float ecart_p[2], ecart_act[2], size;

				ecart_p[0] = abs(pouce.getPBarycentreX() - majeur.getPBarycentreX());
				ecart_p[1] = abs(pouce.getPBarycentreY() - majeur.getPBarycentreY());

				ecart_act[0] = abs(pouce.getBarycentreX() - majeur.getBarycentreX());
				ecart_act[1] = abs(pouce.getBarycentreY() - majeur.getBarycentreY());

				size = min((ecart_p[0] - ecart_act[0]), (ecart_p[1] - ecart_act[1]));

				onscreen_object.at(i).zoom(size);

				/*if(onscreen_object.at(i).getSize() <= 1)
					onscreen_object.erase(onscreen_object.begin() + i);*/
			}
		break;

		case 6: // majeur et index sur l'objet
		break;

		case 7: // trois doigts sur l'objet : rotation de l'objet
			{
				/***** Du coup ici faut que tu trouves un rapport vis à vis de la position des doigts sur l'objet et tout, google sera ton
				ami je pense, et moi j'vais demander à mon pote seb dès que je le vois sur skype. T'as juste à faire la formule, c'tout :hap: ***/

				float ox = 0.0, oy = 0.0; //angle pour rotation en x, et rotation en y
				float	cp_pouce[2] = {pouce.getPBarycentreX(), pouce.getPBarycentreY()},
						cp_index[2] = {index.getPBarycentreX(), index.getPBarycentreY()},
						cp_majeur[2]= {majeur.getPBarycentreX(), majeur.getPBarycentreY()},
						/*** ci dessus, les coordonnées à l'instant t-1 de chaques doigt ***/
						/*** ci dessous, les coordonnées à l'instant t de chaque doigts ***/
						c_pouce[2] = {pouce.getBarycentreX(), pouce.getBarycentreY()},
						c_index[2] = {index.getBarycentreX(), index.getBarycentreY()},
						c_majeur[2]= {majeur.getBarycentreX(), majeur.getBarycentreY()};


				/**** Formule et déroulement mathématique ****/

				onscreen_object.at(i).setRotationX(ox);
				onscreen_object.at(i).setRotationY(oy);
				
			}
		break;
		}
	}

	glutPostRedisplay();
}


///===================================================================================================================
/// Programme principal
///===================================================================================================================

int main(int argc, char *argv[])
{
    /// Chargement des objets
	unsigned char  Rouge[3]={255,0,0};
	unsigned char  Bleu[3]={0,0,255};
	//on ne stocke plus dans des mesh, mais dans un tableau possible de mesh à charger. L'id du meche à charger par le patron correspond
	//à l'indice dans le tableau de mesh
	mesh.push_back(new MeshObj("Others\\legoTexture.obj",NULL));
	mesh.push_back(new MeshObj("Others\\cube2.obj", NULL));

	printf("Chargement des objets réussi\n");
	
    /// Initialisation de glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glClearColor(0, 0, 0, 0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glShadeModel(GL_SMOOTH);

    /// Initialisation d'ARToolKit et de la fenetre + appel boucle infini
    arInit();
    arVideoCapStart();
    glutPositionWindow((glutGet(GLUT_SCREEN_WIDTH)-cparam.xsize)/2, (glutGet(GLUT_SCREEN_HEIGHT)-cparam.ysize)/2);
    glutReshapeFunc(resize);
    glutMotionFunc(mouseMove);
    argMainLoop(mouseClick, key, mainLoop);

    return EXIT_SUCCESS;
}

