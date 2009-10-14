/*************************************
* Arquivo: fractal.h
* Descrição: Contém as definições de variável e funções necessárias para serem acessadas ao longo do programa
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

#ifndef FRACTAL_H
#define FRACTAL_H

extern int mudouResolucao;
extern int fractalResolucao;
extern int fractalCor;
extern int fractalFundo;

enum cores
{
    AZUL,
    VERMELHO,
    VERDE,
    AMARELO,
    ROSA,
    CIANO
};

void fractalInit();
void DrawUpperLeft();

#endif // FRACTAL_H
