//
// Created by peros on 2023.11.04.
//
/**
 * A segedfuggvenyeket tarolo header file
 *
 * @author Eros Pal
 * @since 2023.11.04.
 */

#ifndef OPENMESH_AUXILIARY_H
#define OPENMESH_AUXILIARY_H
#include <string>
#include "OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh"

typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;

/**
 * Beolvassa a megadott filet
 * @param file a beolvasando file
 * @param mesh a mesh, amibe tarolni kell az adatokat
 * @since 1.1
 */
void readMesh(const std::string& file, MyMesh& mesh);

/**
 * A pontok koordinatainak tarolasara szolgalo struktura
 * [x, y, z]
 * @since 1.1
 */
struct Point{
    double coordinates[3]{};
    double weight = -1.0;
    double e{};

    bool operator==(const Point& other) const {
        return  std::abs(coordinates[0] - other.coordinates[0]) <= e &&
                std::abs(coordinates[1] - other.coordinates[1]) <= e &&
                std::abs(coordinates[2] - other.coordinates[2]) <= e;
    }

    /// @since 3.1
    bool operator!=(const Point& other) const {
        return  !(std::abs(coordinates[0] - other.coordinates[0]) <= e &&
                std::abs(coordinates[1] - other.coordinates[1]) <= e &&
                std::abs(coordinates[2] - other.coordinates[2]) <= e);
    }

    Point operator-(const Point& other) const {
        Point p;
        p.coordinates[0] = coordinates[0] - other.coordinates[0];
        p.coordinates[1] = coordinates[1] - other.coordinates[1];
        p.coordinates[2] = coordinates[2] - other.coordinates[2];
        return p;
    }

    Point operator+(const Point& other) const {
        Point p;
        p.coordinates[0] = coordinates[0] + other.coordinates[0];
        p.coordinates[1] = coordinates[1] + other.coordinates[1];
        p.coordinates[2] = coordinates[2] + other.coordinates[2];
        return p;
    }

    Point operator*(const double& other) const {
        Point p;
        p.coordinates[0] = coordinates[0] * other;
        p.coordinates[1] = coordinates[1] * other;
        p.coordinates[2] = coordinates[2] * other;
        return p;
    }

};

/**
 * Iranyitott elek tarolasara szolgalo struktura
 * p1 -> p2
 * p1 es p2 a pontok, weight a sulyuk
 * @since 1.2
 */
struct Edge{
    Point p1;
    Point p2;
    double weight;
    double e;

    Edge(Point p1, Point p2, double weight, double e) : p1(p1), p2(p2), weight(weight), e(e) {}

    bool operator==(const Edge& other) const {
        return  std::abs(p1.coordinates[0] - other.p1.coordinates[0]) <= e &&
                std::abs(p1.coordinates[1] - other.p1.coordinates[1]) <= e &&
                std::abs(p1.coordinates[2] - other.p1.coordinates[2]) <= e &&
                std::abs(p2.coordinates[0] - other.p2.coordinates[0]) <= e &&
                std::abs(p2.coordinates[1] - other.p2.coordinates[1]) <= e &&
                std::abs(p2.coordinates[2] - other.p2.coordinates[2]) <= e &&
                std::abs(weight - other.weight) <= e;
    }

    bool operator!=(const Edge& other) const {
        return  !(std::abs(p1.coordinates[0] - other.p1.coordinates[0]) <= e &&
                std::abs(p1.coordinates[1] - other.p1.coordinates[1]) <= e &&
                std::abs(p1.coordinates[2] - other.p1.coordinates[2]) <= e &&
                std::abs(p2.coordinates[0] - other.p2.coordinates[0]) <= e &&
                std::abs(p2.coordinates[1] - other.p2.coordinates[1]) <= e &&
                std::abs(p2.coordinates[2] - other.p2.coordinates[2]) <= e &&
                std::abs(weight - other.weight) <= e);
    }

    /**
     * Az elek kezdopont magassag szerinti rendezesehez szukseges fuggveny
     */
    public: static bool sort(Edge e1, Edge e2) {
        return e1.p1.coordinates[1] > e2.p1.coordinates[1];
    }

};

/**
 * A parameterkent kapott bemeneti es kimeneti pontokat osszekoti fuggolegesen es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param intersectPoints a metszespontok koordinatai
 * @param desc a leiras
 * @since 1.1
 */
void writeInternalLines(const std::string &outputFileName, const std::string &inputFileName,
                        std::vector<Point> &intersectPoints, const std::string &desc);

/**
 * Kiszamolja a haromszog teruletet
 * @param x1 a kerdeses pont x koordinataja
 * @param z1 a kerdeses pont z koordinataja
 * @param x2 az masodik pont x koordinataja
 * @param z2 az masodik pont z koordinataja
 * @param x3 az harmadik pont x koordinataja
 * @param z3 az harmadik pont z koordinataja
 * @return a haromszog terulete
 * @since 1.1
 */
double area(double x1, double z1, double x2, double z2, double x3, double z3);

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az x koordinata alapjan, majd az z, majd az y koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.1
 */
bool comparePoints(const Point& p1, const Point& p2);

/**
 * Visszaadja a ket pont kozotti vektor sulyat
 * @param actualPoint aktualis pont
 * @param adjacentPoint szomszedos pont
 * @return 0 ha a ket pont kozti vektor szoge hatarerteken beluli, egyebkent a szog erteke lesz a suly, -1 ha alatta van
 * @since 1.2
 */
double thisEdgeLeadsToPoint(const Point &actualPoint, const Point &adjacentPoint, double l);

/**
 * Kitorli a rossz, hibas pontokat (zajt)
 * (Egymast koveto pont paroknak meg kell egyeznie az x es z koordinatajuknak)
 * @param intersectPoints a pontok listaja
 * @param e a kuszobertek
 * @since 1.1
 */
void deleteWrongPoints(std::vector<Point> &intersectPoints, double e);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersectPoints a pontok tombje
 * @param p a keresendo pont
 * @param l a kuszobertek
 * @return benne van-e
 * @since 1.2
 */
bool isIncluded(std::vector<Point> &intersectPoints, const Point &p, double l);

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param edges a kiirando elek
 * @since 1.2
 */
void writeInputEdges(const std::string& outputFileName, const std::string& inputFileName, std::vector<Edge>& edges);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersectPoints a pontok tombje
 * @param p a keresendo pont
 * @return visszaadja az y koordinatat, ha nincs benne, akkopr 0.0
 * @since 1.2
 */
double getY(std::vector<Point> &intersectPoints, const Point &p, double l);

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareInputPointsYXZ(const Point &p1, const Point &p2);

/**
 * Osszehasonlitja a ket kapott el kezdopontjanak koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareEdgesInputPoints(const Edge& e1, const Edge& e2);

/**
 * Kikeresi az elek kozul a pontot és visszaadja az indexet
 * @param points a pontok halmaza
 * @param p a keresett pont
 * @return
 */
int findPoint(std::vector<Point> &points, const Point &p);

/**
 * Beallitja a pontoknak, hogy milyen a sulyuk
 * @param edges az elek
 * @param inputPoints a pontok
 * @param maxWeight a maximalis suly
 * @return a pontok listaja, aminke a sulyuk nem tul nagy
 * @since 1.3
 */
std::vector<Point>
setWeightAllPointsAndGetSupportPoints(std::vector<Edge> &edges, std::vector<Point> &inputPoints, double maxWeight);

/**
 * A parameterkent kapott pontokat kiirja a .obj fileba
 * @param file_name a kimeneti file neve
 * @param inputFileName az eredeti alakzat neve
 * @param count a szamlalo erteke
 * @param points a pontok
 * @since 1.3
 */
void writePoints(const std::string& outputFileName, const std::string& inputFileName, int count, std::vector<Point>& points);

/**
 * A kapott mesh-ben minden pontnal az y es a z koordinatat felcsereli
 * @param mesh a mesh
 * @since 1.5
*/
void swapYZ(MyMesh& mesh);

/**
 * A parameterkent kapott pontok kozott haromszog alapu hasabokat csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a haromszog merete
 * @param minY a legkisebb y koordinata
 * @since 1.5
 */
void generateAndWriteSupportLines(const std::string &outputFileName, const std::string &inputFileName,
                                  std::vector<Point> &points, double diameter, double minY);

/**
 * A parameterkent kapott mesht kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param mesh a mesh
 */
void writeMesh(const std::string& outputFileName, MyMesh& mesh);

/**
 * A parameterkent kapott pontok kozott hengereket csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a henger atmeroje
 * @param minY a legkisebb y koordinata
 * @since 2.1
 */
void generateAndWriteSupportCylinder(const std::string &outputFileName, const std::string &inputFileName,
                                     std::vector<Point> &points, double diameter, double minY);

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
                                       OpenMesh::PolyMesh_ArrayKernelT<> meshObject);

/**
 * Kiirja a futtatas adatait a konzolra
 * @param inputFileName
 */
void writeStartLog(const std::string &inputFileName);

/**
 * Kiirja hogy keszen van
 */
void writeEndLog();

/**
 * Kiirja a logot a konsolra
 * @param log a kiirando szoveg
 */
void writeLog(const std::string &log);

/**
 * Keresztszorzatot szamol ket pont kozott
 * @param p1 az elso pont
 * @param p2 a masodik pont
 * @return a keresztszorzat
 * @since 3.1
 */
Point crossProduct(Point p1, Point p2);

/**
 * Skalaris szorzatot szamol ket pont kozott
 * @param p1 az elso pont
 * @param p2 a masodik pont
 * @return a skalaris szorzat
 * @since 3.1
 */
double dotProduct(Point &p1, Point &p2);

#endif //OPENMESH_AUXILIARY_H