#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("pol.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

#include <math.h>

void zadanie();
void osnovanie();
void bok();

class Dots {
public:
	double x;
	double y;
	double z;
};

Dots deturn[8];
const int okr = 10;
Dots okrdot[okr];

double visot = 5;

void Toupp(int size, Dots* mas)
{
	for (int i = 0; i < size; i++) {
		mas[i].z += visot;
	}
}

void giveokr() {
	Dots m, vect;
	m.x = (deturn[5].x + deturn[6].x) / 2;
	m.y = (deturn[5].y + deturn[6].y) / 2;

	double rad, a1, A1, sa;

	vect.x = deturn[6].x - deturn[5].x;
	vect.y = deturn[6].y - deturn[5].y;
	vect.z = deturn[6].z - deturn[5].z;


	rad = sqrt(pow(vect.x, 2) + pow(vect.y, 2) + pow(vect.z, 2)) / 2;

	a1 = acos((deturn[5].x - m.x) / rad);

	sa = 3.14159265 / okr;

	A1 = asin((deturn[5].y - m.y) / rad);

	for (int i = 0; i < okr; i++) {
		//x=a+r*cos(t)
		//y=b+r*sin(t)
		okrdot[i].x = m.x - rad * cos(a1 - sa * (i + 1));
		okrdot[i].y = m.y - (rad * sin(A1 + sa * (i + 1)));
		okrdot[i].z = 0;
	}
}

Dots getNormal(Dots d1, Dots d2, Dots d3) {
	Dots norm;
	norm.x = ((d2.y	- d1.y)*(d3.z - d1.z) - (d3.y - d1.y) * (d2.z - d1.z));
	norm.y = ( - (d2.x - d1.x) * (d3.z - d1.z) + (d3.x - d1.x) * (d2.z - d1.z));
	norm.z = ((d2.y - d1.y) * (d3.x - d1.x) - (d3.y - d1.y) * (d2.x - d1.x));
	return norm;
}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.9, 0.1, 0.9, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	zadanie();
	giveokr();
	osnovanie();
	bok();
	Toupp(8, deturn);
	Toupp(okr, okrdot);
	osnovanie();
	////������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	////����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void zadanie()
{
	deturn[0].x = 1;
	deturn[0].y = 2;
	deturn[0].z = 0;

	deturn[1].x = 3;
	deturn[1].y = 10;
	deturn[1].z = 0;

	deturn[2].x = 7;
	deturn[2].y = 5;
	deturn[2].z = 0;

	deturn[3].x = 2;
	deturn[3].y = 1;
	deturn[3].z = 0;

	deturn[4].x = 4;
	deturn[4].y = -3;
	deturn[4].z = 0;

	deturn[5].x = 0;
	deturn[5].y = -6;
	deturn[5].z = 0;

	deturn[6].x = -4;
	deturn[6].y = -3;
	deturn[6].z = 0;

	deturn[7].x = -3;
	deturn[7].y = 1;
	deturn[7].z = 0;
}


void osnovanie()
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glNormal3d(0,0,deturn[0].z-0.1);
	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9,0.3);
	glTexCoord2d(0.5, 0.55);
	glVertex3d(deturn[0].x, deturn[0].y, deturn[0].z);
	glTexCoord2d(0.6, 0.95);
	glVertex3d(deturn[1].x, deturn[1].y, deturn[1].z);
	glTexCoord2d(0.8, 0.7);
	glVertex3d(deturn[2].x, deturn[2].y, deturn[2].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9, 0.3);
	glTexCoord2d(0.5, 0.55);
	glVertex3d(deturn[0].x, deturn[0].y, deturn[0].z);
	glTexCoord2d(0.8, 0.7);
	glVertex3d(deturn[2].x, deturn[2].y, deturn[2].z);
	glTexCoord2d(0.55, 0.5);
	glVertex3d(deturn[3].x, deturn[3].y, deturn[3].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9, 0.3);
	glTexCoord2d(0.5, 0.55);
	glVertex3d(deturn[0].x, deturn[0].y, deturn[0].z);
	glTexCoord2d(0.55, 0.5);
	glVertex3d(deturn[3].x, deturn[3].y, deturn[3].z);
	glTexCoord2d(0.3, 0.5);
	glVertex3d(deturn[7].x, deturn[7].y, deturn[7].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9, 0.3);
	glTexCoord2d(0.55, 0.5);
	glVertex3d(deturn[3].x, deturn[3].y, deturn[3].z);
	glTexCoord2d(0.65, 0.3);
	glVertex3d(deturn[4].x, deturn[4].y, deturn[4].z);
	glTexCoord2d(0.3, 0.5);
	glVertex3d(deturn[7].x, deturn[7].y, deturn[7].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9, 0.3);
	glTexCoord2d(0.45, 0.15);
	glVertex3d(deturn[5].x, deturn[5].y, deturn[5].z);
	glTexCoord2d(0.65, 0.3);
	glVertex3d(deturn[4].x, deturn[4].y, deturn[4].z);
	glTexCoord2d(0.3, 0.5);
	glVertex3d(deturn[7].x, deturn[7].y, deturn[7].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9, 0.3);
	glTexCoord2d(0.45, 0.15);
	glVertex3d(deturn[5].x, deturn[5].y, deturn[5].z);
	glTexCoord2d(0.25, 0.3);
	//glVertex3d(deturn[6].x, deturn[6].y, deturn[6].z);
	glVertex3dv((double*)&deturn[6]);
	glTexCoord2d(0.3, 0.5);
	glVertex3d(deturn[7].x, deturn[7].y, deturn[7].z);
	glEnd();
	int i;
	for (i = 0; i < okr - 1; i++) {
		glBegin(GL_TRIANGLES);
		glColor4d(0.9, 0, 0.9, 0.3);
		glTexCoord2d(0.35, 0.225);
		glVertex3d((deturn[5].x + deturn[6].x) / 2, (deturn[5].y + deturn[6].y) / 2, deturn[5].z);
		glTexCoord2d((okrdot[i].x+9)/20, (okrdot[i].y + 9) / 20);
		glVertex3d(okrdot[i].x, okrdot[i].y, okrdot[i].z);
		glTexCoord2d((okrdot[i+1].x + 9) / 20, (okrdot[i+1].y + 9) / 20);
		glVertex3d(okrdot[i + 1].x, okrdot[i + 1].y, okrdot[i + 1].z);
		glEnd();
	}
	glBegin(GL_TRIANGLES);
	glColor4d(0.9, 0, 0.9, 0.3);
	glTexCoord2d(0.45, 0.15);
	glVertex3d(deturn[5].x, deturn[5].y, deturn[5].z);
	glTexCoord2d((okrdot[0].x + 9) / 20, (okrdot[0].y + 9) / 20);
	glVertex3d(okrdot[0].x, okrdot[0].y, okrdot[0].z);
	glTexCoord2d(0.25, 0.3);
	glVertex3d(deturn[6].x, deturn[6].y, deturn[6].z);
	glEnd();
}

void bok() {
	Dots vrem;
	double rezmer, rif=0;
	for (int i = 0; i < 7; i++) {
		if (i == 5) {
			
			for (int ii = okr -2; ii > 0; ii--) {
			vrem = okrdot[ii];
			vrem.z += visot;
			vrem = getNormal(okrdot[ii], okrdot[ii + 1], vrem);
			glNormal3d(vrem.x, vrem.y, vrem.z);
			glBegin(GL_QUADS);
			glColor3d(0.5, 0, 0.3);
			rezmer = sqrt(pow(deturn[i].x - deturn[i + 1].x, 2) + pow(deturn[i].y - deturn[i + 1].y, 2));
			glTexCoord2d(rif + rezmer * 3.14159 / (okr + 1) / 20 / 2.974, 0);
			glVertex3d(okrdot[ii].x, okrdot[ii].y, okrdot[ii].z);
			glTexCoord2d(rif + rezmer * 3.14159 / (okr + 1) / 20 / 2.974, 0.15);
			glVertex3d(okrdot[ii].x, okrdot[ii].y, okrdot[ii].z + visot);
			glTexCoord2d(rif, 0.15);
			glVertex3d(okrdot[ii + 1].x, okrdot[ii + 1].y, okrdot[ii + 1].z + visot);
			glTexCoord2d(rif , 0);
			glVertex3d(okrdot[ii + 1].x, okrdot[ii + 1].y, okrdot[ii + 1].z);
			glEnd();
			rif += rezmer*3.14159/(okr+1) / 20 / 2.974;
			}

			vrem = okrdot[1];
			vrem.z += visot;
			vrem = getNormal(okrdot[1], vrem,okrdot[0]);
			glNormal3d(vrem.x, vrem.y, vrem.z);
			glBegin(GL_QUADS);
			glColor3d(0.5, 0, 0.3);
			rezmer = sqrt(pow(deturn[i].x - deturn[i + 1].x, 2) + pow(deturn[i].y - deturn[i + 1].y, 2));
			glTexCoord2d(rif + rezmer * 3.14159 / (okr + 1) / 20 / 2.974, 0);
			glVertex3d(okrdot[0].x, okrdot[0].y, okrdot[0].z);
			glTexCoord2d(rif + rezmer * 3.14159 / (okr + 1) / 20 / 2.974, 0.15);
			glVertex3d(okrdot[0].x, okrdot[0].y, okrdot[0].z + visot);
			glTexCoord2d(rif, 0.15);
			glVertex3d(okrdot[1].x, okrdot[1].y, okrdot[1].z + visot);
			glTexCoord2d(rif, 0);
			glVertex3d(okrdot[1].x, okrdot[1].y, okrdot[1].z);
			glEnd();
			rif += rezmer * 3.14159 / (okr + 1) / 20 / 2.974;

		vrem = okrdot[0];
		vrem.z += visot;
		vrem = getNormal(okrdot[0], vrem, deturn[6]);
		glNormal3d(vrem.x, vrem.y, vrem.z);
		glBegin(GL_QUADS);
		glColor3d(0.5, 0, 0.3);
		glTexCoord2d(rif, 0);
		glVertex3d(okrdot[0].x, okrdot[0].y, okrdot[0].z);
		glTexCoord2d(rif, 0.15);
		glVertex3d(okrdot[0].x, okrdot[0].y, okrdot[0].z + visot);
		glTexCoord2d(rif + rezmer * 3.14159 / (okr + 1) / 20 / 2.974, 0.15);
		glVertex3d(deturn[6].x, deturn[6].y, deturn[6].z + visot);
		glTexCoord2d(rif + rezmer * 3.14159 / (okr + 1) / 20 / 2.974, 0);
		glVertex3d(deturn[6].x, deturn[6].y, deturn[6].z);
		glEnd();
		rif += rezmer * 3.14159 / (okr + 1) / 20 / 2.974;
		i++;
		}
		vrem = deturn[i];
		vrem.z += visot;
		vrem = getNormal(deturn[i], vrem, deturn[i + 1]);
		glNormal3d(vrem.x, vrem.y, vrem.z);
		glBegin(GL_QUADS);
		glColor3d(0.5, 0, 0.3);
		rezmer=sqrt(pow(deturn[i].x- deturn[i + 1].x, 2)+ pow(deturn[i].y - deturn[i + 1].y, 2));
		glTexCoord2d(rif, 0);
		glVertex3d(deturn[i].x, deturn[i].y, deturn[i].z);
		glTexCoord2d(rif, 0.15);
		glVertex3d(deturn[i].x, deturn[i].y, deturn[i].z + visot);
		glTexCoord2d(rif + rezmer / 20 / 2.974, 0.15);
		glVertex3d(deturn[i + 1].x, deturn[i + 1].y, deturn[i + 1].z + visot);
		glTexCoord2d(rif + rezmer / 20 / 2.974, 0);
		glVertex3d(deturn[i + 1].x, deturn[i + 1].y, deturn[i + 1].z);
		glEnd();
		rif += rezmer / 20 / 2.974;
	}

	vrem = deturn[7];
	vrem.z += visot;
	vrem = getNormal(deturn[7], vrem, deturn[0]);
	glNormal3d(vrem.x, vrem.y, vrem.z);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0, 0.3);
	rezmer = sqrt(pow(deturn[7].x - deturn[0].x, 2) + pow(deturn[7].y - deturn[0].y, 2));
	glTexCoord2d(rif, 0);
	glVertex3d(deturn[7].x, deturn[7].y, deturn[7].z);
	glTexCoord2d(rif, 0.15);
	glVertex3d(deturn[7].x, deturn[7].y, deturn[7].z + visot);
	glTexCoord2d(rif + rezmer / 20 / 2.974, 0.15);
	glVertex3d(deturn[0].x, deturn[0].y, deturn[0].z + visot);
	glTexCoord2d(rif + rezmer / 20 / 2.974, 0);
	glVertex3d(deturn[0].x, deturn[0].y, deturn[0].z);
	glEnd();

	
}
