# Análisis comparativo de rendimiento

## Objetivo

Se comparan el código original, `red_electrica.cc`, y la versión optimizada,
`red_electrica_optimizado.cc`. Ambos representan una red eléctrica como un
grafo disperso y detectan sus componentes conectados mediante DFS.

## Optimizaciones aplicadas

| Parte | Código original | Versión optimizada | Beneficio |
|---|---|---|---|
| Almacenamiento de nodos y conexiones | `unordered_map` para nodos y listas de adyacencia | `vector` para nodos y adyacencias; un `unordered_map` traduce los ID externos a índices | Mejora la localidad de memoria y evita búsquedas hash por nodo durante el DFS. |
| Nodos visitados | `unordered_map<int, bool>` | `vector<unsigned char>` | Consulta y marca nodos por índice, sin el coste del hash. |
| DFS | Recursivo | Iterativo, usando un `vector` como pila | Evita el riesgo de desbordar la pila de llamadas en grafos profundos. |
| Sectores | `set<int>` por componente | `vector<size_t>` por componente | Agregar nodos tiene coste amortizado `O(1)` en vez de `O(log V)`; no hace falta ordenarlos para recorrerlos. |
| Conexiones al imprimir | `sector.count(destino)` en cada arista | Comparación de etiquetas mediante `sectorDeNodo[destino]` | Comprobar si dos nodos pertenecen al mismo sector cuesta `O(1)`, sin búsquedas en un `set`. |
| Detección de generación | Se revisa cada nodo del sector durante la impresión | Se registra `tieneGeneracion` durante el DFS | Evita recorrer los sectores una segunda vez para saber si contienen generación. |
| Eliminación de línea | Bucle manual para borrar conexiones en ambos sentidos | `remove_if` y `erase` en cada lista de adyacencia | Recorre cada lista afectada una vez; el coste sigue siendo lineal respecto al grado de los nodos. |

## Evaluación según los criterios

| Criterio | Código original | Versión optimizada |
|---|---|---|
| **Eficiencia temporal** | El DFS es `O(V + E)`. El uso de `unordered_map` para visitados y de `set` para sectores añade operaciones de hash y búsquedas/inserciones `O(log V)`. | El DFS es `O(V + E)`. Usa índices y vectores para visitar y agrupar nodos, y etiquetas de componente para evitar búsquedas en `set`.|
| **Eficiencia espacial** | Almacena nodos, adyacencias y visitados con mapas hash, además de un `set` por sector. La complejidad espacial es `O(V + E)`, con sobrecoste por las estructuras asociativas. | Almacena nodos, adyacencias, visitados y sectores en vectores, más un mapa ID→índice. La complejidad espacial también es `O(V + E)`; los vectores suelen tener menor sobrecoste y mejor localidad de memoria. |
| **Claridad** | Más directo para aprender: las estructuras se indexan por ID y el DFS es recursivo. | Tiene más detalles de implementación —índices internos, pila explícita y etiquetas de componente—. Los comentarios explican las optimizaciones, pero el código puede ser menos inmediato. |
| **Adecuación** | La lista de adyacencia es apropiada para una red dispersa; el DFS encuentra componentes conectados usando líneas activas. | Conserva la lista de adyacencia y el DFS sobre líneas activas. Es apropiado para redes dispersas y mejora el coste adicional de las estructuras. |

## Complejidad y alcance

La detección de componentes conectados es `O(V + E)` en ambos códigos. La
versión optimizada no cambia esa complejidad asintótica, reduce el sobrecoste
de las operaciones auxiliares y puede mejorar el comportamiento en redes
grandes.

## Conclusión

Ambas versiones cumplen para detectar componentes separados por cortes de
líneas. El código original es más sencillo de explicar; el optimizado reduce
costes auxiliares y es más adecuado a escala. Para representar también fallas
de nodos, hay que corregir la limitación funcional indicada.
