#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dipendente.h"
#include "divisione.h"
#define I 2
#define OPERAIO 0
#define AMMINISTRATIVO 1
#define TECNICO 2
#define INFORMATICO 3

struct div_s
{
    char* sigla;
    int* dip;
    int dimDip;
    int actDimDip;
    int* lav;
    int dimLav;
    int actDimLav;
    dipendente_t* rifDipendenti;
};

void getLavoro(divisione_t d, int i, char* lavoro);
void stampaLavoro(int l, FILE* f);

void creaDiv(divisione_t* d)
{
    (*d) = malloc(sizeof(struct div_s));

    (*d)->dip = malloc(I*sizeof(int));
    (*d)->lav = malloc(I*sizeof(int));
    (*d)->dimDip = 0;
    (*d)->dimLav = 0;
    (*d)->actDimDip = I;
    (*d)->actDimLav = I;
    (*d)->rifDipendenti = NULL;
}

void distruggiDiv(divisione_t d)
{
    free(d->sigla);
    free(d->dip);
    free(d->lav);

    free(d);
}

void acquisisciDiv(divisione_t d, char* sigla)
{
    d->sigla=strdup(sigla);
}

void insertDip(int i, divisione_t d, dipendente_t* rif, int lavoro)
{
    if(d->rifDipendenti==NULL)      d->rifDipendenti = rif;

    if ( ((d->dimDip)+1) > d->actDimDip )
    {
        d->actDimDip*=I;    d->actDimLav*=I;
        d->dip = realloc(d->dip, (d->actDimDip)* sizeof(int));
        d->lav = realloc(d->lav, (d->actDimLav)* sizeof(int));
    }

    d->dip[(d->dimDip)++] = i;
    d->lav[(d->dimLav)++] = lavoro;
}

void stampaDivisione(FILE *salv, divisione_t d)
{
    int i;
    for(i=0; i<d->dimDip; i++)
    {
        stampaDipendente(d->rifDipendenti[d->dip[i]], salv);
        fprintf(salv, " ");
        stampaLavoro(d->lav[i], salv);
        fprintf(salv, " ");
        fprintf(salv, "%s\n", d->sigla);
    }
}

char* getSigla(divisione_t d)
{
    return strdup(d->sigla);
}

void getLavoro(divisione_t d, int i, char* lavoro)
{
    switch(d->lav[i])
    {
        case OPERAIO:
            strcpy(lavoro, "Operaio");
            break;
        case AMMINISTRATIVO:
            strcpy(lavoro, "Amministrativo");
            break;
        case TECNICO:
            strcpy(lavoro, "Tecnico");
            break;
        case INFORMATICO:
            strcpy(lavoro, "Informatico");
            break;
        default:
            strcpy(lavoro, "ND");
    }
}

void stampaLavoro(int l, FILE* f)
{
    switch(l)
    {
        case OPERAIO:
            fprintf(f, "o");
            break;
        case AMMINISTRATIVO:
            fprintf(f, "a");
            break;
        case TECNICO:
            fprintf(f, "t");
            break;
        case INFORMATICO:
            fprintf(f, "i");
            break;
        default:
            fprintf(f, "ND");
    }
}
