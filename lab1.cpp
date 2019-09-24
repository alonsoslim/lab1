//Updated by: Alonso Gomez
//Date: 09/24/19
//HW 01
//
//modified by: Alonso Gomez
//date: 08/27/2019
//
//3350 Spring 2019 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
// .general animation framework
// .animation loop
// .object definition and movement
// .collision detection
// .mouse/keyboard interaction
// .object constructor
// .coding style
// .defined constants
// .use of static variables
// .dynamic memory allocation
// .simple opengl components
// .git
//
//elements we will add to program...
//   .Game constructor
//   .multiple particles
//   .gravity
//   .collision detection
//   .more objects
//
#include <iostream>
using namespace std;
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

const int MAX_PARTICLES = 3000;
const float GRAVITY     = 0.25;

//some structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width = 80, height = 12;
	float radius;
	Vec center;
};

struct Particle {
    int r, g, b;
	Shape s;
	Vec velocity;
};

class Global {
public:
	int xres, yres;
	Shape box[5];
	Particle particle[MAX_PARTICLES];
	int n;
	Global();
} g;

class X11_wrapper {
private:
	Display *dpy;
	Window win;
	GLXContext glc;
public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent();
	void swapBuffers();
} x11;

//Function prototypes
void init_opengl(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void movement();
void render();
void makeParticle(int x, int y);


//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
	srand(time(NULL));
	init_opengl();
	//Main animation loop
	int done = 0;
	while (!done) {
		//Process external events.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			check_mouse(&e);
			done = check_keys(&e);
		}

        for (int i = 0; i < 10 ; i++) {
            makeParticle(130, 460);
        }

		movement();
		render();
		x11.swapBuffers();
	}
	return 0;
}

//-----------------------------------------------------------------------------
//Global class functions
//-----------------------------------------------------------------------------
Global::Global()
{
	xres = 800;
	yres = 600;
	//define a box shape
	n = 0;
}

//-----------------------------------------------------------------------------
//X11_wrapper class functions
//-----------------------------------------------------------------------------
X11_wrapper::~X11_wrapper()
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w = g.xres, h = g.yres;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		cout << "\n\tcannot connect to X server\n" << endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		cout << "\n\tno appropriate visual found\n" << endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask =
		ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 Lab1");
}

bool X11_wrapper::getXPending()
{
	//See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	//Get a pending event.
	XEvent e;
	XNextEvent(dpy, &e);
	return e;
}

void X11_wrapper::swapBuffers()
{
	glXSwapBuffers(dpy, win);
}
//-----------------------------------------------------------------------------

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	//Set the screen background color
	glClearColor(0.69, 0.88, 0.89, 1.0);
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
}

void makeParticle(int x, int y)
{
	//Add a particle to the particle system.
	//
	if (g.n >= MAX_PARTICLES)
		return;
	cout << "makeParticle() " << x << " " << y << endl;
	//set position of particle
	Particle *p = &g.particle[g.n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = -0.2;
	p->velocity.x =  ((double)rand() / (double)RAND_MAX) - 0.5 + .75;
	++g.n;
}

void check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	//Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify) {
		//This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed.
			int y = g.yres - e->xbutton.y;
			makeParticle(e->xbutton.x, y);
			makeParticle(e->xbutton.x, y);
			makeParticle(e->xbutton.x, y);
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify) {
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves.

			int y =g.yres - e->xbutton.y;
			for (int i=0; i<4; i++) {
			    makeParticle(e->xbutton.x, y);
			}
		}
	}
}

int check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress) {
		switch (key) {
			case XK_1:
				//Key 1 was pressed
				break;
			case XK_a:
				//Key A was pressed
				break;
			case XK_Escape:
				//Escape key was pressed
				return 1;
		}
	}
	return 0;
}

void movement()
{
	if (g.n <= 0)
		return;
	for (int i=0; i<g.n; i++) {
		Particle *p = &g.particle[i];
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;
		p->velocity.y = p->velocity.y - GRAVITY;
		//check for collision with shapes...
        
        for (int j = 0; j < 5; j++) {
		    if (p->s.center.y < g.box[j].center.y + g.box[j].height &&
		        p->s.center.y > g.box[j].center.y - g.box[j].height &&
		        p->s.center.x > g.box[j].center.x - g.box[j].width &&
		        p->s.center.x < g.box[j].center.x + g.box[j].width){
		        p->velocity.y = - (p->velocity.y * 0.55);
                p->velocity.x = p->velocity.x * 1.02;
		    }
        }
		//check for off-screen
		if (p->s.center.y < 0.0) {
			//cout << "off screen" << endl;
			g.particle[i] = g.particle[g.n-1];
	    	    	--g.n;
		}
	}
}

void render()
{
    Rect r;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...
	//draw the box
	glColor3ub(104,207,155);
    g.box[0].center.x = 100;
    g.box[0].center.y = 400;
    for (int i = 1; i < 5; i++) {
        g.box[i].center.x = g.box[i-1].center.x + 90;
        g.box[i].center.y = g.box[i-1].center.y - 60;
    }
    
    float w[5], h[5];

    for (int i = 0; i < 5; i++) {
        glPushMatrix();  
        glTranslatef(g.box[i].center.x, g.box[i].center.y, g.box[i].center.z);
	    w[i] = g.box[i].width;
	    h[i] = g.box[i].height;
	    glBegin(GL_QUADS);
	        glVertex2i(-w[i], -h[i]);
	        glVertex2i(-w[i],  h[i]);
	        glVertex2i( w[i],  h[i]);
	        glVertex2i( w[i], -h[i]);
	    glEnd();
	    glPopMatrix();
    }
	//
    // Requirments label over box
    r.center = 0;
    r.bot = 394;
    r.left = 60;
    ggprint8b(&r, 16, 0x00ffffff, "Requirements");
    r.bot = 334;
    r.left = 170;
    ggprint8b(&r, 16, 0x00ffffff, "Design");
    r.bot = 274;
    r.left = 235;
    ggprint8b(&r, 16, 0x00ffffff, "Implementation");
    r.bot = 214;
    r.left = 350;
    ggprint8b(&r, 16, 0x00ffffff, "Testing");
    r.bot = 154;
    r.left = 430;
    ggprint8b(&r, 16, 0x00ffffff, "Maintenance");
    
    //Draw particles here
    //There is at least one particle to draw.
    for (int i=0; i<g.n; i++) {
        glPushMatrix();
        if (i % 2 == 1) {
            g.particle[i].r = 20;
            g.particle[i].g = 75;
            g.particle[i].b = 255;
        }
        else {
            g.particle[i].r = 131;
            g.particle[i].g = 8;
            g.particle[i].b = 255;
        }
        if (i % 8 == 2) {
            g.particle[i].r = 245;
            g.particle[i].g = 245;
            g.particle[i].b = 255;
        }
        glColor3ub(g.particle[i].r, g.particle[i].g, g.particle[i].b);
        Vec *c = &g.particle[i].s.center;
        w[0] = h[0] = 2;
        glBegin(GL_QUADS);
            glVertex2i(c->x-w[0], c->y-h[0]);
            glVertex2i(c->x-w[0], c->y+h[0]);
            glVertex2i(c->x+w[0], c->y+h[0]);
            glVertex2i(c->x+w[0], c->y-h[0]);
        glEnd();
        glPopMatrix();
    }
	
	//
	//Draw your 2D text here

}

