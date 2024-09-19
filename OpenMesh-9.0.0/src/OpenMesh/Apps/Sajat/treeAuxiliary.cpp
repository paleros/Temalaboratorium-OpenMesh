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
 * return a metszespont
 * @since 3.1
 */
Point passTheModel(Point& neighbourPoint, Point& lowestPoint, MyMesh& meshObject, double e){
    neighbourPoint.e = e;
    lowestPoint.e = e;

    std::vector<Point> intersectPoints;

    /// Vegigmegy az osszes tarolt haromszogon
    for(MyMesh::FaceIter fi = meshObject.faces_begin(); fi != meshObject.faces_end(); fi++) {
        MyMesh::FaceHandle fh = *fi;

        /// Az aktualis haromszog csucspontjai
        Point a;
        Point b;
        Point c;
        int counter = 0;

        /// Kiaszamoljuk az aktualis haromszog csucspontjait
        for (MyMesh::FaceVertexIter fvi = meshObject.fv_iter(fh); fvi.is_valid(); ++fvi) {
            MyMesh::VertexHandle vh = *fvi;
            // Biztos van elegansabb megoldas, nekem ez jutott most eszembe
            if (counter == 0) {
                a.coordinates[0] = meshObject.point(vh)[0];
                a.coordinates[1] = meshObject.point(vh)[1];
                a.coordinates[2] = meshObject.point(vh)[2];
                a.e = e;
            } else if (counter == 1) {
                b.coordinates[0] = meshObject.point(vh)[0];
                b.coordinates[1] = meshObject.point(vh)[1];
                b.coordinates[2] = meshObject.point(vh)[2];
                b.e = e;
            } else if (counter == 2) {
                c.coordinates[0] = meshObject.point(vh)[0];
                c.coordinates[1] = meshObject.point(vh)[1];
                c.coordinates[2] = meshObject.point(vh)[2];
                c.e = e;
            }
            counter++;
        }

        /// Az aktualis haromszog es a szakasz metszespontjanak kiszamitasa, ha letezik

        /// A haromszog normalvektora
        Point normal;
        normal = crossProduct(b - a, c - a);

        double d = dotProduct(normal, a);

        /// A szakasz iranyvektora
        Point direction;
        direction = lowestPoint - neighbourPoint;
        direction.e = e;

        /// Az egyenes parametere ahol metszi a haromszog sikjat
        double t = (d - dotProduct(normal, neighbourPoint)) / dotProduct(normal, direction);

        /// Ha a szakasz ket vegpontja kozott van az egyenesen a metszespont
        if (t >= 0 && t <= 1) {

            /// Az metszespont kiszamitasa
            Point intersection;
            intersection = neighbourPoint + (direction * t);
            intersection.e = e;

            /// A metszespont a haromszogon belul van-e
            Point AB = b - a;
            Point BC = c - b;
            Point CA = a - c;

            Point AI = intersection - a;
            Point BI = intersection - b;
            Point CI = intersection - c;

            Point cross1 = crossProduct(AB, AI);
            Point cross2 = crossProduct(BC, BI);
            Point cross3 = crossProduct(CA, CI);

            //TODO ez nem jelez jól?
            if (dotProduct(normal, cross1) >= 0 && dotProduct(normal, cross2) >= 0 && dotProduct(normal, cross3) >= 0) {
                if (intersection.coordinates[1] > lowestPoint.coordinates[1]) {
                    intersectPoints.push_back(intersection);
                }
            }

        }
    }

    // TODO ez csak tesztekeshez kell
    if (intersectPoints.size() > 0){
        printf("Metszespontok szama: %d\n", intersectPoints.size());
    }

    /// A legkozelebbi metszespont kell nekunk
    for(auto &intersectPoint : intersectPoints){
        if(getDistance(neighbourPoint, intersectPoint) < getDistance(neighbourPoint, lowestPoint)){
            lowestPoint = intersectPoint;
            return intersectPoint;
        }
    }
    Point badPoint;
    badPoint.coordinates[0] = 0;
    badPoint.coordinates[1] = 0;
    badPoint.coordinates[2] = 0;
    badPoint.e = -1;
    return badPoint;

}