#include "../include/auto.h"

#include <omp.h>
#include <stdio.h>

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

    // calcular_ruta is thread-safe: uses only thread-local stack arrays and
    // reads ciudad->grid fields that are immutable after Ciudad_new (i.e.,
    // tiene_semaforo). No locks needed here.
    calcular_ruta(ciudad, &automovil);
    return automovil;
}

void Auto_update(Auto *automovil, Ciudad *ciudad) {
    if (!automovil->activo)
        return;

    if (automovil->idx >= automovil->tam_ruta) {
        automovil->activo = false;
#pragma omp critical(stdout)
        printf("Auto %d TERMINO en (%d,%d)\n", automovil->id,
               automovil->destino.x, automovil->destino.y);
        return;
    }

    Coordenada actual = (automovil->idx > 0)
                            ? automovil->ruta[automovil->idx - 1]
                            : automovil->origen;
    Coordenada sig = automovil->ruta[automovil->idx];

    // Semaphore state is read-only during the vehicle phase — semaforos were
    // fully updated (with an implicit barrier) before this parallel region.
    Interseccion *inter_actual = &ciudad->grid[actual.x][actual.y];
    if (inter_actual->tiene_semaforo) {
        Semaforo *s = &ciudad->semaforos[inter_actual->id_semaforo];
        if (s->estado == ROJO) {
#pragma omp critical(stdout)
            printf("Auto %d en ROJO en (%d,%d)\n", automovil->id, actual.x,
                   actual.y);
            return;
        }
    }

    // Lock only the destination cell. The source cell is exclusively owned by
    // this vehicle (no two vehicles share a position), so setting it to false
    // is safe without a lock.
    Interseccion *inter_sig = &ciudad->grid[sig.x][sig.y];
    omp_set_lock(&ciudad->grid_locks[sig.x][sig.y]);

    if (!inter_sig->ocupada) {
        if (automovil->idx > 0)
            inter_actual->ocupada = false;
        inter_sig->ocupada = true;
#pragma omp critical(stdout)
        printf("Auto %d AVANZA a (%d,%d)\n", automovil->id, sig.x, sig.y);
        automovil->idx++;
    }

    omp_unset_lock(&ciudad->grid_locks[sig.x][sig.y]);
}

// Thread-safe: all state is on the stack; ciudad->grid is read-only here
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

            int peso = ciudad->grid[nx][ny].tiene_semaforo ? 2 : 1;
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

    for (int i = 0; i < n; i++)
        automovil->ruta[i] = tmp[n - 1 - i];

    return 1;
}
