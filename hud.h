#ifndef HUD_H
#define HUD_H

#include <vector>
#include "bouttonHud.h"

using namespace std;

class hud
{
	private:
		vector<bouttonHud> listeBoutton;

	public:
		hud(void);
		~hud(void);

		void addBoutton(char* nom,bool mipmap,int X,int Y,int tX,int tY,bool visible=true); //ajout d'un boutton a la liste
		void show(); //affiche liste de boutton

		//indice=  position du boutton ds vector<bouttonHud> listeBoutton;
		void setVisible(bool value,int indice); //parametre un boutton pour qu'il soit visible ou non
		bool isVisible(int indice); //savoir si boutton visible ou pas  
		bool isOnBB(int indice,int posX, int posY,int tailleFenetreY);  //permet de savoir si point x,y est sur le boutton
		void infoBbox(int indice,int tailleFenetreY); //affiche les coordonées du boutton
};

#endif