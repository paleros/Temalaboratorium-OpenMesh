//
// Created by peros on 2024.04.03.
//
/**
 * A fa alaku alatamasztast kiszamolo fuggvenyeket tarolo file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#include <fstream>
#include "treeMain.h"

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
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 3.1
 */
double
treeSupportGenerated(MyMesh &meshObject, std::string &inputFile, std::vector<Point> &supportPointsAll, double diameter,
                     double l, double e, int groupingValue, bool isFinish) {

    /// Az aktualis ponnak minden iranyban az ennyi l -lel odébb lévő szomszédjai
    double angle = M_PI / 5;

    /// Ha nincs alatamasztando pont, akkor nem csinal semmit
    if (supportPointsAll.empty()) {
        writeLog("\tNo support points found");
        /// A kimeneti fajlok uresek lesznek
        std::ofstream file("outputs/6-supportLineTree.obj");
        if (!file) {
            std::cout << "Error: The file " << "outputs/6-supportLineTree.obj" << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << "# Input edges generated from " << inputFile << " by BTMLYV\n";
        file.close();
        std::ofstream file2("outputs/7-supportTree.obj");
        if (!file2) {
            std::cout << "Error: The file " << "outputs/7-supportTree.obj" << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file2 << "# Support objects generated from " << inputFile << " by BTMLYV\n";
        file2.close();

        return 0;
    }

    double minY = getMinY(meshObject);

    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);

    for (auto & i : supportPointsAll) {
        i._weight = -2;   /// Az e erteke -1, ha a targyat kozvetlenul tamaszta
    }

    /// Azok a pontok, amelyek kelloen kozel vannak egymashoz
    std::vector<Point> neighbourPoints;
    std::vector<Point> intersectPs;
    std::vector<Edge> supportTree;
    double maxY = getMaxY(supportPointsAll);

    /// Sorban vegigmegy az alatamasztando pontokon es osszekoti oket a fa struktura szerint
    for (int k = 0; k < (int)supportPointsAll.size(); k++) {

        if (getMaxY(supportPointsAll) <= maxY - (((double)groupingValue / 2) / tan(angle))){
            groupingValue *= 3;
        }

        /// Kikeressuk a szomszedos pontokat
        getNeigbourPoints(supportPointsAll, supportPointsAll[k], neighbourPoints, l, groupingValue);

        for (auto &actualNeighbourPoint : neighbourPoints) {
            for (auto &neighbourPoint : neighbourPoints) {
                if (actualNeighbourPoint == neighbourPoint) {
                    continue;
                }
                /// Az alatamasztando szomszedos ket pont
                double Ax = actualNeighbourPoint._coordinates[0];
                double Ay = actualNeighbourPoint._coordinates[1];
                double Az = actualNeighbourPoint._coordinates[2];
                double Bx = neighbourPoint._coordinates[0];
                double By = neighbourPoint._coordinates[1];
                double Bz = neighbourPoint._coordinates[2];
                double X, Y, Z;

                /// A ket pont tavolsaga es a megfelelo szoghoz tartozo magassag
                double distance = sqrt(pow(Ax - Bx, 2) + 0 + pow(Az - Bz, 2));
                double h = std::round((distance / tan(angle)) * 1e10) / 1e10;

                /// Az egyenesek masik pontjai
                double Cx = Bx;
                double Cy = Ay - h;
                double Cz = Bz;
                double Dx = Ax;
                double Dy = By - h;
                double Dz = Az;

                /// Az egyenes iranyvektorainak koordinatai
                double ADx= Ax -Cx;
                double ADy= Ay -Cy;
                double ADz= Az -Cz;
                double BDx= Bx -Dx;
                double BDy= By -Dy;
                double BDz= Bz -Dz;

                /// A metszespont kiszamitasa
                double a = dot(ADx, ADy, ADz, ADx, ADy, ADz);
                double b = dot(ADx, ADy, ADz, BDx, BDy, BDz);
                double c = dot(BDx, BDy, BDz, BDx, BDy, BDz);
                double d = dot(ADx, ADy, ADz, Ax-Bx, Ay-By, Az-Bz);
                double f = dot(BDx, BDy, BDz, Ax-Bx, Ay-By, Az-Bz);
                if (a * c - b * b < 1.0e-7) {
                    X = Ax;
                    Y = Ay;
                    Z = Az;

                    Point newPoint;
                    newPoint._coordinates[0] = X;
                    newPoint._coordinates[1] = Y;
                    newPoint._coordinates[2] = Z;
                    newPoint._e = e;

                    intersectPs.push_back(newPoint);
                    continue;
                }
                double D = a * c - b * b;
                double s = (b * f - c * d) / D;
                double t = (a * f - b * d) / D;
                Ax = ((Ax + ADx * s) + (Bx + BDx * t)) / 2;
                Ay = ((Ay + ADy * s) + (By + BDy * t)) / 2;
                Az = ((Az + ADz * s) + (Bz + BDz * t)) / 2;
                X = Ax;
                Y = Ay;
                Z = Az;

                Point newPoint;
                newPoint._coordinates[0] = X;
                newPoint._coordinates[1] = Y;
                newPoint._coordinates[2] = Z;
                newPoint._e = e;

                intersectPs.push_back(newPoint);
            }
        }

        /// A legalacsonyabban levo pontot hozzaadjuk az alatamasztando pontokhoz
        Point lowestPoint;
        if(!intersectPs.empty()){
            std::sort(intersectPs.begin(), intersectPs.end(), compareInputPointsYXZ);
            lowestPoint = intersectPs[0];
            /// Ha nem log ki alul az alatamasztas
            if(lowestPoint._coordinates[1] > minY){
                supportPointsAll.push_back(lowestPoint);
                k = -1;
                bool usedOriginalLowPoint = true;

                /// Osszekoti a megfelelo pontokat, majd kitorli a mar alatamasztott pontokat
                for (auto & neighbourPoint : neighbourPoints) {
                    Point nextPoint;
                    nextPoint = lowestPoint;

                    Point intersectPoint;
                    intersectPoint = passTheModel(neighbourPoint, nextPoint, meshObject, e);

                    if(intersectPoint._e == -1){     /// Az e erteke -1, ha nem metszi az alakzatot
                        /// Ha nem metszi az alakzatot, akkor hasznalja az eredeti also pontot
                        usedOriginalLowPoint = true;

                        /// Ha az alakzaton belul van az el, akkor nem hozzuk letre, ha kivul, akkor pedig letrehozzuk
                        if(!isItInside(meshObject, neighbourPoint, nextPoint)){
                            supportTree.emplace_back(neighbourPoint, nextPoint, 0, -1);
                        }

                    }else{
                        /// Ha metszi az alakzatot, akkor hasznalja a metszespontot
                        nextPoint = intersectPoint;
                        usedOriginalLowPoint = false;
                        /// Ha az alakzaton belul van az el, akkor nem hozzuk letre, ha kivul, akkor pedig letrehozzuk
                        if(!isItInside(meshObject, neighbourPoint, nextPoint)){
                            supportTree.emplace_back(neighbourPoint, nextPoint, 0, e);
                        }
                    }

                    /// Torli a mar alatamasztott pontokat
                    supportPointsAll.erase(std::remove(supportPointsAll.begin(),
                                                            supportPointsAll.end(), neighbourPoint),
                                                            supportPointsAll.end());
                }

                /// Ha nem hasznaljuk a regi also pontot, akkor toroljuk
                if (!usedOriginalLowPoint) {
                    supportPointsAll.erase(std::remove(supportPointsAll.begin(),
                                                            supportPointsAll.end(), lowestPoint),
                                                            supportPointsAll.end());
                }
            }
        }

        neighbourPoints.clear();
        intersectPs.clear();

        std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);
    }

    /// A tamaszfa vegpontjaibol egyeneseket huzuk a legalso pontig
    for (auto & i : supportPointsAll) {
        Point nextPoint;
        nextPoint._coordinates[0] = i._coordinates[0];
        nextPoint._coordinates[1] = minY;
        nextPoint._coordinates[2] = i._coordinates[2];
        nextPoint._e = e; /// Az e erteke -2, ha a talajt erinti a tamasz
        Point intersect = passTheModel(i, nextPoint, meshObject, e);
        if (intersect._e != -1){
            nextPoint = intersect;
        }
        if(!isItInside(meshObject, i, nextPoint)){
            supportTree.emplace_back(i, nextPoint, 0, -2);
        }
    }

    writeInputEdges("outputs/6-supportLineTree.obj", inputFile, supportTree, isFinish);
    if (isFinish) {
        writeLog("\tTreeLineSupportObjects written to file");
    }

    std::vector<Tree> trees;
    separateTree(supportTree, trees);
    writeSupportTreeDynamic("outputs/7-supportTree.obj", inputFile, trees, minY, diameter / 8, isFinish);

    double point = calculatePoint(supportTree);
    return point;
}