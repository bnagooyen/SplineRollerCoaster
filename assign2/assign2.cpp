// assign2.cpp : Defines the entry point for the console application.
//

/*
Brian Nguyen
CSCI 480 Computer Graphics
Assignment 2: Simulating a Roller Coaster
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <GL/glu.h>
#include <GL/glut.h>
#include <sstream>
#include <iostream>
using namespace std;


//class definitions for use in construction of spline tracks
class vec
{

private:
	double x;
	double y;
	double z;


public:
	
	vec();
	vec(double x, double y, double z);
	~vec();

	void setX(double value);
	void setY(double value);
	void setZ(double value);
	double getX();
	double getY();
	double getZ();
	
	vec* addVec(vec* v);
	vec* crossProduct(vec* v);
	double dotVec(vec* v);
	double magnitude(void);
	void normalize(void);

	

	


};


class matrix
{


private:

	double** arrayPointer;
	int ROWS;
	int COLS;

	
public:
	int getRows();
	int getCols();
	double getCell(int row, int col);
	
	matrix();
	matrix(int rows, int cols);
	matrix* multiplyMat(matrix* m);
	void setCell(int row, int col, double value);

	

};





vec::vec()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

vec::vec(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


double vec::dotVec(vec* v)
{
	double dotProduct = 0;

	dotProduct = dotProduct + (getX() * v->getX());
	dotProduct = dotProduct + (getY() * v->getY());
	dotProduct = dotProduct + (getZ() * v->getZ());

	return dotProduct;
}




vec* vec::crossProduct(vec* v)
{
	double x = (getY()*v->getZ()) - (getZ()*v->getY());
	double y = (getZ()*v->getX()) - (getX()*v->getZ());
	double z = (getX()*v->getY()) - (getY()*v->getX());
	vec* crossProduct = new vec();
	crossProduct->setX(x);
	crossProduct->setY(y);
	crossProduct->setZ(z);
	return crossProduct;
}


vec* vec::addVec(vec* v)
{
	double x = getX() + v->getX();
	double y = getY() + v->getY();
	double z = getZ() + v->getZ();

	vec* sumvec = new vec();
	sumvec->setX(x);
	sumvec->setY(y);
	sumvec->setZ(z);
	return sumvec;
}



double vec::magnitude(void)
{
	double x;
	double y;
	double z;

	x = pow(getX(), 2);
	y = pow(getY(), 2);
	z = pow(getZ(), 2);

	return sqrt(x + y + z);
}


void vec::normalize(void)
{
	double MAG = magnitude();
	double nvalue1 = getX() / MAG;
	double nvalue2 = getY() / MAG;
	double nvalue3 = getZ() / MAG;
	setX(nvalue1);
	setY(nvalue2);
	setZ(nvalue3);

}


void vec::setX(double value)
{
	this->x = value;
}

void vec::setY(double value)
{
	this->y = value;
}

void vec::setZ(double value)
{
	this->z = value;
}



double vec::getX()
{
	return this->x;
}

double vec::getY()
{
	return this->y;
}

double vec::getZ()
{
	return this->z;
}




matrix::matrix()
{
	ROWS = 4;
	COLS = 4;

	arrayPointer = new double*[4];
	for (int i = 0; i < 4; i++)
	{
		arrayPointer[i] = new double[4];
	}

	

	for (int j = 0; j < 4; j++)
	{
		for (int k = 0; k < 4; k++)
		{
			arrayPointer[j][k] = 0;
		}
	}
}

matrix::matrix(int rows, int cols)
{
	ROWS = rows;
	COLS = cols;

	arrayPointer = new double*[ROWS];
	for (int i = 0; i < ROWS; i++)
	{
		arrayPointer[i] = new double[COLS];
	}

	for (int j = 0; j < ROWS; j++)
	{
		for (int k = 0; k < COLS; k++)
		{
			arrayPointer[j][k] = 0;
		}
	}
}









void matrix::setCell(int row, int col, double value)
{
	int r = row;
	int c = col;
	arrayPointer[r][c] = value;
}

matrix* matrix::multiplyMat(matrix* m)
{
	matrix* returnValue = new matrix(ROWS, m->getCols());

	for (int i = 0; i < returnValue->getRows(); i++)
	{
		for (int j = 0; j < returnValue->getCols(); j++)
		{
			returnValue->setCell(i, j, 0);

			for (int k = 0; k < getCols(); k++)
			{
				double val1 = returnValue->getCell(i, j);
				double val2 = (getCell(i, k) * m->getCell(k, j));
				returnValue->setCell(i, j, val1 + val2);
			}
		}
	}
	return returnValue;
}

double matrix::getCell(int row, int col)
{
	return arrayPointer[row][col];
}


int matrix::getRows()
{
	return ROWS;
}

int matrix::getCols()
{
	return COLS;
}



/* represents one control point along the spline */
struct point
{
	double x;
	double y;
	double z;
};

/* spline struct which contains how many control points, and an array of control points */
struct spline
{
	int numControlPoints;
	struct point *points;
};

/* the spline array */
struct spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;



//matrices
matrix* cpMat = new matrix(4, 3);
matrix* uMat = new matrix(1, 4);
matrix* duMat = new matrix(1, 4);


//textures
string skyText1 = "sk5.jpg";
string skyText2 = "sk4.jpg";
string skyText3 = "sk3.jpg";
string skyText4 = "sk2.jpg";
string skyText5 = "sk2.jpg";
string skyText6 = "sk1.jpg";
string woodText = "Wood.jpg";

matrix* basisMatrix;


point* splinePoints;
point* TopLeftIn;
point* TopLeftOut;
point* BotLeftIn;
point* BotLeftOut;
point* TopRightIn;
point* TopRightOut;
point* BotRightIn;
point* BotRightOut;

vec* tanSplines;
vec* splineNorm;
vec* bisplineNorm;



//movement speed
int uIncrement = 1000;
int imageNum = 0;

int splineSeg;
int sizePointSpline;
int current = 0;

float accel = 5;



GLuint sb1;
GLuint sb2;
GLuint sb3;
GLuint sb4;
GLuint sb5;
GLuint sb6;
GLuint wood;

Pic* sky1;
Pic* sky2;
Pic* sky3;
Pic* sky4;
Pic* sky5;
Pic* sky6;
Pic* woodBoard;



int loadSplines(char *argv) {
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, i = 0, j, iLength;

	/* load the track file */
	fileList = fopen(argv, "r");
	if (fileList == NULL) {
		printf("can't open file\n");
		exit(1);
	}

	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &g_iNumOfSplines);

	g_Splines = (struct spline *)malloc(g_iNumOfSplines * sizeof(struct spline));


	/* reads through the spline files */
	for (j = 0; j < g_iNumOfSplines; j++) {
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		g_Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
		g_Splines[j].numControlPoints = iLength;

		
		splineSeg += (iLength - 3);

		/* saves the data to the struct */
		while (fscanf(fileSpline, "%lf %lf %lf",
			&g_Splines[j].points[i].x,
			&g_Splines[j].points[i].y,
			&g_Splines[j].points[i].z) != EOF) {
			i++;
		}
	}

	free(cName);

	return 0;
}





void leftSide(){
	

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < sizePointSpline - 1; i++)
	{
		

		glVertex3f(TopLeftIn[i].x, TopLeftIn[i].y, TopLeftIn[i].z);
		glVertex3f(TopLeftIn[i + 1].x, TopLeftIn[i + 1].y, TopLeftIn[i + 1].z);
		glVertex3f(TopLeftOut[i].x, TopLeftOut[i].y, TopLeftOut[i].z);
		glVertex3f(TopLeftOut[i + 1].x, TopLeftOut[i + 1].y, TopLeftOut[i + 1].z);
		glColor3f(0.75, 0.75, 0.75);
	}

	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < sizePointSpline - 1; i++)
	{
		

		glVertex3f(TopLeftOut[i].x, TopLeftOut[i].y, TopLeftOut[i].z);
		glVertex3f(TopLeftOut[i + 1].x, TopLeftOut[i + 1].y, TopLeftOut[i + 1].z);
		glVertex3f(BotLeftOut[i].x, BotLeftOut[i].y, BotLeftOut[i].z);
		glVertex3f(BotLeftOut[i+1].x, BotLeftOut[i+1].y, BotLeftOut[i+1].z);
		glColor3f(0.75, 0.75, 0.75);
		
	}

	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < sizePointSpline - 1; i++)
	{
		
		glVertex3f(TopLeftIn[i].x, TopLeftIn[i].y, TopLeftIn[i].z);
		glVertex3f(TopLeftIn[i+1].x, TopLeftIn[i+1].y, TopLeftIn[i+1].z);
		glVertex3f(BotLeftIn[i].x, BotLeftIn[i].y, BotLeftIn[i].z);
		glVertex3f(BotLeftIn[i+1].x, BotLeftIn[i+1].y, BotLeftIn[i+1].z);
		glColor3f(0.75, 0.75, 0.75);
		
	}

	glEnd();

}

void rightSide(){
	
	glBegin(GL_TRIANGLE_STRIP);



	for (int i = 0; i < sizePointSpline - 1; i++)
	{
		

		glVertex3f(TopRightIn[i].x, TopRightIn[i].y, TopRightIn[i].z);
		glVertex3f(TopRightIn[i+1].x, TopRightIn[i+1].y, TopRightIn[i+1].z);
		glVertex3f(TopRightOut[i].x, TopRightOut[i].y, TopRightOut[i].z);
		glVertex3f(TopRightOut[i + 1].x, TopRightOut[i + 1].y, TopRightOut[i + 1].z);
		glColor3f(0.75, 0.75, 0.75);

		
	}

	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < sizePointSpline - 1; i++)
	{
		

		glVertex3f(TopRightOut[i].x, TopRightOut[i].y, TopRightOut[i].z);
		glVertex3f(TopRightOut[i+1].x, TopRightOut[i+1].y, TopRightOut[i+1].z);
		glVertex3f(BotRightOut[i].x, BotRightOut[i].y, BotRightOut[i].z);
		glVertex3f(BotRightOut[i + 1].x, BotRightOut[i + 1].y, BotRightOut[i + 1].z);
		glColor3f(0.75, 0.75, 0.75);
		
	}

	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < sizePointSpline - 1; i++)
	{
		
		glVertex3f(TopRightIn[i].x, TopRightIn[i].y, TopRightIn[i].z);
		glVertex3f(TopRightIn[i+1].x, TopRightIn[i+1].y, TopRightIn[i+1].z);
		glVertex3f(BotRightIn[i].x, BotRightIn[i].y, BotRightIn[i].z);
		glVertex3f(BotRightIn[i + 1].x, BotRightIn[i + 1].y, BotRightIn[i + 1].z);

		glColor3f(0.75, 0.75, 0.75);
		
	}

	glEnd();
}

void railings(){


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, wood);
	for (int i = 0; i < sizePointSpline; i++)
	{
		if ((i % 50 == 1) && (i + 10 < sizePointSpline))
		{

			glBegin(GL_QUADS);

			glTexCoord2f(0, 0);
			glTexCoord2f(0, 1);
			glTexCoord2f(1, 1);
			glTexCoord2f(0, 1);

			glVertex3f(TopLeftIn[i].x, TopLeftIn[i].y, TopLeftIn[i].z);
			glVertex3f(TopRightIn[i].x, TopRightIn[i].y, TopRightIn[i].z);
			glVertex3f(TopRightIn[i + 10].x, TopRightIn[i + 10].y, TopRightIn[i + 10].z);
			glVertex3f(TopLeftIn[i + 10].x, TopLeftIn[i + 10].y, TopLeftIn[i + 10].z);

			glEnd();


			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glTexCoord2f(0, 1);
			glTexCoord2f(1, 1);
			glTexCoord2f(0, 1);

			glVertex3f(TopLeftIn[i].x, TopLeftIn[i].y, TopLeftIn[i].z);
			glVertex3f(TopRightIn[i].x, TopRightIn[i].y, TopRightIn[i].z);
			glVertex3f(BotRightIn[i].x, BotRightIn[i].y, BotRightIn[i].z);
			glVertex3f(BotLeftIn[i].x, BotLeftIn[i].y, BotLeftIn[i].z);

			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}
void movement(){
	vec* horizontalVector = new vec(-splinePoints[current].x, 0, 0);
	horizontalVector->normalize();
	float dotProduct = horizontalVector->dotVec(&tanSplines[current]);
	float pi = atan(1) * 4;
	float angle = acos(dotProduct) * (180 / pi);

	if (angle < 90)
		accel = accel + 0.05;
	
	else if (angle > 90)
		accel = accel- 0.2;
	

	if (current >= sizePointSpline)
	{
		current = 0;
		accel = 5;
	}

	if (accel < 5)
		accel = 5;
	else if (accel > 12)
		accel = 12;
	
	current += ceil(accel);


	
}

void positioning(){
	if (sizePointSpline == 0)
	{

		double x = 0;
		double y = 1;
		double z = 0;

		vec* StartingLoc = new vec();
		StartingLoc->setX(x);
		StartingLoc->setY(y);
		StartingLoc->setZ(z);


		vec* newNorm = tanSplines[sizePointSpline].crossProduct(StartingLoc);
		newNorm->normalize();
		double newX = newNorm->getX();
		double newY = newNorm->getY();
		double newZ = newNorm->getZ();
		splineNorm[sizePointSpline].setX(newX);
		splineNorm[sizePointSpline].setY(newY);
		splineNorm[sizePointSpline].setZ(newZ);



		vec* newBinormal = tanSplines[sizePointSpline].crossProduct(newNorm);
		newBinormal->normalize();
		double newBiX = newBinormal->getX();
		double newBiY = newBinormal->getY();
		double newBiZ = newBinormal->getZ();
		bisplineNorm[sizePointSpline].setX(newBiX);
		bisplineNorm[sizePointSpline].setY(newBiY);
		bisplineNorm[sizePointSpline].setZ(newBiZ);

		newNorm = tanSplines[sizePointSpline].crossProduct(newBinormal);
		newNorm->normalize();
		double newNormX = newNorm->getX();
		double newNormY = newNorm->getY();
		double newNormZ = newNorm->getZ();
		splineNorm[sizePointSpline].setX(newNormX);
		splineNorm[sizePointSpline].setY(newNormY);
		splineNorm[sizePointSpline].setZ(newNormZ);
	}
	else 
	{
		//normal
		vec* newNorm = bisplineNorm[sizePointSpline - 1].crossProduct(&tanSplines[sizePointSpline]);
		newNorm->normalize();
		double newNormX = newNorm->getX();
		double newNormY = newNorm->getY();
		double newNormZ = newNorm->getZ();

		splineNorm[sizePointSpline].setX(newNormX);
		splineNorm[sizePointSpline].setY(newNormY);
		splineNorm[sizePointSpline].setZ(newNormZ);

		//binormal
		vec* newBinormal = tanSplines[sizePointSpline].crossProduct(newNorm);
		newBinormal->normalize();
		double newBiX = newBinormal->getX();
		double newBiY = newBinormal->getY();
		double newBiZ = newBinormal->getZ();
		bisplineNorm[sizePointSpline].setX(newBiX);
		bisplineNorm[sizePointSpline].setY(newBiY);
		bisplineNorm[sizePointSpline].setZ(newBiZ);

	}
}







void skycubeDraw()
{
	

	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D);

	glRotatef(270, 1, 0, 0);
	
	glBindTexture(GL_TEXTURE_2D, sb1);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); 
	glVertex3f(-(100), -(100), (100));
	glTexCoord2f(1, 1); 
	glVertex3f(-(100), (100), (100));
	glTexCoord2f(0, 1); 
	glVertex3f((100), (100), (100));
	glTexCoord2f(0, 0); 
	glVertex3f((100), -(100), (100));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, sb2);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); 
	glVertex3f((100), -(100), -(100));
	glTexCoord2f(1, 1); 
	glVertex3f((100), (100), -(100));
	glTexCoord2f(0, 1); 
	glVertex3f(-(100), (100), -(100));
	glTexCoord2f(0, 0); 
	glVertex3f(-(100), -(100), -(100));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, sb3);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex3f(-(100), (100), -(100));
	glTexCoord2f(0, 1); 
	glVertex3f(-(100), (100), (100));
	glTexCoord2f(0, 0); 
	glVertex3f(-(100), -(100), (100));
	glTexCoord2f(1, 0);
	glVertex3f(-(100), -(100), -(100));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, sb4);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); 
	glVertex3f((100), -(100), -(100));
	glTexCoord2f(1, 0); 
	glVertex3f((100), -(100), (100));
	glTexCoord2f(1, 1);
	glVertex3f((100), (100), (100));
	glTexCoord2f(0, 1); 
	glVertex3f((100), (100), -(100));
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, sb5);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); 
	glVertex3f((100), (100), -(100));
	glTexCoord2f(1, 0); 
	glVertex3f((100), (100), (100));
	glTexCoord2f(1, 1); 
	glVertex3f(-(100), (100), (100));
	glTexCoord2f(0, 1); 
	glVertex3f(-(100), (100), -(100));
	glEnd();

	glBindTexture(GL_TEXTURE_2D, sb6);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); 
	glVertex3f(-(100), -(100), -(100));
	glTexCoord2f(1, 0); 
	glVertex3f(-(100), -(100), (100));
	glTexCoord2f(1, 1); 
	glVertex3f((100), -(100), (100));
	glTexCoord2f(0, 1); 
	glVertex3f((100), -(100), -(100));
	glEnd();



	glDisable(GL_TEXTURE_2D);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLoadIdentity();

	double x;
	double y;
	double z;

	vec* LookAtCamViewVec = new vec();
	LookAtCamViewVec->setX(splinePoints[current].x);
	LookAtCamViewVec->setY(splinePoints[current].y);
	LookAtCamViewVec->setZ(splinePoints[current].z);

	vec* lookAt = LookAtCamViewVec->addVec(&tanSplines[current]);

	gluLookAt(LookAtCamViewVec->getX(), LookAtCamViewVec->getY(), LookAtCamViewVec->getZ(),lookAt->getX(), lookAt->getY(), lookAt->getZ(),bisplineNorm[current].getX(), bisplineNorm[current].getY(), bisplineNorm[current].getZ());

	glPushMatrix();
	skycubeDraw();
	glPopMatrix();

	leftSide();
	rightSide();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	

	
	
	movement();
	railings();
	glutSwapBuffers();

}

void doIdle()
{
	

	
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, aspectRatio, 0.01, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



void myinit()
{
	
	


	sky1 = jpeg_read((char*)skyText1.c_str(), NULL);
	sky2 = jpeg_read((char*)skyText2.c_str(), NULL);
	sky3 = jpeg_read((char*)skyText3.c_str(), NULL);
	sky4 = jpeg_read((char*)skyText4.c_str(), NULL);
	sky5 = jpeg_read((char*)skyText5.c_str(), NULL);
	sky6 = jpeg_read((char*)skyText6.c_str(), NULL);
	woodBoard = jpeg_read((char*)woodText.c_str(), NULL);

	//SkyCube textures being blended
	glGenTextures(1, &sb1);
	glBindTexture(GL_TEXTURE_2D, sb1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky1->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	
	glGenTextures(1, &sb2);
	glBindTexture(GL_TEXTURE_2D, sb2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky2->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	
	glGenTextures(1, &sb3);
	glBindTexture(GL_TEXTURE_2D, sb3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky3->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	
	glGenTextures(1, &sb4);
	glBindTexture(GL_TEXTURE_2D, sb4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky4->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	
	glGenTextures(1, &sb5);
	glBindTexture(GL_TEXTURE_2D, sb5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky5->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	
	glGenTextures(1, &sb6);
	glBindTexture(GL_TEXTURE_2D, sb6);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, sky6->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//Wood
	glGenTextures(1, &wood);
	glBindTexture(GL_TEXTURE_2D, wood);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, woodBoard->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	splineSeg = 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// I've set the argv[1] to track.txt.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your track file name for the "Command Arguments"
	if (argc<2)
	{
		printf("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}

	

	basisMatrix = new matrix();

	//Third row
	basisMatrix->setCell(3, 0, 0);
	basisMatrix->setCell(3, 1, 2);
	basisMatrix->setCell(3, 2, 0);
	basisMatrix->setCell(3, 3, 0);

	//second row 
	basisMatrix->setCell(2, 0, -1);
	basisMatrix->setCell(2, 1, 0);
	basisMatrix->setCell(2, 2, 1);
	basisMatrix->setCell(2, 3, 0.0);

	//first row
	basisMatrix->setCell(1, 0, 2);
	basisMatrix->setCell(1, 1, -5);
	basisMatrix->setCell(1, 2, 4);
	basisMatrix->setCell(1, 3, -1);

	
	basisMatrix->setCell(0, 0, -1);
	basisMatrix->setCell(0, 1, 3);
	basisMatrix->setCell(0, 2, -3);
	basisMatrix->setCell(0, 3, 1);

	loadSplines(argv[1]);

	splinePoints = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));

	//left splines
	TopLeftIn = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));
	TopLeftOut = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));
	BotLeftIn = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));
	BotLeftOut = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));

	//right splines
	TopRightIn = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));
	TopRightOut = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));
	BotRightIn = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));
	BotRightOut = (struct point*)malloc((splineSeg * (uIncrement + 5)) * sizeof(struct point));

	tanSplines = (vec*)malloc((splineSeg * (uIncrement + 5)) * sizeof(vec));
	splineNorm = (vec*)malloc((splineSeg * (uIncrement + 5)) * sizeof(vec));
	bisplineNorm = (vec*)malloc((splineSeg * (uIncrement + 5)) * sizeof(vec));


	
	sizePointSpline = 0;

	for (int i = 0; i < g_iNumOfSplines; i++)
	{
		for (int j = 0; j < g_Splines[i].numControlPoints - 3; j++)
		{
			for (int k = 0; k <= uIncrement; k++)
			{
				double uValue = 0.001 * k;
				double xVal10 = (splineNorm[sizePointSpline].getX() / 10);
				double negXVal10 = (-bisplineNorm[sizePointSpline].getX() / 10);
				

				cpMat->setCell(0, 0, g_Splines[i].points[j].x);
				cpMat->setCell(0, 1, g_Splines[i].points[j].y);
				cpMat->setCell(0, 2, g_Splines[i].points[j].z);

				cpMat->setCell(1, 0, g_Splines[i].points[j + 1].x);
				cpMat->setCell(1, 1, g_Splines[i].points[j + 1].y);
				cpMat->setCell(1, 2, g_Splines[i].points[j + 1].z);

				cpMat->setCell(2, 0, g_Splines[i].points[j + 2].x);
				cpMat->setCell(2, 1, g_Splines[i].points[j + 2].y);
				cpMat->setCell(2, 2, g_Splines[i].points[j + 2].z);

				cpMat->setCell(3, 0, g_Splines[i].points[j + 3].x);
				cpMat->setCell(3, 1, g_Splines[i].points[j + 3].y);
				cpMat->setCell(3, 2, g_Splines[i].points[j + 3].z);
				

				

				uMat->setCell(0, 0, pow(uValue, 3));
				uMat->setCell(0, 1, pow(uValue, 2));
				uMat->setCell(0, 2, pow(uValue, 1));
				uMat->setCell(0, 3, pow(uValue, 0));


				

				duMat->setCell(0, 0, (3.0 * pow(uValue, 2)));
				duMat->setCell(0, 1, (2.0 * pow(uValue, 1)));
				duMat->setCell(0, 2, (1.0 * pow(uValue, 0)));
				duMat->setCell(0, 3, 0.0);

				matrix* basis_cpMat = basisMatrix->multiplyMat(cpMat);
				
				
				

				matrix* posMat = uMat->multiplyMat(basis_cpMat);
				double posZ = posMat->getCell(0, 2) / 2;
				double posY = posMat->getCell(0, 1) / 2;
				double posX = posMat->getCell(0, 0) / 2;
				splinePoints[sizePointSpline].z = posZ;
				splinePoints[sizePointSpline].y = posY;
				splinePoints[sizePointSpline].x = posX;
				
				

				matrix* tangentMat = duMat->multiplyMat(basis_cpMat);
				double tanZ = tangentMat->getCell(0, 2)/2;
				double tanY = tangentMat->getCell(0, 1)/2;
				double tanX = tangentMat->getCell(0, 0)/2;
				vec* tempTan = new vec();
				tempTan->setX(tanX);
				tempTan->setY(tanY);
				tempTan->setZ(tanZ);
				tempTan->normalize();


				tanSplines[sizePointSpline].normalize();
				
				double vTanX = tempTan->getX();
				double vTanY = tempTan->getY();
				double vTanZ = tempTan->getZ();
				tanSplines[sizePointSpline].setX(vTanX);
				tanSplines[sizePointSpline].setY(vTanY);
				tanSplines[sizePointSpline].setZ(vTanZ);

				

				positioning();

				TopLeftIn[sizePointSpline].x = splinePoints[sizePointSpline].x + splineNorm[sizePointSpline].getX() / 10 + -bisplineNorm[sizePointSpline].getX() / 10;
				TopLeftIn[sizePointSpline].y = splinePoints[sizePointSpline].y + splineNorm[sizePointSpline].getY() / 10 + -bisplineNorm[sizePointSpline].getY() / 10;
				TopLeftIn[sizePointSpline].z = splinePoints[sizePointSpline].z + splineNorm[sizePointSpline].getZ() / 10 + -bisplineNorm[sizePointSpline].getZ() / 10;
				BotLeftIn[sizePointSpline].x = splinePoints[sizePointSpline].x + splineNorm[sizePointSpline].getX() / 10 + -bisplineNorm[sizePointSpline].getX() / 9;
				BotLeftIn[sizePointSpline].y = splinePoints[sizePointSpline].y + splineNorm[sizePointSpline].getY() / 10 + -bisplineNorm[sizePointSpline].getY() / 9;
				BotLeftIn[sizePointSpline].z = splinePoints[sizePointSpline].z + splineNorm[sizePointSpline].getZ() / 10 + -bisplineNorm[sizePointSpline].getZ() / 9;


				TopLeftOut[sizePointSpline].x = splinePoints[sizePointSpline].x + splineNorm[sizePointSpline].getX() / 10 + -bisplineNorm[sizePointSpline].getX() / 10;
				TopLeftOut[sizePointSpline].y = splinePoints[sizePointSpline].y + splineNorm[sizePointSpline].getY() / 10 + -bisplineNorm[sizePointSpline].getY() / 10;
				TopLeftOut[sizePointSpline].z = splinePoints[sizePointSpline].z + splineNorm[sizePointSpline].getZ() / 10 + -bisplineNorm[sizePointSpline].getZ() / 10;
				BotLeftOut[sizePointSpline].x = splinePoints[sizePointSpline].x + splineNorm[sizePointSpline].getX() / 9 + -bisplineNorm[sizePointSpline].getX() / 9;
				BotLeftOut[sizePointSpline].y = splinePoints[sizePointSpline].y + splineNorm[sizePointSpline].getY() / 9 + -bisplineNorm[sizePointSpline].getY() / 9;
				BotLeftOut[sizePointSpline].z = splinePoints[sizePointSpline].z + splineNorm[sizePointSpline].getZ() / 9 + -bisplineNorm[sizePointSpline].getZ() / 9;

				BotRightOut[sizePointSpline].x = splinePoints[sizePointSpline].x + -splineNorm[sizePointSpline].getX() / 9 + -bisplineNorm[sizePointSpline].getX() / 9;
				BotRightOut[sizePointSpline].y = splinePoints[sizePointSpline].y + -splineNorm[sizePointSpline].getY() / 9 + -bisplineNorm[sizePointSpline].getY() / 9;
				BotRightOut[sizePointSpline].z = splinePoints[sizePointSpline].z + -splineNorm[sizePointSpline].getZ() / 9 + -bisplineNorm[sizePointSpline].getZ() / 9;
				TopRightOut[sizePointSpline].x = splinePoints[sizePointSpline].x + -splineNorm[sizePointSpline].getX() / 10 + -bisplineNorm[sizePointSpline].getX() / 9;
				TopRightOut[sizePointSpline].y = splinePoints[sizePointSpline].y + -splineNorm[sizePointSpline].getY() / 10 + -bisplineNorm[sizePointSpline].getY() / 9;
				TopRightOut[sizePointSpline].z = splinePoints[sizePointSpline].z + -splineNorm[sizePointSpline].getZ() / 10 + -bisplineNorm[sizePointSpline].getZ() / 9;

				TopRightIn[sizePointSpline].x = splinePoints[sizePointSpline].x + -splineNorm[sizePointSpline].getX() / 10 + -bisplineNorm[sizePointSpline].getX() / 10;
				TopRightIn[sizePointSpline].y = splinePoints[sizePointSpline].y + -splineNorm[sizePointSpline].getY() / 10 + -bisplineNorm[sizePointSpline].getY() / 10;
				TopRightIn[sizePointSpline].z = splinePoints[sizePointSpline].z + -splineNorm[sizePointSpline].getZ() / 10 + -bisplineNorm[sizePointSpline].getZ() / 10;
				BotRightIn[sizePointSpline].x = splinePoints[sizePointSpline].x + -splineNorm[sizePointSpline].getX() / 10 + -bisplineNorm[sizePointSpline].getX() / 9;
				BotRightIn[sizePointSpline].y = splinePoints[sizePointSpline].y + -splineNorm[sizePointSpline].getY() / 10 + -bisplineNorm[sizePointSpline].getY() / 9;
				BotRightIn[sizePointSpline].z = splinePoints[sizePointSpline].z + -splineNorm[sizePointSpline].getZ() / 10 + -bisplineNorm[sizePointSpline].getZ() / 9;


				++sizePointSpline;
			}
		}
	}
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(640, 480);

	glutCreateWindow("Roller Coaster Simulation");

	/* do initialization */
	myinit();

	glutReshapeFunc(reshape);

	glutDisplayFunc(display);
	glutIdleFunc(doIdle);

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	return 0;
}