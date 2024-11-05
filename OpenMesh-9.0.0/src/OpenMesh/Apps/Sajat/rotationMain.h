//
// Created by peros on 2024.10.08.
//

#ifndef OPENMESH_ROTATIONMAIN_H
#define OPENMESH_ROTATIONMAIN_H

#include <string>
#include <vector>
#include "auxiliary.h"

/**
 * Kiszamitja hogy a megadott elforgatassal mennyire jo a tamasztas
 * @param angles az elforgatas szoge
 * @param mesh az alakzat
 * @param inputFile a bemeneti fajl
 * @param isTreeSupport az alatamasztas tipusa
 * @param roundNumber a aktualis kor szama
 * @return
 */
double getOptimalValue(std::vector<double> &angles, MyMesh &mesh, std::string &inputFile, bool isTreeSupport,
                       int &roundNumber);

/**
 * Az alakzat optimalis forgatasat megkereso fuggveny
 * Nelder-Mead algoritmussal
 * @param inputFile a bemeneti fajl
 * @param mesh az alakzat
 * @param supportType az alatamasztas tipusa
 * @param algorithmType az optimalizalasi algoritmus
 * @return az optimalis forgatas
 * @since 4.1
 */
std::vector<double> optimalSide(std::string &inputFile, MyMesh &mesh, SupportType supportType, AlgorithmType algorithmType);

/**
 * A program futtatasa
 * @param inputFile a bemeneti file
 * @param supportType az alatamasztas tipusa
 * @param algorithmType az optimalis oldal keresese
 * @since 4.1
 */
void run(std::string inputFile, SupportType supportType, AlgorithmType algorithmType);

#endif //OPENMESH_ROTATIONMAIN_H
