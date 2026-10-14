#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum class TipoNodo
{
    Generacion,
    Subestacion,
    Transformador,
    Consumidor
};

struct Arista
{
    size_t destino;
    double peso;
    int capacidad;
    bool activo;
};

struct Nodo
{
    int id;
    TipoNodo tipo;
    string nombre;
    bool activo;
};

class RedElectrica
{
    // Optimización: nodos y adyacencias usan índices densos en vectores.
    // El mapa solo traduce IDs externos a índices al agregar/buscar nodos.
    unordered_map<int, size_t> indicePorId;
    vector<Nodo> nodos;
    vector<vector<Arista>> adyacentes;

    struct Sector
    {
        vector<size_t> nodos;
        bool tieneGeneracion = false;
    };

public:
    void agregarNodo(int id, TipoNodo tipo, const string &nombre, bool activo = true)
    {
        const auto encontrado = indicePorId.find(id);
        if (encontrado != indicePorId.end())
        {
            Nodo &nodo = nodos[encontrado->second];
            nodo.tipo = tipo;
            nodo.nombre = nombre;
            nodo.activo = activo;
            return;
        }

        const size_t indice = nodos.size();
        indicePorId.emplace(id, indice);
        nodos.push_back({id, tipo, nombre, activo});
        adyacentes.emplace_back();
    }

    void agregarLinea(int origen, int destino, double peso, int capacidad, bool activo = true)
    {
        const auto origenIt = indicePorId.find(origen);
        const auto destinoIt = indicePorId.find(destino);
        if (origenIt == indicePorId.end() || destinoIt == indicePorId.end())
        {
            cerr << "Error: se intento conectar nodos que no existen.\n";
            return;
        }

        const size_t a = origenIt->second;
        const size_t b = destinoIt->second;
        adyacentes[a].push_back({b, peso, capacidad, activo});
        adyacentes[b].push_back({a, peso, capacidad, activo});
    }

    void eliminarLinea(int origen, int destino)
    {
        const auto origenIt = indicePorId.find(origen);
        const auto destinoIt = indicePorId.find(destino);
        if (origenIt == indicePorId.end() || destinoIt == indicePorId.end())
            return;

        const size_t a = origenIt->second;
        const size_t b = destinoIt->second;
        // Optimización: erase-remove recorre cada lista una sola vez.
        const auto quitarDestino = [](vector<Arista> &lista, size_t destinoIndice)
        {
            lista.erase(remove_if(lista.begin(), lista.end(),
                                  [destinoIndice](const Arista &arista)
                                  {
                                      return arista.destino == destinoIndice;
                                  }),
                        lista.end());
        };
        quitarDestino(adyacentes[a], b);
        quitarDestino(adyacentes[b], a);
    }

    void desactivarNodo(int id)
    {
        const auto it = indicePorId.find(id);
        if (it != indicePorId.end())
            nodos[it->second].activo = false;
    }

    void mostrarRed() const
    {
        cout << "Nodos de la red:\n";
        for (const Nodo &nodo : nodos)
        {
            cout << "  Nodo " << nodo.id << " | " << nodo.nombre
                 << " | Tipo: " << static_cast<int>(nodo.tipo) << " | "
                 << (nodo.activo ? "Activo" : "Inactivo") << "\n";
        }

        cout << "\nConexiones:\n";
        for (size_t i = 0; i < adyacentes.size(); ++i)
        {
            cout << "  " << nodos[i].id << " -> ";
            for (const Arista &arista : adyacentes[i])
            {
                cout << "(" << nodos[arista.destino].id << ", peso=" << arista.peso
                     << ", cap=" << arista.capacidad
                     << ", activo=" << (arista.activo ? "true" : "false") << ")  ";
            }
            cout << '\n';
        }
    }

    void detectarSectoresAislados() const
    {
        vector<Sector> sectores;
        // Optimización: arreglo contiguo de visitados reemplaza el hash por nodo.
        vector<unsigned char> visitado(nodos.size(), 0);
        vector<size_t> sectorDeNodo(nodos.size());
        vector<size_t> pendientes;
        pendientes.reserve(nodos.size());

        for (size_t inicio = 0; inicio < nodos.size(); ++inicio)
        {
            if (visitado[inicio])
                continue;

            const size_t idSector = sectores.size();
            sectores.emplace_back();
            Sector &sector = sectores.back();
            pendientes.push_back(inicio);
            visitado[inicio] = 1;
            sectorDeNodo[inicio] = idSector;

            // Optimización: DFS iterativo evita desbordar la pila en redes profundas.
            while (!pendientes.empty())
            {
                const size_t actual = pendientes.back();
                pendientes.pop_back();
                sector.nodos.push_back(actual);
                sector.tieneGeneracion |= nodos[actual].tipo == TipoNodo::Generacion;

                for (const Arista &arista : adyacentes[actual])
                {
                    if (arista.activo && !visitado[arista.destino])
                    {
                        visitado[arista.destino] = 1;
                        sectorDeNodo[arista.destino] = idSector;
                        pendientes.push_back(arista.destino);
                    }
                }
            }
        }

        imprimirGrafoPorSectores(sectores, sectorDeNodo);
    }

private:
    void imprimirGrafoPorSectores(const vector<Sector> &sectores,
                                  const vector<size_t> &sectorDeNodo) const
    {
        cout << "Grafo por sectores:\n";
        for (size_t s = 0; s < sectores.size(); ++s)
        {
            const Sector &sector = sectores[s];
            cout << (sector.tieneGeneracion ? "\n[CON ENERGIA]\n" : "\n[SIN ENERGIA]\n");

            for (size_t indice : sector.nodos)
            {
                cout << "  [" << nodos[indice].id << "] " << nodos[indice].nombre << "\n";
                for (const Arista &arista : adyacentes[indice])
                {
                    // Optimización: componente precomputado evita sector.count() en set.
                    if (arista.activo && sectorDeNodo[arista.destino] == s &&
                        indice < arista.destino)
                    {
                        cout << "      +---- [" << nodos[arista.destino].id << "] "
                             << nodos[arista.destino].nombre << "\n";
                    }
                }
            }
        }
    }
};

int main()
{
    RedElectrica red;

    red.agregarNodo(1, TipoNodo::Generacion, "Central Termica");
    red.agregarNodo(2, TipoNodo::Subestacion, "Subestacion Norte");
    red.agregarNodo(3, TipoNodo::Subestacion, "Subestacion Sur");
    red.agregarNodo(4, TipoNodo::Transformador, "Transformador A");
    red.agregarNodo(5, TipoNodo::Transformador, "Transformador B");
    red.agregarNodo(6, TipoNodo::Consumidor, "Barrio Centro");
    red.agregarNodo(7, TipoNodo::Consumidor, "Barrio Este");
    red.agregarNodo(8, TipoNodo::Consumidor, "Barrio Oeste");

    red.agregarLinea(1, 2, 12.5, 200);
    red.agregarLinea(1, 3, 15.0, 180);
    red.agregarLinea(2, 4, 8.0, 120);
    red.agregarLinea(3, 5, 9.2, 130);
    red.agregarLinea(4, 6, 3.5, 80);
    red.agregarLinea(5, 7, 4.0, 90);
    red.agregarLinea(5, 8, 5.1, 85);

    cout << "=== Red electrica inicial ===\n";
    red.mostrarRed();
    cout << "\n=== Simulacion: corte de linea entre 2 y 4 ===\n";
    red.eliminarLinea(2, 4);
    red.mostrarRed();
    red.detectarSectoresAislados();
}
