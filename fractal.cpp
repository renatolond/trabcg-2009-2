#include <GL/glut.h>
#include <complex>
#include <cmath>

#define MAXITER 200

using namespace std;

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
struct RGBType {float r; float g; float b;};

class comp
{
    public:
    long double re, im;
};

RGBType *pixels;

void init (void)
{

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
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

bool isMandelbrot(long double m, long double n, int& t)
{
    comp a, b, c, z;

	z.re = 0;
	z.im = 0;

//	m += 0.25;
//	n += 0.25;

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

void calculaMandelbrot()
{
    int i, j, k, l, t;

    for ( i = 0 , k = -3.0 ; i < wHeight ; i++, k+=stepK )
    {
        for( j = 0 , l = -5.6 ; j < wWidth ; j++, l += stepL )
        {
            int ind;
            ind = wHeight * i + j;

			if ( isMandelbrot(k,l,t) )
			{
				double g =(t/50.0);
				pixels[ind].r = pixels[ind].b = 1;
				pixels[ind].g = g;
			}
			else
			{
			    pixels[ind].r = pixels[ind].g = 1;
			    pixels[ind].b = 1;
			}
        }
    }
}

void display (void)
{
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
				glBegin(GL_QUADS);
                    glVertex2d(i,j);
                    glVertex2d(i-stepI,j);
                    glVertex2d(i-stepI,j-stepJ);
                    glVertex2d(i,j-stepJ);
				glEnd();
			}
			else
			{
				glColor3f(0,0,1);
				glBegin(GL_QUADS);
                    glVertex2d(i,j);
                    glVertex2d(i-stepI,j);
                    glVertex2d(i-stepI,j-stepJ);
                    glVertex2d(i,j-stepJ);
				glEnd();
			}
		}
	}

	//if ( pixels != 0 )
        //glDrawPixels(wHeight,wWidth,GL_RGB,GL_FLOAT,pixels);


	glutSwapBuffers();
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

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	wWidth = w;
	wHeight = h;

	if ( pixels != 0 )
	{
	    //for ( int i = 0 ; i < wHeight ; i++ )
            //delete[] pixels[i];
        delete[] pixels;
        pixels = 0;
	}

    pixels = new RGBType [wHeight*wWidth];

	correctCamera();
    //calculaMandelbrot();
}

void keyboard(unsigned char key, int x, int y)
{
}


int main (int argc, char **argv)
{
    pixels = 0;
//    size = 0;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowPosition (50, 50);
	glutInitWindowSize (500, 500);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
