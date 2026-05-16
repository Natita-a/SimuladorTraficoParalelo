#include <stdio.h>

#include "mapa.h"

int main() {

    Ciudad ciudad;

    ciudad_init(&ciudad);

    ciudad_imprimir(&ciudad);

    actualizar_semaforos(&ciudad);

    return 0;
}
