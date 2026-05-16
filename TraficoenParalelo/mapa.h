#ifndef MAPA_H
#define MAPA_H

#define FILAS 12
#define COLUMNAS 12

#define NUM_SEMAFOROS 20

#define MAX_RUTA 100
#define MAX_NOMBRE 32

typedef struct {

    int x;
    int y;

} Coord;

typedef enum {

    ROJO,
    AMARILLO,
    VERDE

} EstadoSemaforo;

typedef enum {

    NORTE,
    SUR,
    ESTE,
    OESTE

} Direccion;

typedef struct {

    int id;

    Coord posicion;

    EstadoSemaforo estado;

    Direccion direccion;

    int t_verde;
    int t_amarillo;
    int t_rojo;

    int timer;

} Semaforo;

typedef struct {

    int id;

    Coord posicion;

    int ocupada;

    int tiene_semaforo;

    int id_semaforo;

} Interseccion;

typedef struct {

    int id;

    Coord origen;

    Coord destino;

    Coord posicion_actual;

    Coord ruta[MAX_RUTA];

    int tam_ruta;

    int paso_actual;

    int llego;

} Vehiculo;

typedef struct {

    Interseccion grid[FILAS][COLUMNAS];

    char calles_h[FILAS][MAX_NOMBRE];

    char calles_v[COLUMNAS][MAX_NOMBRE];

    Semaforo semaforos[NUM_SEMAFOROS];

    int num_semaforos;

} Ciudad;

void ciudad_init(Ciudad *c);

void ciudad_imprimir(const Ciudad *c);

void actualizar_semaforos(Ciudad *c);

#endif
