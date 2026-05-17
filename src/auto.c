#include <stdio.h>

#include "auto.h"

#define INFINITO 99999

static int calcular_ruta(Ciudad *ciudad, Auto *automovil);

Auto Auto_new(Ciudad *ciudad, int id, Coordenada origen, Coordenada destino) {
    Auto automovil = {
        .id = id,
        .origen = origen,
        .destino = destino,
        .idx = 0,
        .tam_ruta = 0,
        .activo = true,
    };

    calcular_ruta(ciudad, &automovil);

    return automovil;
}

void Auto_update(Auto *automovil) {
    if (!automovil->activo)
        return;

    if (automovil->idx >= automovil->tam_ruta) {
        automovil->activo = false;
        printf("Auto %d TERMINO en (%d,%d)\n", automovil->id,
               automovil->destino.x, automovil->destino.y);
        return;
    }

    Coordenada sig = automovil->ruta[automovil->idx];

    printf("Auto %d AVANZA a (%d,%d)\n", automovil->id, sig.x, sig.y);

    automovil->idx++;
}

static int calcular_ruta(Ciudad *ciudad, Auto *automovil) {
    int dist[FILAS][COLUMNAS];
    int vis[FILAS][COLUMNAS];
    Coordenada padre[FILAS][COLUMNAS];

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            dist[i][j] = INFINITO;
            vis[i][j] = 0;
            padre[i][j].x = -1;
            padre[i][j].y = -1;
        }
    }

    dist[automovil->origen.x][automovil->origen.y] = 0;

    for (int k = 0; k < FILAS * COLUMNAS; k++) {

        Coordenada u = {-1, -1};
        int menor = INFINITO;

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                if (!vis[i][j] && dist[i][j] < menor) {
                    menor = dist[i][j];
                    u.x = i;
                    u.y = j;
                }
            }
        }

        if (u.x == -1)
            break;

        if (u.x == automovil->destino.x && u.y == automovil->destino.y)
            break;

        vis[u.x][u.y] = 1;

        for (int d = 0; d < 4; d++) {

            int nx = u.x + dx[d];
            int ny = u.y + dy[d];

            if (nx < 0 || nx >= FILAS || ny < 0 || ny >= COLUMNAS)
                continue;

            if (vis[nx][ny])
                continue;

            int peso = 1;

            if (ciudad->grid[nx][ny].tiene_semaforo)
                peso = 2;

            int nd = dist[u.x][u.y] + peso;

            if (nd < dist[nx][ny]) {
                dist[nx][ny] = nd;
                padre[nx][ny] = u;
            }
        }
    }

    if (dist[automovil->destino.x][automovil->destino.y] == INFINITO) {
        printf("Auto %d: no se encontro ruta\n", automovil->id);
        return 0;
    }

    Coordenada tmp[MAX_RUTA];
    int n = 0;
    Coordenada cur = automovil->destino;

    while (!(cur.x == automovil->origen.x && cur.y == automovil->origen.y)) {
        tmp[n++] = cur;
        cur = padre[cur.x][cur.y];
        if (n >= MAX_RUTA)
            break;
    }

    tmp[n++] = automovil->origen;

    automovil->tam_ruta = n;

    for (int i = 0; i < n; i++) {
        automovil->ruta[i] = tmp[n - 1 - i];
    }

    return 1;
}
