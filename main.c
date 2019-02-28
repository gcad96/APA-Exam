#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "dipendente.h"
#include "divisione.h"

#define L 10
#define TIPOLOGIE 4
#define I 2
#define FILE_DIV "delle divisioni.txt"
#define FILE_DIP "dei dipendenti.txt"
#define FILE_ASS "delle associazioni.txt"
#define FILE_SALV "salvataggio.txt"

#define OPERAIO 0
#define AMMINISTRATIVO 1
#define TECNICO 2
#define INFORMATICO 3

typedef struct soluzuione_s
{
    int* dipV;
    int* sigV;
    int* tipV;
}* Soluzione;


int leggiFileDip(dipendente_t** dipendenti, int* n);
int leggiFileDiv(divisione_t** divisioni, int* n, int*** m, int*** c, int*** r);
int leggiFileAss(divisione_t *div, dipendente_t *dip, int dimDiv, int **c, int **m, int **r, int n);

void calcolaCc(int** Cc, int dimDiv, int dimDip, int* sigV, int* tipV, int* dipV, dipendente_t* dipendenti);
void calcolaMm(int** Mm, int dimDiv, int dimDip, int* sigV, int* tipV, int* dipV, dipendente_t* dipendenti);
int verifica(dipendente_t* dipendenti, int* dipV, int* tipV, int* sigV, int dimDip, int dimDiv, int** c, int** m, int** Mm, int** Cc);
float deltaAvg(dipendente_t* dipendenti, int* dipV, int* tipV, int* sigV, int dimDip, int dimDiv, int** r, int** Mm, int** Cc);

int attribuzioneOttima(int dimDiv, dipendente_t* dipendenti, int** c, int**m, int** r, int dimDip, divisione_t* divisioni,
							 char* nome_file, Soluzione* soluzione);
void calcola_attribDivisione(int pos, int M, int n, int d, int *val, int *sol, dipendente_t *dipendenti, int **c, int **m,
                             int **r, float *min, Soluzione soluzione, int *dipV, int *tipV, int *sigV, int **Mm, int **Cc,
                             int *soll);
void calcola_attribTipologia(int pos, int M, int n, int *val, int *sol, int* tipV, int* dipV, int* sigV, int dimDiv,
                             dipendente_t* dipendenti, int** c, int**m, int** r, float* min, Soluzione soluzione, int** Mm, int** Cc);
int cercaDip(int* v, int val, int dim);
void cambiaSoluzione(Soluzione soluzione, int* dipV, int* sigV, int* tipV, int dim);

int cercaPerMatricola(dipendente_t* dip, int dim, int matr);
int cercaPerSigla(divisione_t* div, int dim, char* sigla);
int convertiTipologia(char c);

void stampaSoluzione(FILE* f, Soluzione sol, dipendente_t* dip, divisione_t* div, int dim);

void elaborazioneSoluzioneOttima(divisione_t* divisioni, Soluzione soluzione, int dimDiv, int dimDip, dipendente_t* dipendenti);

int SalvaSoluzione(divisione_t* divisioni, int dimDiv);

void distruggiDivisioni(divisione_t* divisioni, int dimDiv);



int main(int argc, char** argv)
{

    if(argc!=2)
    {
        fprintf(stderr, "Errore nel passaggio dei parametri sulla riga di comando.\n");
        return -1;
    }

    dipendente_t* dipendenti; int dimDip;
    if (!leggiFileDip(&dipendenti, &dimDip))
    {
        fprintf(stderr, "Errore durante l'apertura del file.\n");
        return 1;
    }

    int** minNumAddetti; int** compMin; int** compOtt;
    divisione_t* divisioni; int dimDiv;
    if (!leggiFileDiv(&divisioni, &dimDiv, &minNumAddetti, &compMin, &compOtt))
    {
        fprintf(stderr, "Errore durante l'apertura del file.\n");
        return 1;
    }

    if (!leggiFileAss(divisioni, dipendenti, dimDiv, compMin, minNumAddetti, compOtt, dimDip))
    {
        fprintf(stderr, "Errore durante l'apertura del file.\n");
        return 1;
    }

    Soluzione soluzione;
    if(!attribuzioneOttima(dimDiv, dipendenti, compMin, minNumAddetti, compOtt, dimDip, divisioni, argv[1], &soluzione))
    {
        fprintf(stderr, "Errore durante l'apertura del file.\n");
        return 1;
    }

    elaborazioneSoluzioneOttima(divisioni, soluzione, dimDiv, dimDip, dipendenti);

    if(!SalvaSoluzione(divisioni, dimDiv))
    {
        fprintf(stderr, "Errore durante il salvataggio.\n");
        return 2;
    }

    distruggiDivisioni(divisioni, dimDiv);

    free(dipendenti);   free(divisioni);    free(minNumAddetti);    free(compMin);  free(compOtt);  free(soluzione);

    return 0;

}


int leggiFileDip(dipendente_t** dipendenti, int* n)
{
    FILE* f;
    if( (f=fopen(FILE_DIP, "r"))==NULL )    return 0;

    fscanf(f, "%d ", n);

    *dipendenti = malloc((*n)*sizeof(dipendente_t));

    int i;
    for(i=0; i<(*n); i++)      allocaDip(f, &((*dipendenti)[i]));

    fclose(f);

    return 1;
}

int leggiFileDiv(divisione_t** divisioni, int* n, int*** m, int*** c, int*** r)
{
    FILE* f;
    if( (f=fopen(FILE_DIV, "r"))==NULL )    return 0;

    (*divisioni) = malloc( (I)*sizeof(divisione_t) );

    (*m) = malloc( (I)*sizeof(int*) );
    (*c) = malloc( (I)*sizeof(int*) );
    (*r) = malloc( (I)*sizeof(int*) );

    char sigla[L];

    int i=0;
    int dim=I;
    while(fscanf(f, "%s", sigla)==1)
    {
        if ( i > dim )
        {
            dim*=I;
            (*divisioni) = realloc((*divisioni), (dim)*sizeof(divisione_t));
            (*m) = realloc((*m), (dim)*sizeof(int*));
            (*c) = realloc((*c), (dim)*sizeof(int*));
            (*r) = realloc((*r), (dim)*sizeof(int*));
        }

        divisione_t div;
        creaDiv(&div);
        acquisisciDiv(div, sigla);
        (*divisioni)[i] = div;

        (*m)[i] = malloc(TIPOLOGIE* sizeof(int));
        (*c)[i] = malloc(TIPOLOGIE* sizeof(int));
        (*r)[i] = malloc(TIPOLOGIE* sizeof(int));
        int j;
        for(j=0; j<TIPOLOGIE; j++)
        {
            fscanf(f, "%d %d %d", &((*m)[i][j]),  &((*c)[i][j]), &((*r)[i][j]));
        }
        i++;
    }

    (*divisioni) = realloc((*divisioni), (i)*sizeof(divisione_t));
    (*m) = realloc((*m), (i)*sizeof(int*));
    (*c) = realloc((*c), (i)*sizeof(int*));
    (*r) = realloc((*r), (i)*sizeof(int*));
    *n = i;

    fclose(f);

    return 1;
}

int leggiFileAss(divisione_t *div, dipendente_t *dip, int dimDiv, int **c, int **m, int **r, int n)
{
    FILE* f;
    if( (f=fopen(FILE_ASS, "r"))==NULL )    return 0;

    int* dipV = malloc(n*sizeof(int));
    int* tipV = malloc(n*sizeof(int));
    int* sigV = malloc(n*sizeof(int));

    int i;
    for(i=0; i<n; i++)
    {
        int matricola;
        char tipologia;
        char sigla[L];
        fscanf(f, "%d %c %s", &matricola, &tipologia, sigla);
        dipV[i] = cercaPerMatricola(dip, n, matricola);
        tipV[i] = convertiTipologia(tipologia);
        sigV[i] = cercaPerSigla(div, dimDiv, sigla);
    }

    int** Mm = malloc(dimDiv*sizeof(int*));
    int** Cc = malloc(dimDiv*sizeof(int*));
    int j;
    for(i=0; i<dimDiv; i++)
    {
        Mm[i] = malloc(TIPOLOGIE* sizeof(int));
        Cc[i] = malloc(TIPOLOGIE* sizeof(int));
        for(j=0; j<TIPOLOGIE; j++)
        {
            Mm[i][j] = 0;
            Cc[i][j] = 0;
        }
    }

    if (verifica(dip, dipV, tipV, sigV, n, dimDiv, c, m, Mm, Cc))
    {
        printf("Allocazione proposta valida.\n");
    }
    else
    {
        printf("Allocazione proposta non valida.\n");
    }

    printf("Spostamento complessivo medio: %f\n", deltaAvg(dip, dipV, tipV, sigV, n, dimDiv, r, Mm, Cc));

    for(i=0; i<dimDiv; i++)
    {
        free(Mm[i]);
        free(Cc[i]);
    }

    free(Mm);
    free(Cc);

    fclose(f);

    return 1;
}

int attribuzioneOttima(int dimDiv, dipendente_t* dipendenti, int** c, int**m, int** r, int dimDip, divisione_t* divisioni, char* nome_file, Soluzione* soluzione)
{
    FILE* f;
    if( (f=fopen(nome_file, "w"))==NULL )    return 0;

    int* val = malloc(dimDip*sizeof(int));
    int i;
    for(i=0; i<dimDip; i++)
    {
        val[i] = i;
    }
    int* sol = malloc(dimDip*sizeof(int));
    (*soluzione) = malloc(sizeof(struct soluzuione_s));     (*soluzione)->dipV = malloc(dimDip* sizeof(int));    (*soluzione)->tipV = malloc(dimDip* sizeof(int));      (*soluzione)->sigV = malloc(dimDip* sizeof(int));
    float min = INT_MAX;
    int* dipV = malloc(dimDip*sizeof(int));
    int* tipV = malloc(dimDip*sizeof(int));
    int* sigV = malloc(dimDip*sizeof(int));

    int** Mm = malloc(dimDiv*sizeof(int*));
    int** Cc = malloc(dimDiv*sizeof(int*));

    int* soll = malloc(dimDip*sizeof(int));

    for(i=0; i<dimDiv; i++)
    {
        Mm[i] = malloc(TIPOLOGIE* sizeof(int));
        Cc[i] = malloc(TIPOLOGIE* sizeof(int));
    }
    calcola_attribDivisione(0, 0, dimDip, dimDiv, val, sol, dipendenti, c, m, r, &min, *soluzione, dipV, tipV, sigV, Mm,
                            Cc, soll);
    stampaSoluzione(f, *soluzione, dipendenti, divisioni, dimDip);

    fclose(f);

    for(i=0; i<dimDiv; i++)
    {
        free(Mm[i]);
        free(Cc[i]);
    }

    free(Mm);
    free(Cc);

    free(val);
    free(sol);

    free(dipV);
    free(tipV);
    free(sigV);

    free(soll);

    return 1;
}

void calcola_attribDivisione(int pos, int M, int n, int d, int *val, int *sol, dipendente_t *dipendenti, int **c, int **m,
                             int **r, float *min, Soluzione soluzione, int *dipV, int *tipV, int *sigV, int **Mm, int **Cc,
                             int *soll)
{
    int i, j;

    if (pos>=n)
    {
        if (M<=d)
        {
            int k=0;
            for(i=0; i<M; i++)
            {
                for(j=0; j<n; j++)
                {
                    if(sol[j]==i)
                    {
                        dipV[k]=val[j];
                        sigV[k++]=i;
                    }
                }
            }
            calcola_attribTipologia(0, 0, n, val, soll, tipV, dipV, sigV, d, dipendenti, c, m, r, min, soluzione, Mm, Cc);
        }

        return;
    }

    for(i=0; i<M; i++)
    {
        sol[pos] = i;
        calcola_attribDivisione(pos + 1, M, n, d, val, sol, dipendenti, c, m, r, min, soluzione, dipV, tipV, sigV, Mm,
                                Cc, soll);
    }
    sol[pos] = M;
    calcola_attribDivisione(pos + 1, M + 1, n, d, val, sol, dipendenti, c, m, r, min, soluzione, dipV, tipV, sigV, Mm,
                            Cc, soll);

}

void calcola_attribTipologia(int pos, int M, int n, int *val, int *sol, int* tipV, int* dipV, int* sigV, int dimDiv, dipendente_t* dipendenti, int** c, int**m, int** r, float* min, Soluzione soluzione, int** Mm, int** Cc)
{
    int i, j;

    if (pos>=n)
    {
        if (M==TIPOLOGIE)
        {
            for(i=0; i<M; i++)
            {
                for(j=0; j<n; j++)
                {
                    if(sol[j]==i)
                    {
                        int indiceDipendente = cercaDip(dipV, val[j], n);
                        tipV[indiceDipendente]=i;
                    }
                }
            }
            if(verifica(dipendenti, dipV, tipV, sigV, n, dimDiv, c, m, Mm, Cc))
            {
                float avg;
                if ((avg=deltaAvg(dipendenti, dipV, tipV, sigV, n, dimDiv, r, Mm, Cc))<(*min))
                {
                    *min = avg;
                    cambiaSoluzione(soluzione, dipV, sigV, tipV, n);
                }
            }
        }
        return;
    }

    for(i=0; i<M; i++)
    {
        sol[pos] = i;
        calcola_attribTipologia(pos + 1, M, n, val, sol, tipV, dipV, sigV, dimDiv, dipendenti, c, m, r, min, soluzione, Mm, Cc);
    }
    sol[pos] = M;
    calcola_attribTipologia(pos + 1, M +1, n, val, sol, tipV, dipV, sigV, dimDiv, dipendenti, c, m, r, min, soluzione, Mm, Cc);

}

int cercaDip(int* v, int val, int dim)
{
    int i;
    for(i=0; i<dim; i++)
    {
        if(v[i]==val)   return i;
    }

    return 0;
}

int verifica(dipendente_t* dipendenti, int* dipV, int* tipV, int* sigV, int dimDip, int dimDiv, int** c, int** m, int** Mm, int** Cc)
{
    int i, j;
    for(i=0; i<dimDiv; i++)
    {
        for(j=0; j<TIPOLOGIE; j++)
        {
            Mm[i][j] = 0;
            Cc[i][j] = 0;
        }
    }
    calcolaCc(Cc, dimDiv, dimDip, sigV, tipV, dipV, dipendenti);
    calcolaMm(Mm, dimDiv, dimDip, sigV, tipV, dipV, dipendenti);
    for(i=0; i<dimDiv; i++)
    {
        for(j=0; j<TIPOLOGIE; j++)
        {
            if( (Cc[i][j] < c[i][j]) || (Mm[i][j] < m[i][j]) )
            {
                return 0;
            }
        }
    }

    return 1;
}

float deltaAvg(dipendente_t* dipendenti, int* dipV, int* tipV, int* sigV, int dimDip, int dimDiv, int** r, int** Mm, int** Cc)
{
    int tot = 0;

    int i, j;
    for(i=0; i<dimDiv; i++)
    {
        for(j=0; j<TIPOLOGIE; j++)
        {
            Mm[i][j] = 0;
            Cc[i][j] = 0;
        }
    }

    calcolaCc(Cc, dimDiv, dimDip, sigV, tipV, dipV, dipendenti);

    for(i=0; i<dimDiv; i++)
    {
        for(j=0; j<TIPOLOGIE; j++)
        {
            tot += abs(r[i][j]-Cc[i][j]);
        }
    }

    return ( (float) tot ) / dimDiv;
}

void calcolaCc(int** Cc, int dimDiv, int dimDip, int* sigV, int* tipV, int* dipV, dipendente_t* dipendenti)
{
    int i, j;
    for(i=0; i<dimDiv; i++)
    {
        for(j=0; j<dimDip; j++)
        {
            if (sigV[j]==i)
            {
                Cc[i][tipV[j]]+= getCompetenzePerTipologia(dipendenti[dipV[j]], tipV[j]);
            }
        }
    }
}

void calcolaMm(int** Mm, int dimDiv, int dimDip, int* sigV, int* tipV, int* dipV, dipendente_t* dipendenti)
{
    int i, j;
    for(i=0; i<dimDiv; i++)
    {
        for(j=0; j<dimDip; j++)
        {
            if (sigV[j]==i)
            {
                Mm[i][tipV[j]]++;
            }
        }
    }
}

int cercaPerMatricola(dipendente_t* dip, int dim, int matr)
{
    int i;
    for(i=0; i<dim; i++)
    {
        if( getMatricola(dip[i]) == matr )  return i;
    }

    return -1;
}

int convertiTipologia(char c)
{
    if (strcmp(&c, "o")==0)
    {
        return OPERAIO;
    }

    if (strcmp(&c, "a")==0)
    {
        return AMMINISTRATIVO;
    }

    if (strcmp(&c, "t")==0)
    {
        return TECNICO;
    }

    if (strcmp(&c, "i")==0)
    {
        return INFORMATICO;
    }

    return -1;
}

int cercaPerSigla(divisione_t* div, int dim, char* sigla)
{
    int i;
    for(i=0; i<dim; i++)
    {
        char* s = getSigla(div[i]);
        if( strcmp(s, sigla)==0 )  return i;
        free(s);
    }

    return -1;
}

void cambiaSoluzione(Soluzione soluzione, int* dipV, int* sigV, int* tipV, int dim)
{
    int i;
    for(i=0; i<dim; i++)
    {
        soluzione->dipV[i] = dipV[i];
        soluzione->tipV[i] = tipV[i];
        soluzione->sigV[i] = sigV[i];
    }
}

void stampaSoluzione(FILE* f, Soluzione sol, dipendente_t* dip, divisione_t* div, int dim)
{
    int i;
    for(i=0; i<dim; i++)
    {
        int matricola = getMatricola(dip[sol->dipV[i]]);
        char* sigla = getSigla(div[sol->sigV[i]]);
        fprintf(f, "%d - %s\n", matricola, sigla);
        free(sigla);
    }
}

void elaborazioneSoluzioneOttima(divisione_t* divisioni, Soluzione soluzione, int dimDiv, int dimDip, dipendente_t* dipendenti)
{
    int i;
    for(i=0; i<dimDip; i++)
    {
        int indiceDipendente = soluzione->dipV[i]; //printf("%d\n", indiceDipendente);
        insertDip(indiceDipendente, divisioni[soluzione->sigV[i]], dipendenti, soluzione->tipV[i]);
    }
}

int SalvaSoluzione(divisione_t* divisioni, int dimDiv)
{
    FILE* f;
    if( (f=fopen(FILE_SALV, "w"))==NULL )    return 0;
    setbuf(f, 0);
    int i;
    for(i=0; i<dimDiv; i++)
    {
        stampaDivisione(f, divisioni[i]);
    }

    fclose(f);

    return 1;
}

void distruggiDivisioni(divisione_t* divisioni, int dimDiv)
{
    int i;
    for(i=0; i<dimDiv; i++)
    {
        distruggiDiv(divisioni[i]);
    }
}
