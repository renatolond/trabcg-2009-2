#include <GL/glui.h>
#include <cmath>
#include <vector>

#define MAXITER 200

using namespace std;

int main_window;
int   segments = 8;
GLUI *glui;
GLUI_Listbox *listaVeloc;

int wWidth, wHeight;
int size;

double posI[3], posF[3];
double stepI;
double stepJ;
double stepK;
double stepL;
int fractalResolucao;
int mudouResolucao;
GLuint fractalListIndex;

int mudouFreq;
float waveAmp, waveTime, waveFreq, waveLambda,t;
int waveSkip, waveOn[5];
vector<double> wavePoints[5];
vector<double> waveResult(51);
float waveColors[5][3];
int waveSee, waveSpeed;
int waveSkipLimit;

GLuint texture[1];

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;

struct color{
    double r, g, b;
} mandel[30*15][30*15];

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.
int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1)
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
	printf("File Not Found : %s\n",filename);
	return 0;
    }

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }
//    printf("Width of %s: %lu\n", filename, image->sizeX);

    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
//    printf("Height of %s: %lu\n", filename, image->sizeY);

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	printf("Error reading planes from %s.\n", filename);
	return 0;
    }
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	printf("Error reading bpp from %s.\n", filename);
	return 0;
    }
    if (bpp != 24) {
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
	return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }

    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
void LoadGLTextures() {
    // Load Texture
    Image *image1;

    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("violin.bmp", image1)) {
	exit(1);
    }

    // Create Texture
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
};

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

void correctResolucao()
{
	posI[0] = -2.88;
	posI[1] = 2.88;
	posF[0] = 2.88;
	posF[1] = -2.88;
	stepI = (posF[0] - posI[0])/(30*fractalResolucao);
	stepK = (15.4 - (-3.0))/(30*fractalResolucao);
	stepJ = (posF[1] - posI[1])/(30*fractalResolucao);
	stepL = (5.7 - (-5.6))/(30*fractalResolucao);
//	printf("[stepI: %f ; stepJ: %f]\n",stepI, stepJ);
}

void DrawWhiteSquare()
{
    glColor3f(1.0f,1.0f,1.0f);

	glBegin(GL_QUADS);
		glVertex3f(-0.5,-0.5,0.0);
		glVertex3f(-0.5,0.5,0.0);
		glVertex3f(0.5,0.5,0.0);
		glVertex3f(0.5,-0.5,0.0);
	glEnd();

}

void DrawViolin()
{
    glColor3f(1.0f,1.0f,1.0f);

	glBegin(GL_QUADS);
    glVertex2f(-2.88f, -2.88f);
    glVertex2f( 2.88f, -2.88f);
    glVertex2f( 2.88f,  2.88f);
    glVertex2f(-2.88f,  2.88f);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-2.88f, -1.60f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( 2.88f, -1.60f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( 2.88f,  1.60f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-2.88f,  1.60f);

    glEnd();
}

void calculaOndas()
{

    const double longitd = 0.5;

    for ( int i = 0 ; i < 5 ; i++ ) wavePoints[i] = vector<double>(51);

    double stepX = longitd/wavePoints[0].size();


    for ( int j = 0 ; j < waveResult.size() ; j++ )
        waveResult[j] = 0;

    for ( int i = 0 ; i < 5 ; i++ )
    {
        if ( !waveOn[i] )
            continue;
        double amp, k, x, w;
        amp = waveAmp/10;
//        k = 2*M_PI/waveLambda;
        k = 2*M_PI*(i+1.0);
//        w = 2*M_PI/waveFreq;
        w = 2*M_PI*(i+1.0);
        //t = waveTime;
        x = 0;
        for ( int j = 0 ; j < wavePoints[i].size()-1 ; j++, x+=stepX )
        {
            wavePoints[i][j] = amp * sin(k*x - w*t) + amp * sin(k*x + w*t);
            waveResult[j] += wavePoints[i][j];
            //printf("[%lf %lf %lf %lf]\n",amp, k, w, points[i]);
        }
    }
}

void DrawWaves()
{


    if ( mudouFreq )
    {
        calculaOndas();
        mudouFreq = 0;
        waveSkip = 0;
    }

    glColor3f(0.0, 0.0, 0.0);

    {
        double x;
        double stepX;
        stepX = (2.44*2.0)/(waveResult.size());
        x = -2.44;
        glLineWidth(3);
        glBegin(GL_LINES);
            for ( int i = 0 ; i < waveResult.size()-1 ; i++ , x+=stepX)
            {
                glVertex2f(x, waveResult[i]);
                glVertex2f(x+stepX, waveResult[i+1]);
            }
        glEnd();
        glLineWidth(1);
    }

    if ( waveSee )
    {
        for ( int i = 0 ; i < 5 ; i++ )
        {
            if ( !waveOn[i] ) continue;
        glColor3fv(waveColors[i]);
        double x;
        double stepX;
        stepX = (2.44*2.0)/(wavePoints[i].size());
        x = -2.44;
        glLineWidth(3);
        glBegin(GL_LINES);
            for ( int j = 0 ; j < wavePoints[i].size()-1 ; j++ , x+=stepX)
            {
                glVertex2f(x, wavePoints[i][j]);
                glVertex2f(x+stepX, wavePoints[i][j+1]);
            }
        glEnd();
        glLineWidth(1);
        }
    }

    if ( ++waveSkip > waveSkipLimit )
    {
        t += 0.01;
        if ( t > 1 ) t = 0;
        waveSkip = 0;
        mudouFreq = 1;
    }
}

void DrawUpperRight()
{
    DrawViolin();
    DrawWaves();
}

void calculaMandelbrot()
{
    double k, l;
    int i, j;

    glDeleteLists(fractalListIndex,1);
    fractalListIndex = glGenLists(1);

    for ( i = 0 , k = -3.0 ; i < 30*fractalResolucao ; i++ , k+=stepK)
	{
		for ( j = 0 , l = -5.6 ; j < 30*fractalResolucao ; j++ , l+=stepL)
		{
			int t;

			if ( isMandelbrot(k,l,t) )
			{
				double g =(t/50.0);
				mandel[i][j].r = mandel[i][j].b = 0;
				mandel[i][j].g = g;
			}
			else
			{
				mandel[i][j].r = mandel[i][j].g = 0;
				mandel[i][j].b = 1;
			}
		}
	}

    glNewList(fractalListIndex, GL_COMPILE);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0,0,1);

    for ( i = 0, k = posI[0] ; i < 30*fractalResolucao ; i++ , k+=stepI)
	{
		for ( j = 0, l = posI[1] ; j < 30*fractalResolucao ; j++ , l+=stepJ)
		{
			glColor3f(mandel[i][j].r,mandel[i][j].g,mandel[i][j].b);
			glBegin(GL_QUADS);
                glVertex2d(k,l);
                glVertex2d(k+stepI,l);
                glVertex2d(k+stepI,l+stepJ);
                glVertex2d(k,l+stepJ);
			glEnd();
		}
	}

	glEndList();
}

void DrawUpperLeft()
{
	if ( mudouResolucao )
	{
	    correctResolucao();
	    calculaMandelbrot();
	    mudouResolucao = 0;
	}

	glCallList(fractalListIndex);

}

void DrawBorder()
{
    double border = 2.885;

    //gluOrtho2D(a, b, c, d);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glVertex2d(-border,-border);
        glVertex2d(-border, border);
        glVertex2d( border, border);
        glVertex2d( border,-border);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void display ()
{
    glutSetWindow(main_window);
    glClear(GL_COLOR_BUFFER_BIT);
    for ( int view = 0 ; view < 4 ; view++ )
    {
        if (view==0)									// If We Are Drawing The First Scene
		{
			// Set The Viewport To The Top Left.  It Will Take Up Half The Screen Width And Height
			glViewport (0, wHeight/2, wWidth/2, wHeight/2);
            DrawUpperLeft();
            DrawBorder();
			//DrawUpperLeft();
		}
		else if (view==1)
		{
			glViewport (wWidth/2, wHeight/2, wWidth/2, wHeight/2);
            DrawUpperRight();
            DrawBorder();
		}
		else if(view==2)
		{
			glViewport (wWidth/2, 0, wWidth/2, wHeight/2);
            DrawWhiteSquare();
            DrawBorder();
		}
		else
		{
			glViewport (0, 0, wWidth/2, wHeight/2);
            DrawWhiteSquare();
            DrawBorder();
		}
    }
    glutSwapBuffers();
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
//	wasReshaped = 1;
}

void gluiCallback(int id)
{
    switch ( id )
    {
        case 1:
            mudouResolucao = 1;
            break;
        case 2:
            mudouFreq = 1;
            break;
        case 3:
            printf("[%d %d",waveSpeed, waveSkipLimit);

            if ( waveSpeed == 1 )
                waveSkipLimit = 0;
            else if ( waveSpeed == 2 )
                waveSkipLimit  = 3;
            else if ( waveSpeed == 3 )
                waveSkipLimit = 10;
            printf("-%d]\n",waveSkipLimit);
    }
}

void createGluiControls()
{
    GLUI_Rollout *rollout1 = glui->add_rollout("Fractal",1);
    GLUI_Spinner *spinner1 = glui->add_spinner_to_panel(rollout1, "Resolucao", GLUI_SPINNER_INT, &fractalResolucao, 1, gluiCallback);
    spinner1->set_int_limits(1, 12);
    GLUI_Rollout *rollout2 = glui->add_rollout("Ondas",1);
    GLUI_EditText *editTextAmp = glui->add_edittext_to_panel(rollout2, "Amplitude: ", GLUI_EDITTEXT_FLOAT, &waveAmp);

    GLUI_Checkbox *check1st = glui->add_checkbox_to_panel(rollout2, "1st", &waveOn[0],2,gluiCallback);
    GLUI_Checkbox *check2nd = glui->add_checkbox_to_panel(rollout2, "2nd", &waveOn[1],2,gluiCallback);
    GLUI_Checkbox *check3rd = glui->add_checkbox_to_panel(rollout2, "3rd", &waveOn[2],2,gluiCallback);
    GLUI_Checkbox *check4th = glui->add_checkbox_to_panel(rollout2, "4th", &waveOn[3],2,gluiCallback);
    GLUI_Checkbox *check5th = glui->add_checkbox_to_panel(rollout2, "5th", &waveOn[4],2,gluiCallback);
    GLUI_Checkbox *checkSee = glui->add_checkbox_to_panel(rollout2, "Ver orig",&waveSee,2,gluiCallback);
    listaVeloc = glui->add_listbox_to_panel(rollout2, "Velocidade", &waveSpeed,3,gluiCallback);

    listaVeloc->add_item(1, "Normal");
    listaVeloc->add_item(2, "Lento");
    listaVeloc->add_item(3, "Muito lento");
}

void init (void)
{
    LoadGLTextures();
    glEnable(GL_TEXTURE_2D);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);
	mudouResolucao = 1;
	fractalResolucao = 12;
	waveAmp = 1.0;
	waveTime = 1.0;
    waveFreq = 1.0;
    waveLambda = 1.0;
    waveSkip = 0;
    for ( int i = 0 ; i < 5 ; i++ )
        waveOn[i] = 0;
    mudouFreq = 1;
    waveSee = 0;
    waveColors[0][0] = 1.0; waveColors[0][1] = waveColors[0][2] = 0.0;
    waveColors[1][1] = 1.0; waveColors[1][0] = waveColors[1][2] = 0.0;
    waveColors[2][2] = 1.0; waveColors[2][0] = waveColors[2][1] = 0.0;
    waveColors[3][0] = waveColors[3][1] = 1.0; waveColors[3][2] = 0.0;
    waveColors[4][0] = waveColors[4][2] = 1.0; waveColors[4][1] = 0.0;
    fractalListIndex = glGenLists(1);
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 500, 500 );

  main_window = glutCreateWindow( "Trabalho 1 de Comp Graf" );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  init();
  GLUI_Master.set_glutIdleFunc( display );

  glui = GLUI_Master.create_glui( "Configs" );
  createGluiControls();
  glui->set_main_gfx_window( main_window );


  glutMainLoop();
}
