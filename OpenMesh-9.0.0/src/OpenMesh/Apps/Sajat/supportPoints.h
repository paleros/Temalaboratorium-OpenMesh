//
// Created by peros on 2024.04.03.
//
/**
 * A beolvasast es az alatamasztando pontokat kiszamolo fuggvenyeket tarolo header file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#ifndef OPENMESH_SUPPORTPOINTS_H
#define OPENMESH_SUPPORTPOINTS_H
#include "auxiliary.h"

/**
 * Az alakzat beolvasasaert es az alatamasztando pontokert felelos fuggveny
 * @param diameter az alatamasztas vastagsaga
 * @param l a sugarak kozti tavolsag
 * @param e a double ertekek kovetkezteben megjeleno hiba korrekcios erteke
 * @param inputFile a bemeneti file
 * @param intersectPoints a metszespontok
 * @param count szamlalo
 * @param meshObject az alakzat
 * @param edges az alakzaton belul futo elek
 * @param inputPoints a bemeneti pontok
 * @param supportPointsAll az osszes alatamasztando pont
 * @param maxWeight a maximalis suly
 */
void supportPointsGenerated(double diameter, double l, double e, const std::string& inputFile,
                            std::vector<Point> &intersectPoints, int &count, MyMesh &meshObject, std::vector<Edge> &edges,
                            std::vector<Point> &inputPoints, std::vector<Point> &supportPointsAll, double maxWeight);

#endif //OPENMESH_SUPPORTPOINTS_H
