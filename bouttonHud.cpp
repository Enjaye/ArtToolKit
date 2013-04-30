
#include "bouttonHud.h"

bouttonHud::bouttonHud(void)
{
	texture[0]=0;
	tailleX=0;
	tailleY=0;
	x=0;
	y=0;
	visible=false;
}


bouttonHud::bouttonHud(char* nom,bool mipmap,int X,int Y,int tX,int tY,bool isvisile)
{
	tailleX=tX;
	tailleY=tY;
	x=X;
	y=Y;
	visible=isvisile;

	glGenTextures(1,texture);
	if(!charge_texture(nom,0,texture,mipmap)) printf("Chargement texture: echec\n");
	else printf("Chargement texture: ok\n");	
}

bouttonHud::~bouttonHud(void){}

int bouttonHud::getTailleX(void){return tailleX;}
int bouttonHud::getTailleY(void){return tailleY;}
int bouttonHud::getX(void){return x;}
int bouttonHud::getY(void){return y;}

void bouttonHud::show(void)
{

	if(visible==true)
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
			glNormal3f( 0.0f, 0.0f, 1.0f);
					glBindTexture(GL_TEXTURE_2D,getTexture());
					glBegin(GL_QUADS);
						 glTexCoord2d(0,0);glVertex2i(getX(),getY());
						 glTexCoord2d(0,1);glVertex2i(getX(),getTailleY());
						 glTexCoord2d(1,1);glVertex2i(getTailleX(),getTailleY());
						 glTexCoord2d(1,0);glVertex2i(getTailleX(),getY());
					glEnd();	

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glFlush();
	}
}

bool bouttonHud::isVisible(void){return visible;}
void bouttonHud::setVisible(bool value){visible=value;}

GLuint bouttonHud::getTexture(void){return texture[0];}

bool bouttonHud::isOnBB(int posX, int posY,int tailleFenetreY)
{
	if(posX < x || posX > tailleX)
		return false;
	
	int yMax=tailleFenetreY-y;
	int yMin=tailleFenetreY-tailleY;

	if(posY < yMin || posY > yMax)
		return false;

	return true;
}

void bouttonHud::infoBbox(int tailleFenetreY)
{
	int yMax=tailleFenetreY-y;
	int yMin=tailleFenetreY-tailleY;
	printf("x1: %d  x2: %d  y1: %d  y2: %d\n",x,tailleX,yMin,yMax);
}