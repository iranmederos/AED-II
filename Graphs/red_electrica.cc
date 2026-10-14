#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>

#include "DFS.h"

using namespace std;

// ------------------------------------------------------------
// Modelo del grafo para una red eléctrica
// ------------------------------------------------------------

enum class TipoNodo
{
    Generacion,
    Subestacion,
    Transformador,
    Consumidor
};

struct Arista
{
    int destino;
    double peso;   // distancia o longitud del tendido
    int capacidad; // capacidad máxima de la línea
    bool activo;   // true si la línea está operativa

    Arista(int d, double p, int c, bool a = true)
        : destino(d), peso(p), capacidad(c), activo(a) {}
};

struct Nodo
{
    int id;
    TipoNodo tipo;
    string nombre;
    bool activo; // representa si el nodo está operativo

    Nodo(int i, TipoNodo t, const string &n, bool a = true)
        : id(i), tipo(t), nombre(n), activo(a) {}
};

class RedElectrica
{
private:
    unordered_map<int, Nodo> nodos;
    unordered_map<int, vector<Arista>> adyacentes;

public:
    // --------------------------------------------------------
    // Agregar nodos de la red
    // --------------------------------------------------------
    void agregarNodo(int id, TipoNodo tipo, const string &nombre, bool activo = true)
    {
        auto it = nodos.find(id);
        if (it == nodos.end())
        {
            nodos.emplace(id, Nodo(id, tipo, nombre, activo));
        }
        else
        {
            it->second.tipo = tipo;
            it->second.nombre = nombre;
            it->second.activo = activo;
        }
        if (adyacentes.find(id) == adyacentes.end())
        {
            adyacentes[id] = {};
        }
    }

    // --------------------------------------------------------
    // Agregar una línea eléctrica entre dos nodos
    // --------------------------------------------------------
    void agregarLinea(int origen, int destino, double peso, int capacidad, bool activo = true)
    {
        if (nodos.find(origen) == nodos.end() || nodos.find(destino) == nodos.end())
        {
            cout << "Error: se intento conectar nodos que no existen.\n";
            return;
        }

        adyacentes[origen].push_back(Arista(destino, peso, capacidad, activo));
        adyacentes[destino].push_back(Arista(origen, peso, capacidad, activo));
    }

    // --------------------------------------------------------
    // Eliminar una línea por falla o corte
    // --------------------------------------------------------
    void eliminarLinea(int origen, int destino)
    {
        auto itOrigen = adyacentes.find(origen);
        auto itDestino = adyacentes.find(destino);

        if (itOrigen == adyacentes.end() || itDestino == adyacentes.end())
        {
            return;
        }

        // Remover la conexión en ambas direcciones
        for (auto it = itOrigen->second.begin(); it != itOrigen->second.end();)
        {
            if (it->destino == destino)
            {
                it = itOrigen->second.erase(it);
            }
            else
            {
                ++it;
            }
        }

        for (auto it = itDestino->second.begin(); it != itDestino->second.end();)
        {
            if (it->destino == origen)
            {
                it = itDestino->second.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // --------------------------------------------------------
    // Modelar falla de un nodo
    // --------------------------------------------------------
    void desactivarNodo(int id)
    {
        if (nodos.find(id) != nodos.end())
        {
            nodos.at(id).activo = false;
        }
    }

    // --------------------------------------------------------
    // Mostrar la estructura de la red
    // --------------------------------------------------------
    void mostrarRed() const
    {
        cout << "Nodos de la red:\n";
        for (const auto &par : nodos)
        {
            const Nodo &n = par.second;
            cout << "  Nodo " << n.id << " | " << n.nombre << " | "
                 << "Tipo: " << static_cast<int>(n.tipo) << " | "
                 << (n.activo ? "Activo" : "Inactivo") << "\n";
        }

        cout << "\nConexiones:\n";
        for (const auto &par : adyacentes)
        {
            int origen = par.first;
            cout << "  " << origen << " -> ";
            for (const auto &arista : par.second)
            {
                cout << "(" << arista.destino << ", peso=" << arista.peso
                     << ", cap=" << arista.capacidad
                     << ", activo=" << (arista.activo ? "true" : "false")
                     << ")  ";
            }
            cout << endl;
        }
    }

    // --------------------------------------------------------
    // Detectar componentes o sectores desconectados
    // --------------------------------------------------------
    void detectarSectoresAislados() const
    {
        unordered_map<int, bool> visitado;
        vector<set<int>> sectores;

        for (const auto &par : nodos)
        {
            int id = par.first;
            visitado[id] = false;
        }

        for (const auto &par : nodos)
        {
            int id = par.first;
            if (!visitado[id])
            {
                set<int> sectorActual;
                dfsSectorSinEnergia(id, adyacentes, visitado, sectorActual);
                sectores.push_back(sectorActual);
            }
        }

        imprimirGrafoPorSectores(sectores);
    }

    bool tieneGeneracion(const set<int> &sector) const
    {
        for (int id : sector)
        {
            if (nodos.at(id).tipo == TipoNodo::Generacion)
                return true;
        }
        return false;
    }

    void imprimirGrafoPorSectores(const vector<set<int>> &sectores) const
    {
        cout << "Grafo por sectores:\n";

        for (const auto &sector : sectores)
        {
            cout << (tieneGeneracion(sector) ? "\n[CON ENERGIA]\n" : "\n[SIN ENERGIA]\n");
            for (int id : sector)
            {
                cout << "  [" << id << "] " << nodos.at(id).nombre << "\n";
                for (const auto &arista : adyacentes.at(id))
                {
                    if (arista.activo && sector.count(arista.destino) && id < arista.destino)
                    {
                        cout << "      +---- [" << arista.destino << "] "
                             << nodos.at(arista.destino).nombre << "\n";
                    }
                }
            }
        }
    }

    const unordered_map<int, vector<Arista>> &obtenerAdyacencia() const
    {
        return adyacentes;
    }

    const unordered_map<int, Nodo> &obtenerNodos() const
    {
        return nodos;
    }
};

// ------------------------------------------------------------
// Ejemplo de uso para el trabajo de grafos
// ------------------------------------------------------------
int main()
{
    RedElectrica red;

    // Nodos principales
    red.agregarNodo(1, TipoNodo::Generacion, "Central Termica");
    red.agregarNodo(2, TipoNodo::Subestacion, "Subestacion Norte");
    red.agregarNodo(3, TipoNodo::Subestacion, "Subestacion Sur");
    red.agregarNodo(4, TipoNodo::Transformador, "Transformador A");
    red.agregarNodo(5, TipoNodo::Transformador, "Transformador B");
    red.agregarNodo(6, TipoNodo::Consumidor, "Barrio Centro");
    red.agregarNodo(7, TipoNodo::Consumidor, "Barrio Este");
    red.agregarNodo(8, TipoNodo::Consumidor, "Barrio Oeste");

    // Líneas de transmisión
    red.agregarLinea(1, 2, 12.5, 200, true);
    red.agregarLinea(1, 3, 15.0, 180, true);
    red.agregarLinea(2, 4, 8.0, 120, true);
    red.agregarLinea(3, 5, 9.2, 130, true);
    red.agregarLinea(4, 6, 3.5, 80, true);
    red.agregarLinea(5, 7, 4.0, 90, true);
    red.agregarLinea(5, 8, 5.1, 85, true);

    cout << "=== Red electrica inicial ===\n";
    red.mostrarRed();

    // Simulacion de falla en una linea
    cout << "\n=== Simulacion: corte de linea entre 2 y 4 ===\n";
    red.eliminarLinea(2, 4);
    red.mostrarRed();

    red.detectarSectoresAislados();

    return 0;
}
