/*************************************
* Arquivo: main.cpp
* Descrição: Contém as principais funções, como inicialização, atualização da tela, desenho da interface, entre outros
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

#include <GL/glui.h>
#include "circles.h"
#include "fractal.h"
#include "piriform.h"
#include "standingwaves.h"

using namespace std;

int main_window;
int   segments = 8;
GLUI *glui;

int wWidth, wHeight;
int size;

void DrawBorder()
{
    // Mais uma vez, tiramos esse valor da experimentação
    double border = 2.885;

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

    // Delimita a área da primeira viewport (Esquerda, em cima)
	glViewport (0, wHeight/2, wWidth/2, wHeight/2);
    DrawUpperLeft();
    DrawBorder();

    // Delimita a área da segunda viewport (Direita, em cima)
	glViewport (wWidth/2, wHeight/2, wWidth/2, wHeight/2);
    DrawUpperRight();
    DrawBorder();

    // Delimita a área da terceira viewport (Esquerda, em baixo)
	glViewport (0, 0, wWidth/2, wHeight/2);
    DrawLowerLeft();
    DrawBorder();

    // Delimita a área da quarta viewport (Direita, em baixo)
	glViewport (wWidth/2, 0, wWidth/2, wHeight/2);
    DrawLowerRight();
    DrawBorder();

    glutSwapBuffers();
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
            if ( waveSpeed == 1 )
                waveSkipLimit = 0;
            else if ( waveSpeed == 2 )
                waveSkipLimit  = 3;
            else if ( waveSpeed == 3 )
                waveSkipLimit = 10;
            break;
        case 4:
            if ( piriSpeed == 1 )
                piriSkipLimit = 0;
            else if ( piriSpeed == 2 )
                piriSkipLimit  = 3;
            else if ( piriSpeed == 3 )
                piriSkipLimit = 10;
            break;
        case 5:
            if ( circleSpeed == 1 )
                circleSkipLimit = 0;
            else if ( circleSpeed == 2 )
                circleSkipLimit  = 3;
            else if ( circleSpeed == 3 )
                circleSkipLimit = 10;
            break;
        case 6:
            mudouResolucao =1;

    }
}

void createGluiControls()
{
    GLUI_Rollout *rollout1 = glui->add_rollout("Fractal",1);
    GLUI_Spinner *spinner1 = glui->add_spinner_to_panel(rollout1, "Resolucao", GLUI_SPINNER_INT, &fractalResolucao, 1, gluiCallback);
    spinner1->set_int_limits(1, 12);
    GLUI_Listbox *listaCores = glui->add_listbox_to_panel(rollout1, "Cor:", &fractalCor,6,gluiCallback);
    listaCores->add_item(AZUL, "Azul");
    listaCores->add_item(VERDE, "Verde");
    listaCores->add_item(VERMELHO, "Vermelho");
    listaCores->add_item(AMARELO, "Amarelo");
    listaCores->add_item(ROSA, "Rosa");
    listaCores->add_item(CIANO, "Ciano");
    GLUI_Listbox *listaCoresFundo = glui->add_listbox_to_panel(rollout1, "Fundo:", &fractalFundo,6,gluiCallback);
    listaCoresFundo->add_item(AZUL, "Azul");
    listaCoresFundo->add_item(VERDE, "Verde");
    listaCoresFundo->add_item(VERMELHO, "Vermelho");
    listaCoresFundo->add_item(AMARELO, "Amarelo");
    listaCoresFundo->add_item(ROSA, "Rosa");
    listaCoresFundo->add_item(CIANO, "Ciano");

    GLUI_Rollout *rollout2 = glui->add_rollout("Ondas",1);
    GLUI_EditText *editTextAmp = glui->add_edittext_to_panel(rollout2, "Amplitude: ", GLUI_EDITTEXT_FLOAT, &waveAmp);
    GLUI_Checkbox *check1st = glui->add_checkbox_to_panel(rollout2, "1st", &waveOn[0],2,gluiCallback);
    GLUI_Checkbox *check2nd = glui->add_checkbox_to_panel(rollout2, "2nd", &waveOn[1],2,gluiCallback);
    GLUI_Checkbox *check3rd = glui->add_checkbox_to_panel(rollout2, "3rd", &waveOn[2],2,gluiCallback);
    GLUI_Checkbox *check4th = glui->add_checkbox_to_panel(rollout2, "4th", &waveOn[3],2,gluiCallback);
    GLUI_Checkbox *check5th = glui->add_checkbox_to_panel(rollout2, "5th", &waveOn[4],2,gluiCallback);
    GLUI_Checkbox *checkSee = glui->add_checkbox_to_panel(rollout2, "Ver orig",&waveSee,2,gluiCallback);
    GLUI_Listbox *listaVeloc = glui->add_listbox_to_panel(rollout2, "Velocidade", &waveSpeed,3,gluiCallback);
    listaVeloc->add_item(1, "Normal");
    listaVeloc->add_item(2, "Lento");
    listaVeloc->add_item(3, "Muito lento");

    GLUI_Rollout *rollout3 = glui->add_rollout("Piriforme",1);
    GLUI_EditText *editTextA = glui->add_edittext_to_panel(rollout3, "A: ", GLUI_EDITTEXT_FLOAT, &piriA);
    GLUI_EditText *editTextB = glui->add_edittext_to_panel(rollout3, "B: ", GLUI_EDITTEXT_FLOAT, &piriB);
    GLUI_Listbox *listaVeloc2 = glui->add_listbox_to_panel(rollout3, "Velocidade", &piriSpeed,4,gluiCallback);
    listaVeloc2->add_item(1, "Normal");
    listaVeloc2->add_item(2, "Lento");
    listaVeloc2->add_item(3, "Muito lento");

    GLUI_Rollout *rollout4 = glui->add_rollout("Circle",1);
    GLUI_Checkbox *checkAnim = glui->add_checkbox_to_panel(rollout4, "Animar", &circleAnim);
    GLUI_Listbox *listaVeloc3 = glui->add_listbox_to_panel(rollout4, "Velocidade", &circleSpeed,5,gluiCallback);
    listaVeloc3->add_item(1, "Normal");
    listaVeloc3->add_item(2, "Lento");
    listaVeloc3->add_item(3, "Muito lento");
}

void init (void)
{
    LoadGLTextures();
    glEnable(GL_TEXTURE_2D);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_FLAT);

	waveInit();
	fractalInit();
	circleInit();
	piriInit();

	glui->sync_live();

}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition( 0, 50 );
  glutInitWindowSize( 800, 800 );

  main_window = glutCreateWindow( "Trabalho 1 de Comp Graf" );
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  GLUI_Master.set_glutIdleFunc( display );

  glui = GLUI_Master.create_glui( "Configs" , 0, 850, 50);
  createGluiControls();
  init();
  glui->set_main_gfx_window( main_window );


  glutMainLoop();
}
