#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dipendente.h"
#define L 20
#define N 4

void allocaDip(FILE* f, dipendente_t* d)
{
    char nome[L];
    char cogn[L];
    int c1, c2, c3, c4;
    int m;

    fscanf(f, "%d %s %s %d %d %d %d", &m, nome, cogn, &c1, &c2, &c3, &c4);

    (*d).mat = m;
    (*d).nome = strdup(nome);
    (*d).cogn = strdup(cogn);
    (*d).comp = malloc(N*sizeof(int));

    int i=0;
    (*d).comp[i++] = c1;
    (*d).comp[i++] = c2;
    (*d).comp[i++] = c3;
    (*d).comp[i++] = c4;
}

int getMatricola(dipendente_t d)
{
    return d.mat;
}

void stampaDipendente(dipendente_t d, FILE* f)
{
    fprintf(f, "%d", d.mat);
}

int getCompetenzePerTipologia(dipendente_t d, int tipologia)
{
    return d.comp[tipologia];
}
