/*************************************
* Arquivo: circles.cpp
* Descrição: Contém as funções relacionadas à exibição dos círculos
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

#include "circles.h"
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

float circleColors[10][3];
float circleRaios[10];
int circleAnim;
int circleSkip, circleSkipLimit, circleSpeed;

void circleInit()
{
    srand(circleRaios[3]);
    for ( int i = 0 ; i < 10 ; i++ )
    {
        // Escolhe as cores dos círculos aleatoriamente. Não evita círculos de cores iguais.
        circleColors[i][0] = (double )rand() / RAND_MAX;
        circleColors[i][1] = (double )rand() / RAND_MAX;
        circleColors[i][2] = (double )rand() / RAND_MAX;
        circleRaios[i] = (i+1)*0.3;
    }
}

void drawCircle(float radius)
{
   glBegin(GL_LINE_LOOP);

   for (int i=0; i < 360; i++)
   {
      float degInRad = i*M_PI/180;
      glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
   }

   glEnd();
}

void DrawLowerRight()
{
    for ( int i = 0 ; i < 10 ; i++ )
    {
        glColor3fv(circleColors[i]);
        drawCircle(circleRaios[i]);
    }

    // Se circleAnim = 1, então é para animar os círculos.
    // circleSkip serve para limitar a velocidade de atualização da animação pois
    // quando o fractal está com uma resolução pequena, a atualização da animação fica muito acelerada
    // e fica difícil de ver.
    // circleSkipLimit é definida pelo que o usuário escolhe na tela (Normal=0, Lento=3, Muito Lento=10)
    if ( circleAnim && ++circleSkip > circleSkipLimit )
    {
        circleSkip=0;
        int n;
        for(n=0 ; n<10; n++)
        {
            circleRaios[n] += 0.1;
            if(circleRaios[n] > 2.5 )
            {
                circleRaios[n] = 0.0;
            }
        }
    }
}

