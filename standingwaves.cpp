/*************************************
* Arquivo: standingwaves.cpp
* Descrição: Contém as funções relacionadas à exibição das ondas estacionárias
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

#include "standingwaves.h"
#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

int mudouFreq;
float waveAmp, waveTime, waveFreq, waveLambda,t;
int waveSkip, waveOn[5];
vector<double> wavePoints[5];
vector<double> waveResult(51);
float waveColors[5][3];
int waveSee, waveSpeed;
int waveSkipLimit;

const int numeroPontos = 51;

GLuint texture[1];

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;

void waveInit()
{
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
}

// Conforme discutido na lista de discussão, função para carregar o bmp e a textura usadas a partir do
// tutorial do NeHe para texturas: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
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


void DrawViolin()
{
    glColor3f(1.0f,1.0f,1.0f);

    // Novamente, 2.88 foi obtido a partir de experimentação.
    // Esse quadrado é desenhado para limpar a tela com um fundo branco
	glBegin(GL_QUADS);
        glVertex2f(-2.88f, -2.88f);
        glVertex2f( 2.88f, -2.88f);
        glVertex2f( 2.88f,  2.88f);
        glVertex2f(-2.88f,  2.88f);
    glEnd();

    // Agora, prendemos a textura, e desenhamos um retângulo com o violino.
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

    for ( int i = 0 ; i < 5 ; i++ ) wavePoints[i] = vector<double>(numeroPontos);

    double stepX = longitd/wavePoints[0].size();


    for ( int j = 0 ; j < (int)waveResult.size() ; j++ )
        waveResult[j] = 0;

    for ( int i = 0 ; i < 5 ; i++ )
    {
        // Só calculamos para as ondas que estão sendo exibidas, assim já evitamos cálculos desnecessários
        if ( !waveOn[i] )
            continue;

        double amp, k, x, w;
        amp = waveAmp/10;

        // K = 2*PI/lambda , fazemos lambda = 1 para a 1a harmonica, e daí dividimos conforme mudamos
        // lambda = 1/2 para a segunda, lambda = 1/3 para a terceira, etc;
        k = 2*M_PI*(i+1.0);

        // w = 2*PI*freq , fazemos frequencia = 1 para a 1a harmonica, e daí multiplicamos conforme mudamos
        // freq = 2 para a segunda, freq = 3 para a terceira, etc;
        w = 2*M_PI*(i+1.0);

        x = 0;
        for ( int j = 0 ; j < (int)wavePoints[i].size()-1 ; j++, x+=stepX )
        {
            // y = y0 sin (kx - wt) + y0 sin (kx + wt)
            wavePoints[i][j] = amp * sin(k*x - w*t) + amp * sin(k*x + w*t);

            // waveResult guarda a onda resultante das interferências entre as ondas
            waveResult[j] += wavePoints[i][j];
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

    // Desenha a resultante
    double x;
    double stepX;
    stepX = (2.44*2.0)/(waveResult.size());
    x = -2.44;
    glLineWidth(3);
    glBegin(GL_LINES);
        for ( int i = 0 ; i < (int)waveResult.size()-1 ; i++ , x+=stepX)
        {
            glVertex2f(x, waveResult[i]);
            glVertex2f(x+stepX, waveResult[i+1]);
        }
    glEnd();
    glLineWidth(1);

    // Caso o usuário tenha marcado para ver as ondas originais, desenha
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
            for ( int j = 0 ; j < (int)wavePoints[i].size()-1 ; j++ , x+=stepX)
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
