/*************************************
* Arquivo: fractal.cpp
* Descrição: Contém as funções relacionadas à exibição do fractal
* Autores: Renato dos Santos Cerqueira  (DRE: 105093538)
*          Cláudio Sérgio Forain Júnior (DRE: 105049864)
*
* (C) Copyright 2009, Renato Cerqueira, Cláudio Forain
*
* This file is part of CompGrafProject09-2.
*
* CompGrafProject09-2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* Foobar is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with CompGrafProject09-2.  If not, see <http://www.gnu.org/licenses/>.
*
*************************************/

#include "fractal.h"
#include <GL/glut.h>

double posI[3], posF[3];
double stepI;
double stepJ;
double stepK;
double stepL;
int fractalResolucao;
int mudouResolucao;
GLuint fractalListIndex;

#define MAXITER 200

void fractalInit()
{
   	mudouResolucao = 1;
	fractalResolucao = 12;

    fractalListIndex = glGenLists(1);
}

class comp
{
    public:
    long double re, im;
};

struct color{
    double r, g, b;
} mandel[30*15][30*15];

void correctResolucao()
{

    // Valores descobertos por tentativa e erro. São assim por causa do glPerspective e do gluLookAt
	posI[0] = -2.88;
	posI[1] = 2.88;
	posF[0] = 2.88;
	posF[1] = -2.88;

	// Descobre a janela de desenho em unidades do OpenGL
	stepI = (posF[0] - posI[0])/(30*fractalResolucao);
	stepJ = (posF[1] - posI[1])/(30*fractalResolucao);

	// Descobre a janela de desenho em unidades matemáticas
	stepK = (15.4 - (-3.0))/(30*fractalResolucao);
	stepL = (5.7 - (-5.6))/(30*fractalResolucao);
}

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

		// Se a distância do Z até o centro for maior que 4, então ele é um ponto do fractal
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

    // Guarda o fractal numa lista, para desenhar mais rápido, já que o fractal pode ter muitos pontos para desenhar
    // (30*30*12*12 = 129600) é bastante! =)
    glNewList(fractalListIndex, GL_COMPILE);
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0,0,1);
        glBegin(GL_QUADS);

            for ( i = 0, k = posI[0] ; i < 30*fractalResolucao ; i++ , k+=stepI)
            {
                for ( j = 0, l = posI[1] ; j < 30*fractalResolucao ; j++ , l+=stepJ)
                {
                    glColor3f(mandel[i][j].r,mandel[i][j].g,mandel[i][j].b);
                        glVertex2d(k,l);
                        glVertex2d(k+stepI,l);
                        glVertex2d(k+stepI,l+stepJ);
                        glVertex2d(k,l+stepJ);
                }
            }
        glEnd();
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

