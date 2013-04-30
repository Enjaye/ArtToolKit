#ifndef BOUTTONHUD_H
#define BOUTTONHUD_H

#include "loadtexture.h"
#include "differ.h"
#include <stdio.h>

class bouttonHud
{

		private:

			/*
						////////////////////////[] <- tailleX/tailleY
						//						//
						//		texture 		//
						//						//
						//						//
				 x/y -> []////////////////////////

			*/
			int tailleX;
			int tailleY;
			int x; 
			int y;

			GLuint texture[1];
			bool visible;

			differ time;

		public:
			bouttonHud(void);
			bouttonHud(char* nom,bool mipmap,int X,int Y,int tX,int tY,bool isvisile);
			~bouttonHud(void);

			void show(void);  //affiche boutton 

			int getTailleX(void);
			int getTailleY(void);
			int getX(void);
			int getY(void);

			GLuint getTexture(void);  

			bool isVisible(void);	//test si le boutton est visible ou pas
			void setVisible(bool value); // permet de configuré le boutton pour qu'il soit visible ou non

			bool isOnBB(int posX, int posY,int tailleFenetreY);  //permet de savoir si point x,y est sur le boutton
			void infoBbox(int tailleFenetreY); //affiche les coordonées du boutton
};

#endif