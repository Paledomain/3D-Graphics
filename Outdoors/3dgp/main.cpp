#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// 3D Models
C3dglTerrain terrain, road, river;
C3dglModel rock, house, lamp, bowl;
C3dglSkyBox skybox, skybox2;
		
// GLSL Objects (Shader Program)
C3dglProgram Program;

// City grid size
const int GRID_NUM = 3;
const int GRID_X = 64;
const int GRID_Z = 64;

//texture identifiers
GLuint idTexGrass;
GLuint idTexRoad;
GLuint idTexRiver;
GLuint idTexRock;
GLuint idTexNone;

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15.f;		// Tilt Angle
vec3 cam(0);				// Camera movement values


//light switches
int switch1, switch2;


bool init()
{
	//Load Bitmaps
	C3dglBitmap bm;

	bm.Load("models/grass.png", GL_RGBA);
	if (!bm.GetBits()) return false;

		glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexGrass);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/road.png", GL_RGBA);
	if (!bm.GetBits()) return false;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexRoad);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/river.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexRiver);
	glBindTexture(GL_TEXTURE_2D, idTexRiver);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/rock.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;

		glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexRock);
	glBindTexture(GL_TEXTURE_2D, idTexRock);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	// none (simple-white) texture
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);


	// Send the texture info to the shaders
	Program.SendUniform("texture0", 0);

	//start switches
	switch1 = 1;
	switch2 = 1;



	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!Program.Create()) return false;
	if (!Program.Attach(VertexShader)) return false;
	if (!Program.Attach(FragmentShader)) return false;
	if (!Program.Link()) return false;
	if (!Program.Use(true)) return false;

	// glut additional setup
	glutSetVertexAttribCoord3(Program.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(Program.GetAttribLocation("aNormal"));

	// load your 3D models here!
	if (!terrain.loadHeightmap("models\\heightmap.bmp", 10)) return false;
	if (!road.loadHeightmap("models\\roadmap.png", 10)) return false;
	if (!river.loadHeightmap("models\\rivermap.bmp", 10)) return false;
	if (!rock.load("models\\Rock_9.obj")) return false;
	if (!lamp.load("models\\lamp.obj")) return false;
	if (!bowl.load("models\\Bowl.obj")) return false;
	if (!house.load("models\\WoodenCabinObj\\WoodenCabinObj\\WoodenCabinObj.obj")) return false;
	house.loadMaterials("models\\WoodenCabinObj\\WoodenCabinObj\\");

	// load Sky Box

		if (!skybox.load("models\\sb_graveyard\\rt.png",
			"models\\sb_graveyard\\fd.png",
			"models\\sb_graveyard\\lt.png",
			"models\\sb_graveyard\\bk.png",
			"models\\sb_graveyard\\up.png",
			"models\\sb_graveyard\\dn.png")) return false;

		if (!skybox2.load("models\\sor_cwd\\rt.jpg",
			"models\\sor_cwd\\fd.jpg",
			"models\\sor_cwd\\lt.jpg",
			"models\\sor_cwd\\bk.jpg",
			"models\\sor_cwd\\up.jpg",
			"models\\sor_cwd\\dn.jpg")) return false; 
	

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(4.0, 1.5, 30.0),
		vec3(4.0, 1.5, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.2f, 0.6f, 1.f, 1.0f);   // blue sky background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

void done()
{
}

void render()
{
	// this global variable controls the animation
	float theta = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wrap the world
	mat4 inv = inverse(matrixView);
	float gridX = -GRID_X * (int)floor((inv[3][0] + GRID_X / 2) / GRID_X);
	float gridZ = -GRID_Z * (int)floor((inv[3][2] + GRID_Z / 2) / GRID_Z);

	// apply grid wrap
	matrixView = translate(matrixView, vec3(-gridX, 0, -gridZ));

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	matrixView = m * matrixView;

	// setup View Matrix
	Program.SendUniform("matrixView", matrixView);

	// calculate the Y position of the camera - above the ground
	float Y;

	if (inverse(matrixView)[3][1] < 13) 
	{
		Y = -terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]);
	}
	else
	{
		Y = -1.2f;
		//printf("%.0f", inverse(matrixView)[3][1]);
	}


	//render fog

	if (switch2 == 1)
	{
		Program.SendUniform("fogDensity", 0.1f);
		Program.SendUniform("fogColour", 0.25, 0.25, 0.28);
	}
	else
	{
		Program.SendUniform("fogDensity", 0.05f);
		Program.SendUniform("fogColour", 0.0, 0.0, 0.0);
	}
	
	
	// render ambient light
	Program.SendUniform("lightAmbient.on", 1);
	Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);

	// render directional light
	Program.SendUniform("lightDir.on", 1);
	Program.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	Program.SendUniform("lightDir.diffuse", 1.0, 0.8, 0.3);
	Program.SendUniform("materialDiffuse", 0.2, 0.2, 0.6);

	//render point light
	Program.SendUniform("lightPointOn", switch1);
	Program.SendUniform("lightPoint1.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint2.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint3.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint4.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint4.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint5.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint6.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint7.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint8.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint9.diffuse", 0.7, 0.5, 0.3);
	Program.SendUniform("lightPoint10.diffuse", 0.6, 0.6, 1.0);
	Program.SendUniform("lightPoint11.diffuse", 0.6, 0.6, 1.0);
	Program.SendUniform("lightPoint12.diffuse", 0.6, 0.6, 1.0);


	//render specular light (shine from point light)
	Program.SendUniform("lightPoint1.specular", 0.5, 0.5, 0.5);
	Program.SendUniform("lightPoint2.specular", 0.5, 0.5, 0.5);
	Program.SendUniform("materialSpecular", 0.6, 0.6, 1.0);
	Program.SendUniform("shininess", 13.0);

	


	// render the skybox
	Program.SendUniform("lightDir.on", 0);
	Program.SendUniform("lightPointOn", 0);

	Program.SendUniform("lightAmbient.color", 1.0, 1.0, 1.0);
	Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	Program.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);
	m = matrixView;

	if (switch2 == 1)
	{
		skybox.render(m);
	}
	else
	{
		skybox2.render(m);
	}
	

	Program.SendUniform("lightDir.on", 1);
	Program.SendUniform("lightPointOn", switch1);

	glBindTexture(GL_TEXTURE_2D, idTexNone);


	// render point lights
	int lightCounter = 1;
	string lightString;
	string lightString2 = ".position";

	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			char lightString[50];
			sprintf_s(lightString, "lightPoint%d.position", lightCounter);

			Program.SendUniform(lightString, i * GRID_X + 4.0f, Y + 3.01f, j * GRID_Z + -3.0f);
			Program.SendUniform("att_quadratic", 0.03f);
			Program.SendUniform("att_linear", 0.1f);
			Program.SendUniform("att_const", 0.0f);

			lightCounter++;
		}

	// render house point lights
	Program.SendUniform("lightPoint10.position", 2.0f, Y + 0.2f, 3.5f);
	Program.SendUniform("att_quadratic", 0.1f);
	Program.SendUniform("att_linear", 0.0f);
	Program.SendUniform("att_const", 0.0f);

	Program.SendUniform("lightPoint11.position", 1.0f, Y + 0.2f, 1.7f);
	Program.SendUniform("att_quadratic", 0.1f);
	Program.SendUniform("att_linear", 0.0f);
	Program.SendUniform("att_const", 0.0f);

	Program.SendUniform("lightPoint12.position", -1.0f, Y + 0.2f, 1.7f);
	Program.SendUniform("att_quadratic", 0.1f);
	Program.SendUniform("att_linear", 0.0f);
	Program.SendUniform("att_const", 0.0f);


	// lamp
	Program.SendUniform("speedX", 0.0f);
	Program.SendUniform("speedY", 0.0f);


	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			m = translate(m, vec3(4.0f, 3.01f, -3.0f));
			m = scale(m, vec3(0.03f, 0.03f, 0.03f));
			m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));

			Program.SendUniform("materialDiffuse", 1.0f, 1.0f, 0.7f);
			Program.SendUniform("materialAmbient", 1.0f, 1.0f, 0.7f);

			if (switch1 == 1)
				Program.SendUniform("lightAmbient.color", 1.0, 1.0, 1.0);
			else
				Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
			lamp.render(1, m);

			Program.SendUniform("lightAmbient.color", 0.3, 0.3, 0.3);
			Program.SendUniform("materialDiffuse", 0.1f, 0.1f, 0.1f);
			Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.3f);
			lamp.render(0, m);
		}


	// bowl
	Program.SendUniform("materialDiffuse", 0.1f, 0.1f, 0.1f);
	Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.3f);
	Program.SendUniform("speedX", 0.0f);
	Program.SendUniform("speedY", 0.0f);

	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			m = translate(m, vec3(4.35f, 7.1f, -3.015f));
			m = scale(m, vec3(0.5f, 0.5f, 0.5f));
			m = rotate(m, radians(180.f), vec3(0.0f, 0.0f, 1.0f));
			bowl.render(m);
		}




	// terrain!
	glBindTexture(GL_TEXTURE_2D, idTexGrass);

	Program.SendUniform("materialDiffuse", 0.8f, 0.8f, 0.2f);	// grassy green
	Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.3f);
	Program.SendUniform("speedX", 0.0f);
	Program.SendUniform("speedY", 0.0f);

	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			terrain.render(m);
		}

	// road
	glBindTexture(GL_TEXTURE_2D, idTexRoad);

	Program.SendUniform("materialDiffuse", 0.3f, 0.3f, 0.16f);
	Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.16f);
	Program.SendUniform("speedX", 0.0f);
	Program.SendUniform("speedY", 0.0f);

	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			m = translate(m, vec3(6.0f, 0.01f, 0.0f));
			road.render(m);
		}

	// river
	glBindTexture(GL_TEXTURE_2D, idTexRiver);

	Program.SendUniform("materialDiffuse", 0.6f, 0.8f, 0.8f);
	Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.3f);
	Program.SendUniform("speedX", 1.0f);
	Program.SendUniform("speedY", -7.0f);
	Program.SendUniform("time", (float)(GetTickCount() % 1000) / 1000.0f);

	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			m = translate(m, vec3(0.0f, -1.0f, 0.0f));
			river.render(m);
		}

	// rock
	glBindTexture(GL_TEXTURE_2D, idTexRock);

	Program.SendUniform("materialDiffuse", 0.6f, 0.8f, 0.8f);
	Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.3f);
	Program.SendUniform("speedX", 0.0f);
	Program.SendUniform("speedY", 0.0f);

	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			m = translate(m, vec3(20.5f, 4.01f, -5.0f));
			m = scale(m, vec3(0.05f, 0.05f, 0.05f));
			m = rotate(m, radians(0.f), vec3(0.0f, 0.0f, 1.0f));
			rock.render(m);
		}


	// house
	Program.SendUniform("materialDiffuse", 0.3f, 0.3f, 0.1f);
	Program.SendUniform("materialAmbient", 0.3f, 0.3f, 0.3f);
	Program.SendUniform("speedX", 0.0f);
	Program.SendUniform("speedY", 0.0f);

	for (int i = -GRID_NUM; i <= GRID_NUM; i++)
		for (int j = -GRID_NUM; j <= GRID_NUM; j++)
		{
			m = matrixView;
			m = translate(matrixView, vec3(0, Y, 0));
			m = translate(m, vec3(i * GRID_X, 0, j * GRID_Z));
			m = translate(m, vec3(.0f, 3.01f, 3.0f));
			m = scale(m, vec3(0.07f, 0.07f, 0.07f));
			m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
			house.render(m);
		}


	


	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void reshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	Program.SendUniform("matrixProjection", perspective(radians(60.f), ratio, 0.02f, 1000.f));
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	case 'z': if (switch1 == 1) switch1 = 0; else switch1 = 1; break;
	case 'c': if (switch2 == 1) switch2 = 0; else switch2 = 1; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}

