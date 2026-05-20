# Simulador de Trafico Paralelo

Simulación discreta de tráfico urbano escrita en C, paralelizada con **OpenMP**.
Los vehículos calculan rutas mínimas a través de una cuadrícula de 12×12
intersecciones usando el algoritmo de Dijkstra y avanzan paso a paso respetando
el estado de los semáforos. El cálculo de rutas, la actualización de semáforos y
el movimiento de vehículos se ejecutan como hilos concurrentes.

## Acerca del proyecto

La simulación modela una cuadrícula de 12×12 que representa el trazado de una
ciudad, con avenidas horizontales y calles verticales nombradas. Las entidades
principales son:

- **Intersecciones** (`Interseccion`) — celdas de la cuadrícula que pueden ser
  ocupadas por un solo vehículo a la vez.

- **Semáforos** (`Semaforo`) — ubicados en 20 intersecciones fijas, ciclan entre
  `VERDE → AMARILLO → ROJO` con duraciones configurables.

- **Vehículos** (`Auto`) — cada uno calcula su propia ruta de menor costo al ser
  creado (las intersecciones con semáforo tienen peso 2, las normales peso 1) y
  avanza una celda por paso de simulación cuando el camino está libre y el
  semáforo no está en rojo.

Cada paso de simulación ejecuta tres fases paralelas:

1. Todos los hilos de semáforos actualizan su estado de forma concurrente.

2. Todos los hilos de vehículos intentan avanzar de forma concurrente; la
   contención sobre la celda de destino se resuelve con mutexes `omp_lock_t` por
   intersección.

3. La cuadrícula de ocupación se reinicia en paralelo para el siguiente paso.

El bucle termina cuando todos los vehículos han llegado a su destino.

## Compilación

### Requisitos previos

| Requisito      | Versión mínima | Notas                                      |
| -------------- | -------------- | ------------------------------------------ |
| GCC            | 9              | Debe soportar C17 y OpenMP 4.5             |
| OpenMP runtime | 4.5            | Incluido con GCC como `libgomp`            |
| GNU Make       | 3.82           |                                            |
| tomlc17        | —              | Incluido en `third_party/tomlc17/`         |
| clang-format   | —              | Opcional — requerido solo para `make fmt`  |
| clang-tidy     | —              | Opcional — requerido solo para `make lint` |

En Debian/Ubuntu:

```bash
sudo apt install gcc libomp-dev make clang-format clang-tidy
```

En Arch:

```bash
sudo pacman -S gcc openmp make clang
```

### Clonar el repositorio

```bash
git clone https://github.com/youruser/city-traffic-sim.git
cd city-traffic-sim
```

> [!IMPORTANT]
>
> El parser `tomlc17` debe estar presente en `third_party/tomlc17/src/tomlc17.h`
> y `third_party/tomlc17/src/tomlc17.c`. Si no fue descargado durante el clone,
> inicialízalo con:
>
> ```bash
> git submodule update --init --recursive
> ```

### Compilación

El proyecto incluye un Makefile de estilo Cargo. Todos los binarios se generan
bajo `target/`.

**Build de depuración** (sin optimizaciones, con ASan/UBSan y símbolos de
depuración):

```bash
make build
# → target/debug/my_project
```

**Build de release** (O3, `-march=native`, sin sanitizadores):

```bash
make release
# → target/release/my_project
```

**Solo verificación de sintaxis** (sin binario de salida, equivalente a
`cargo check`):

```bash
make check
```

**Referencia completa de targets:**

| Target             | Equivalente             | Descripción                                                 |
| ------------------ | ----------------------- | ----------------------------------------------------------- |
| `make build`       | `cargo build`           | Binario de depuración                                       |
| `make release`     | `cargo build --release` | Binario optimizado                                          |
| `make run`         | `cargo run`             | Compila y ejecuta el binario de depuración                  |
| `make run-release` | `cargo run --release`   | Compila y ejecuta el binario de release                     |
| `make test`        | `cargo test`            | Compila y ejecuta `tests/` contra las fuentes de biblioteca |
| `make check`       | `cargo check`           | Verificación de sintaxis, sin binario                       |
| `make fmt`         | `cargo fmt`             | Formatea con `clang-format`                                 |
| `make lint`        | `cargo clippy`          | Analiza con `clang-tidy`                                    |
| `make clean`       | `cargo clean`           | Elimina `target/`                                           |

**El control del número de hilos** se realiza en tiempo de ejecución, no en
compilación — establece `OMP_NUM_THREADS` al invocar make o el binario
directamente (ver [Uso](#uso)).

## Uso

```text
make run [ARGS='<ruta_config>']
```

**Ejecutar con valores por defecto** (busca `traffic.toml` en el directorio de
trabajo; usa los valores internos si el archivo no existe):

```bash
make run
```

**Ejecutar con una configuración explícita:**

```bash
make run ARGS='configs/centro.toml'
```

**Ejecutar el binario de release optimizado:**

```bash
make run-release ARGS='configs/centro.toml'
```

**Control del paralelismo** — establece `OMP_NUM_THREADS` como prefijo de
cualquier invocación a make. Si no está definido, OpenMP usa el número de CPUs
lógicas reportadas por el sistema operativo.

```bash
OMP_NUM_THREADS=8 make run
OMP_NUM_THREADS=4 make run-release ARGS='traffic.toml'
```

**Ejecutar las pruebas:**

```bash
make test
```

Las fuentes de prueba se encuentran en `tests/`. El runner las compila enlazadas
contra todas las fuentes de biblioteca (es decir, todo lo que hay en `src/`
excepto `main.c`), por lo que la API completa de simulación está disponible
desde el código de prueba.

## Configuración (TOML)

Crea un archivo `traffic.toml` (o cualquier ruta que pases por línea de
comandos) para sobreescribir los valores por defecto. Todas las claves son
opcionales; las ausentes vuelven a los valores indicados a continuación.

```toml
[simulation]
num_vehicles = 20     # Número de vehículos a crear  (1 – 200)

[semaphore]
green_duration  = 5   # Pasos que el semáforo permanece en verde   (≥ 1)
yellow_duration = 1   # Pasos que el semáforo permanece en amarillo (≥ 1)
red_duration    = 4   # Pasos que el semáforo permanece en rojo    (≥ 1)
```

| Clave                       | Tipo   | Por defecto | Rango        |
| --------------------------- | ------ | ----------- | ------------ |
| `simulation.num_vehicles`   | entero | `20`        | `[1, 200]`   |
| `semaphore.green_duration`  | entero | `5`         | `[1, 65535]` |
| `semaphore.yellow_duration` | entero | `1`         | `[1, 65535]` |
| `semaphore.red_duration`    | entero | `4`         | `[1, 65535]` |

Los valores fuera de rango se ajustan silenciosamente. Un error de parseo en el
archivo genera una advertencia en `stderr` y vuelve a los valores por defecto;
un archivo inexistente se ignora silenciosamente.

## Supuestos

- **Topología de la cuadrícula** — la ciudad siempre es 12×12
  (`FILAS × COLUMNAS`). Todos los vehículos entran y salen a través de las 20
  celdas de borde predefinidas.

- **Un vehículo por celda** — cada intersección aloja como máximo un vehículo a
  la vez. Un vehículo que no puede avanzar (celda ocupada o semáforo en rojo)
  simplemente espera al siguiente paso.

- **Tiempo discreto** — un paso de simulación corresponde a un tick lógico.
  `sleep_ms(1000)` agrega un retardo de 1 segundo en tiempo real entre pasos
  para facilitar la observación; elimínalo o redúcelo para hacer benchmarks.

- **Posiciones fijas de semáforos** — las 20 posiciones de los semáforos están
  hardcodeadas en `src/mapa.c`. Cambiar el tamaño de la cuadrícula requiere
  actualizar tanto `FILAS`/`COLUMNAS` en `include/mapa.h` como el arreglo
  `POS_SEMAFOROS`.

- **Ponderación de rutas** — las intersecciones con semáforo tienen costo 2 de
  traversal, las normales costo 1. Las rutas se calculan una vez al crear el
  vehículo y no reaccionan a las condiciones de tráfico en tiempo real.

- **Rotación de destinos** — con más de 15 vehículos la lista de destinos fijos
  rota con `% 15`, por lo que los vehículos a partir del índice 14 reutilizan
  destinos desde el inicio de la lista.

## Notas importantes

- **Seguridad de hilos en `calcular_ruta`** — la implementación de Dijkstra en
  `src/auto.c` es completamente segura para hilos: reserva todos los arreglos de
  trabajo (`dist`, `vis`, `padre`) en la pila del hilo y solo lee el campo
  `tiene_semaforo` de `ciudad->grid`, que es inmutable tras el retorno de
  `Ciudad_new`.

- **Garantías de orden entre fases** — las tres regiones paralelas `for` en el
  bucle principal llevan una barrera implícita de OpenMP al salir. El estado de
  los semáforos queda totalmente confirmado antes de que cualquier hilo de
  vehículo lo lea, y el movimiento de vehículos queda totalmente confirmado
  antes de que se ejecute el reinicio de ocupación.

- **Granularidad de los locks** — el bloqueo es por intersección
  (`omp_lock_t grid_locks[FILAS][COLUMNAS]`), no global. Solo se bloquea la
  celda de destino de un intento de movimiento; la celda de origen se libera sin
  lock porque cada vehículo es el único propietario de su posición actual.

- **`Ciudad_destroy` debe llamarse** antes de que el proceso termine para
  liberar todos los handles `omp_lock_t` y evitar fugas de recursos.

- **Orden en `stdout`** — todas las llamadas a `printf` dentro de regiones
  paralelas están protegidas por `#pragma omp critical(stdout)`. Las líneas
  aparecerán en orden no determinista entre vehículos, lo cual es el
  comportamiento esperado en una simulación concurrente; el número de paso
  impreso antes de cada fase sirve como referencia de orden lógico.

- **Límite de `MAX_RUTA`** — una ruta de más de 100 celdas se trunca. En una
  cuadrícula de 12×12 la ruta más corta máxima es de 22 pasos, por lo que este
  límite nunca se alcanza en condiciones normales de operación.
