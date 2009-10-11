#include <GL/glui.h>

#define MAXITER 200

using namespace std;

int main_window;
int   segments = 8;
GLUI *glui;

int wWidth, wHeight;
int size;

GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];

double posI[3], posF[3];
double stepI;
double stepJ;
double stepK;
double stepL;

int wasReshaped;

class comp
{
    public:
    long double re, im;
};

bool isMandelbrot(long double m, long double n, int& t)
{
    comp a, b, c, z;

	z.re = 0;
	z.im = 0;

    // A = 1 ; B = m + ni
    a.re = 1; a.im = 0;
    b.re = m; b.im = n;

    // C = A/B
    c.re = (a.re*b.re + a.im*b.im)/(b.re*b.re + b.im*b.im);
    c.im = (a.im*b.re - a.re*b.im)/(b.re*b.re + b.im*b.im);

    c.re += 0.25;


	for ( t = 0 ; t < MAXITER ; t++ )
	{
		long double  x, y;
		comp n;

		x = z.re;
		y = z.im;

		if ( x*x + y*y > 4.0 )
		{
			return true;
		}

        // Z*Z
		n.re = z.re*z.re - z.im*z.im;
		n.im = z.re*z.im + z.im*z.re;

        // Z = Z^2 + C
		z.re = n.re + c.re;
		z.im = n.im + c.im;
	}

	return false;
}

void GetOGLPos(int x, int y, GLfloat z, double pos[3])
{
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;
	winZ = z;

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	pos[0] = posX;
	pos[1] = posY;
	pos[2] = posZ;
}

void correctCamera()
{

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	glVertex3i(50,-50,0);
	glVertex3i(50,50,0);
	glVertex3i(-50,50,0);
	glVertex3i(-50,-50,0);
	glEnd();
	GLfloat winz;
	glReadPixels( 0, 0, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz );
	GetOGLPos(0,0,winz,posI);
	GetOGLPos(wHeight, wWidth, winz, posF);

	stepI = (posF[0] - posI[0])/wWidth;
	stepK = (15.4 - (-3.0))/wWidth;
	stepJ = (posF[1] - posI[1])/wHeight;
	stepL = (5.7 - (-5.6))/wHeight;
    printf("[z:%f][%f %f] [%f %f] [%f %f]\n",winz,posI[0],posI[1],posF[0],posF[1],stepI,stepJ);

	glDisable(GL_DEPTH_TEST);
}

void DrawUpperLeft()
{
    glColor3f(1.0f,0.0f,0.0f);

	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5,-0.5,0.0);
		glVertex3f(0.5,0.0,0.0);
		glVertex3f(0.0,0.5,0.0);
	glEnd();

}

void DrawUpperLeftt()
{
	if ( wasReshaped )
	{
	    correctCamera();
	    wasReshaped = 0;
	}

    long double i,j,k,l;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0,0,1);

    for ( i = posI[0] , k = -3.0 ; i < posF[0] ; i+=stepI , k+=stepK)
	{
		for ( j = posI[1] , l = -5.6 ; j > posF[1] ; j+=stepJ , l+=stepL)
		{
			int t;

			if ( isMandelbrot(k,l,t) )
			{
				double g =(t/50.0);
				glColor3f(0,g,0);
				glBegin(GL_POINTS);
				glVertex2d(i,j);
				glEnd();
			}
			else
			{
				glColor3f(0,0,1);
				glBegin(GL_POINTS);
				glVertex2d(i,j);
				glEnd();
			}
		}
	}
}

void display ()
{
    glClear(GL_COLOR_BUFFER_BIT);
//    for ( int view = 0 ; view < 4 ; view++ )
    {
//        if (view==0)									// If We Are Drawing The First Scene
		{
			// Set The Viewport To The Top Left.  It Will Take Up Half The Screen Width And Height
//			glViewport (0, wHeight/2, wWidth/2, wHeight/2);
//			glMatrixMode (GL_PROJECTION);						// Select The Projection Matrix
//			glLoadIdentity ();							// Reset The Projection Matrix
//            gluPerspective(45.0, (GLfloat) wWidth/(GLfloat) wHeight, 0.1, 500.0);
//            glMatrixMode(GL_MODELVIEW);
//            glLoadIdentity();
//            gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

			DrawUpperLeft();
		}
//		else if (view==1)
//		{
//			glViewport (wWidth/2, wHeight/2, wWidth/2, wHeight/2);
////			glMatrixMode (GL_PROJECTION);						// Select The Projection Matrix
////			glLoadIdentity ();							// Reset The Projection Matrix
//		}
//		else if(view==2)
//		{
//			glViewport (wWidth/2, 0, wWidth/2, wHeight/2);
////			glMatrixMode (GL_PROJECTION);						// Select The Projection Matrix
////			glLoadIdentity ();							// Reset The Projection Matrix
//		}
//		else
//		{
//			glViewport (0, 0, wWidth/2, wHeight/2);
////			glMatrixMode (GL_PROJECTION);						// Select The Projection Matrix
////			glLoadIdentity ();							// Reset The Projection Matrix
//		}
    }
}

void idle ()
{
}

void reshape (int w, int h)
{
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	wWidth = w;
	wHeight = h;
	wasReshaped = 1;
}

void createGluiControls()
{
  GLUI_Spinner *segment_spinner = glui->add_spinner( "Segments:", GLUI_SPINNER_INT, &segments );
  segment_spinner->set_int_limits( 3, 60 );
  glui->add_separator();
}

void init (void)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 500, 500 );

  main_window = glutCreateWindow( "GLUI Example 1" );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  GLUI_Master.set_glutIdleFunc( display );

  glui = GLUI_Master.create_glui( "GLUI" );
  createGluiControls();
  glui->set_main_gfx_window( main_window );


  init();
  glutMainLoop();
}
