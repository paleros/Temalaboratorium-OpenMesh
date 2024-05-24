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
 * @since 3.1
 */
Point* getNeighbour(int x, int y, int z, Point* p, std::vector<Point> &supportPointsAll, double l, double angle){
    Point actualPoint;
    actualPoint.coordinates[0] = p->coordinates[0] - x * l;
    actualPoint.coordinates[1] = p->coordinates[1];
    actualPoint.coordinates[2] = p->coordinates[2] - z * l;
    
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
 * @since 3.1
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
 * @since 3.1
 */
double dot(double Ax, double Ay, double Az, double Bx, double By, double Bz){
    return Ax*Bx+ Ay*By+Az*Bz;
}

/**
 * Visszaadja a legnagyobb x erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb x erteke
 * @since 3.1
 */
double getMaxX(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsXYZAll);
    return supportPointsAll[0].coordinates[0];
}

/**
 * Visszaadja a legnagyobb z erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb z erteke
 * @since 3.1
 */
double getMaxZ(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsZXYAll);
    return supportPointsAll[0].coordinates[2];
}

/**
 * Visszaadja a legnagyobb y erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb y erteke
 * @since 3.1
 */
double getMaxY(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);
    return supportPointsAll[0].coordinates[1];
}

/**
 * Visszaadja a legkisebb y erteket
 * @param supportPointsAll a pontok
 * @return a legkisebb y erteke
 * @since 3.1
 */
double getMinY(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);
    return supportPointsAll[supportPointsAll.size()-1].coordinates[1];
}

/**
 * Az aktualis ponnak minden negativ iranyban az l -lel odébb lévő szomszédjai adja vissza
 * @param supportPointsAll az osszes alatamasztando pont
 * @param actualPoint az aktualis pont
 * @param neighbourPoints a szomszedos pontok
 * @param l a racs tavolsaga
 * @param groupingValue a szomszedos pontok szama
 * @since 3.1
 */
void getNeigbourPoints(std::vector<Point> &supportPointsAll, Point &actualPoint, std::vector<Point> &neighbourPoints,
                       double l, double groupingValue){

    for (auto & i : supportPointsAll){
        if (i.coordinates[0] <= actualPoint.coordinates[0] &&
            i.coordinates[0] >= actualPoint.coordinates[0] - l * groupingValue &&
            //i.coordinates[1] <= actualPoint.coordinates[1] &&
            i.coordinates[1] > actualPoint.coordinates[1] - l *groupingValue &&
            i.coordinates[2] <= actualPoint.coordinates[2] &&
            i.coordinates[2] >= actualPoint.coordinates[2] - l * groupingValue) {
            neighbourPoints.push_back(i);
        }
    }
}

/**
 * A ket pont altal meghatarozott szakasz metszi-e az alakzatot, ha igen,
 * akkor a metszespontot beallitja a lowestPoint-ra
 * @param supportPointsAll az osszes alatamasztando pont
 * @param neighbourPoint a szakasz egyik pontja
 * @param lowestPoint a szakasz masik, also pontja
 * @param meshObject az alakzat
 * @param e a hibahatar
 * return igaz, ha metszi az alakzatot
 * @since 3.1
 */
bool doesItPassTeModel(Point& neighbourPoint, Point& lowestPoint, MyMesh& meshObject, double e){
    neighbourPoint.e = e;
    lowestPoint.e = e;

    std::vector<Point> intersectPoints;

    /// Vegigmegy az osszes tarolt haromszogon
    for(MyMesh::FaceIter fi = meshObject.faces_begin(); fi != meshObject.faces_end(); fi++) {
        MyMesh::FaceHandle fh = *fi;

        /// Az aktualis haromszog csucspontjai
        Point t1, t2, t3;
        int c = 0;

        /// Kiaszamoljuk az aktualis haromszog csucspontjait
        for (MyMesh::FaceVertexIter fvi = meshObject.fv_iter(fh); fvi.is_valid(); ++fvi) {
            MyMesh::VertexHandle vh = *fvi;
            // Biztos van elegansabb megoldas, nekem ez jutott most eszembe
            if (c == 0) {
                t1.coordinates[0] = meshObject.point(vh)[0];
                t1.coordinates[1] = meshObject.point(vh)[1];
                t1.coordinates[2] = meshObject.point(vh)[2];
                t1.e = e;
            } else if (c == 1) {
                t2.coordinates[0] = meshObject.point(vh)[0];
                t2.coordinates[1] = meshObject.point(vh)[1];
                t2.coordinates[2] = meshObject.point(vh)[2];
                t2.e = e;
            } else if (c == 2) {
                t3.coordinates[0] = meshObject.point(vh)[0];
                t3.coordinates[1] = meshObject.point(vh)[1];
                t3.coordinates[2] = meshObject.point(vh)[2];
                t3.e = e;
            }
            c++;
        }

        /// Az aktualis haromszog es az el metszespontjanak kiszamitasa, ha letezik

        Point s1, s2;
        s1 = neighbourPoint;
        s2 = lowestPoint;

        /// A haromszog normalvektora
        Point normal;
        normal.coordinates[0] = (t2.coordinates[1] - t1.coordinates[1]) * (t3.coordinates[2] - t1.coordinates[2]) -
                                (t2.coordinates[2] - t1.coordinates[2]) * (t3.coordinates[1] - t1.coordinates[1]);
        normal.coordinates[1] = (t2.coordinates[2] - t1.coordinates[2]) * (t3.coordinates[0] - t1.coordinates[0]) -
                                (t2.coordinates[0] - t1.coordinates[0]) * (t3.coordinates[2] - t1.coordinates[2]);
        normal.coordinates[2] = (t2.coordinates[0] - t1.coordinates[0]) * (t3.coordinates[1] - t1.coordinates[1]) -
                                (t2.coordinates[1] - t1.coordinates[1]) * (t3.coordinates[0] - t1.coordinates[0]);
        normal.e = e;

        /// A szakasz iranyvektora
        Point direction;
        direction = s2 - s1;
        direction.e = e;

        /// Az egyenes parametere ahol metszi a haromszog sikjat
        double t = (normal.coordinates[0] * (t1.coordinates[0] - s1.coordinates[0]) +
                    normal.coordinates[1] * (t1.coordinates[1] - s1.coordinates[1]) +
                    normal.coordinates[2] * (t1.coordinates[2] - s1.coordinates[2])) /
                   (normal.coordinates[0] * direction.coordinates[0] +
                    normal.coordinates[1] * direction.coordinates[1] +
                    normal.coordinates[2] * direction.coordinates[2]);

        /// Ha a szakasz ket vegpontja kozott van az egyenesen a metszespont
        if (0 - t < -e && t - 1 < -e){

            /// Az metszespont kiszamitasa
            Point intersection;
            intersection = s1 + (direction * t);
            intersection.e = e;

            /*/// A metszespont a haromszogon belul van-e
            Point AB, BC, CA, AP, BP, CP;
            AB = t2 - t1;
            AB.e = e;
            BC = t3 - t2;
            BC.e = e;
            CA = t1 - t3;
            CA.e = e;
            AP = intersection - t1;
            AP.e = e;
            BP = intersection - t2;
            BP.e = e;
            CP = intersection - t3;
            CP.e = e;

            double dotAB = AB.coordinates[0] * AP.coordinates[0] + AB.coordinates[1] * AP.coordinates[1] +
                           AB.coordinates[2] * AP.coordinates[2];
            double dotBC = BC.coordinates[0] * BP.coordinates[0] + BC.coordinates[1] * BP.coordinates[1] +
                           BC.coordinates[2] * BP.coordinates[2];
            double dotCA = CA.coordinates[0] * CP.coordinates[0] + CA.coordinates[1] * CP.coordinates[1] +
                           CA.coordinates[2] * CP.coordinates[2];
*/
            //if (dotAB >= 0 && dotBC >= 0 && dotCA >= 0) {
                //TODO idonkent detektal egy hibas metszespontot
                if (intersection != lowestPoint /*&& intersection != neighbourPoint*/) {
                    intersectPoints.push_back(intersection);
                }
            //}

        }
    }
    bool ret = false;
    /// A legkozelebbi metszespont kell nekunk
    for(auto &intersectPoint : intersectPoints){
        if(getDistance(neighbourPoint, intersectPoint) < getDistance(neighbourPoint, lowestPoint)){
            lowestPoint = intersectPoint;
            ret = true;
        }
    }
    return ret;
}