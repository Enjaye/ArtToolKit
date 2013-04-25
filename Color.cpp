#include "Color.hpp"
#include "Enums.hpp"

Color::Color()
{
	h = s = v = 0;
	tolerance = 15;
}

Color::Color(int type)
{
	h = s = v = 0;
	tolerance = 15;
	doigt = type;
}

Color::~Color(){}

CvPoint		Color::binarisation(IplImage* image, int *nbPixels, bool paint)
{
	int x, y;
	CvScalar pixel;
	IplImage *hsv, *rgb, *mask;
	IplConvKernel *kernel;
	int sommeX = 0, sommeY = 0;
	*nbPixels = 0;
 
	// Create the mask &initialize it to white (no color detected)
	mask = cvCreateImage(cvGetSize(image), image->depth, 1);

	rgb = cvCreateImage(cvGetSize(image), image->depth, 3);
	cvCvtColor(image, rgb, CV_RGBA2BGR);

	// Create the hsv image
	hsv = cvCloneImage(rgb);
	cvCvtColor(rgb, hsv, CV_BGR2HSV); 


 
	// We create the mask
	cvInRangeS(hsv, cvScalar(h - tolerance -1, s - tolerance, 0), cvScalar(h + tolerance -1, s + tolerance, 255), mask);

	// Create kernels for the morphological operation
	kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);

	// Morphological opening (inverse because we have white pixels on black background)
	cvDilate(mask, mask, kernel, 1);
	cvErode(mask, mask, kernel, 1);

	// We go through the mask to look for the tracked object and get its gravity center
	for(x = 0; x < mask->width; x++) {
		for(y = 0; y < mask->height; y++) { 
 
			// If its a tracked pixel, count it to the center of gravity's calcul
			if(((uchar *)(mask->imageData + y*mask->widthStep))[x] == 255) {
				sommeX += x;
				sommeY += y;
				(*nbPixels)++;
			}
		}
	}

	// Show the result of the mask image
	//if(paint)
		//cvShowImage(MASK_WINDOW, mask);

 
	// We release the memory of kernels
	cvReleaseStructuringElement(&kernel);
 
	// We release the memory of the mask
	cvReleaseImage(&mask);
	// We release the memory of the hsv image
    cvReleaseImage(&hsv);
	cvReleaseImage(&rgb);

	p_barycentre = barycentre;

	// If there is no pixel, we return a center outside the image, else we return the center of gravity
	if(*nbPixels > 0)
	{
		barycentre.x = (int)(sommeX / (*nbPixels));
		barycentre.y = (int)(sommeY / (*nbPixels));
		return barycentre;
	}
	else
		return cvPoint(-1, -1);
}

void		Color::addObjectToVideo(IplImage* image, CvPoint objectNextPos, int nbPixels, CvPoint objectPos)
{
	int objectNextStepX, objectNextStepY;
 
	// Calculate circle next position (if there is enough pixels)
	if (nbPixels > 10) {
 
		// Reset position if no pixel were found
		if (objectPos.x == -1 || objectPos.y == -1) {
			objectPos.x = objectNextPos.x;
			objectPos.y = objectNextPos.y;
		}
 
		// Move step by step the object position to the desired position
		if (abs(objectPos.x - objectNextPos.x) > STEP_MIN) {
			objectNextStepX = max(STEP_MIN, min(STEP_MAX, abs(objectPos.x - objectNextPos.x) / 2));
			objectPos.x += (-1) * sign(objectPos.x - objectNextPos.x) * objectNextStepX;
		}
		if (abs(objectPos.y - objectNextPos.y) > STEP_MIN) {
			objectNextStepY = max(STEP_MIN, min(STEP_MAX, abs(objectPos.y - objectNextPos.y) / 2));
			objectPos.y += (-1) * sign(objectPos.y - objectNextPos.y) * objectNextStepY;
		}
 
	// -1 = object isn't within the camera range
	} else {
 
		objectPos.x = -1;
		objectPos.y = -1;
 
	}
 
	// Draw an object (circle) centered on the calculated center of gravity
	if (nbPixels > 10 && doigt == MAJEUR)
		cvDrawCircle(image, barycentre, 5, CV_RGB(255, 0, 0), -1);
	else if(nbPixels > 10 && doigt == POUCE)
		cvDrawCircle(image, barycentre, 5, CV_RGB(0, 0, 255), -1);
	else if(nbPixels > 10 && doigt == INDEX)
		cvDrawCircle(image, barycentre, 5, CV_RGB(0, 255, 0), -1);
 
	// We show the image on the window
	//cvShowImage(COLOR_WINDOW, image);
}

void	Color::changeColor(int hue, int saturation, int value)
{
	h = hue;
	s = saturation;
	v = value;
	printf("h : %d, s : %d, v : %d\n", h, s, v);
}

bool	Color::compareCouleur(int hue, int saturation)
{
	if((hue >= h - tolerance -1) && (hue <= h + tolerance - 1) && (saturation >= s - tolerance) && (saturation <= s + tolerance))
	{	printf("couleur deja prise\n");
		return false;
	}

	return true;
}

int		Color::getBarycentreX()
{
	return barycentre.x;
}

int		Color::getBarycentreY()
{
	return barycentre.y;
}

int		Color::getPBarycentreX()
{
	return p_barycentre.x;
}

int		Color::getPBarycentreY()
{
	return p_barycentre.y;
}