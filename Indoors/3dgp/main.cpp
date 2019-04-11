#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Include GLM extensions
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

//texture identifiers
GLuint idTexWood;
GLuint idTexMetal;
GLuint idTexMarble;
GLuint idTexNone;


// 3D models
C3dglModel utah;
C3dglModel table;
C3dglModel vase;
C3dglModel lamp;
C3dglModel livingRoom;
C3dglModel nymph;
C3dglModel tree;

// Buffer name
unsigned buf;

//light switches
int switch1;
int switch2;

// Vertex Data:
float vertices[] = {
	-4, 0,-4, 0, 4,-7, 4, 0,-4, 0, 4,-7, 0, 7, 0, 0, 4,-7,
	-4, 0, 4, 0, 4, 7, 4, 0, 4, 0, 4, 7, 0, 7, 0, 0, 4, 7,
	-4, 0,-4,-7, 4, 0,-4, 0, 4,-7, 4, 0, 0, 7, 0,-7, 4, 0,
	4, 0,-4, 7, 4, 0, 4, 0, 4, 7, 4, 0, 0, 7, 0, 7, 4, 0,
	-4, 0,-4, 0,-1, 0,-4, 0, 4, 0,-1, 0, 4, 0,-4, 0,-1, 0,
	4, 0, 4, 0,-1, 0,-4, 0, 4, 0,-1, 0, 4, 0,-4, 0,-1, 0 };



// GLSL Program
C3dglProgram Program;


// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15;		// Tilt Angle
float angleRot = 0.1f;		// Camera orbiting angle
vec3 cam(0);				// Camera movement values

bool init()
{
	//load bitmaps
	C3dglBitmap bm;

	bm.Load("models/oak.bmp", GL_RGBA);
	if (!bm.GetBits()) return false;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexWood);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
				GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/marble1.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexMarble);
	glBindTexture(GL_TEXTURE_2D, idTexMarble);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/metal.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexMetal);
	glBindTexture(GL_TEXTURE_2D, idTexMetal);
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

	glutSetVertexAttribCoord3(Program.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(Program.GetAttribLocation("aNormal"));

	// load your 3D models here!
	if (!utah.load("models\\utah_teapot_hires.obj")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!lamp.load("models\\lamp.obj")) return false;
	if (!livingRoom.load("models\\LivingRoomObj\\LivingRoom.obj")) return false;
	livingRoom.loadMaterials("models\\LivingRoomObj\\");
	if (!tree.load("models\\christmas_tree\\christmas_tree.obj")) return false;
	tree.loadMaterials("models\\christmas_tree\\");
	if (!nymph.load("models\\nymph1.obj")) return false;

	// Generate 1 buffer name
	glGenBuffers(1, &buf);
	// Bind (activate) the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	// Send data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// GL_ARRAY_BUFFER – informs OGL that this is an array buffer
	// GL_STATIC_DRAW – informs OGL that the buffer is written once, read many times



	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(					//field of view
		vec3(0.0, 25.0, 10.0),				//position
		vec3(0.0, 25.0, 0.0),				//angle
		vec3(0.0, 1.0, 0.0));				//orientation(up vector)

	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift+AD or arrow key to auto-orbit" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

void done()
{
	glDeleteBuffers(1, &buf);

}

void render()
{
	// this global variable controls the animation
	float theta = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	m = m * matrixView;
	m = rotate(m, radians(angleRot), vec3(0.f, 1.f, 0.f));				// animate camera orbiting
	matrixView = m;

	// setup View Matrix
	Program.SendUniform("matrixView", matrixView);
	Program.SendUniform("materialDiffuse", 0.2, 0.2, 0.6);

	//render ambient light
	Program.SendUniform("lightAmbient.on", 1);
	Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);

	//render directional light
	Program.SendUniform("lightDir.on", 1);
	Program.SendUniform("lightDir.direction", -0.5, -0.6, -0.5);
	Program.SendUniform("lightDir.diffuse", 0.9, 0.4, 0.4);	  // dimmed white light

	//render point light
	Program.SendUniform("lightPoint1.on", switch1);
	Program.SendUniform("lightPoint1.diffuse", 0.2, 0.2, 0.4);

	Program.SendUniform("lightPoint2.on", switch2);
	Program.SendUniform("lightPoint2.diffuse", 0.7, 0.3, 0.0);

	//render specular light (shine from point light)
	Program.SendUniform("lightPoint1.specular", 0.5, 0.5, 0.5);
	Program.SendUniform("lightPoint2.specular", 0.5, 0.5, 0.5);
	Program.SendUniform("materialSpecular", 0.6, 0.6, 1.0);
	Program.SendUniform("shininess", 3.0);
	

	


	//PYRAMID!
	glBindTexture(GL_TEXTURE_2D, idTexMarble);

	Program.SendUniform("materialDiffuse", 0.7, 0.7, 0.7);
	Program.SendUniform("materialSpecular", 0.3, 0.3, 0.3);
	Program.SendUniform("shininess", 6.0);

	m = matrixView;
	m = translate(m, vec3(12.0f, 16.3f, -2.3f));
	m = scale(m, vec3(0.6f, 0.6f, 0.6f));
	m = rotate(m, radians(180.f), vec3(1.0f, 0.0f, 0.0f));
	m = rotate(m, radians(theta*2.0f), vec3(0.0f, 1.0f, 0.0f));
	Program.SendUniform("matrixModelView", m);
	// Bind (activate) the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buf);

	// render nearly as usually
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), 0);
	glNormalPointer(GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glDrawArrays(GL_TRIANGLES, 0, 18);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);


	//light bulb1
	Program.SendUniform("lightPoint1.position", 34.6f, 16.0f, 10.1f);
	if(switch1 == 1)
		Program.SendUniform("lightAmbient.color", 0.8, 0.8, 1.0);
	else
		Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);

	m = matrixView;
	m = translate(m, vec3(34.6f, 16.0f, 10.1f));
	//m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	
	Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);//to redim ambient light to avoid making everything bright

	
	//light bulb2
	if (switch2 == 1)
		Program.SendUniform("lightAmbient.color", 1.0, 0.9, 0.5);
	else
		Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	m = matrixView;
	m = translate(m, vec3(12.0f, 16.3f, -2.3f));
	m = rotate(m, radians(theta*2.0f), vec3(0.0f, -1.0f, 0.0f));
	m = translate(m, vec3(2.0f, 0.0f, 2.0f));

	vec3 bulbPos = vec3(m[3]);
	Program.SendUniform("lightPoint2.position", (bulbPos.x), -(bulbPos.y), -(bulbPos.z));
	
	Program.SendUniform("matrixModelView", m);//for lightbulb
	Program.SendUniform("lightPoint2.matrix", m);//for point light
	glutSolidSphere(0.25, 32, 32);

	Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);


	//table
	glBindTexture(GL_TEXTURE_2D, idTexWood);

	Program.SendUniform("materialDiffuse", 0.4, 0.2, 0.1);
	Program.SendUniform("materialSpecular", 0.4, 0.2, 0.1);
	Program.SendUniform("shininess", 3.0);

	m = matrixView;
	m = translate(m, vec3(15.0f, 0.0f, 0.0f));
	m = scale(m, vec3(0.016f, 0.016f, 0.016f));
	table.render(0, m);//chair
	table.render(1, m);//table

	//chair copies
	m = matrixView;
	m = translate(m, vec3(15.0f, 0.0f, 0.0f));
	m = scale(m, vec3(0.016f, 0.016f, 0.016f));
	m = rotate(m, radians(150.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);//chair

	m = matrixView;
	m = translate(m, vec3(20.0f, 0.0f, 0.0f));
	m = scale(m, vec3(0.016f, 0.016f, 0.016f));
	m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);//chair

	m = matrixView;
	m = translate(m, vec3(10.0f, 0.0f, 0.0f));
	m = scale(m, vec3(0.016f, 0.016f, 0.016f));
	m = rotate(m, radians(270.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);//chair

	//vase
	glBindTexture(GL_TEXTURE_2D, idTexMarble);

	Program.SendUniform("materialDiffuse", 0.1, 0.1, 0.2);
	Program.SendUniform("materialSpecular", 0.5, 0.5, 1.0);
	Program.SendUniform("shininess", 3.0);

	m = matrixView;
	m = translate(m, vec3(17.5f, 12.1f, 0.0f));
	m = scale(m, vec3(0.3f, 0.3f, 0.3f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	vase.render(m);

	//living room

	Program.SendUniform("materialDiffuse", 1.0, 0.0, 0.5);
	Program.SendUniform("materialSpecular", 10.0, 10.0, 10.0);

	m = matrixView;
	m = translate(m, vec3(10.0f, 0.0f, -10.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	livingRoom.render(m);

	//christmas tree
	Program.SendUniform("materialDiffuse", 0.4, 0.4, 0.2);
	Program.SendUniform("materialSpecular", 10.0, 10.0, 10.0);//possibly not needed

	m = matrixView;
	m = translate(m, vec3(45.0f, 0.0f, -29.0f));
	m = scale(m, vec3(12.1f, 12.1f, 12.1f));
	m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	tree.render(m);


	//lamp
	glBindTexture(GL_TEXTURE_2D, idTexMetal);

	Program.SendUniform("materialDiffuse", 0.1, 0.1, 0.1);
	Program.SendUniform("materialSpecular", 0.5, 0.5, 0.5);
	Program.SendUniform("shininess", 10.0);

	m = matrixView;
	m = translate(m, vec3(43.0f, 0.0f, 14.0f));
	m = scale(m, vec3(0.26f, 0.26f, 0.26f));
	m = rotate(m, radians(-25.0f), vec3(0.0f, 1.0f, 0.0f));
	lamp.render(m);

	//nymph
	glBindTexture(GL_TEXTURE_2D, idTexMarble);

	Program.SendUniform("materialDiffuse", 0.7, 0.7, 0.7);
	Program.SendUniform("materialSpecular", 0.3, 0.3, 0.3);
	Program.SendUniform("shininess", 6.0);

	m = matrixView;
	m = translate(m, vec3(14.9f, 24.3f, -2.4f));
	m = scale(m, vec3(2.3f, 2.3f, 2.3f));
	m = rotate(m, radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
	m = rotate(m, radians(225.0f), vec3(0.0f, 1.0f, 0.0f));
	//m = rotate(m, radians(theta*2.0f), vec3(0.0f, 1.0f, 0.0f));
	//m = translate(m, vec3(-0.23f, 0.0f, -0.75f));
	nymph.render(m);
															
	// teapot
	glBindTexture(GL_TEXTURE_2D, idTexMetal);

	Program.SendUniform("materialDiffuse", 0.0, 0.4, 0.2);
	Program.SendUniform("materialSpecular", 0.0, 0.9, 0.2);
	Program.SendUniform("shininess", 12.0);

	m = matrixView;
	m = translate(m, vec3(10.0f, 12.1f, 2.5f));
	m = scale(m, vec3(2.0f, 2.0f, 2.0f));
	m = rotate(m, radians(-113.f), vec3(0.0f, 1.0f, 0.0f));
	utah.render(m);

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
	mat4 matrixProjection = perspective(radians(60.f), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix

	// send entire matrix
	Program.SendUniform("matrixProjection", matrixProjection);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); angleRot = 0.1f; break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); angleRot = -0.1f; break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	case 'z': if (switch1 == 1) switch1 = 0; else switch1 = 1; break;
	case 'c': if (switch2 == 1) switch2 = 0; else switch2 = 1; break;

	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
	// stop orbiting
	if ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) == 0) angleRot = 0;
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
	case GLUT_KEY_F11:		glutFullScreenToggle(); break;
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
	glutCreateWindow("Kaan UNLU, CI5520 3D Graphics Programming");

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

