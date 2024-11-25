//
// Created by peros on 2024.10.08.
//

#ifndef OPENMESH_ROTATIONMAIN_H
#define OPENMESH_ROTATIONMAIN_H

#include <string>
#include <vector>
#include "auxiliary.h"

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
