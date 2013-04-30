#include "differ.h"

differ::differ(int wait)
{
	tempsPrecedent=0;
	tempsActuel=0;
	tempsAttente=wait;
	etat=true;
}

differ::~differ(void){}



bool differ::isPassed(void)
{
	if(isActivate()==false )//si = false, c'est qu'on a deja demarrer et qu'on peux tester, sinon on a pas demarrer
	{
		tempsActuel = SDL_GetTicks();
		if (tempsActuel - tempsPrecedent > tempsAttente)
		{
			etat=true;
			return true;
		}
	}

	return false;
}


void differ::start(void)
{
	if(isActivate()==true) //si etat = true on peux demarrer si = false, c'est qu'on a deja demarrer
	{
		tempsPrecedent=SDL_GetTicks();
		etat=false;
	}
}

void differ::stop(void)
{
	etat=true;
}


void differ::setPrecedent(int value){tempsPrecedent=value;}
void differ::setAttente(int value){tempsAttente=value;}
void differ::setActuel(void){tempsActuel=SDL_GetTicks();}
int differ::getPrecedent(void){return tempsPrecedent;}
int differ::getActuel(void){return tempsActuel;}
int differ::getAttente(void){return tempsAttente;}

bool differ::isActivate(void)
{
	if(etat==true)
		return true;

	return false;
}