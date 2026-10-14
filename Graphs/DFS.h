#pragma once

#include <set>
#include <unordered_map>

template <typename Adyacencia>
void dfsSectorSinEnergia(int nodoActual,
                         const Adyacencia &adyacentes,
                         std::unordered_map<int, bool> &visitado,
                         std::set<int> &sectorActual)
{
    visitado[nodoActual] = true;
    sectorActual.insert(nodoActual);

    for (const auto &arista : adyacentes.at(nodoActual))
    {
        if (arista.activo && !visitado[arista.destino])
            dfsSectorSinEnergia(arista.destino, adyacentes, visitado, sectorActual);
    }
}
