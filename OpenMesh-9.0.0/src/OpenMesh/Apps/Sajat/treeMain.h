//
// Created by peros on 2024.04.03.
//

#ifndef OPENMESH_TREEMAIN_H
#define OPENMESH_TREEMAIN_H
#include "auxiliary.h"
#include "treeAuxiliary.h"

/**
 * Legeneralja a az alatamasztast
 * @param meshObject az alakzat
 * @param inputFile a bemeneti file
 * @param supportPointsAll az alatamasztando pontok
 * @param intersectPoints a metszespontok
 * @param diameter az oszlop atmeroje
 * @param l a racs tavolsaga
 * @param e a hibahatar
 * @param groupingValue a csoportositasi ertek
 * @return az alatamasztas pontja
 * @since 3.0
 */
double
treeSupportGenerated(MyMesh &meshObject, std::string &inputFile, std::vector<Point> &supportPointsAll, double diameter,
                     double l, double e, int groupingValue);

#endif //OPENMESH_TREEMAIN_H
