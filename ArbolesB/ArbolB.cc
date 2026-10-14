#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "IndexarCarpeta.h"

class ArbolB
{
    static constexpr int gradoMinimo = 2;

    struct Nodo
    {
        bool esHoja;
        std::vector<std::string> claves;
        std::vector<std::unique_ptr<Nodo>> hijos;

        explicit Nodo(bool hoja) : esHoja(hoja) {}
    };

    std::unique_ptr<Nodo> raiz = std::make_unique<Nodo>(true);

    void dividirHijo(Nodo &padre, std::size_t indice)
    {
        auto hijo = std::move(padre.hijos[indice]);
        auto nuevoHijo = std::make_unique<Nodo>(hijo->esHoja);
        std::string claveMedia = std::move(hijo->claves[gradoMinimo - 1]);

        nuevoHijo->claves.assign(
            std::make_move_iterator(hijo->claves.begin() + gradoMinimo),
            std::make_move_iterator(hijo->claves.end()));
        hijo->claves.resize(gradoMinimo - 1);

        if (!hijo->esHoja)
        {
            nuevoHijo->hijos.insert(
                nuevoHijo->hijos.end(),
                std::make_move_iterator(hijo->hijos.begin() + gradoMinimo),
                std::make_move_iterator(hijo->hijos.end()));
            hijo->hijos.resize(gradoMinimo);
        }

        padre.claves.insert(padre.claves.begin() + indice, std::move(claveMedia));
        padre.hijos[indice] = std::move(hijo);
        padre.hijos.insert(padre.hijos.begin() + indice + 1, std::move(nuevoHijo));
    }

    void insertarEnNoLleno(Nodo &nodo, const std::string &clave)
    {
        auto posicion = std::lower_bound(nodo.claves.begin(), nodo.claves.end(), clave);

        if (nodo.esHoja)
        {
            nodo.claves.insert(posicion, clave);
            return;
        }

        std::size_t indice = static_cast<std::size_t>(posicion - nodo.claves.begin());
        if (nodo.hijos[indice]->claves.size() == 2 * gradoMinimo - 1)
        {
            dividirHijo(nodo, indice);
            if (clave > nodo.claves[indice])
                ++indice;
        }
        insertarEnNoLleno(*nodo.hijos[indice], clave);
    }

    bool contiene(const Nodo &nodo, const std::string &clave) const
    {
        auto posicion = std::lower_bound(nodo.claves.begin(), nodo.claves.end(), clave);
        if (posicion != nodo.claves.end() && *posicion == clave)
            return true;
        if (nodo.esHoja)
            return false;

        return contiene(*nodo.hijos[static_cast<std::size_t>(posicion - nodo.claves.begin())], clave);
    }

    void imprimirNodo(const Nodo &nodo, const std::string &prefijo,
                      bool ultimo, const std::string &etiqueta) const
    {
        std::cout << prefijo;
        if (!etiqueta.empty())
            std::cout << (ultimo ? "`-- " : "|-- ") << etiqueta;
        std::cout << "[";
        for (std::size_t i = 0; i < nodo.claves.size(); ++i)
        {
            if (i > 0)
                std::cout << " | ";
            std::cout << std::filesystem::path(nodo.claves[i]).filename().string();
        }
        std::cout << "]\n";

        for (std::size_t i = 0; i < nodo.hijos.size(); ++i)
        {
            const bool ultimoHijo = i + 1 == nodo.hijos.size();
            const std::string nuevoPrefijo = prefijo +
                                             (etiqueta.empty() ? "" : (ultimo ? "    " : "|   "));
            imprimirNodo(*nodo.hijos[i], nuevoPrefijo, ultimoHijo,
                         "hijo " + std::to_string(i) + ": ");
        }
    }

public:
    void insertar(const std::string &ruta)
    {
        if (contiene(*raiz, ruta))
            return;

        if (raiz->claves.size() == 2 * gradoMinimo - 1)
        {
            auto nuevaRaiz = std::make_unique<Nodo>(false);
            nuevaRaiz->hijos.push_back(std::move(raiz));
            dividirHijo(*nuevaRaiz, 0);
            raiz = std::move(nuevaRaiz);
        }

        insertarEnNoLleno(*raiz, ruta);
    }

    bool contiene(const std::string &ruta) const
    {
        return contiene(*raiz, ruta);
    }

    void imprimir() const
    {
        imprimirNodo(*raiz, "", true, "");
    }
};

int main()
{

    ArbolB indice;

    indexarCarpeta("archivos_mock", indice);

    std::cout << "Arbol B resultante (cada clave es un archivo indexado):\n";
    std::cout << "Raiz\n";
    indice.imprimir();
}
