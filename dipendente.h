#ifndef DIPENDENTE_H
#define DIPENDENTE_H

typedef struct dipendente_s
{
    int mat;
    char* nome;
    char* cogn;
    int* comp;
} dipendente_t;

void allocaDip(FILE* f, dipendente_t* d);
int getMatricola(dipendente_t d);
int getCompetenzePerTipologia(dipendente_t d, int tipologia);
void stampaDipendente(dipendente_t d, FILE* f);

#endif //DIPENDENTE_H
