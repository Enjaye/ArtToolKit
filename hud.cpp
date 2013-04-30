#include "hud.h"


hud::hud(void)
{
}

hud::~hud(void){}


void hud::setVisible(bool value,int indice){ listeBoutton[indice].setVisible(value);  }
bool hud::isVisible(int indice){return listeBoutton[indice].isVisible();}

bool hud::isOnBB(int indice,int posX, int posY,int tailleFenetreY)
{
	if(posX < listeBoutton[indice].getX() || posX > listeBoutton[indice].getTailleX())
		return false;
	
	int yMax=tailleFenetreY-listeBoutton[indice].getY();
	int yMin=tailleFenetreY-listeBoutton[indice].getTailleY();

	if(posY < yMin || posY > yMax)
		return false;

	return true;
}

void hud::infoBbox(int indice,int tailleFenetreY)
{
	int yMax=tailleFenetreY-listeBoutton[indice].getY();
	int yMin=tailleFenetreY-listeBoutton[indice].getTailleY();
	printf("x1: %d  x2: %d  y1: %d  y2: %d\n",listeBoutton[indice].getX(),listeBoutton[indice].getY(),yMin,yMax);
}



void hud::addBoutton(char* nom,bool mipmap,int X,int Y,int tX,int tY,bool visible)
{
	listeBoutton.push_back(bouttonHud(nom,mipmap,X,Y,tX,tY,visible));
}


void hud::show()
{
	glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 640, 0.0, 480);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	for(int i=0;i<listeBoutton.size();i++)
	{
		if(listeBoutton[i].isVisible()==true)
		{
				glNormal3f( 0.0f, 0.0f, 1.0f);
				glBindTexture(GL_TEXTURE_2D,listeBoutton[i].getTexture());
                glBegin(GL_QUADS);
					 glTexCoord2d(0,0);glVertex2i(listeBoutton[i].getX(), listeBoutton[i].getY());
					 glTexCoord2d(0,1);glVertex2i(listeBoutton[i].getX(),  listeBoutton[i].getTailleY());
					 glTexCoord2d(1,1);glVertex2i(listeBoutton[i].getTailleX(),  listeBoutton[i].getTailleY());
                     glTexCoord2d(1,0);glVertex2i(listeBoutton[i].getTailleX(), listeBoutton[i].getY());
                glEnd();	
		}
	}

	
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
	glFlush();
}

