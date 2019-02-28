#ifndef DIVISIONE_H
#define DIVISIONE_H

typedef struct div_s * divisione_t;

void creaDiv(divisione_t* d);
void distruggiDiv(divisione_t d);
void acquisisciDiv(divisione_t d, char* sigla);
void insertDip(int i, divisione_t d, dipendente_t* rif, int lavoro);
char* getSigla(divisione_t d);
void stampaDivisione(FILE *salv, divisione_t d);

#endif //DIVISIONE_H
