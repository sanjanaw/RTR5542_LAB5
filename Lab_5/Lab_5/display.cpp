#define _CRT_SECURE_NO_WARNINGS

#define m_pi 3.14159
#define FPS 10

#include<ply.h>
#include<vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<iostream>

using namespace std;

#include<GL/glew.h>
#include<GL/GL.h>
#include<GL/glut.h>

#include<glm/glm.hpp>
#include<glm/gtx/transform.hpp> 

Vertex ** read(PlyFile *, int*, int*, Face***, float*, float*, float*);

int sphere = 1; /////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~sphere! (0 or 1) *************************************************************************
int reflection = 2; ///////////////~~~~~~~~~~~~~~~~~~~~~~reflection! (0 or 2) ***********************************************************************
int width = 700, height = 700;
int mouse = 0;
float xmouse = 0.0, ymouse = 0.0;
float xdrag = 0.0, ydrag = 0.0;
float mousescale = 1.0;
glm::mat4 scaleScene = glm::mat4(1.0f);
float cenx1, ceny1, cenz1, cenx2, ceny2, cenz2, cenx3, ceny3, cenz3;
int animation = 0;
float th = 0.0, ts = 0.0, td = 0.0;
glm::mat4 transformcam = glm::mat4(1.0f);
typedef struct
{
	float position[4];
	float normal[4];
	float colour[4];
	float mat_ambient[4];
	float mat_diffuse[4];
	float mat_specular[4];
	float mat_shine;
} thisVertex;
typedef struct
{
	float position[4];
	float normal[4];
	float colour[4];
	float mat_ambient[4];
	float mat_diffuse[4];
	float mat_specular[4];
	float mat_shine;
	float texCoord[2];
} texVertex;

void createMan(glm::mat4*);
void initMan(thisVertex*, thisVertex*, thisVertex*, GLuint*, GLuint *, GLuint *, GLuint*, GLuint*, GLuint*, GLuint*, GLuint*, GLuint*);

int nstacks = 100;
int nslices = 100;
int nvertices;
int nindices;
thisVertex *lvertices;
GLuint *lindices;

GLuint vboHandle[9];
GLuint indexVBO[9];

Vertex **plyVertices1;
Face **plyFaces1;
thisVertex *vertices1;
int countvertex1;
int countfaces1;
GLuint *indices1;
Vertex **plyVertices2;
Face **plyFaces2;
thisVertex *vertices2;
int countvertex2;
int countfaces2;
GLuint *indices2;
Vertex **plyVertices3;
Face **plyFaces3;
thisVertex *vertices3;
int countvertex3;
int countfaces3;
GLuint *indices3;

texVertex *sqvertices;
GLuint *sqindices;

int manvertices = nslices*nstacks;
int manindices = 2 * (nstacks - 1)*(nslices + 1);
thisVertex *cvertices ;
GLuint *cindices ;
thisVertex *vertices4;
GLuint *indices4;
thisVertex *vertices5;
GLuint *indices5;
thisVertex *vertices6;
GLuint *indices6;

GLuint programObject;
GLuint SetupGLSL(char*);

//global light properties
GLfloat light_ambient[] = { 0.7,0.7,0.7,1.0 };
GLfloat light_diffuse[] = { 0.9,0.9,0.9,1.0 };
GLfloat light_specular[] = { 0.9,0.9,0.9,1.0 };
GLfloat light_position[] = { 0.8,0.8,3.0,1.0 };
GLfloat mat_shine = 25.0f;

glm::mat4 model, lmodel, wmodel;
glm::mat4 transformlight;
GLfloat changeambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat changediffuse[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat changespecular[] = { 0.0f,0.0f,0.0f,0.0f };
float changeshine;
int wireframe = 0;

GLuint cubemaptex[2];

GLubyte readImage[550][550][4];
GLubyte texImage[12][256][256][4];
GLubyte oTexImage[12][256][256][4];
GLuint renderTex[12];

void read_Image(char* image, int n) {
	FILE* in = fopen(image, "r");

	int height, width, ccv;
	char header[100];
	fscanf(in, "%s %d %d %d", header, &width, &height, &ccv);

	printf("%s %d %d %d\n", header, width, height, ccv);
	int r, g, b;

	for (int i = height - 1; i >= 0; i--)
		for (int j = 0; j<width; j++)
		{
			fscanf(in, "%d %d %d", &r, &g, &b);
			readImage[i][j][0] = (GLubyte)r;
			readImage[i][j][1] = (GLubyte)g;
			readImage[i][j][2] = (GLubyte)b;
			readImage[i][j][3] = 255;
		}

	for (int i = 0; i<256; i++)
		for (int j = 0; j<256; j++) {
			if (i<height && j <width) {
				texImage[n][i][j][0] = readImage[i][j][0]; texImage[n][i][j][1] = readImage[i][j][1];
				texImage[n][i][j][2] = readImage[i][j][2];	texImage[n][i][j][3] = 255;
			}
			else {
				texImage[n][i][j][0] = 0; 	texImage[n][i][j][1] = 0; 	texImage[n][i][j][2] = 0;
				texImage[n][i][j][3] = 255;
			}
		}
	fclose(in);
}

void read_Image2(char* image, int n) {
	FILE* in = fopen(image, "r");

	int height, width, ccv;
	char header[100];
	fscanf(in, "%s %d %d %d", header, &width, &height, &ccv);

	printf("%s %d %d %d\n", header, width, height, ccv);
	int r, g, b;

	for (int i = height - 1; i >= 0; i--)
		for (int j = 0; j<width; j++)
		{
			fscanf(in, "%d %d %d", &r, &g, &b);
			readImage[i][j][0] = (GLubyte)r;
			readImage[i][j][1] = (GLubyte)g;
			readImage[i][j][2] = (GLubyte)b;
			readImage[i][j][3] = 255;
		}

	for (int i = 0; i<256; i++)
		for (int j = 0; j<256; j++) {
			if (i<height && j <width) {
				oTexImage[n][i][j][0] = readImage[i][j][0]; oTexImage[n][i][j][1] = readImage[i][j][1];
				oTexImage[n][i][j][2] = readImage[i][j][2];	oTexImage[n][i][j][3] = 255;
			}
			else {
				oTexImage[n][i][j][0] = 0; 	oTexImage[n][i][j][1] = 0; 	oTexImage[n][i][j][2] = 0;
				oTexImage[n][i][j][3] = 255;
			}
		}
	fclose(in);
}
void initTexture() {
	glGenTextures(12, renderTex);

	read_Image("1.ppm",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTex[0]); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[0]);
	

	read_Image("2.ppm", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderTex[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[1]);
	
	read_Image("3.ppm", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, renderTex[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[2]);
	
	read_Image("4.ppm", 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, renderTex[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[3]);
	 
	read_Image("5.ppm", 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, renderTex[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[4]);
	 
	read_Image("6.ppm", 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, renderTex[5]); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[5]);


	read_Image2("o1.ppm", 0);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, renderTex[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[0]);
	read_Image2("o2.ppm", 1);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, renderTex[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[1]);
	read_Image2("o3.ppm", 2);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, renderTex[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[2]);
	read_Image2("o4.ppm", 3);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, renderTex[9]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[3]);
	read_Image2("o5.ppm", 4);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, renderTex[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[4]);
	read_Image2("o6.ppm", 5);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, renderTex[11]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[5]);
	
}

void initCubemap() {   //changesssss!!!!!!!!!!!******************************************************************************************
	
	glGenTextures(2, cubemaptex);

	read_Image("1c.ppm", 6);
	read_Image("2c.ppm", 7);
	read_Image("3c.ppm", 8);
	read_Image("4c.ppm", 9);
	read_Image("5c.ppm", 10);
	read_Image("6c.ppm", 11);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptex[0]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[6]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[7]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[8]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[9]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[10]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage[11]);

	read_Image2("o1c.ppm", 6);
	read_Image2("o2c.ppm", 7);
	read_Image2("o3c.ppm", 8);
	read_Image2("o4c.ppm", 9);
	read_Image2("o5c.ppm", 10);
	read_Image2("o6c.ppm", 11);

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptex[1]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[8]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[7]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[6]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[9]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[10]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, oTexImage[11]);
}

void resize(int x, int y) {
	width = x;
	height = y;
}

void light() {

	glUseProgram(programObject);
	GLuint useTexture = glGetUniformLocation(programObject, "useTexture");
	glUniform1i(useTexture, 0);

	GLuint position = glGetAttribLocation(programObject, "position");
	GLuint normal = glGetAttribLocation(programObject, "normal");
	GLuint colour = glGetAttribLocation(programObject, "colour");
	GLuint mambient = glGetAttribLocation(programObject, "mambient");
	GLuint mdiffuse = glGetAttribLocation(programObject, "mdiffuse");
	GLuint mspecular = glGetAttribLocation(programObject, "mspecular");


	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(colour);

	glEnableVertexAttribArray(mambient);
	glEnableVertexAttribArray(mdiffuse);
	glEnableVertexAttribArray(mspecular);


	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[3]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[3]);

	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 0);
	glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 16);
	glVertexAttribPointer(colour, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 32);
	glVertexAttribPointer(mambient, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 48);
	glVertexAttribPointer(mdiffuse, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 64);
	glVertexAttribPointer(mspecular, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 80);

}


void hammerhead() {

	glUseProgram(programObject);
	GLuint useTexture = glGetUniformLocation(programObject, "useTexture");
	glUniform1i(useTexture, reflection);////////////////////////////////////////////////////////////////////////....................................................................................
	GLuint texLoc = glGetUniformLocation(programObject, "cubeMap");
	glUniform1i(texLoc, 12);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptex[0]);		/////////////////////????????

	GLuint position = glGetAttribLocation(programObject, "position");
	GLuint normal = glGetAttribLocation(programObject, "normal");
	GLuint colour = glGetAttribLocation(programObject, "colour");
	GLuint mambient = glGetAttribLocation(programObject, "mambient");
	GLuint mdiffuse = glGetAttribLocation(programObject, "mdiffuse");
	GLuint mspecular = glGetAttribLocation(programObject, "mspecular");

	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(colour);

	glEnableVertexAttribArray(mambient);
	glEnableVertexAttribArray(mdiffuse);
	glEnableVertexAttribArray(mspecular);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[0]);

	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 0);
	glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 16);
	glVertexAttribPointer(colour, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 32);
	glVertexAttribPointer(mambient, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 48);
	glVertexAttribPointer(mdiffuse, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 64);
	glVertexAttribPointer(mspecular, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 80);
}

void shark() {

	glUseProgram(programObject);
	GLuint useTexture = glGetUniformLocation(programObject, "useTexture");
	glUniform1i(useTexture, reflection);////////////////////////////////////////////////////////////////////////....................................................................................
	GLuint texLoc = glGetUniformLocation(programObject, "cubeMap");
	glUniform1i(texLoc, 12);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptex[0]);

	GLuint position = glGetAttribLocation(programObject, "position");
	GLuint normal = glGetAttribLocation(programObject, "normal");
	GLuint colour = glGetAttribLocation(programObject, "colour");
	GLuint mambient = glGetAttribLocation(programObject, "mambient");
	GLuint mdiffuse = glGetAttribLocation(programObject, "mdiffuse");
	GLuint mspecular = glGetAttribLocation(programObject, "mspecular");

	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(colour);

	glEnableVertexAttribArray(mambient);
	glEnableVertexAttribArray(mdiffuse);
	glEnableVertexAttribArray(mspecular);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[1]);

	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 0);
	glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 16);
	glVertexAttribPointer(colour, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 32);
	glVertexAttribPointer(mambient, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 48);
	glVertexAttribPointer(mdiffuse, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 64);
	glVertexAttribPointer(mspecular, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 80);
}

void dolphin() {
	glUseProgram(programObject);
	GLuint useTexture = glGetUniformLocation(programObject, "useTexture");
	glUniform1i(useTexture, reflection);////////////////////////////////////////////////////////////////////////....................................................................................
	GLuint texLoc = glGetUniformLocation(programObject, "cubeMap");
	glUniform1i(texLoc, 12);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptex[0]);

	GLuint position = glGetAttribLocation(programObject, "position");
	GLuint normal = glGetAttribLocation(programObject, "normal");
	GLuint colour = glGetAttribLocation(programObject, "colour");
	GLuint mambient = glGetAttribLocation(programObject, "mambient");
	GLuint mdiffuse = glGetAttribLocation(programObject, "mdiffuse");
	GLuint mspecular = glGetAttribLocation(programObject, "mspecular");

	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(colour);

	glEnableVertexAttribArray(mambient);
	glEnableVertexAttribArray(mdiffuse);
	glEnableVertexAttribArray(mspecular);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[2]);

	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 0);
	glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 16);
	glVertexAttribPointer(colour, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 32);
	glVertexAttribPointer(mambient, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 48);
	glVertexAttribPointer(mdiffuse, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 64);
	glVertexAttribPointer(mspecular, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 80);
}

void cylinder(int n) {
	glUseProgram(programObject);
	GLuint useTexture = glGetUniformLocation(programObject, "useTexture");
	glUniform1i(useTexture, 2);////////////////////////////////////////////////////////////////////////....................................................................................
	GLuint texLoc = glGetUniformLocation(programObject, "cubeMap");
	glUniform1i(texLoc, n);
	glActiveTexture(GL_TEXTURE0+n);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptex[n%12]);

	GLuint position = glGetAttribLocation(programObject, "position");
	GLuint normal = glGetAttribLocation(programObject, "normal");
	GLuint colour = glGetAttribLocation(programObject, "colour");
	GLuint mambient = glGetAttribLocation(programObject, "mambient");
	GLuint mdiffuse = glGetAttribLocation(programObject, "mdiffuse");
	GLuint mspecular = glGetAttribLocation(programObject, "mspecular");

	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(colour);

	glEnableVertexAttribArray(mambient);
	glEnableVertexAttribArray(mdiffuse);
	glEnableVertexAttribArray(mspecular);

	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 0);
	glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 16);
	glVertexAttribPointer(colour, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 32);
	glVertexAttribPointer(mambient, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 48);
	glVertexAttribPointer(mdiffuse, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 64);
	glVertexAttribPointer(mspecular, 4, GL_FLOAT, GL_FALSE, sizeof(thisVertex), (char*)NULL + 80);
}

void square(int n) {
	
	glUseProgram(programObject);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glActiveTexture(GL_TEXTURE0+n);
	glBindTexture(GL_TEXTURE_2D, renderTex[n]);

	GLuint position = glGetAttribLocation(programObject, "position");
	GLuint normal = glGetAttribLocation(programObject, "normal");
	GLuint colour = glGetAttribLocation(programObject, "colour");
	GLuint mambient = glGetAttribLocation(programObject, "mambient");
	GLuint mdiffuse = glGetAttribLocation(programObject, "mdiffuse");
	GLuint mspecular = glGetAttribLocation(programObject, "mspecular");
	GLuint texCoord = glGetAttribLocation(programObject, "texCoord");
	GLuint useTexture = glGetUniformLocation(programObject, "useTexture");
	GLuint texLoc = glGetUniformLocation(programObject, "Tex1");
	glUniform1i(useTexture, 1);
	glUniform1i(texLoc, n);

	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(colour);
	glEnableVertexAttribArray(texCoord);

	glEnableVertexAttribArray(mambient);
	glEnableVertexAttribArray(mdiffuse);
	glEnableVertexAttribArray(mspecular);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[7]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[7]);

	glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 0);
	glVertexAttribPointer(normal, 4, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 16);
	glVertexAttribPointer(colour, 4, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 32);
	glVertexAttribPointer(mambient, 4, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 48);
	glVertexAttribPointer(mdiffuse, 4, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 64);
	glVertexAttribPointer(mspecular, 4, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 80);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(texVertex), (char*)NULL + 100);
}

void display() {
	glEnable(GL_DEPTH_TEST);    // need depth test to correctly draw 3D objects 
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if (animation == 1) {
		th -= 0.12;
		td -= 0.07;
		ts -= 0.1;
		if (th <= -3.5) th = 4.5;
		if (td <= -4.0) td = 3.5;
		if (ts <= -3.5) ts = 4.5;
	}
	//to change light properties
	mat_shine += changeshine;
	if (mat_shine < 2.0f) mat_shine = 2.0f;
	for (int i = 0; i < 4; i++) {
		light_ambient[i] = light_ambient[i] + changeambient[i];
		light_diffuse[i] = light_diffuse[i] + changediffuse[i];
		light_specular[i] = light_specular[i] + changespecular[i];
	}

	glUseProgram(programObject);

	GLuint local2clip = glGetUniformLocation(programObject, "local2clip");
	GLuint local2eye = glGetUniformLocation(programObject, "local2eye");
	GLuint world2eye = glGetUniformLocation(programObject, "world2eye");
	GLuint modell = glGetUniformLocation(programObject, "lmodel");

	GLuint lambient = glGetUniformLocation(programObject, "lambient");
	GLuint ldiffuse = glGetUniformLocation(programObject, "ldiffuse");
	GLuint lspecular = glGetUniformLocation(programObject, "lspecular");
	GLuint shine = glGetUniformLocation(programObject, "shine");
	GLuint lposition = glGetUniformLocation(programObject, "lposition");
	GLuint normalm = glGetUniformLocation(programObject, "normalm");
	GLuint eye2world = glGetUniformLocation(programObject, "eye2world");


	glUniform4f(lambient, light_ambient[0], light_ambient[1], light_ambient[2], light_ambient[3]);
	glUniform4f(ldiffuse, light_diffuse[0], light_diffuse[1], light_diffuse[2], light_diffuse[3]);
	glUniform4f(lspecular, light_specular[0], light_specular[1], light_specular[2], light_specular[3]);
	glUniform1f(shine, mat_shine);

	glm::mat4 projection = glm::perspective(0.5f, 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	view = view *transformcam;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//mouse control
	model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(5, 5, 5));

	model = glm::rotate(model, ymouse, glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, xmouse, glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(mousescale, mousescale, mousescale));
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//to get wireframe
	if (wireframe % 2 == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::mat4 modelview;
	glm::mat4 mvp;

	light();
	lmodel = scaleScene;				//to NOT move light when camera frame is being changed
	lmodel = glm::translate(lmodel, glm::vec3(light_position[0], light_position[1], light_position[2]));
	lmodel = lmodel*transformlight;		// for translation of light
	lmodel = glm::scale(lmodel, glm::vec3(0.05, 0.05, 0.05));
	modelview = view*lmodel;
	mvp = projection*modelview;
	glm::mat4 normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(modell, 1, GL_FALSE, &lmodel[0][0]);
	glUniformMatrix4fv(world2eye, 1, GL_FALSE, &view[0][0]);
	
	glm::mat4 viewi;

	glUniformMatrix4fv(eye2world, 1, GL_FALSE, &viewi[0][0]);
	glUniform4f(lposition, light_position[0], light_position[1], light_position[2], light_position[3]);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLE_STRIP, nindices, GL_UNSIGNED_INT, (char*)NULL + 0);

	modelview = view*model;
	viewi = glm::inverse(modelview);
	glUniformMatrix4fv(eye2world, 1, GL_FALSE, &viewi[0][0]);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~aquarium~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	hammerhead();
	glm::mat4 model1 = model; 
	model1 = glm::translate(model1, glm::vec3(-0.7, 0.6, -0.5));

	model1 = glm::translate(model1, glm::vec3(th, 0.0, 0.0));
	model1 = glm::scale(model1, glm::vec3(0.01f, 0.01f, 0.01f));
	modelview = view*model1;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, countfaces1 * 3, GL_UNSIGNED_INT, 0);

	shark();
	glm::mat4 model2 = model; 
	model2 = glm::translate(model2, glm::vec3(-1.0*cenx2, -1.0*ceny2, -1.0*cenz2));
	model2 = glm::translate(model2, glm::vec3(-0.8, -0.8, -1.0));
	model2 = glm::translate(model2, glm::vec3(ts, 0.0, 0.0));
	
	model2 = glm::rotate(model2, -1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	model2 = glm::scale(model2, glm::vec3(0.3f, 0.3f, 0.3f));
	modelview = view*model2;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, countfaces2 * 3, GL_UNSIGNED_INT, 0);

	dolphin();
	glm::mat4 model3 = model;
	model3 = glm::translate(model3, glm::vec3(0.9, 0.0, 0.0));
	model3 = glm::translate(model3, glm::vec3(td, 0.0, 0.0));
	
	model3 = glm::rotate(model3, -1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
	model3 = glm::scale(model3, glm::vec3(0.0017f, 0.0017f, 0.0017f));
	modelview = view*model3;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, countfaces3 * 3, GL_UNSIGNED_INT, 0);

	//!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~walls~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	square(2);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(5.0f, 0.0f, 0.0f));	//positive-x
	wmodel = glm::rotate(wmodel, (float)m_pi / 2, glm::vec3(0, 1, 0));
	wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(0);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(-5.0f, 0.0f, 0.0f));	//negative-x
	wmodel = glm::rotate(wmodel, (float)(-1.0 * m_pi / 2), glm::vec3(0, 1, 0));
	wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(4);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, 5.0f, 0.0f));	//positive-y
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi / 2), glm::vec3(1, 0, 0));
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi), glm::vec3(0, 0, 1));
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi), glm::vec3(1, 0, 0));
	wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(5);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, -5.0f, 0.0f));	//negative-y
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi/2), glm::vec3(1, 0, 0));
	wmodel = glm::rotate(wmodel, (float)m_pi, glm::vec3(0, 0,1));
	wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(3);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, 0.0f, 5.0f));	//positive-z
	wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(1);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, 0.0f, -5.0f));	//negative-z
	wmodel = glm::rotate(wmodel, (float)m_pi, glm::vec3(0, 1, 0));
	wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	square(6);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(-30.0f, -30.0f, -30.0f));
	wmodel = glm::translate(wmodel, glm::vec3(40.0f, 0.0f, 0.0f));	//positive-x
	wmodel = glm::rotate(wmodel, (float)m_pi / 2, glm::vec3(0, 1, 0));
	wmodel = glm::scale(wmodel, glm::vec3(80.0f, 80.0f, 80.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(8);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(-30.0f, -30.0f, -30.0f));
	wmodel = glm::translate(wmodel, glm::vec3(-40.0f, 0.0f, 0.0f));	//negative-x
	wmodel = glm::rotate(wmodel, (float)(-1.0 * m_pi / 2), glm::vec3(0, 1, 0));
	wmodel = glm::scale(wmodel, glm::vec3(80.0f, 80.0f, 80.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(10);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(-30.0f, -30.0f, -30.0f));
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, 40.0f, 0.0f));	//positive-y
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi / 2), glm::vec3(1, 0, 0));
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi), glm::vec3(0, 0, 1));
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi ), glm::vec3(1, 0, 0));
	wmodel = glm::scale(wmodel, glm::vec3(80.0f, 80.0f, 80.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(11);
	wmodel = model;	
	wmodel = glm::translate(wmodel, glm::vec3(-30.0f, -30.0f, -30.0f));
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, -40.0f, 0.0f));	//negative-y
	wmodel = glm::rotate(wmodel, (float)(1.0*m_pi / 2), glm::vec3(1, 0, 0));
	wmodel = glm::rotate(wmodel, (float)m_pi, glm::vec3(0, 0, 1));
	wmodel = glm::scale(wmodel, glm::vec3(80.0f, 80.0f, 80.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(9);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(-30.0f, -30.0f, -30.0f));
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, 0.0f, 40.0f));	//positive-z
	wmodel = glm::scale(wmodel, glm::vec3(80.0f, 80.0f, 80.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	square(7);
	wmodel = model;
	wmodel = glm::translate(wmodel, glm::vec3(-30.0f, -30.0f, -30.0f));
	wmodel = glm::translate(wmodel, glm::vec3(0.0f, 0.0f, -40.0f));	//negative-z
	wmodel = glm::rotate(wmodel, (float)m_pi, glm::vec3(0, 1, 0));
	wmodel = glm::scale(wmodel, glm::vec3(80.0f, 80.0f, 80.0f));
	modelview = view*wmodel;
	mvp = projection*modelview;
	normal_matrix = glm::inverse(modelview);
	normal_matrix = glm::transpose(normal_matrix);
	glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	if (sphere == 1) {
		light();
		cylinder(12);
		modelview = view*model;
		mvp = projection*modelview;
		normal_matrix = glm::inverse(modelview);
		normal_matrix = glm::transpose(normal_matrix);
		glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
		glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
		glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
		glDrawElements(GL_TRIANGLE_STRIP, nindices, GL_UNSIGNED_INT, (char*)NULL + 0);
		light();
		cylinder(13);
		wmodel = glm::translate(model, glm::vec3(-30.0f, -30.0f, -30.0f));
		wmodel = glm::scale(wmodel, glm::vec3(10.0f, 10.0f, 10.0f));
		modelview = view*wmodel;
		mvp = projection*modelview;
		normal_matrix = glm::inverse(modelview);
		normal_matrix = glm::transpose(normal_matrix);
		glUniformMatrix4fv(normalm, 1, GL_FALSE, &normal_matrix[0][0]);
		glUniformMatrix4fv(local2eye, 1, GL_FALSE, &modelview[0][0]);
		glUniformMatrix4fv(local2clip, 1, GL_FALSE, &mvp[0][0]);
		glDrawElements(GL_TRIANGLE_STRIP, nindices, GL_UNSIGNED_INT, (char*)NULL + 0);
	}
	cout << "*";
	changeshine = 0.0;
	for (int i = 0; i < 4; i++) {
		changeambient[i] = 0.0f;
		changediffuse[i] = 0.0f;
		changespecular[i] = 0.0f;
	}
	glutSwapBuffers();
}

void initPlyVertices1() {
	vertices1 = new thisVertex[countvertex1];
	for (int i = 0; i < countvertex1; i++) {
		vertices1[i].position[0] = plyVertices1[i]->x;
		vertices1[i].position[1] = plyVertices1[i]->y;
		vertices1[i].position[2] = plyVertices1[i]->z;
		vertices1[i].position[3] = 1.0f;

		vertices1[i].normal[0] = plyVertices1[i]->nx;
		vertices1[i].normal[1] = plyVertices1[i]->ny;
		vertices1[i].normal[2] = plyVertices1[i]->nz;
		vertices1[i].normal[3] = 1.0f;

		vertices1[i].mat_ambient[0] = 0.2f;
		vertices1[i].mat_ambient[1] = 0.2f;
		vertices1[i].mat_ambient[2] = 0.2f;
		vertices1[i].mat_ambient[3] = 1.0f;
		vertices1[i].mat_diffuse[0] = 0.8f;
		vertices1[i].mat_diffuse[1] = 0.8f;
		vertices1[i].mat_diffuse[2] = 0.8f;
		vertices1[i].mat_diffuse[3] = 1.0f;
		vertices1[i].mat_specular[0] = 0.8f;
		vertices1[i].mat_specular[1] = 0.8f;
		vertices1[i].mat_specular[2] = 0.8f;
		vertices1[i].mat_specular[3] = 1.0f;
	}
	int j = 0;
	indices1 = new GLuint[3 * countfaces1];
	for (int i = 0; i < countfaces1; i++) {
		indices1[j++] = plyFaces1[i]->vertices[0];
		indices1[j++] = plyFaces1[i]->vertices[1];
		indices1[j++] = plyFaces1[i]->vertices[2];
	}
}

void initPlyVertices2() {
	vertices2 = new thisVertex[countvertex2];
	for (int i = 0; i < countvertex2; i++) {
		vertices2[i].position[0] = plyVertices2[i]->x;
		vertices2[i].position[1] = plyVertices2[i]->y;
		vertices2[i].position[2] = plyVertices2[i]->z;
		vertices2[i].position[3] = 1.0f;

		vertices2[i].normal[0] = plyVertices2[i]->nx;
		vertices2[i].normal[1] = plyVertices2[i]->ny;
		vertices2[i].normal[2] = plyVertices2[i]->nz;
		vertices2[i].normal[3] = 1.0f;

		vertices2[i].mat_ambient[0] = 0.2f;
		vertices2[i].mat_ambient[1] = 0.2f;
		vertices2[i].mat_ambient[2] = 0.2f;
		vertices2[i].mat_ambient[3] = 1.0f;
		vertices2[i].mat_diffuse[0] = 0.8f;
		vertices2[i].mat_diffuse[1] = 0.8f;
		vertices2[i].mat_diffuse[2] = 0.8f;
		vertices2[i].mat_diffuse[3] = 1.0f;
		vertices2[i].mat_specular[0] = 0.8f;
		vertices2[i].mat_specular[1] = 0.8f;
		vertices2[i].mat_specular[2] = 0.8f;
		vertices2[i].mat_specular[3] = 1.0f;
	}
	int j = 0;
	indices2 = new GLuint[3 * countfaces2];
	for (int i = 0; i < countfaces2; i++) {
		indices2[j++] = plyFaces2[i]->vertices[0];
		indices2[j++] = plyFaces2[i]->vertices[1];
		indices2[j++] = plyFaces2[i]->vertices[2];
	}
}
void initPlyVertices3() {
	vertices3 = new thisVertex[countvertex3];
	for (int i = 0; i < countvertex3; i++) {
		vertices3[i].position[0] = plyVertices3[i]->x;
		vertices3[i].position[1] = plyVertices3[i]->y;
		vertices3[i].position[2] = plyVertices3[i]->z;
		vertices3[i].position[3] = 1.0f;

		vertices3[i].normal[0] = plyVertices3[i]->nx;
		vertices3[i].normal[1] = plyVertices3[i]->ny;
		vertices3[i].normal[2] = plyVertices3[i]->nz;
		vertices3[i].normal[3] = 1.0f;

		vertices3[i].mat_ambient[0] = 0.2f;
		vertices3[i].mat_ambient[1] = 0.2f;
		vertices3[i].mat_ambient[2] = 0.2f;
		vertices3[i].mat_ambient[3] = 1.0f;
		vertices3[i].mat_diffuse[0] = 0.8f;
		vertices3[i].mat_diffuse[1] = 0.8f;
		vertices3[i].mat_diffuse[2] = 0.8f;
		vertices3[i].mat_diffuse[3] = 1.0f;
		vertices3[i].mat_specular[0] = 0.8f;
		vertices3[i].mat_specular[1] = 0.8f;
		vertices3[i].mat_specular[2] = 0.8f;
		vertices3[i].mat_specular[3] = 1.0f;
	}
	int j = 0;
	indices3 = new GLuint[3 * countfaces3];
	for (int i = 0; i < countfaces3; i++) {
		indices3[j++] = plyFaces3[i]->vertices[0];
		indices3[j++] = plyFaces3[i]->vertices[1];
		indices3[j++] = plyFaces3[i]->vertices[2];
	}
}

void initLight() {
	nvertices = nslices*nstacks;
	nindices = 2 * (nslices + 1)*(nstacks - 1);
	lvertices = new thisVertex[nvertices];
	lindices = new GLuint[nindices];

	int n = 0;
	float var = 0;
	float r = 0;
	for (int i = 0; i < nstacks; i++) {
		for (int j = 0; j < nslices; j++) {
			float angle = j * 2 * m_pi / (float)nslices;
			var = (i / (float)(nstacks - 1)) - 0.5;
			r = sqrt(0.25 - (var*var));
			lvertices[n].position[2] = (r)*cos(angle);
			lvertices[n].position[1] = (r)*sin(angle);
			lvertices[n].position[0] = var;
			lvertices[n].position[3] = 1.0;
			lvertices[n].normal[2] = r*cos(angle);
			lvertices[n].normal[1] = r*sin(angle);
			lvertices[n].normal[0] = var;
			lvertices[n].normal[3] = 1.0;
			lvertices[n].colour[0] = 1.0;
			lvertices[n].colour[1] = 1.0;
			lvertices[n].colour[2] = 1.0;

			lvertices[n].mat_ambient[0] = 1.0f;
			lvertices[n].mat_ambient[1] = 1.0f;
			lvertices[n].mat_ambient[2] = 1.0f;
			lvertices[n].mat_ambient[3] = 1.0f;
			lvertices[n].mat_diffuse[0] = 1.0f;
			lvertices[n].mat_diffuse[1] = 1.0f;
			lvertices[n].mat_diffuse[2] = 1.0f;
			lvertices[n].mat_diffuse[3] = 1.0f;
			lvertices[n].mat_specular[0] = 1.0f;
			lvertices[n].mat_specular[1] = 1.0f;
			lvertices[n].mat_specular[2] = 1.0f;
			lvertices[n].mat_specular[3] = 1.0f;
			n++;
		}
	}
	int m = 0;
	for (int i = 0; i < nstacks - 1; i++) {
		for (int j = 0; j < nslices + 1; j++) {
			int k = j%nslices;
			lindices[m++] = i*nslices + k;
			lindices[m++] = (i + 1)*nslices + k;
		}
	}
}


void initCylinder() {
	cvertices = new thisVertex[nvertices];
	cindices = new GLuint[nindices];

	int n = 0;
	for (int i = 0; i < nstacks; i++) {
		for (int j = 0; j < nslices; j++) {
			float angle = j * 2 * m_pi / (float)nslices;
			cvertices[n].position[2] = cos(angle);
			cvertices[n].position[1] = sin(angle);
			cvertices[n].position[0] = (i / (float)(nstacks - 1)) - 0.5;
			cvertices[n].position[3] = 1.0;
			cvertices[n].normal[2] =cos(angle);
			cvertices[n].normal[1] =sin(angle);
			cvertices[n].normal[0] = (i / (float)(nstacks - 1)) - 0.5;
			cvertices[n].normal[3] = 1.0;
			cvertices[n].colour[0] = 1.0;
			cvertices[n].colour[1] = 1.0;
			cvertices[n].colour[2] = 1.0;

			cvertices[n].mat_ambient[0] = 1.0f;
			cvertices[n].mat_ambient[1] = 1.0f;
			cvertices[n].mat_ambient[2] = 1.0f;
			cvertices[n].mat_ambient[3] = 1.0f;
			cvertices[n].mat_diffuse[0] = 1.0f;
			cvertices[n].mat_diffuse[1] = 1.0f;
			cvertices[n].mat_diffuse[2] = 1.0f;
			cvertices[n].mat_diffuse[3] = 1.0f;
			cvertices[n].mat_specular[0] = 1.0f;
			cvertices[n].mat_specular[1] = 1.0f;
			cvertices[n].mat_specular[2] = 1.0f;
			cvertices[n].mat_specular[3] = 1.0f;
			n++;
		}
	}
	int m = 0;
	for (int i = 0; i < nstacks - 1; i++) {
		for (int j = 0; j < nslices + 1; j++) {
			int k = j%nslices;
			cindices[m++] = i*nslices + k;
			cindices[m++] = (i + 1)*nslices + k;
		}
	}
}
void initSquare() {
	sqvertices = new texVertex[4];
	sqindices = new GLuint[6];

	sqindices[0] = 0; sqindices[1] = 1; sqindices[2] = 2; sqindices[3] = 2; sqindices[4] = 1; sqindices[5] = 3;
	sqvertices[0].position[0] = 0.5f; 	sqvertices[0].position[1] = 0.5f; 	sqvertices[0].position[2] = 0.0f; 	sqvertices[0].position[3] = 1.0f;
	sqvertices[0].texCoord[0] = 1.0f; sqvertices[0].texCoord[1] = 1.0f;
	sqvertices[1].position[0] = -0.5f; 	sqvertices[1].position[1] = 0.5f; 	sqvertices[1].position[2] = 0.0f; 	sqvertices[1].position[3] = 1.0f;
	sqvertices[1].texCoord[0] = 0.0f; sqvertices[1].texCoord[1] = 1.0f;
	sqvertices[2].position[0] = 0.5f; 	sqvertices[2].position[1] = -0.5f; 	sqvertices[2].position[2] = 0.0f; 	sqvertices[2].position[3] = 1.0f;
	sqvertices[2].texCoord[0] = 1.0f; sqvertices[2].texCoord[1] = 0.0f;
	sqvertices[3].position[0] = -0.5f; 	sqvertices[3].position[1] = -0.5f; 	sqvertices[3].position[2] = 0.0f; 	sqvertices[3].position[3] = 1.0f;
	sqvertices[3].texCoord[0] = 0.0f; sqvertices[3].texCoord[1] = 0.0f;

	for (int n = 0; n < 4; n++) {
		sqvertices[n].colour[0] = 0.0f;
		sqvertices[n].colour[1] = 0.0f;
		sqvertices[n].colour[2] = 1.0f;
		sqvertices[n].colour[3] = 1.0f;
		sqvertices[n].normal[0] = 0.0f;
		sqvertices[n].normal[1] = 0.0f;
		sqvertices[n].normal[2] = 1.0f;
		sqvertices[n].normal[3] = 1.0f;
		sqvertices[n].mat_ambient[0] = 1.0f;
		sqvertices[n].mat_ambient[1] = 1.0f;
		sqvertices[n].mat_ambient[2] = 1.0f;
		sqvertices[n].mat_ambient[3] = 1.0f;
		sqvertices[n].mat_diffuse[0] = 1.0f;
		sqvertices[n].mat_diffuse[1] = 1.0f;
		sqvertices[n].mat_diffuse[2] = 1.0f;
		sqvertices[n].mat_diffuse[3] = 1.0f;
		sqvertices[n].mat_specular[0] = 1.0f;
		sqvertices[n].mat_specular[1] = 1.0f;
		sqvertices[n].mat_specular[2] = 1.0f;
		sqvertices[n].mat_specular[3] = 1.0f;
	}
}
void initVBO() {
	glGenBuffers(8, vboHandle);   // create interleaved VBO object

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[0]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * countvertex1, vertices1, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[1]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * countvertex2, vertices2, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[2]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * countvertex3, vertices3, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[3]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * nvertices, lvertices, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[4]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * manvertices, vertices4, GL_STATIC_DRAW); // allocate space and copy the position data over  
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[5]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * manvertices, vertices5, GL_STATIC_DRAW); // allocate space and copy the position data over  
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[6]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * manvertices, vertices6, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[8]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(thisVertex) * nvertices, cvertices, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle[7]);   // bind the first handle 
	glBufferData(GL_ARRAY_BUFFER, sizeof(texVertex) * 4, sqvertices, GL_STATIC_DRAW); // allocate space and copy the position data over  

	glBindBuffer(GL_ARRAY_BUFFER, 0);  // clean up 


	glGenBuffers(8, indexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * countfaces1 * 3, indices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * countfaces2 * 3, indices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * countfaces3 * 3, indices3, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * nindices, lindices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * manindices, indices4, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[5]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * manindices, indices5, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * manindices, indices6, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[7]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, sqindices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[8]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * nindices, cindices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void keypress(unsigned char k, int x, int y) {
	if (k == 27) exit(1);
	if (k == 'a') {			//decrease ambient
		for (int i = 0; i < 3; i++) {
			changeambient[i] = -0.1f;
		}
	}
	if (k == 'A') {			//increase ambient
		for (int i = 0; i < 3; i++) {
			changeambient[i] = 0.1f;
		}
	}
	if (k == 's') {			//decrease specular
		for (int i = 0; i < 3; i++) {
			changespecular[i] = -0.1f;
		}
	}
	if (k == 'S') {			//increase specular
		for (int i = 0; i < 3; i++) {
			changespecular[i] = 0.1f;
		}
	}
	if (k == 'd') {			//decrease diffuse
		for (int i = 0; i < 3; i++) {
			changediffuse[i] = -0.1f;
		}
	}
	if (k == 'D') {			//increase diffuse
		for (int i = 0; i < 3; i++) {
			changediffuse[i] = 0.1f;
		}
	}
	if (k == 'w') wireframe++;	//wireframe
	if (k == '+') changeshine = -2.0;	//increase shininess
	if (k == '-') changeshine = 2.0;	//decrease shininess
	if (k == 'q') {					//reset light properties
		for (int i = 0; i < 3; i++) {
			light_ambient[i] = 0.7;
			light_diffuse[i] = 0.9;
			light_specular[i] = 0.9;
		}
		mat_shine = 25.0f;
	}
	if (k == 'u') transformlight = glm::translate(transformlight, glm::vec3(0.0, 0.1, 0.0));	//light translate +y
	if (k == 'j') transformlight = glm::translate(transformlight, glm::vec3(0.0, -0.1, 0.0));	//light translate -y
	if (k == 'h') transformlight = glm::translate(transformlight, glm::vec3(-0.1, 0.0, 0.0));	//light translate -x
	if (k == 'k') transformlight = glm::translate(transformlight, glm::vec3(0.1, 0.0, 0.0));	//light translate +x
	if (k == 'm') transformlight = glm::translate(transformlight, glm::vec3(0.0, 0.0, 0.1));	//light translate +z
	if (k == 'n') transformlight = glm::translate(transformlight, glm::vec3(0.0, 0.0, -0.1));	//light translate -z

	if (k == '3') transformcam = glm::translate(transformcam, glm::vec3(0.0, -0.1, 0.0));	//cam translate +y
	if (k == '4') transformcam = glm::translate(transformcam, glm::vec3(0.0, 0.1, 0.0));	//cam translate -y
	if (k == '5') transformcam = glm::translate(transformcam, glm::vec3(0.0, 0.0, -0.1));	//cam translate +z
	if (k == '6') transformcam = glm::translate(transformcam, glm::vec3(0.0, 0.0, 0.1));	//cam translate -z
	if (k == '7') transformcam = glm::translate(transformcam, glm::vec3(-0.1, 0.0, 0.0));	//cam translate +x
	if (k == '8') transformcam = glm::translate(transformcam, glm::vec3(0.1, 0.0, 0.0));	//cam translate -x


	if (k=='1') animation = 1;
	if (k == '2') {
		animation = 0; //th = 0.0; ts = 0.0; td = 0.0;
	}

	glutPostRedisplay();
}

void click(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) mouse = 1;
		if (button == GLUT_RIGHT_BUTTON) mouse = 2;
		xdrag = x;  ydrag = y;
	}
	if (state == GLUT_UP) mouse = 0;
}

void drag(int x, int y) {
	if (mouse == 1) {
		xmouse += (x - xdrag) / 100;
		ymouse += (ydrag - y) / 100;
		if (xmouse > m_pi) xmouse -= 2 * m_pi;
		else if (xmouse < -1.0 * m_pi) xmouse += 2 * m_pi;
		if (ymouse > m_pi) ymouse -= 2 * m_pi;
		else if (ymouse < -1.0 * m_pi) ymouse += 2 * m_pi;
	}
	if (mouse == 2) {
		if ((x - xdrag) > 0 || (y - ydrag > 0)) mousescale *= 1.01;
		if ((x - xdrag) < 0 || (y - ydrag < 0)) mousescale *= 0.99;
	}
	xdrag = x; ydrag = y;
	glutPostRedisplay();
}
void timer(int v) {
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, v);
}
int main(int argc, char** argv) {

	PlyFile* input1;
	PlyFile* input2;
	PlyFile* input3;
	FILE *file1 = fopen("hammerhead3.ply", "r");
	FILE *file2 = fopen("shark3.ply", "r");
	FILE *file3 = fopen("dolphins3.ply", "r");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); //GLUT_DEPTH FOR 3D
	glutInitWindowSize(width, height);

	glutCreateWindow("Sanjana's Lab 5");

	glutDisplayFunc(display);
	glutKeyboardFunc(keypress);
	glutMouseFunc(click);
	glutMotionFunc(drag);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	// get the ply structure and open the file
	input1 = read_ply(file1);
	input2 = read_ply(file2);
	input3 = read_ply(file3);

	plyVertices1 = read(input1, &countvertex1, &countfaces1, &plyFaces1, &cenx1, &ceny1, &cenz1);
	plyVertices2 = read(input2, &countvertex2, &countfaces2, &plyFaces2, &cenx2, &ceny2, &cenz2);
	plyVertices3 = read(input3, &countvertex3, &countfaces3, &plyFaces3, &cenx3, &ceny3, &cenz3);

	initLight();
	initCylinder();

	initPlyVertices1();  //hammerhead
	initPlyVertices2();  //shark
	initPlyVertices3();  //dolphins
	initSquare();
	initTexture();
	initCubemap();

	initVBO();
	glutTimerFunc(100, timer, 0);

	programObject = SetupGLSL("shaders");

	glutMainLoop();
}