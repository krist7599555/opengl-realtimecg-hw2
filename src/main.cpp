// Modified by Nuttapong Chentanez

// glut_example.c
// Stanford University, CS248, Fall 2000
//
// Demonstrates basic use of GLUT toolkit for CS248 video game assignment.
// More GLUT details at http://reality.sgi.com/mjk_asd/spec3/spec3.html
// Here you'll find examples of initialization, basic viewing transformations,
// mouse and keyboard callbacks, menus, some rendering primitives, lighting,
// double buffering, Z buffering, and texturing.
//
// Matt Ginzton -- magi@cs.stanford.edu



#include <stdio.h>
#include <stdlib.h>


//HACK TO FORCE COMPILE AS WIN32
// #define _WIN32

#include "opengl.hpp"

template<typename T>
struct Vec3 {
	T x, y, z;
	Vec3(T _x, T _y, T _z): x(_x), y(_y), z(_z) {}
};

#define VIEWING_DISTANCE_MIN  1.5
#define TEXTURE_ID_CUBE 1

enum {
	MENU_LIGHTING = 1,
	MENU_POLYMODE,
	MENU_TEXTURING,
	MENU_EXIT
};

typedef int BOOL;
#define TRUE 1
#define FALSE 0

static BOOL g_bLightingEnabled = TRUE;
static BOOL g_bFillPolygons = TRUE;
static BOOL g_bTexture = TRUE;
static BOOL g_bButton1Down = FALSE;
static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static GLfloat g_nearPlane = 1;
static GLfloat g_farPlane = 1000;
static int g_Width = 600;                          // Initial window width
static int g_Height = 600;                         // Initial window height
static int g_yClick = 0;
static float g_lightPos[4] = { 10, 30, 10, 1 };  // Position of light


// INSERT - DO GLOBAL VARS
int is_look_teapot = false;
Vec3<GLfloat> teapot_translate(2, 0, 0);
Vec3<GLfloat> teapot_rotate(2, 0, 0);

Vec3<GLfloat> shift(2, 1, g_fViewDistance);
Vec3<GLfloat> cam_position = shift;
Vec3<GLfloat> cam_lookat(0, 0, 0);
Vec3<GLfloat> cam_up(0, 1, 0);

GLfloat fov = 65.0;

float TRANSLATEION_FACTOR = 0.7;
float ROTATION_FACTOR = 1507.0;

// INSERT - DO GLOBAL VARS - END




#ifdef _WIN32
static DWORD last_idle_time;
#else
static struct timeval last_idle_time;
#endif

void DrawCubeFace(float fSize)
{
	fSize /= 2.0;
	glBegin(GL_QUADS);
	glVertex3f(-fSize, -fSize, fSize);    glTexCoord2f (0, 0);
	glVertex3f(fSize, -fSize, fSize);     glTexCoord2f (1, 0);
	glVertex3f(fSize, fSize, fSize);      glTexCoord2f (1, 1);
	glVertex3f(-fSize, fSize, fSize);     glTexCoord2f (0, 1);
	glEnd();
}

void DrawCubeWithTextureCoords (float fSize)
{
	glPushMatrix();
	DrawCubeFace (fSize);
	glRotatef (90, 1, 0, 0);
	DrawCubeFace (fSize);
	glRotatef (90, 1, 0, 0);
	DrawCubeFace (fSize);
	glRotatef (90, 1, 0, 0);
	DrawCubeFace (fSize);
	glRotatef (90, 0, 1, 0);
	DrawCubeFace (fSize);
	glRotatef (180, 0, 1, 0);
	DrawCubeFace (fSize);
	glPopMatrix();
}

void RenderObjects(void)
{
	float colorBronzeDiff[4] = { 0.8, 0.6, 0.0, 1.0 };
	float colorBronzeSpec[4] = { 1.0, 1.0, 0.4, 1.0 };
	float colorWhite[4]       = { 1.0, 1.0, 1.0, 1.0 };
	float colorNone[4]       = { 0.0, 0.0, 0.0, 0.0 };

	// NOT sure what it is
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
  gluPerspective(fov, (float)g_Width / g_Height, g_nearPlane, g_farPlane);
	// END

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Main object (cube) ... transform to its coordinates, and render
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colorWhite);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	glColor4fv(colorWhite);
	glBindTexture(GL_TEXTURE_2D, TEXTURE_ID_CUBE);

	// Draw Cube
	DrawCubeWithTextureCoords(1.0);

	// Child object (teapot) ... relative transform, and render
	glPushMatrix();
	glTranslatef(
		teapot_translate.x, 
		teapot_translate.y,
		teapot_translate.z
	);
	glRotatef(teapot_rotate.x, 1, 0, 0);
	glRotatef(teapot_rotate.y, 0, 1, 0);
	glRotatef(teapot_rotate.z, 0, 0, 1);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, colorBronzeDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colorBronzeSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
	glColor4fv(colorBronzeDiff);
	glBindTexture(GL_TEXTURE_2D, 0);
	glutSolidTeapot(0.3);
	glPopMatrix();

	glPopMatrix();
}

void display(void)
{
	// Clear frame buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up viewing transformation, looking down -Z axis
	glLoadIdentity();

	// Modify here
	gluLookAt(
		cam_position.x, cam_position.y, cam_position.x,
		cam_lookat.x, cam_lookat.y, cam_lookat.z,
		cam_up.x, cam_up.y, cam_up.z
	);

	// Set up the stationary light
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);

	// Render the scene
	RenderObjects();

	// Make sure changes appear onscreen
	glutSwapBuffers();
}

void reshape(GLint width, GLint height)
{
	g_Width = width;
	g_Height = height;

	glViewport(0, 0, g_Width, g_Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)g_Width / g_Height, g_nearPlane, g_farPlane);
	glMatrixMode(GL_MODELVIEW);
}

void* createTexture(int* width, int* height, int* nComponents) {
	// Checker board texture
	const int W = 128;
	const int H = 128;
	*width = W;
	*height = H;
	*nComponents = 4;
	unsigned char* data = new unsigned char[W*H*4];
	unsigned char* ptr = data;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			int c = 255*(((i/16) % 2) ^ ((j/16) % 2));

			ptr[0] = c;
			ptr[1] = c;
			ptr[2] = c;
			ptr[3] = 255;
			ptr+=4;
		}
	}
	return (void*) data;
}

void InitGraphics(void)
{
	int width, height;
	int nComponents;
	void* pTextureImage;

	if (g_bTexture)
		glEnable(GL_TEXTURE_2D); else
		glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create texture for cube; load marble texture from file and bind it

	pTextureImage = createTexture(&width, &height, &nComponents); //read_texture("marble.rgb", &width, &height, &nComponents);
	glBindTexture(GL_TEXTURE_2D, TEXTURE_ID_CUBE);
	gluBuild2DMipmaps(GL_TEXTURE_2D,     // texture to specify
		GL_RGBA,           // internal texture storage format
		width,             // texture width
		height,            // texture height
		GL_RGBA,           // pixel format
		GL_UNSIGNED_BYTE,	// color component format
		pTextureImage);    // pointer to texture image

	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void MouseButton(int button, int state, int x, int y)
{
	// Respond to mouse button presses.
	// If button1 pressed, mark this state so we know in motion function.

	if (button == GLUT_LEFT_BUTTON)
	{
		g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
		g_yClick = y - 3 * g_fViewDistance;
	}
}

void MouseMotion(int x, int y)
{
	// If button1 pressed, zoom in/out if mouse is moved up/down.

	if (g_bButton1Down)
	{
		g_fViewDistance = (y - g_yClick) / 3.0;
		if (g_fViewDistance < VIEWING_DISTANCE_MIN)
			g_fViewDistance = VIEWING_DISTANCE_MIN;
		glutPostRedisplay();
	}
}

void AnimateScene(void)
{
	float dt;

#ifdef _WIN32
	DWORD time_now;
	time_now = GetTickCount();
	dt = (float) (time_now - last_idle_time) / 1000.0;
#else
	// Figure out time elapsed since last call to idle function
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	dt = (float)(time_now.tv_sec  - last_idle_time.tv_sec) +
		1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
#endif

	// Save time_now for next time
	last_idle_time = time_now;

	// Force redraw
	glutPostRedisplay();
}

void SelectFromMenu(int idCommand)
{
	switch (idCommand)
	{
	case MENU_LIGHTING:
		g_bLightingEnabled = !g_bLightingEnabled;
		if (g_bLightingEnabled)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
		break;

	case MENU_POLYMODE:
		g_bFillPolygons = !g_bFillPolygons;
		glPolygonMode (GL_FRONT_AND_BACK, g_bFillPolygons ? GL_FILL : GL_LINE);
		break;

	case MENU_TEXTURING:
		g_bTexture = !g_bTexture;
		if (g_bTexture)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
		break;

	case MENU_EXIT:
		exit (0);
		break;
	}

	// Almost any menu selection requires a redraw
	glutPostRedisplay();
}

template<typename T>
GLfloat direction(T pred, T neg, T pos) {
	if (pred == neg) return -1.0;
	if (pred == pos) return 1.0;
	return 0.0;
}

void Keyboard(unsigned char key, int x, int y)
{
	char k = key;
	switch (key) {
		case 27:             // ESCAPE key
			exit (0);
			break;
		case 'l': SelectFromMenu(MENU_LIGHTING); break;
		case 'p': SelectFromMenu(MENU_POLYMODE); break;
		case 't': SelectFromMenu(MENU_TEXTURING); break;
	}

	// 3a
	cam_position.y += TRANSLATEION_FACTOR * direction(k, 's', 'w');
	cam_lookat.y   += TRANSLATEION_FACTOR * direction(k, 's', 'w');
	cam_position.x += TRANSLATEION_FACTOR * direction(k, 'a', 'd');
	cam_lookat.x   += TRANSLATEION_FACTOR * direction(k, 'a', 'd');
	cam_position.z += TRANSLATEION_FACTOR * direction(k, 'q', 'e');
	cam_lookat.z   += TRANSLATEION_FACTOR * direction(k, 'q', 'e');

	// 3b
	teapot_translate.x += TRANSLATEION_FACTOR * direction(k, 'j', 'u');
	teapot_translate.y += TRANSLATEION_FACTOR * direction(k, 'h', 'k');
	teapot_translate.z += TRANSLATEION_FACTOR * direction(k, 'y', 'i');

	// 3c
	teapot_rotate.x = ROTATION_FACTOR * direction(k, '3', '4');
	teapot_rotate.y = ROTATION_FACTOR * direction(k, '5', '6');
	teapot_rotate.z = ROTATION_FACTOR * direction(k, '7', '8');
	
	// 3d
	if (k == '1') {
		cam_lookat = is_look_teapot ? Vec3<GLfloat>(0, 0, 0) : teapot_translate;
		is_look_teapot = !is_look_teapot;
	}

	// 3e
	fov += 10 * direction(k, '-', '+');
}

int BuildPopupMenu (void)
{
	int menu;

	menu = glutCreateMenu (SelectFromMenu);
	glutAddMenuEntry ("Toggle lighting\tl", MENU_LIGHTING);
	glutAddMenuEntry ("Toggle polygon fill\tp", MENU_POLYMODE);
	glutAddMenuEntry ("Toggle texturing\tt", MENU_TEXTURING);
	glutAddMenuEntry ("Exit demo\tEsc", MENU_EXIT);

	return menu;
}

int main(int argc, char** argv)
{
	// GLUT Window Initialization:
	glutInit (&argc, argv);
	glutInitWindowSize (g_Width, g_Height);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("CS248 GLUT example");

	// Initialize OpenGL graphics state
	InitGraphics();

	// Register callbacks:
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (Keyboard);
	glutMouseFunc (MouseButton);
	glutMotionFunc (MouseMotion);
	glutIdleFunc (AnimateScene);

	// Create our popup menu
	BuildPopupMenu ();
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	// Get the initial time, for use by animation
#ifdef _WIN32
	last_idle_time = GetTickCount();
#else
	gettimeofday (&last_idle_time, NULL);
#endif

	// Turn the flow of control over to GLUT
	glutMainLoop ();
	return 0;
}



