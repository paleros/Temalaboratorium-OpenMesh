//
// Created by peros on 2024.04.03.
//
/**
 * A hengerekbol allo racsos tartohoz hasonlo oszlopos alatamasztast kiszamolo fuggvenyeket tarolo file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#include "columnMain.h"

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
 * @since 2.2
 */
double columnSupportGenerated(MyMesh& meshObject, std::string &inputFile, std::vector<Point> &supportPointsAll,
                            std::vector<Point> &intersectPoints, double diameter, double l, double e){

    /// Az alatamasztando pontokbol egyeneseket huzunk a legalso pont y koordinataja szerinti sikra
    /// @since 1.4
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZ);
    std::vector<Point> supportLines;
    /// Kiszamoljuk, hogy meddig kell az egyeneseket huzni
    double minY = std::numeric_limits<double>::max(); // Kezdeti érték a minimális y koordinátának

    for (MyMesh::VertexIter v_it = meshObject.vertices_begin(); v_it != meshObject.vertices_end(); ++v_it) {
        MyMesh::Point p = meshObject.point(*v_it);
        if (p[1] < minY) {
            minY = p[1];
        }
    }

    for (const auto & supportPoint : supportPointsAll) {
        Point p;
        p.coordinates[0] = supportPoint.coordinates[0];
        p.coordinates[1] = supportPoint.coordinates[1];
        p.coordinates[2] = supportPoint.coordinates[2];
        p.e = supportPoint.e;
        supportLines.push_back(p);
        p.coordinates[1] = minY;

        /// Ha a metszespontok kozott van olyan kimeno pont, amelyik alacsonyabb, akkor azt a pontot csak addig huzzuk le
        for(int i = 1; i < (int)intersectPoints.size(); i = i + 2){
            if(std::abs(intersectPoints[i].coordinates[0] - supportPoint.coordinates[0]) <= e &&
               std::abs(intersectPoints[i].coordinates[2] - supportPoint.coordinates[2]) <= e){
                if(intersectPoints[i].coordinates[1] > p.coordinates[1] && intersectPoints[i].coordinates[1] < supportPoint.coordinates[1]){
                    p.coordinates[1] = intersectPoints[i].coordinates[1];
                }
            }
        }
        supportLines.push_back(p);
    }

    writeInternalLines("outputs/4-supportLines.obj", inputFile, supportLines, "# Support lines generated from ");
    writeLog("\tSupportLines written to file");


    /// Az alatamasztando pontokat atalakitjuk alazatta, hogy nyomtathato legyen
    /// @since 1.5
    generateAndWriteSupportLines("outputs/5-triangleSupportObjects.obj", inputFile, supportLines, diameter, minY);
    writeLog("\tTriangleSupportObjects written to file");
    /// @since 2.1
    generateAndWriteSupportCylinder("outputs/6-cylinderSupportObjects.obj", inputFile, supportLines, diameter, minY);
    writeLog("\tCylinderSupportObjects written to file");
    generateAndWriteSupportCrossBrace("outputs/7-diagonalSupportObjects.obj", inputFile, supportLines, diameter, l, meshObject);
    writeLog("\tDiagonalsupportObjects written to file");

    /// @since 4.1
    return calculatePoint(supportLines);
}