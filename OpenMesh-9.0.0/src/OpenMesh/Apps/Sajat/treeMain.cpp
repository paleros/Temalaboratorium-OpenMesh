//
// Created by peros on 2024.04.03.
//
/**
 * A fa alaku alatamasztast kiszamolo fuggvenyeket tarolo file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

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
 * @since 3.1
 */
void treeSupportGenerated(MyMesh& meshObject, std::string &inputFile, std::vector<Point> &supportPointsAll,
                            std::vector<Point> &intersectPoints, double diameter, double l, double e){

    /// Az aktualis ponnak minden iranyban az ennyi l -lel odébb lévő szomszédjai
    int groupingValue = 2; //TODO ha nem tudja csoportositani, akkor egyedul marad a pont, ezt le kell kezelni
    double angle = M_PI / 5;

    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);

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
    getNeigbourPoints(supportPointsAll, supportPointsAll[k], neighbourPoints, l,
                      groupingValue);

        for (auto &actualNeighbourPoint : neighbourPoints) {
            for (auto &neighbourPoint : neighbourPoints) {
                if (actualNeighbourPoint == neighbourPoint) {
                    continue;
                }
                /// Az alatamasztando szonszedos ket pont
                double Ax = actualNeighbourPoint.coordinates[0];
                double Ay = actualNeighbourPoint.coordinates[1];
                double Az = actualNeighbourPoint.coordinates[2];
                double Bx = neighbourPoint.coordinates[0];
                double By = neighbourPoint.coordinates[1];
                double Bz = neighbourPoint.coordinates[2];
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
                double c = dot(BDx, BDy, BDz, BDx, BDy, BDz);;
                double d = dot(ADx, ADy, ADz, Ax-Bx, Ay-By, Az-Bz);
                double f = dot(BDx, BDy, BDz, Ax-Bx, Ay-By, Az-Bz);
                if (a * c - b * b < 1.0e-7) {
                    X = Ax;
                    Y = Ay;
                    Z = Az;

                    Point newPoint;
                    newPoint.coordinates[0] = X;
                    newPoint.coordinates[1] = Y;
                    newPoint.coordinates[2] = Z;
                    newPoint.e = e;

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
                newPoint.coordinates[0] = X;
                newPoint.coordinates[1] = Y;
                newPoint.coordinates[2] = Z;
                newPoint.e = e;

                intersectPs.push_back(newPoint);
            }
        }

        /// A legalacsonyabban levo pontot hozzaadjuk az alatamasztando pontokhoz
        Point lowestPoint;
        if(!intersectPs.empty()){
            std::sort(intersectPs.begin(), intersectPs.end(), compareInputPointsYXZ);
            lowestPoint = intersectPs[0];
            supportPointsAll.push_back(lowestPoint);
            k = -1;

            /// Osszekoti a megfelelo pontokat, majd kitorli a mar alatamasztott pontokat
            for (auto &neighbourPoint: neighbourPoints) {
                supportTree.emplace_back(neighbourPoint, lowestPoint, 0, e);
                for(auto it = supportPointsAll.begin(); it != supportPointsAll.end();){
                    if(*it == neighbourPoint){
                        it = supportPointsAll.erase(it);
                    }else{
                        ++it;
                    }
                }
            }
        }

        neighbourPoints.clear();
        intersectPs.clear();

        std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);
    }

    writeInputEdges("outputs/6-supportTree.obj", inputFile, supportTree);
    writeLog("\tTreeSupportObjects written to file");
}