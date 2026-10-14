# Grafos y estructuras de datos

Parcial AED-II UBP

## Contenido

- [Red eléctrica: grafo y DFS](#red-eléctrica-grafo-y-dfs)
- [Árbol B: indexador de archivos](#árbol-b-indexador-de-archivos)
- [Montículo máximo y Heap Sort](#montículo-máximo-y-heap-sort)
- [Requisitos y compilación](#requisitos-y-compilación)

## Red eléctrica: grafo y DFS

### Modelo

La red de distribución eléctrica se representa como un grafo no dirigido:

- **Nodos:** centrales de generación, subestaciones, transformadores y
  consumidores.
- **Aristas:** líneas eléctricas que conectan dos nodos.
- **Atributos de las aristas:** peso (distancia), capacidad y estado operativo.

La apertura de un interruptor o el corte de una línea se representa eliminando
la arista entre sus extremos. El DFS recorre las conexiones activas y obtiene
los componentes conexos. Un componente que no contiene ningún nodo de
generación se informa como un sector sin energía.

### Archivos

- [`Graphs/red_electrica.cc`](Graphs/red_electrica.cc): implementación base
  con listas de adyacencia, DFS y simulación de corte de línea.
- [`Graphs/DFS.h`](Graphs/DFS.h): función DFS reutilizable llamada por la
  implementación base.
- [`Graphs/red_electrica_optimizado.cc`](Graphs/red_electrica_optimizado.cc):
  versión alternativa con almacenamiento indexado, DFS iterativo y vectores
  para visitados y sectores.
- [`Graphs/Analisis-y-criterios.md`](Graphs/Analisis-y-criterios.md):
  comparación de rendimiento y análisis según eficiencia, claridad y
  adecuación.

## Árbol B: indexador de archivos

### Caso de uso

El ejemplo simula el índice de una carpeta de apuntes. Cada archivo regular
se inserta en un árbol B y su ruta se usa como clave. El árbol mantiene sus
claves ordenadas y se imprime al finalizar.

El ejemplo indexa **rutas de archivos**, no su contenido. Los archivos de
muestra están en [`ArbolesB/archivos_mock/`](ArbolesB/archivos_mock/).

### Inserción

La implementación usa un árbol B con grado mínimo 2:

1. Si la raíz está llena, se crea una nueva raíz y se divide la anterior.
2. Se desciende hacia el hijo que corresponde a la clave.
3. Antes de entrar a un hijo lleno, se lo divide y se promueve su clave
   central al padre.
4. Al llegar a una hoja no llena, se inserta la ruta en orden.
5. Si la ruta ya existe, no se vuelve a insertar.

### Archivos

- [`ArbolesB/ArbolB.cc`](ArbolesB/ArbolB.cc): estructura del árbol B, inserción,
  búsqueda interna, impresión y ejemplo ejecutable.
- [`ArbolesB/IndexarCarpeta.h`](ArbolesB/IndexarCarpeta.h): Algoritmo que recorre
  una carpeta e inserta en el árbol las rutas de sus archivos regulares.

La llamada del ejemplo usa `archivos_mock` como ruta relativa. Por eso, hay
que ejecutar el programa desde la carpeta `ArbolesB`.

## Montículo máximo y Heap Sort

Como caso de uso, un planificador puede priorizar tareas según un valor de prioridad. En un
**max-heap**, cada padre es mayor o igual que sus hijos, así que la prioridad
más alta está en la raíz. En un **min-heap**, cada padre es menor o igual que
sus hijos y la raíz contiene el valor mínimo.

El ejemplo de [`Binary Heap/heap.cc`](Binary%20Heap/heap.cc) implementa
Heap Sort usando un max-heap:

1. Construye el max-heap a partir del arreglo.
2. Intercambia la raíz con el último elemento de la zona activa.
3. Reduce el tamaño del heap y restaura la propiedad de max-heap.
4. Repite hasta que el arreglo queda ordenado ascendentemente.

La barra vertical de la salida separa la zona del heap de los valores ya
ordenados. Heap Sort tiene tiempo `O(n log n)` y usa espacio adicional
`O(1)` en esta implementación.

### Consulta a la IA: eficiencia de Heap Sort

**Pregunta:** ¿Cómo se compara Heap Sort con Quick Sort, Merge Sort e
Insertion Sort en tiempo, memoria y estabilidad? ¿En qué casos conviene usar
cada uno?

**Respuesta de la IA:** Heap Sort garantiza `O(n log n)` tanto en el caso
promedio como en el peor caso y ordena en el mismo arreglo, usando `O(1)` de
memoria auxiliar. Quick Sort suele ser rápido en la práctica y tiene un coste
promedio de `O(n log n)`, pero su peor caso es `O(n²)`. Merge Sort garantiza
`O(n log n)` y es estable, aunque normalmente necesita `O(n)` memoria
adicional para arreglos. Insertion Sort puede llegar a `O(n²)`, pero es
sencillo y funciona bien con conjuntos pequeños o casi ordenados. Heap Sort
no es estable y, pese a su garantía temporal y poco uso de memoria, puede ser
más lento en la práctica por el patrón de accesos al arreglo.

**Análisis crítico:** La comparación coincide con los conceptos de
complejidad temporal y espacial estudiados: no alcanza con comparar solo el
caso promedio; también importa la garantía del peor caso y la memoria
auxiliar. Heap Sort resulta conveniente cuando se necesita un límite de
`O(n log n)` y memoria adicional constante. Si la estabilidad es necesaria,
Merge Sort es más adecuado; para datos pequeños o casi ordenados, Insertion
Sort puede ser una opción simple. Quick Sort puede rendir muy bien en
promedio, pero su peor caso debe tenerse en cuenta.

La respuesta de la IA no implica que Heap Sort sea siempre el más rápido,
las complejidades describen el crecimiento del trabajo, no los tiempos
concretos en una máquina. El ejemplo de [`Binary Heap/heap.cc`](Binary%20Heap/heap.cc)
ilustra la construcción del max-heap y las extracciones sucesivas, pero no
incluye una medición comparativa. Para afirmar cuál tarda menos en un caso
real habría que ejecutar pruebas con los mismos datos, compilador y entorno.

## Requisitos y compilación

Se requiere un compilador de C++17 o posterior.

Ejecutar desde la carpeta raíz del proyecto.
