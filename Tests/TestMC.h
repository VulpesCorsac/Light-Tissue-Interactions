#pragma once

#include "../MC_TK/MonteCarlo.h"
#include "../MC_TK/BugerLambert.h"
#include "../MC_TK/MCmultithread.h"

using namespace std;

constexpr float TOTAL_TOLERANCE = 1e-4;

TEST(MC, SingleLayerAbsorptionOnly) {
    using T = double;

    cout << "Single layer, absorption only" << endl;

    constexpr size_t Nz = 1000;
    constexpr size_t Nr = 10000;
    constexpr T selectedRadius = 10e-2;

    const Medium<T> tissue1(1.5, 1000, 0,  1e-3, 0.9); // MFP == D
    const Medium<T> tissue2(1.3,  100, 0,  1e-3, 0.5); // MFP == 10*D
    const Medium<T> tissue3(1.6, 1000, 0, 10e-3, 0.0); // MFP == 0.1*D, it will need more photons

    const vector<Medium<T>> layer1 = {tissue1};
    const vector<Medium<T>> layer2 = {tissue2};
    const vector<Medium<T>> layer3 = {tissue3};

    const Sample<T> sample1(layer1, 1.0, 1.0);
    const Sample<T> sample2(layer2, 1.0, 1.0);
    const Sample<T> sample3(layer3, 1.0, 1.0);

    MonteCarlo<T, Nz, Nr> mc1(sample1, int(1e6), sample1.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc2(sample2, int(1e6), sample2.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc3(sample3, int(5e6), sample3.getTotalThickness(), selectedRadius);

    MCresults<T, Nz, Nr> res1, res2, res3;
    MCresults<T, Nz, Nr> MTres1, MTres2, MTres3;

    mc1.Calculate(res1);
	EXPECT_NEAR(res1.specularReflection, 0.04, 1e-4);
    EXPECT_NEAR(res1.diffuseReflection, 0.004989, 1e-3);
    EXPECT_NEAR(res1.diffuseTransmission, 0.3390, 2e-3);
    EXPECT_NEAR(res1.diffuseTransmission, BugerLambert(1.0, 1.5, 1.0, 1.0), 2e-3);
    EXPECT_NEAR(res1.diffuseTransmission + res1.diffuseReflection + res1.specularReflection + res1.absorbed, 1.0,  TOTAL_TOLERANCE);
    cout << "Test 1 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample1, int(1e6), 4, sample1.getTotalThickness(), selectedRadius, MTres1);
    EXPECT_NEAR(MTres1.specularReflection, 0.04, 1e-4);
    EXPECT_NEAR(MTres1.diffuseReflection, 0.004989, 1e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission, 0.3390, 2e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission, BugerLambert(1.0, 1.5, 1.0, 1.0), 2e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission + MTres1.diffuseReflection + MTres1.specularReflection + MTres1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: 4 threads" << endl;

    mc2.Calculate(res2);
    EXPECT_NEAR(res2.specularReflection, 0.0170132, 1e-4);
    EXPECT_NEAR(res2.diffuseReflection, 0.01346, 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission, 0.8743, 2e-3);
    EXPECT_NEAR(res2.diffuseTransmission, BugerLambert(0.1, 1.3, 1.0, 1.0), 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission + res2.diffuseReflection + res2.specularReflection + res2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample2, int(1e6), 4, sample2.getTotalThickness(), selectedRadius, MTres2);
    EXPECT_NEAR(MTres2.specularReflection, 0.0170132, 1e-4);
    EXPECT_NEAR(MTres2.diffuseReflection, 0.01346, 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission, 0.8743, 2e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission, BugerLambert(0.1, 1.3, 1.0, 1.0), 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission + MTres2.diffuseReflection + MTres2.specularReflection + MTres2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: 4 threads" << endl;

    mc3.Calculate(res3);
    EXPECT_NEAR(res3.specularReflection, 0.05325, 1e-4);
    EXPECT_NEAR(res3.diffuseReflection, 0.0, 1e-3);
    EXPECT_NEAR(res3.diffuseTransmission, 0.00004069, 8e-6);
    EXPECT_NEAR(res3.diffuseTransmission, BugerLambert(10.0, 1.6, 1.0, 1.0), 8e-6);
    EXPECT_NEAR(res3.diffuseTransmission + res3.diffuseReflection + res3.specularReflection + res3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample3, int(2e7), 4, sample3.getTotalThickness(), selectedRadius, MTres3);
    EXPECT_NEAR(MTres3.specularReflection, 0.05325, 1e-4);
    EXPECT_NEAR(MTres3.diffuseReflection, 0.0, 1e-3);
    EXPECT_NEAR(MTres3.diffuseTransmission, 0.00004069, 5e-6);
    EXPECT_NEAR(MTres3.diffuseTransmission, BugerLambert(10.0, 1.6, 1.0, 1.0), 4e-6);
    EXPECT_NEAR(MTres3.diffuseTransmission + MTres3.diffuseReflection + MTres3.specularReflection + MTres3.absorbed, 1.0, 1e-4);
    cout << "Test 3 OK: 4 threads" << endl;

    cout << "All tests passed successfully!" << endl;
}

TEST(MC, MultiLayerAbsorptionOnly) {
    using T = double;

    cout << "Glass-tissue-glass, absorption only" << endl;

    constexpr size_t Nz = 1000;
    constexpr size_t Nr = 10000;
    constexpr T selectedRadius = 10e-2;

    const Medium<T> tissue1(1.5, 1000, 0,  1e-3, 0.9); // MFP == D
    const Medium<T> tissue2(1.3,  100, 0,  1e-3, 0.5); // MFP == 10*D
    const Medium<T> tissue3(1.6, 1000, 0, 10e-3, 0.0); // MFP == 0.1*D, it will need more photons

    const Medium<T> glass1 (1.6 , 0.0, 0.0, 1e-3, 0.0);
    const Medium<T> glass2 (1.4 , 0.0, 0.0, 1e-3, 0.0);
    const Medium<T> glass3 (1.65, 0.0, 0.0, 1e-3, 0.0);

    const vector<Medium<T>> layer1 = {glass1, tissue1, glass1};
    const vector<Medium<T>> layer2 = {glass2, tissue2, glass2};
    const vector<Medium<T>> layer3 = {glass3, tissue3, glass3};

    const Sample<T> sample1(layer1, 1.0, 1.0);
    const Sample<T> sample2(layer2, 1.0, 1.0);
    const Sample<T> sample3(layer3, 1.0, 1.0);

    MonteCarlo<T, Nz, Nr> mc1(sample1, int(1e6), sample1.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc2(sample2, int(1e6), sample2.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc3(sample3, int(5e6), sample3.getTotalThickness(), selectedRadius);

    MCresults<T, Nz, Nr> res1, res2, res3;
    MCresults<T, Nz, Nr> MTres1, MTres2, MTres3;

    mc1.Calculate(res1);
    EXPECT_NEAR(res1.specularReflection, 0.05419, 1e-4);
    EXPECT_NEAR(res1.diffuseReflection, 0.00656, 1e-3);
    EXPECT_NEAR(res1.diffuseTransmission, 0.3292, 2e-3);
    EXPECT_NEAR(res1.diffuseTransmission, BugerLambert(1.0, 1.5, 1.6, 1.6), 2e-3);
    EXPECT_NEAR(res1.diffuseTransmission + res1.diffuseReflection + res1.specularReflection + res1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample1, int(1e6), 4, sample1.getTotalThickness(), selectedRadius, MTres1);
    EXPECT_NEAR(MTres1.specularReflection, 0.05419, 1e-4);
    EXPECT_NEAR(MTres1.diffuseReflection, 0.00656, 1e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission, 0.3292, 2e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission, BugerLambert(1.0, 1.5, 1.6, 1.6), 2e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission + MTres1.diffuseReflection + MTres1.specularReflection + MTres1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: 4 threads" << endl;

    mc2.Calculate(res2);
    EXPECT_NEAR(res2.specularReflection, 0.02907, 1e-4);
    EXPECT_NEAR(res2.diffuseReflection, 0.02244, 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission, 0.853, 2e-3);
    EXPECT_NEAR(res2.diffuseTransmission, BugerLambert(0.1, 1.3, 1.4, 1.4), 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission + res2.diffuseReflection + res2.specularReflection + res2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample2, int(1e6), 4, sample2.getTotalThickness(), selectedRadius, MTres2);
    EXPECT_NEAR(MTres2.specularReflection, 0.02907, 1e-4);
    EXPECT_NEAR(MTres2.diffuseReflection, 0.02244, 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission, 0.853, 2e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission, BugerLambert(0.1, 1.3, 1.4, 1.4), 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission + MTres2.diffuseReflection + MTres2.specularReflection + MTres2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: 4 threads" << endl;

    mc3.Calculate(res3);
    EXPECT_NEAR(res3.specularReflection, 0.06037, 1e-4);
    EXPECT_NEAR(res3.diffuseReflection, 0.0, 3e-3);
    EXPECT_NEAR(res3.diffuseTransmission, 0.00004008, 8e-6);
    EXPECT_NEAR(res3.diffuseTransmission, BugerLambert(10.0, 1.6, 1.65, 1.65), 8e-6);
    EXPECT_NEAR(res3.diffuseTransmission + res3.diffuseReflection + res3.specularReflection + res3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample3, int(2e7), 4, sample3.getTotalThickness(), selectedRadius, MTres3);
    EXPECT_NEAR(MTres3.specularReflection, 0.06037, 1e-4);
    EXPECT_NEAR(MTres3.diffuseReflection, 0.0, 1e-3);
    EXPECT_NEAR(MTres3.diffuseTransmission, 0.00004008, 4e-6);
    EXPECT_NEAR(MTres3.diffuseTransmission, BugerLambert(10.0, 1.6, 1.65, 1.65), 4e-6);
    EXPECT_NEAR(MTres3.diffuseTransmission + MTres3.diffuseReflection + MTres3.specularReflection + MTres3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: 4 threads" << endl;

    cout << "All tests passed successfully!" << endl;
}

TEST(MC, SingleLayerAbsorptionScattering) {
    using T = double;

    cout << "Single layer, absorption & scattering" << endl;

    constexpr size_t Nz = 1000;
    constexpr size_t Nr = 10000;
    constexpr T selectedRadius = 10e-2;

    const Medium<T> tissue1(1.5, 100, 900,  1e-3, 0.9); // MFP == D
    const Medium<T> tissue2(1.3,  20,  80,  1e-3, 0.5); // MFP == 10*D
    const Medium<T> tissue3(1.6, 700, 300, 10e-3, 0.0); // MFP == 0.1*D, it will need more photons

    const vector<Medium<T>> layer1 = {tissue1};
    const vector<Medium<T>> layer2 = {tissue2};
    const vector<Medium<T>> layer3 = {tissue3};

    const Sample<T> sample1(layer1, 1.0, 1.0);
    const Sample<T> sample2(layer2, 1.0, 1.0);
    const Sample<T> sample3(layer3, 1.0, 1.0);

    MonteCarlo<T, Nz, Nr> mc1(sample1, int(5e5), sample1.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc2(sample2, int(5e5), sample2.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc3(sample3, int(1e6), sample3.getTotalThickness(), selectedRadius);

    MCresults<T, Nz, Nr> res1, res2, res3;
    MCresults<T, Nz, Nr> MTres1, MTres2, MTres3;

    mc1.Calculate(res1);
    EXPECT_NEAR(res1.specularReflection, 0.04, 1e-4);
    EXPECT_NEAR(res1.diffuseReflection, 0.0435, 1e-3);
    EXPECT_NEAR(res1.diffuseTransmission, 0.767, 2e-3);
    EXPECT_NEAR(res1.diffuseTransmission + res1.diffuseReflection + res1.specularReflection + res1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample1, int(5e5), 4, sample1.getTotalThickness(), selectedRadius, MTres1);
    EXPECT_NEAR(MTres1.specularReflection, 0.04, 1e-4);
    EXPECT_NEAR(MTres1.diffuseReflection, 0.0435, 1e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission, 0.767, 2e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission + MTres1.diffuseReflection + MTres1.specularReflection + MTres1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: 4 threads" << endl;

    mc2.Calculate(res2);
    EXPECT_NEAR(res2.specularReflection, 0.01701, 1e-4);
    EXPECT_NEAR(res2.diffuseReflection, 0.0272, 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission, 0.9206, 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission + res2.diffuseReflection + res2.specularReflection + res2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample2, int(5e5), 4, sample2.getTotalThickness(), selectedRadius, MTres2);
    EXPECT_NEAR(MTres2.specularReflection, 0.01701, 1e-4);
    EXPECT_NEAR(MTres2.diffuseReflection, 0.0272, 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission, 0.9206, 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission + MTres2.diffuseReflection + MTres2.specularReflection + MTres2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: 4 threads" << endl;

    mc3.Calculate(res3);
    EXPECT_NEAR(res3.specularReflection, 0.05325, 1e-4);
    EXPECT_NEAR(res3.diffuseReflection, 0.0175, 1e-3);
    EXPECT_NEAR(res3.diffuseTransmission, 0.0000549, 2e-5);
    EXPECT_NEAR(res3.diffuseTransmission + res3.diffuseReflection + res3.specularReflection + res3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample3, int(2e6), 4, sample3.getTotalThickness(), selectedRadius, MTres3);
    EXPECT_NEAR(MTres3.specularReflection, 0.05325, 2e-4);
    EXPECT_NEAR(MTres3.diffuseReflection, 0.0175, 1e-3);
    EXPECT_NEAR(MTres3.diffuseTransmission, 0.0000549, 2e-5);
    EXPECT_NEAR(MTres3.diffuseTransmission + MTres3.diffuseReflection + MTres3.specularReflection + MTres3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: 4 threads" << endl;

    cout << "All tests passed successfully!" << endl;
}

TEST(MC, MultiLayerAbsorptionScattering) {
    using T = double;

    cout << "Glass-Tissue-Glass, absorption & scattering" << endl;

    constexpr size_t Nz = 1000;
    constexpr size_t Nr = 10000;
    constexpr T selectedRadius = 10e-2;

    const Medium<T> tissue1(1.5, 100, 900,  1e-3, 0.9); // MFP == D
    const Medium<T> tissue2(1.3,  20,  80,  1e-3, 0.5); // MFP == 10*D
    const Medium<T> tissue3(1.6, 700, 300, 10e-3, 0.0); // MFP == 0.1*D, it will need more photons

    const Medium<T> glass1 (1.6 , 0.0, 0.0, 1e-3, 0.0);
    const Medium<T> glass2 (1.4 , 0.0, 0.0, 1e-3, 0.0);
    const Medium<T> glass3 (1.65, 0.0, 0.0, 1e-3, 0.0);

    const vector<Medium<T>> layer1 = {glass1, tissue1, glass3};
    const vector<Medium<T>> layer2 = {glass2, tissue2, glass2};
    const vector<Medium<T>> layer3 = {glass3, tissue3, glass3};

    const Sample<T> sample1(layer1, 1.0, 1.0);
    const Sample<T> sample2(layer2, 1.0, 1.0);
    const Sample<T> sample3(layer3, 1.0, 1.0);

    MonteCarlo<T, Nz, Nr> mc1(sample1, int(1e6), sample1.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc2(sample2, int(1e6), sample2.getTotalThickness(), selectedRadius);
    MonteCarlo<T, Nz, Nr> mc3(sample3, int(1e6), sample3.getTotalThickness(), selectedRadius);

    MCresults<T, Nz, Nr> res1, res2, res3;
    MCresults<T, Nz, Nr> MTres1, MTres2, MTres3;

    mc1.Calculate(res1);
    EXPECT_NEAR(res1.specularReflection, 0.05419, 1e-4);
    EXPECT_NEAR(res1.diffuseReflection, 0.05813, 5e-3);
    EXPECT_NEAR(res1.diffuseTransmission, 0.7394, 1e-3);
    EXPECT_NEAR(res1.diffuseTransmission + res1.diffuseReflection + res1.specularReflection + res1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample1, int(1e6), 4, sample1.getTotalThickness(), selectedRadius, MTres1);
    EXPECT_NEAR(MTres1.specularReflection, 0.05419, 1e-4);
    EXPECT_NEAR(MTres1.diffuseReflection, 0.05813, 5e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission, 0.7394, 2e-3);
    EXPECT_NEAR(MTres1.diffuseTransmission + MTres1.diffuseReflection + MTres1.specularReflection + MTres1.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 1 OK: 4 threads" << endl;

    mc2.Calculate(res2);
    EXPECT_NEAR(res2.specularReflection, 0.02907, 1e-4);
    EXPECT_NEAR(res2.diffuseReflection, 0.03695, 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission, 0.8987, 1e-3);
    EXPECT_NEAR(res2.diffuseTransmission + res2.diffuseReflection + res2.specularReflection + res2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample2, int(1e6), 4, sample2.getTotalThickness(), selectedRadius, MTres2);
    EXPECT_NEAR(MTres2.specularReflection, 0.02907, 1e-4);
    EXPECT_NEAR(MTres2.diffuseReflection, 0.03695, 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission, 0.8987, 1e-3);
    EXPECT_NEAR(MTres2.diffuseTransmission + MTres2.diffuseReflection + MTres2.specularReflection + MTres2.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 2 OK: 4 threads" << endl;

    mc3.Calculate(res3);
    EXPECT_NEAR(res3.specularReflection, 0.06037, 1e-4);
    EXPECT_NEAR(res3.diffuseReflection, 0.01718, 1e-3);
    EXPECT_NEAR(res3.diffuseTransmission, 0.0000541, 2e-5); // yeah that's a lot
    EXPECT_NEAR(res3.diffuseTransmission + res3.diffuseReflection + res3.specularReflection + res3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: single thread" << endl;

    MCmultithread<T, Nz, Nr>(sample3, int(1e7), 4, sample3.getTotalThickness(), selectedRadius, MTres3);
    EXPECT_NEAR(MTres3.specularReflection, 0.06037, 1e-4);
    EXPECT_NEAR(MTres3.diffuseReflection, 0.01718, 1e-3);
    EXPECT_NEAR(MTres3.diffuseTransmission, 0.0000541, 5e-6);
    EXPECT_NEAR(MTres3.diffuseTransmission + MTres3.diffuseReflection + MTres3.specularReflection + MTres3.absorbed, 1.0, TOTAL_TOLERANCE);
    cout << "Test 3 OK: 4 threads" << endl;
}