//
// Created by peros on 2024.04.03.
//
/**
 * Az oszlopos tamasz fuggvenyeit tartalmazo header file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#ifndef OPENMESH_COLUMNAUXILIARY_H
#define OPENMESH_COLUMNAUXILIARY_H
#include "auxiliary.h"
#include <string>
#include "OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh"

/**
 * A parameterkent kapott pontok kozott haromszog alapu hasabokat csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a haromszog merete
 * @param minY a legkisebb y koordinata
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 1.5
 */
void generateAndWriteSupportLines(const std::string &outputFileName, const std::string &inputFileName,
                                  std::vector<Point> &points, double diameter, double minY, bool isFinish);

/**
 * A parameterkent kapott pontok kozott hengereket csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a henger atmeroje
 * @param minY a legkisebb y koordinata
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 2.1
 */
void generateAndWriteSupportCylinder(const std::string &outputFileName, const std::string &inputFileName,
                                     std::vector<Point> &points, double diameter, double minY, bool isFinish);

/**
 * Ha egy atlo metszi az alakzatot, akkor azt torli
 * @param edges az atlok
 * @param meshObject az alakzat
 * @param l hibahatar
 * @since 2.1.3
 */
void deleteWrongDiagonals(std::vector<Edge>& edges, OpenMesh::PolyMesh_ArrayKernelT<> meshObject, double l);

/**
 * A parameterkent kapott pontok kozotti alatamasztasok kozott generalja a keresztmerevitoket
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter az alatamasztas atmeroje
 * @param l a kuszobertek
 * @param meshObject az alakzat
 * @since 2.1.2
 */
void generateAndWriteSupportCrossBrace(const std::string &outputFileName, const std::string &inputFileName,
                                       std::vector<Point> &points, double diameter, double l,
                                       OpenMesh::PolyMesh_ArrayKernelT<> meshObject, bool isFinish);

#endif //OPENMESH_COLUMNAUXILIARY_H