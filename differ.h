#ifndef DIFFER_H
#define DIFFER_H

#include <SDL/SDL.h>

class differ
{
	private:
			
		int tempsPrecedent;
		int tempsActuel;
		int tempsAttente; //en ms
		bool etat;

	public:
		differ(int tempsAttente=0); 
		~differ(void);

		bool isPassed(void);

		void setPrecedent(int value);
		void setActuel(void);
		void setAttente(int value);

		int getPrecedent(void);
		int getActuel(void);
		int getAttente(void);

		void start(void);
		void stop(void);
		bool isActivate(void);
};

#endif;