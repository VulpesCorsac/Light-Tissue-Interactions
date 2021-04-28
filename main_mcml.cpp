// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "MCML/Tissue.h"
#include "MCML/DataContainer.h"
#include "MCML/ThreadManagement.h"

#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>

int main() {
	using T = double;

    //test script with Buger arption
    T mean_free_pathlength = 0.001;
    T attenuation = 0.9; // albedo
    T g = 0.9;
    T width = 0.01;
    T n = 1.5;

    Tissue<T> tissue(width, n, mean_free_pathlength, g, attenuation);
    T z0 = 0;
    T z1 = 0.002;
    T r1 = 0.001;
    int nR = 500;
    int nZ = 500;

    DataContainer<T> data(nR, nZ, z0, z1, r1);

    T num = 1000;
    T treshold = 0.01;
    T chance = 0.1; //check to be greater than threshold

    constexpr bool debug = false;
    constexpr int numder_of_threads = 1;

    set_up_threads(numder_of_threads, tissue, data, ThreadParams<T>(num, chance, treshold, debug, 0), true);


    double transited = std::accumulate(data.get_transit().begin(), data.get_transit().end(), 0);
    double reflected = std::accumulate(data.get_reflect().begin(), data.get_reflect().end(), 0);
    double absorbed = 0;

    for (int i = 0; i < data.NZ; i++)
        absorbed += std::accumulate(data.get_medium()[i].begin(), data.get_medium()[i].end(), 0);

    std::cout << std::endl << "------------------------------------------------" << std::endl;
    std::cout << "Coefficient of transition: " << sqrt(transited/num) << std::endl;
    std::cout << "Coefficient of reflectance: " << reflected/num << std::endl;
    std::cout << "Coefficient of adsorption: " << absorbed/num << std::endl;
    std::cout << "Buger lambert says: " << exp(-attenuation*width) << std::endl;

    return 0;
}
