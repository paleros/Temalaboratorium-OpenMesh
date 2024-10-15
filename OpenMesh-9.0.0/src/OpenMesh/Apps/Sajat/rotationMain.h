//
// Created by peros on 2024.10.08.
//

#ifndef OPENMESH_ROTATIONMAIN_H
#define OPENMESH_ROTATIONMAIN_H

#include <string>
#include <vector>

/**
 * Kiszamitja hogy a megadott elforgatassal mennyire jo a tamasztas
 * @param angles az elforgatas szoge
 * @param inputFile a bemeneti fajl
 * @param isTreeSupport az alatamasztas tipusa
 * @return
 */
double getPoint(std::vector<double> &angles, std::string& inputFile, bool isTreeSupport);

/**
 * Az alakzat optimalis forgatasat megkereso fuggveny
 * Nelder-Mead algoritmussal
 * @param inputFile a bemeneti fajl
 * @param isTreeSupport az alatamasztas tipusa
 * @return az optimalis forgatas
 * @since 4.1
 */
std::vector<double> optimalSide(std::string& inputFile, bool isTreeSupport);

/**
 * A program futtatasa
 * @param inputFile a bemeneti file
 * @param isTreeSupport az alatamasztas tipusa
 * @param findOptimalSide az optimalis oldal keresese
 * @since 4.1
 */
void run(std::string inputFile, bool isTreeSupport, bool findOptimalSide);

#endif //OPENMESH_ROTATIONMAIN_H
