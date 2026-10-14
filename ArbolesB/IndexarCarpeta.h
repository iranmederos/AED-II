#pragma once

#include <filesystem>
#include <string>

template <typename Indice>
void indexarCarpeta(const std::filesystem::path &carpeta, Indice &indice)
{
    for (const auto &entrada : std::filesystem::directory_iterator(carpeta))
    {
        if (entrada.is_regular_file())
        {
            const std::string ruta = entrada.path().string();
            indice.insertar(ruta);
        }
    }
}
