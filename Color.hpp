#ifndef COLOR_HPP
#define COLOR_HPP

#include <OPENCV/highgui.h>
#include <OPENCV/cv.h>

class Color
{
protected :
		int h, s, v, tolerance, doigt;
		CvPoint barycentre, p_barycentre;
		

public :

	Color();
	Color(int type);
	~Color();

	CvPoint		binarisation(IplImage* image, int *nbPixels, bool paint);
	void		addObjectToVideo(IplImage* image, CvPoint objectNextPos, int nbPixels, CvPoint objectPos);
	void		changeColor(int hue, int saturation, int value);
	bool	    compareCouleur(int hue, int saturation);
	int			getBarycentreX();
	int			getBarycentreY();
	int			getPBarycentreX();
	int			getPBarycentreY();
	
};

#endif