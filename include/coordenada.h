#ifndef COORDENADA_H
#define COORDENADA_H

typedef struct {
    int x;
    int y;
} Coordenada;

// Contructor
Coordenada Coordenada_new(int x, int y);
void Coordenada_to_string(const Coordenada *coord);

#endif // !COORDENADA_H
