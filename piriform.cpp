/*************************************
* Arquivo: piriform.cpp
* Descrição: Contém as funções relacionadas à exibição da curva piriforme
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

* CompGrafProject09-2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with CompGrafProject09-2.  If not, see <http://www.gnu.org/licenses/>.
*
*************************************/

#include "piriform.h"
#include <GL/glut.h>
#include <math.h>

typedef struct
{
float x;
float y;
}CURVE;
CURVE piriform, next;
int piriMax;
float piriA, piriB;
int piriSkip, piriSkipLimit, piriSpeed;

void piriInit()
{
        piriA = piriB = 1;
}

void drawEllipse(float xradius, float yradius)
{
   glBegin(GL_LINE_LOOP);

   for (int i=0; i < 360; i++)
   {
      float degInRad = i*M_PI/180;
      glVertex2f(cos(degInRad)*xradius,sin(degInRad)*yradius);
   }

   glEnd();
}


void DrawLowerLeft()
{
    glColor3f(1.0,1.0,1.0);

    // Desenha a elipse que é usada para desenhar a curva piriforme
    drawEllipse(piriA, piriB);

    // Translada a piriforme para que fique centrada na elipse
    glTranslatef(-piriA,0,0);

    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);

    // piriMax é usada para desenhar a piriforme aos poucos.
    for(int k=0;k<piriMax; k++)
    {
        glLineWidth(3);
        int i = k + 270;
        if ( i > 360 ) i -= 360;
        piriform.x = piriA*(1+sin(i*M_PI/180));
        piriform.y = piriB*cos(i*M_PI/180)*(1+sin(i*M_PI/180));
        next.x = piriA*(1+sin((i+1)*M_PI/180));
        next.y = piriB*cos((i+1)*M_PI/180)*(1+sin((i+1)*M_PI/180));

        glVertex2f(piriform.x,piriform.y);
        glVertex2f(next.x,next.y);
    }
    glEnd();

    glTranslatef(piriA,0,0);

    // piriSkip serve para limitar a velocidade de atualização da animação pois
    // quando o fractal está com uma resolução pequena, a atualização da animação fica muito acelerada
    // e fica difícil de ver.
    // piriSkipLimit é definida pelo que o usuário escolhe na tela (Normal=0, Lento=3, Muito Lento=10)
    if ( ++piriSkip > piriSkipLimit )
    {
        piriSkip = 0;
        piriMax++;
        if ( piriMax > 360 ) piriMax = 0;
    }
}
