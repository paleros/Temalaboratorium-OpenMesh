//
// Created by peros on 2024.04.03.
//
/**
 * A fa tamasz fuggvenyeit tartalmazo file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#include <string>
#include <fstream>
#include <queue>
#include <utility>
#include "OpenMesh/Core/IO/MeshIO.hh"
#include "treeAuxiliary.h"

/**
 * Az alatamasztando pontok kozul visszaadja a szomszedosat ha van
 * @param x ezen a tengelyen ennyi a tavolsag l szorzoja
 * @param y ezen a tengelyen ennyi a tavolsag l szorzoja
 * @param z ezen a tengelyen ennyi a tavolsag l szorzoja
 * @param coordinates ezen a tengelyen keressuk a szomszedot ('x', 'y', 'z')
 * @param polarity a tengelyen az iranya a szomszednak ('+', '-')
 * @param p a pont, aminek a szomszedjat keressuk
 * @param supportPointsAll az osszes alatamasztando pont
 * @param l a racs tavolsaga
 * @param angle a szog
 * @return a szomszedos pont
 */
Point* getNeighbour(int x, int y, int z, Point* p, std::vector<Point> &supportPointsAll, double l, double angle){
    Point actualPoint;
    actualPoint.coordinates[0] = p->coordinates[0] - x * l;
    actualPoint.coordinates[1] = p->coordinates[1];
    actualPoint.coordinates[2] = p->coordinates[2] - z * l;

//TODO midig csak akkor szomszedos, ha a racsponton van, ez a plusz tamaszpontoknal problema lehet
//TODO van egy-ket alap pont, amit nem tamaszt ala

    for (auto &supportPoint : supportPointsAll) {
        double distance = sqrt(pow(actualPoint.coordinates[0] - supportPoint.coordinates[0], 2) + 0 +
                                    pow(actualPoint.coordinates[2] - supportPoint.coordinates[2], 2));
        double h = std::round((distance / tan(angle)) * 1e10) / 1e10;

        if (supportPoint.coordinates[0] <= actualPoint.coordinates[0] &&
            supportPoint.coordinates[1] >= actualPoint.coordinates[1] - h &&
            supportPoint.coordinates[1] <= actualPoint.coordinates[1] &&
            supportPoint.coordinates[2] <= actualPoint.coordinates[2]) {
            return &supportPoint;
        }
    }

    return nullptr;
}

/**
 * A ket kapott pont kozotti tavolsagot adja vissza
 * @param p1 az elso pont
 * @param p2 a masodik pont
 * @return a ket pont kozotti tavolsag
 */
double getDistance(Point &p1, Point &p2){
    return sqrt(pow(p1.coordinates[0] - p2.coordinates[0], 2) +
                pow(p1.coordinates[1] - p2.coordinates[1], 2) +
                pow(p1.coordinates[2] - p2.coordinates[2], 2));
}

/**
 * A parameterkent kapott pontokat osszehasonlitja az y, x es z koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsYXZAll(const Point &p1, const Point &p2){
    if (p1.coordinates[1] > p2.coordinates[1]) {
        return true;
    } else if (p1.coordinates[1] == p2.coordinates[1]) {
        if (p1.coordinates[0] > p2.coordinates[0]) {
            return true;
        } else if (p1.coordinates[0] == p2.coordinates[0]) {
            return p1.coordinates[2] > p2.coordinates[2];
        }
    }
    return false;
}

/**
 * A parameterkent kapott pontokat osszehasonlitja az x, y es z koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsXYZAll(const Point &p1, const Point &p2){
    if (p1.coordinates[0] > p2.coordinates[0]) {
        return true;
    } else if (p1.coordinates[0] == p2.coordinates[0]) {
        if (p1.coordinates[1] > p2.coordinates[1]) {
            return true;
        } else if (p1.coordinates[1] == p2.coordinates[1]) {
            return p1.coordinates[2] > p2.coordinates[2];
        }
    }
    return false;
}

/**
 * A parameterkent kapott pontokat osszehasonlitja az z, x es y koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsZXYAll(const Point &p1, const Point &p2){
    if (p1.coordinates[2] > p2.coordinates[2]) {
        return true;
    } else if (p1.coordinates[2] == p2.coordinates[2]) {
        if (p1.coordinates[0] > p2.coordinates[0]) {
            return true;
        } else if (p1.coordinates[0] == p2.coordinates[0]) {
            return p1.coordinates[1] > p2.coordinates[1];
        }
    }
    return false;
}

/**
 * Vektorok koordinatainak szorzata
 * @param Ax A pont x koordinataja
 * @param Ay A pont y koordinataja
 * @param Az A pont z koordinataja
 * @param Bx B pont x koordinataja
 * @param By B pont y koordinataja
 * @param Bz B pont z koordinataja
 * @return a szorzat
 */
double dot(double Ax, double Ay, double Az, double Bx, double By, double Bz){
    return Ax*Bx+ Ay*By+Az*Bz;
}

/**
 * Visszaadja a legnagyobb x erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb x erteke
 */
double getMaxX(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsXYZAll);
    return supportPointsAll[0].coordinates[0];
}

/**
 * Visszaadja a legnagyobb z erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb z erteke
 */
double getMaxZ(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsZXYAll);
    return supportPointsAll[0].coordinates[2];
}

/**
 * Visszaadja a legnagyobb y erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb y erteke
 */
double getMaxY(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);
    return supportPointsAll[0].coordinates[1];
}

/**
 * Az aktualis ponnak minden negativ iranyban az l -lel odébb lévő szomszédjai adja vissza
 * @param supportPointsAll az osszes alatamasztando pont
 * @param actualPoint az aktualis pont
 * @param neighbourPoints a szomszedos pontok
 * @param l a racs tavolsaga
 * @param groupingValue a szomszedos pontok szama
 */
void getNeigbourPoints(std::vector<Point> &supportPointsAll, Point &actualPoint, std::vector<Point> &neighbourPoints,
                       double l, double groupingValue){

    for (int i = 0; i < (int)supportPointsAll.size(); i++){
        if (supportPointsAll[i].coordinates[0] <= actualPoint.coordinates[0] &&
            supportPointsAll[i].coordinates[0] >= actualPoint.coordinates[0] - l * groupingValue &&
            //supportPointsAll[i].coordinates[1] <= actualPoint.coordinates[1] &&
            //supportPointsAll[i].coordinates[1] > actualPoint.coordinates[1] - l &&
            supportPointsAll[i].coordinates[2] <= actualPoint.coordinates[2] &&
            supportPointsAll[i].coordinates[2] >= actualPoint.coordinates[2] - l * groupingValue) {
            neighbourPoints.push_back(supportPointsAll[i]);
        }
    }
}