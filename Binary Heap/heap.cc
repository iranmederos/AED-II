#include <cassert>
#include <iostream>
#include <vector>

void imprimirEstado(const std::vector<int> &datos, int tamanoHeap)
{
    std::cout << "[ ";
    for (int i = 0; i < static_cast<int>(datos.size()); ++i)
    {
        if (tamanoHeap > 0 && tamanoHeap < static_cast<int>(datos.size()) && i == tamanoHeap)
            std::cout << "| ";
        std::cout << datos[i] << " ";
    }
    std::cout << "]\n";
}

void bajarEnHeap(std::vector<int> &datos, int raiz, int tamanoHeap)
{
    int mayor = raiz;
    const int izquierdo = 2 * raiz + 1;
    const int derecho = 2 * raiz + 2;

    if (izquierdo < tamanoHeap && datos[izquierdo] > datos[mayor])
        mayor = izquierdo;
    if (derecho < tamanoHeap && datos[derecho] > datos[mayor])
        mayor = derecho;

    if (mayor != raiz)
    {
        std::swap(datos[raiz], datos[mayor]);
        bajarEnHeap(datos, mayor, tamanoHeap);
    }
}

void heapSort(std::vector<int> &datos)
{
    const int cantidad = static_cast<int>(datos.size());

    std::cout << "Datos iniciales: ";
    imprimirEstado(datos, cantidad);

    for (int i = cantidad / 2 - 1; i >= 0; --i)
        bajarEnHeap(datos, i, cantidad);

    std::cout << "Max-heap construido: ";
    imprimirEstado(datos, cantidad);

    for (int fin = cantidad - 1; fin > 0; --fin)
    {
        std::swap(datos[0], datos[fin]);
        bajarEnHeap(datos, 0, fin);
        std::cout << "Luego de extraer el maximo: ";
        imprimirEstado(datos, fin);
    }
}

int main()
{
    std::vector<int> datos = {4, 10, 3, 5, 1};

    heapSort(datos);

    assert((datos == std::vector<int>{1, 3, 4, 5, 10}));
    std::cout << "Resultado ordenado: ";
    imprimirEstado(datos, 0);
}