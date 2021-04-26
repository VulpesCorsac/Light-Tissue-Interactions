// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "MC_TK/Photon.h"
#include "MC_TK/Medium.h"
#include "MC_TK/MonteCarlo.h"
#include "MC_TK/BugerLambert.h"

int main (int argc, char **argv) {
    using T = float;

    // Vector3D<T> startCoord, startDir;
    // startCoord = Vector3D<T>(0,0,0);
    // startDir = Vector3D<T>(0,0,1);

    // Photon<T> photon(startCoord, startDir, 1.0, 0.0);
    Medium<T> tissue(1.4, 500, 0, 1e-3, 0.99);
    MonteCarlo<T> mc(tissue, 1e5, 1, 1e-4, 1e-5, 0.1, 1e-4);
    mc.Normalize();

    std::cout << BugerLambert(tissue.getTau(), tissue.getN(), tissue.getN(), tissue.getN()) << std::endl;

    return 0;
}
