#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "glm.h"

#include <cmath>

#include "Matrices.h"

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "freeglut.lib")

#ifndef GLUT_WHEEL_UP
# define GLUT_WHEEL_UP   0x0003
# define GLUT_WHEEL_DOWN 0x0004
#endif

#ifndef GLUT_KEY_ESC
# define GLUT_KEY_ESC 0x001B
#endif

#ifndef max
# define max(a,b) (((a)>(b))?(a):(b))
# define min(a,b) (((a)<(b))?(a):(b))
#endif

// Shader attributes
GLint iLocPosition;
GLint iLocColor;
GLint iLocMVP;
GLint Colorstate;

char filename[7][40] = { "ColorModels/armadillo12KC.obj", "ColorModels/al7KC.obj", "ColorModels/brain18KC.obj", "ColorModels/Dino20KC.obj", "ColorModels/laurana2KC.obj", "ColorModels/igea17KC.obj", "ColorModels/teapot4KC.obj" };
GLMmodel* OBJ;
GLMmodel* BASE;
GLfloat* vertices;
GLfloat* colors;

GLfloat* bvertices;
GLfloat* bcolors;

int colorgate = 0;
int gate = 0;
int solide_gate = 0;
int filenameindex = 0;

float bmtx, bmty, bmtz;
float mmrx = 0, mmry = 0, mmrz = 0;
float bmrx, bmry, bmrz;
float mmsx = 1, mmsy = 1, mmsz = 1;
float bmsx, bmsy, bmsz;
float vetx = 0, vety = 0, vetz = 0;
float vctx = 0, vcty = 0, vctz = 0;
float ptx, pty, ptz;

int mvpstate = 0;
int tsrstate = 0;
int projectstate = 0;

int startx = 0;
int starty = 0;
int endx = 0;
int endy = 0;

float tempx = 0;
float tempy = 0;
float tempz = 0;
float sendx = 1;
float sendy = 1;
float sendz = 1;

int mousestate = -1;

Matrix4 OBJnorm;

Matrix4 OT = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1);
Matrix4 OM = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1);

Matrix4 OV = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1);

Matrix4 OS = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1);

Matrix4 OR = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	);

Matrix4 OMVP;
Matrix4 BM;
Matrix4 BMVP;
Matrix4 ect;
Matrix4 echange;
Matrix4 OP;
Matrix4 paranorm = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	);
Matrix4 persnorm = Matrix4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	);

Matrix4 OBJscale, OBJtrans, BASEscale, BASEtrans,BASEtrans2;

float pleft = -1;
float pright = 1;
float pup = 1;
float pdown = -1;
float pnear = 1;
float pfar = 5;

//center放在(0, 0, -1)
//eye放在 + Z或 - Z上(0, 0, 0)
//up以Y軸(0, 1, 0)

Vector3 eyep = Vector3(0, 0, 0);
Vector3 eyeo = Vector3(0, 0, 0);
Vector3 centerp = Vector3(0, 0, -1);
Vector3 centero = Vector3(0, 0, -1);
Vector3 upp = Vector3(0, 1, 0);
Vector3 forward, right, up;

Vector3 RX, RY, RZ,P1P2,P1P3;

void traverseColorModel()
{
	int i;

	GLfloat maxVal[3];
	GLfloat minVal[3];

	int indc[3];
	GLfloat tmp;
	colors = (GLfloat*)malloc(sizeof(GLfloat)* OBJ->numtriangles * 9);
	vertices = (GLfloat*)malloc(sizeof(GLfloat)* OBJ->numtriangles * 9);
	bcolors = (GLfloat*)malloc(sizeof(GLfloat)* BASE->numtriangles * 9);
	bvertices = (GLfloat*)malloc(sizeof(GLfloat)* BASE->numtriangles * 9);

	// The center position of the model 
	OBJ->position[0] = 0;
	OBJ->position[1] = 0;
	OBJ->position[2] = 0;

	int first = 1;
	for (i = 0; i<(int)OBJ->numtriangles; i++)
	{
		// the index of each vertex
		indc[0] = OBJ->triangles[i].vindices[0];
		indc[1] = OBJ->triangles[i].vindices[1];
		indc[2] = OBJ->triangles[i].vindices[2];

		if (first){
			maxVal[0] = OBJ->vertices[indc[0] * 3 + 0];
			maxVal[1] = OBJ->vertices[indc[0] * 3 + 1];
			maxVal[2] = OBJ->vertices[indc[0] * 3 + 2];
			minVal[0] = OBJ->vertices[indc[0] * 3 + 0];
			minVal[1] = OBJ->vertices[indc[0] * 3 + 1];
			minVal[2] = OBJ->vertices[indc[0] * 3 + 2];
			first = 0;
		}
		for (int inindc = 0; inindc < 3; inindc++){
			for (int otn = 0; otn < 3; otn++){
				tmp = (OBJ->vertices[indc[inindc] * 3 + otn]);
				maxVal[otn] = max(maxVal[otn], tmp);
				minVal[otn] = min(minVal[otn], tmp);
				vertices[i * 9 + inindc * 3 + otn] = tmp;
				colors[i * 9 + inindc * 3 + otn] = OBJ->colors[indc[inindc] * 3 + otn];
			}
		}
	}

	OBJ->position[0] = (maxVal[0] + minVal[0]) / 2;
	OBJ->position[1] = (maxVal[1] + minVal[1]) / 2;
	OBJ->position[2] = (maxVal[2] + minVal[2]) / 2;

	GLfloat scale0 = maxVal[0] - minVal[0];
	GLfloat scale1 = maxVal[1] - minVal[1];
	GLfloat scale2 = maxVal[2] - minVal[2];

	GLfloat scalein = max(max(scale0, scale1), scale2) / 2;

	OBJscale = Matrix4(
		1 / scalein, 0, 0, 0,
		0, 1 / scalein, 0, 0,
		0, 0, 1 / scalein, 0,
		0,0,0,1
		);
	OBJtrans = Matrix4(
		1, 0, 0, -OBJ->position[0],
		0, 1, 0, -OBJ->position[1],
		0, 0, 1, -OBJ->position[2],
		0,0,0,1
		);

	/*for (i = 0; i < (int)OBJ->numtriangles * 3; i++){
		vertices[i * 3 + 0] = (vertices[i * 3 + 0] - OBJ->position[0]) / scalein;
		vertices[i * 3 + 1] = (vertices[i * 3 + 1] - OBJ->position[1]) / scalein;
		vertices[i * 3 + 2] = (vertices[i * 3 + 2] - OBJ->position[2]) / scalein;
	}*/


	BASE->position[0] = 0;
	BASE->position[1] = 0;
	BASE->position[2] = 0;
	GLfloat bmaxVal[3] = { 0, 0, 0 };
	GLfloat bminVal[3] = { 0, 0, 0 };

	first = 1;
	for (i = 0; i<(int)BASE->numtriangles; i++)
	{
		// the index of each vertex
		indc[0] = BASE->triangles[i].vindices[0];
		indc[1] = BASE->triangles[i].vindices[1];
		indc[2] = BASE->triangles[i].vindices[2];

		if (first){
			bmaxVal[0] = BASE->vertices[indc[0] * 3 + 0];
			bmaxVal[1] = BASE->vertices[indc[0] * 3 + 1];
			bmaxVal[2] = BASE->vertices[indc[0] * 3 + 2];
			bminVal[0] = BASE->vertices[indc[0] * 3 + 0];
			bminVal[1] = BASE->vertices[indc[0] * 3 + 1];
			bminVal[2] = BASE->vertices[indc[0] * 3 + 2];
			first = 0;
		}
		for (int inindc = 0; inindc < 3; inindc++){
			for (int otn = 0; otn < 3; otn++){
				tmp = (BASE->vertices[indc[inindc] * 3 + otn]);
				bmaxVal[otn] = max(bmaxVal[otn], tmp);
				bminVal[otn] = min(bminVal[otn], tmp);
				bvertices[i * 9 + inindc * 3 + otn] = tmp;
				bcolors[i * 9 + inindc * 3 + otn] = BASE->colors[indc[inindc] * 3 + otn];
			}
		}
	}

	BASE->position[0] = (bmaxVal[0] + bminVal[0]) / 2;
	BASE->position[1] = (bmaxVal[1] + bminVal[1]) / 2;
	BASE->position[2] = (bmaxVal[2] + bminVal[2]) / 2;

	scale0 = bmaxVal[0] - bminVal[0];
	scale1 = bmaxVal[1] - bminVal[1];
	scale2 = bmaxVal[2] - bminVal[2];

	scalein = max(max(scale0, scale1), scale2) / 2;

	BASEscale = Matrix4(
		1 / scalein, 0, 0, 0,
		0, 1 / scalein *0.01, 0, 0,
		0, 0, 1 / scalein, 0,
		0, 0, 0, 1
		);
	BASEtrans = Matrix4(
		1, 0, 0, -BASE->position[0],
		0, 1, 0, -BASE->position[1],
		0, 0, 1, -BASE->position[2],
		0, 0, 0, 1
		);
	BASEtrans2 = Matrix4(
		1, 0, 0, 0,
		0, 1, 0, -0.99,
		0, 0, 1, 0,
		0, 0, 0, 1
		);
	for (i = 0; i < (int)BASE->numtriangles * 3; i++){
		bvertices[i * 3 + 0] = (bvertices[i * 3 + 0] - BASE->position[0]);
		bvertices[i * 3 + 1] = (bvertices[i * 3 + 1] - BASE->position[1]);
		bvertices[i * 3 + 2] = (bvertices[i * 3 + 2] - BASE->position[2]);
	}
}

void loadOBJModel(int filenameindex)
{
	// read an obj model here
	if(OBJ != NULL){
		free(OBJ);
	}
	OBJ = glmReadOBJ(filename[filenameindex]);
	BASE = glmReadOBJ("ColorModels/boxC.obj");
	//printf("%s\n", filename);

	// traverse the color model
	traverseColorModel();
}

void onIdle()
{
	glutPostRedisplay();
}

void onDisplay(void)
{
	// clear canvas
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocColor);

	GLfloat* triangle_color = colors;
	GLfloat* triangle_vertex = vertices;
	GLfloat* base_color = bcolors;
	GLfloat* base_vertex = bvertices;

	switch (mvpstate)
	{
		case 0:
			switch (tsrstate)
			{
			case 0:
				OT = Matrix4(
					1, 0, 0, tempx,
					0, 1, 0, tempy,
					0, 0, 1, tempz,
					0, 0, 0, 1);
				break;
			case 1:
				OS = Matrix4(
					sendx, 0, 0, 0,
					0, sendy, 0, 0,
					0, 0, sendz, 0,
					0, 0, 0, 1
					);
				break;
			}
			break;
	}
	//printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", mmchange[0], mmchange[1], mmchange[2], mmchange[3], mmchange[4], mmchange[5], mmchange[6], mmchange[7], mmchange[8], mmchange[9], mmchange[10], mmchange[11], mmchange[12], mmchange[13], mmchange[14], mmchange[15]);
	//std::cout << OP << std::endl;
	OM = OR * OM;
	//OM = mmchange * OM;
	

	//printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", OP[0], OP[1], OP[2], OP[3], OP[4], OP[5], OP[6], OP[7], OP[8], OP[9], OP[10], OP[11], OP[12], OP[13], OP[14], OP[15]);
	OMVP = OP*OV*OT*OS*OR*OBJscale*OBJtrans;

	GLfloat omvp[16];
	// row-major ---> column-major
	omvp[0] = OMVP[0];  omvp[4] = OMVP[1];   omvp[8]  = OMVP[2];    omvp[12] = OMVP[3];  
	omvp[1] = OMVP[4];  omvp[5] = OMVP[5];   omvp[9]  = OMVP[6];    omvp[13] = OMVP[7];  
	omvp[2] = OMVP[8];  omvp[6] = OMVP[9];   omvp[10] = OMVP[10];   omvp[14] = OMVP[11];  
	omvp[3] = OMVP[12]; omvp[7] = OMVP[13];  omvp[11] = OMVP[14];   omvp[15] = OMVP[15];

	// bind array pointers to shader
	glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex);
	glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, triangle_color);
	
	// bind uniform matrix to shader
	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, omvp);

	// draw the array we just bound
	if (solide_gate){
		glDrawArrays(GL_TRIANGLES, 0, 3 * OBJ->numtriangles);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glDrawArrays(GL_TRIANGLES, 0, 3 * OBJ->numtriangles);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	BM = Matrix4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	BMVP = OP*OV*BM*BASEtrans2*BASEscale*BASEtrans;

	GLfloat bmvp[16];
	bmvp[0] = BMVP[0];  bmvp[4] = BMVP[1];   bmvp[8] = BMVP[2];    bmvp[12] = BMVP[3];
	bmvp[1] = BMVP[4];  bmvp[5] = BMVP[5];   bmvp[9] = BMVP[6];    bmvp[13] = BMVP[7];
	bmvp[2] = BMVP[8];  bmvp[6] = BMVP[9];   bmvp[10] = BMVP[10];   bmvp[14] = BMVP[11];
	bmvp[3] = BMVP[12]; bmvp[7] = BMVP[13];  bmvp[11] = BMVP[14];   bmvp[15] = BMVP[15];

	// bind array pointers to shader
	glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, base_vertex);
	glVertexAttribPointer(iLocColor, 3, GL_FLOAT, GL_FALSE, 0, base_color);

	// bind uniform matrix to shader
	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, bmvp);

	// draw the array we just bound
	if (solide_gate){
		glDrawArrays(GL_TRIANGLES, 0, 3 * BASE->numtriangles);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glDrawArrays(GL_TRIANGLES, 0, 3 * BASE->numtriangles);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glutSwapBuffers();
}

void showShaderCompileStatus(GLuint shader, GLint *shaderCompiled)
{
	glGetShaderiv(shader, GL_COMPILE_STATUS, shaderCompiled);
	if(GL_FALSE == (*shaderCompiled))
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character.
		GLchar *errorLog = (GLchar*) malloc(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		fprintf(stderr, "%s", errorLog);

		glDeleteShader(shader);
		free(errorLog);
	}
}

void setShaders()
{
	GLuint v, f, p;
	char *vs = NULL;
	char *fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shader.vert");
	fs = textFileRead("shader.frag");

	glShaderSource(v, 1, (const GLchar**)&vs, NULL);
	glShaderSource(f, 1, (const GLchar**)&fs, NULL);

	free(vs);
	free(fs);

	// compile vertex shader
	glCompileShader(v);
	GLint vShaderCompiled;
	showShaderCompileStatus(v, &vShaderCompiled);
	if(!vShaderCompiled) system("pause"), exit(123);

	// compile fragment shader
	glCompileShader(f);
	GLint fShaderCompiled;
	showShaderCompileStatus(f, &fShaderCompiled);
	if(!fShaderCompiled) system("pause"), exit(456);

	p = glCreateProgram();

	// bind shader
	glAttachShader(p, f);
	glAttachShader(p, v);

	// link program
	glLinkProgram(p);

	iLocPosition = glGetAttribLocation (p, "av4position");
	iLocColor    = glGetAttribLocation (p, "av3color");
	Colorstate = glGetUniformLocation(p, "Cstate");
	iLocMVP		 = glGetUniformLocation(p, "mvp");

	glUseProgram(p);
	glUniform1i(Colorstate, colorgate);
}


void onMouse(int who, int state, int x, int y)
{
	if (mvpstate == 3)
		mousestate = who;
	if (gate){
		printf("%18s(): (%d, %d) \n", __FUNCTION__, x, y);
	}
	if (projectstate){
		printf("Perspective ");
	}
	else{
		printf("Parallera ");
	}
	switch (who)
	{
	case GLUT_LEFT_BUTTON:    break;
	case GLUT_MIDDLE_BUTTON:  break;
	case GLUT_RIGHT_BUTTON:   break;
	case GLUT_WHEEL_UP:      
		switch (mvpstate){
		case 0:
			printf("Model ");
			switch (tsrstate)
			{
				case 0:
					printf("Translate.\n");
					tempz += 1.0 / 150;
					break;
				case 1:
					printf("Scale.\n");
					sendz *= 1.01;
					break;
				case 2:
					printf("Rotate.\n");
					OR = Matrix4(
						cos(1.0/10), -sin(1.0/10), 0, 0,
						sin(1.0/10), cos(1.0/10), 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1) * OR;
					break;
				default:
					break;
			}
			break;
		case 1:
			printf("View Eye.\n");
			vetz += 0.01;
			eyep = Vector3(vetx, vety, vetz) + eyeo;
			forward = centerp - eyep;
			right = forward.cross(Vector3(0.0, 1.0, 0.0));
			up = right.cross(forward);
			upp = eyep + up;

			P1P2 = centerp - eyep;
			RZ = P1P2.normalize();
			P1P3 = upp - eyep;
			RX = (P1P2.cross(P1P3)).normalize();
			RY = RX.cross(RZ);

			ect = Matrix4(
				RX.x, RX.y, RX.z, 0,
				RY.x, RY.y, RY.z, 0,
				-RZ.x, -RZ.y, -RZ.z, 0,
				0, 0, 0, 1
				);
			echange = ect * Matrix4(
				1, 0, 0, -eyep.x,
				0, 1, 0, -eyep.y,
				0, 0, 1, -eyep.z,
				0, 0, 0, 1
				);
			OV = echange;
			break;
		case 2:
			printf("View Center.\n");
			vctz += 0.01;
			centerp = Vector3(vctx, vcty, vctz) + centero;
			eyep = Vector3(0, 0, 2) + eyeo;
			forward = centerp - eyep;
			right = forward.cross(Vector3(0.0, 1.0, 0.0));
			up = right.cross(forward);
			upp = eyep + up;

			P1P2 = centerp - eyep;
			RZ = P1P2.normalize();
			P1P3 = upp - eyep;
			RX = (P1P2.cross(P1P3)).normalize();
			RY = RX.cross(RZ);

			ect = Matrix4(
				RX.x, RX.y, RX.z, 0,
				RY.x, RY.y, RY.z, 0,
				-RZ.x, -RZ.y, -RZ.z, 0,
				0, 0, 0, 1
				);
			echange = ect * Matrix4(
				1, 0, 0, -eyep.x,
				0, 1, 0, -eyep.y,
				0, 0, 1, -eyep.z,
				0, 0, 0, 1
				);
			OV = echange;
			break;
		}
		break;
	case GLUT_WHEEL_DOWN:    
		switch (mvpstate){
		case 0:
			printf("Model ");
			switch (tsrstate)
			{
			case 0:
				printf("Translate.\n");
				tempz += -1.0 / 150;
				break;
			case 1:
				printf("Scale.\n");
				sendz *= 0.99;
				break;
			case 2:
				printf("Rotate.\n");
				OR = Matrix4(
					cos(-1.0 / 10), -sin(-1.0 / 10), 0, 0,
					sin(-1.0 / 10), cos(-1.0 / 10), 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1) * OR;
				break;
			default:
				break;
			}
			break;
		case 1:
			printf("View Eye.\n");
			vetz += -0.01;
			eyep = Vector3(vetx, vety, vetz) + eyeo;
			forward = centerp - eyep;
			right = forward.cross(Vector3(0.0, 1.0, 0.0));
			up = right.cross(forward);
			upp = eyep + up;

			P1P2 = centerp - eyep;
			RZ = P1P2.normalize();
			P1P3 = upp - eyep;
			RX = (P1P2.cross(P1P3)).normalize();
			RY = RX.cross(RZ);

			ect = Matrix4(
				RX.x, RX.y, RX.z, 0,
				RY.x, RY.y, RY.z, 0,
				-RZ.x, -RZ.y, -RZ.z, 0,
				0, 0, 0, 1
				);
			echange = ect * Matrix4(
				1, 0, 0, -eyep.x,
				0, 1, 0, -eyep.y,
				0, 0, 1, -eyep.z,
				0, 0, 0, 1
				);
			OV = echange;
			break;
		case 2:
			printf("View Center.\n");
			vctz += -0.01;
			centerp = Vector3(vctx, vcty, vctz) + centero;
			eyep = Vector3(0, 0, 2) + eyeo;
			forward = centerp - eyep;
			std::cout << forward << std::endl;
			right = forward.cross(Vector3(0.0, 1.0, 0.0));
			up = right.cross(forward);
			upp = eyep + up;

			P1P2 = centerp - eyep;
			RZ = P1P2.normalize();
			P1P3 = upp - eyep;
			RX = (P1P2.cross(P1P3)).normalize();
			RY = RX.cross(RZ);

			ect = Matrix4(
				RX.x, RX.y, RX.z, 0,
				RY.x, RY.y, RY.z, 0,
				-RZ.x, -RZ.y, -RZ.z, 0,
				0, 0, 0, 1
				);
			echange = ect * Matrix4(
				1, 0, 0, -eyep.x,
				0, 1, 0, -eyep.y,
				0, 0, 1, -eyep.z,
				0, 0, 0, 1
				);
			OV = echange;
			break;
		}
		break;
	}
	switch (state)
	{
		case GLUT_DOWN:
			startx = x;
			starty = y;
			break;
		case GLUT_UP:
			break;
	}
}

void onMouseMotion(int x, int y)
{
	if (gate){
		printf("%18s(): (%d, %d) mouse move\n", __FUNCTION__, x, y);	
	}
	if (projectstate){
		printf("Perspective ");
	}
	else{
		printf("Parallera ");
	}
	switch (mvpstate){
	case 0:
		printf("Model ");
		switch (tsrstate)
		{
		case 0:
			printf("Translate.\n");
			tempx += (x - startx) * 0.001;
			tempy += (starty - y) * 0.001;
			break;
		case 1:
			printf("Scale.\n");
			if (startx > x){
				sendx *= 1.01;
			}
			else{
				if (startx < x)
					sendx *= 0.99;
			}
			if (starty > y)
				sendy *= 1.01;
			else{
				if (starty < y)
					sendy *= 0.99;
			}
			if (sendx < 0.05)
				sendx = 0.05;
			if (sendy < 0.05)
				sendy = 0.05;
			break;
		case 2:
			printf("Rotate.\n");
			mmry = (x - startx) * 0.1 / 10;
			mmrx = (y - starty) * 0.1 / 10;
			OR = OR * Matrix4(
				1, 0, 0, 0,
				0, cos(mmrx), -sin(mmrx), 0,
				0, sin(mmrx), cos(mmrx), 0,
				0, 0, 0, 1);
			OR = OR * Matrix4(
				cos(mmry), 0, sin(mmry), 0,
				0, 1, 0, 0,
				-sin(mmry), 0, cos(mmry), 0,
				0, 0, 0, 1
				);
			break;
		default:
			break;
		}
		break;
	case 1:
		printf("View Eye.\n");
		vetx += (x - startx) * 0.01;
		vety += (starty - y) * 0.01;

		eyep = Vector3(vetx, vety, vety) + eyeo;
		forward = centerp - eyep;
		right = forward.cross(Vector3(0.0,1.0,0.0));
		up = right.cross(forward);
		upp = eyep + up;

		P1P2 = centerp - eyep;
		RZ = P1P2.normalize();
		P1P3 = upp - eyep;
		RX = (P1P2.cross(P1P3)).normalize();
		RY = RX.cross(RZ).normalize();
				
		ect = Matrix4(
			RX.x, RX.y, RX.z, 0,
			RY.x, RY.y, RY.z, 0,
			-RZ.x, -RZ.y, -RZ.z, 0,
			0,0,0,1
			);
		echange = ect * Matrix4(
			1, 0, 0, -eyep.x,
			0, 1, 0, -eyep.y,
			0, 0, 1, -eyep.z,
			0,0,0,1
			);
		OV = echange;
		break;
	case 2:
		printf("View Center.\n");
		vctx += (x - startx) * 0.01;
		vcty += (starty - y) * 0.01;
		centerp = Vector3(vctx, vcty, vcty) + centero;
		eyep = Vector3(0, 0, 2) + eyeo;
		forward = centerp - eyep;
		right = forward.cross(Vector3(0.0, 1.0, 0.0));
		up = right.cross(forward);
		upp = eyep + up;

		P1P2 = centerp - eyep;
		RZ = P1P2.normalize();
		P1P3 = upp - eyep;
		RX = (P1P2.cross(P1P3)).normalize();
		RY = RX.cross(RZ).normalize();

		ect = Matrix4(
			RX.x, RX.y, RX.z, 0,
			RY.x, RY.y, RY.z, 0,
			-RZ.x, -RZ.y, -RZ.z, 0,
			0, 0, 0, 1
			);
		echange = ect * Matrix4(
			1, 0, 0, -eyep.x,
			0, 1, 0, -eyep.y,
			0, 0, 1, -eyep.z,
			0, 0, 0, 1
			);
		OV = echange;
		break;
	case 3:
		
		if (projectstate){
			//perspective		
			switch (mousestate)
			{
				case GLUT_LEFT_BUTTON:
					printf("Change Left Right Top Bottom.\n");
					pright += 0.01 * (x - startx);
					pleft -= 0.01 * (x - startx);
					pup += 0.01 * (starty - y);
					pdown -= 0.01 * (starty - y);
					break;
				case GLUT_RIGHT_BUTTON:
					printf("Change Near Far.\n");
					pnear += 0.01 * (x - startx);
					pnear += 0.01 * (starty - y);
					pfar += 0.01 * (starty - y);
					pfar += 0.01 * (x - startx);
					break;
			}
			OP = Matrix4(
				2 * pnear / (pright - pleft), 0, (pright + pleft) / (pright - pleft), 0,
				0, 2 * pnear / (pup - pdown), (pup + pdown) / (pup - pdown), 0,
				0, 0, -(pfar + pnear) / (pfar - pnear), -2 * pfar * pnear / (pfar - pnear),
				0, 0, -1, 0
				);
		}
		else{
			//parall
			switch (mousestate)
			{
				case GLUT_LEFT_BUTTON:
					printf("Change Left Right Top Bottom.\n");
					pright += 0.01 * (x - startx);
					pleft -= 0.01 * (x - startx);
					pup += 0.01 * (starty - y);
					pdown -= 0.01 * (starty - y);
					break;
				case GLUT_RIGHT_BUTTON:
					printf("Change Near Far.\n");
					pnear += 0.01 * (x - startx);
					pnear += 0.01 * (starty - y);
					pfar += 0.01 * (starty - y);
					pfar += 0.01 * (x - startx);
					break;
			}	
			OP = Matrix4(
				2 / (pright - pleft), 0, 0, -(pright + pleft) / (pright - pleft),
				0, 2 / (pup - pdown), 0, -(pup + pdown) / (pup - pdown),
				0, 0, -2 / (pfar - pnear), -(pfar + pnear) / (pfar - pnear),
				0, 0, 0, 1
				);
		}
		OP = OP * Matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, -2,
			0, 0, 0, 1);
		break;
	}
	startx = x;
	starty = y;
}

void onKeyboard(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case GLUT_KEY_ESC: /* the Esc key */
		exit(0);
		break;
	case 'h':
	case 'H':
		printf("-.-.-.-.-.-.-.-.-.-Help menu (H/h).-.-.-.-.-.-.-.-.-.-\n");
		/*printf(".  W/w -- switch between solid:wired rendering mode. .\n");*/
		printf(".  X/x -- move to next model	                     .\n");
		printf("|  Z/z -- move to previous model.                    |\n");	
		/*printf("|  M/m -- open or close Mouse callback Messages.     |\n");
		printf(".  C/c -- change color.                              .\n");
		printf("|  S/s -- show author information.                   |\n");*/
		printf(".  T/t -- Translation.                               .\n");
		printf(".  R/r -- Rotation.                                  |\n");
		printf(".  R/r -- Scaling.                                   .\n");
		printf(".  E/e -- Eye position.                              |\n");
		printf(".  L/l -- Center position.                           .\n");
		printf(".  O/o -- Switch projection.                         |\n");
		printf(".  P/p -- Projection transformation.                 .\n");
		printf(".-.-.-.-.-..-.-.-.-Help menu (H/h)-.-.-.-.-.-.-.-.-.-.\n");
		break;
	case 'w':
	case 'W':
		solide_gate = !solide_gate;
		if (solide_gate){
			printf("Show Wireframes.\n\n");
		}
		else{
			printf("Show Solide.\n\n");
		}
		//switch between solid:wired rendering mode
		break;
	case 'z':
	case 'Z':
		free(colors);
		free(vertices);
		filenameindex = (filenameindex + 6) % 7;
		loadOBJModel(filenameindex);
		//move to previous model
		break;
	case 'x':
	case 'X':
		free(colors);
		free(vertices);
		filenameindex = (filenameindex + 1) % 7;
		loadOBJModel(filenameindex);
		//move to next model
		break;
	case 'm':
	case 'M':
		gate = !gate;
		if (gate){
			printf("The Mouse callback messages have been opeded.\n\n");
		}
		else{
			printf("The Mouse callback messages have been closed.\n\n");
		}
		break;
	case 'c':
	case 'C':
		colorgate = (colorgate + 1) % 4;
		setShaders();
		printf("Change color.\n");
		break;
	case 'T':
	case 't':
		printf("Model Translate.\n");
		mvpstate = 0;
		tsrstate = 0;
		break;
	case 'S':
	case 's':
		printf("Model Scale.\n");
		mvpstate = 0;
		tsrstate = 1;
		break;
	case 'R':
	case 'r':
		printf("Model Rotate.\n");
		mvpstate = 0;
		tsrstate = 2;
		break;
	case 'E':
	case 'e':
		printf("Change Eye.\n");
		mvpstate = 1;
		break;
	case 'L':
	case 'l':
		printf("Change Lookat(Center).\n");
		mvpstate = 2;
		break;
	case 'O':
	case 'o':
		projectstate = !projectstate;
		printf("%d\n", projectstate);
		if (projectstate){
			OP = Matrix4(
				2 * pnear / (pright - pleft), 0, (pright + pleft) / (pright - pleft), 0,
				0, 2 * pnear / (pup - pdown), (pup + pdown) / (pup - pdown), 0,
				0, 0, -(pfar + pnear) / (pfar - pnear), -2 * pfar * pnear / (pfar - pnear),
				0, 0, -1, 0
				);
		}
		else{
			OP = Matrix4(
				2 / (pright - pleft), 0, 0, -(pright + pleft) / (pright - pleft),
				0, 2 / (pup - pdown), 0, -(pup + pdown) / (pup - pdown),
				0, 0, -2 / (pfar - pnear), -(pfar + pnear) / (pfar - pnear),
				0, 0, 0, 1
				);
		}
		OP = OP * Matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, -2,
			0, 0, 0, 1);
		break;
	case 'P':
	case 'p':
		mvpstate = 3;
		break;
	}
	
	printf("\n");
}

void onKeyboardSpecial(int key, int x, int y){
	printf("%18s(): (%d, %d) ", __FUNCTION__, x, y);
	switch(key)
	{
		case GLUT_KEY_LEFT:
			printf("key: LEFT ARROW");
			break;
			
		case GLUT_KEY_RIGHT:
			printf("key: RIGHT ARROW");
			break;

		default:
			printf("key: 0x%02X      ", key);
			break;
	}
	printf("\n");
}


void onWindowReshape(int width, int height)
{
	printf("%18s(): %dx%d\n", __FUNCTION__, width, height);
}

int main(int argc, char **argv) 
{
	if (projectstate){
		OP = Matrix4(
			2 * pnear / (pright - pleft), 0, (pright + pleft) / (pright - pleft), 0,
			0, 2 * pnear / (pup - pdown), (pup + pdown) / (pup - pdown), 0,
			0, 0, -(pfar + pnear) / (pfar - pnear), -2 * pfar * pnear / (pfar - pnear),
			0, 0, -1, 0
			);
	}
	else{
		OP = Matrix4(
			2 / (pright - pleft), 0, 0, -(pright + pleft) / (pright - pleft),
			0, 2 / (pup - pdown), 0, -(pup + pdown) / (pup - pdown),
			0, 0, -2 / (pfar - pnear), -(pfar + pnear) / (pfar - pnear),
			0, 0, 0, 1
			);
	}
	OP = OP * Matrix4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, -2,
		0, 0, 0, 1);
	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create window
	glutInitWindowPosition(500, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("105062467 2017-CGHW2");

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(1);
	}

	// load obj models through glm
	loadOBJModel(0);

	// register glut callback functions
	glutDisplayFunc (onDisplay);
	glutIdleFunc    (onIdle);
	glutKeyboardFunc(onKeyboard);
	glutSpecialFunc (onKeyboardSpecial);
	glutMouseFunc   (onMouse);
	glutMotionFunc  (onMouseMotion);
	glutReshapeFunc (onWindowReshape);

	// set up shaders here
	setShaders();
	
	glEnable(GL_DEPTH_TEST);

	// main loop
	glutMainLoop();

	// free
	glmDelete(OBJ);
	glmDelete(BASE);

	return 0;
}

