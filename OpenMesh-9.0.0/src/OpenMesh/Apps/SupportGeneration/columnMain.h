//
// Created by peros on 2024.04.03.
//
/**
 * A hengerekbol allo racsos tartohoz hasonlo oszlopos alatamasztast kiszamolo fuggvenyeket tarolo header file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#ifndef OPENMESH_COLUMNMAIN_H
#define OPENMESH_COLUMNMAIN_H
#include "auxiliary.h"
#include "columnAuxiliary.h"

/**
 * Legeneralja a az alatamasztast
 * @param meshObject az alakzat
 * @param inputFile a bemeneti file
 * @param supportPointsAll az alatamasztando pontok
 * @param intersectPoints a metszespontok
 * @param diameter az oszlop atmeroje
 * @param l a racs tavolsaga
 * @param e a hibahatar
 * @return az alatamasztas pontja
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 2.2
 */
double columnSupportGenerated(MyMesh &meshObject, std::string &inputFile, std::vector<Point> &supportPointsAll,
                              std::vector<Point> &intersectPoints, double diameter, double l, double e, bool isFinish);

#endif //OPENMESH_COLUMNMAIN_H