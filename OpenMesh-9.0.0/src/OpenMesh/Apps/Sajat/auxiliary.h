//
// Created by peros on 2023.11.04..
//
/**
 * A segedfuggvenyeket tarolo header file
 *
 * @author Eros Pal
 * @since 2023.11.04.
 */

#include <string>
#include "OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh"

#ifndef OPENMESH_AUXILIARY_H
#define OPENMESH_AUXILIARY_H

#endif //OPENMESH_AUXILIARY_H

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
    double e;

    bool operator==(const Point& other) const {
        return  std::abs(coordinates[0] - other.coordinates[0]) <= e &&
                std::abs(coordinates[1] - other.coordinates[1]) <= e &&
                std::abs(coordinates[2] - other.coordinates[2]) <= e;
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
};

/**
 * A parameterkent kapott tombot kiirja a .obj fileba
 * @param file_name a kimeneti file neve
 * @param intersect_points a metszespontok koordinatai
 * @since 1.1
 */
void writeInternalLines(const std::string& output_file_name, const std::string& input_file_name, std::vector<Point>& intersect_points);

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
 * @param intersect_points a pontok listaja
 * @param e a kuszobertek
 * @since 1.1
 */
void deleteWrongPoints(std::vector<Point> &intersect_points, double e);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param p a keresendo pont
 * @returnbenne van-e
 * @since 1.2
 */
bool isIncluded(std::vector<Point> &intersect_points, const Point &p, double l);

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param output_file_name a kimeneti file neve
 * @param input_file_name a bemeneti file neve
 * @param edges a kiirando elek
 * @since 1.2
 */
void writeInputEdges(const std::string& output_file_name, const std::string& input_file_name, std::vector<Edge>& edges);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param p a keresendo pont
 * @return visszaadja az y koordinatat, ha nincs benne, akkopr 0.0
 * @since 1.2
 */
double getY(std::vector<Point> &intersect_points, const Point &p, double l);

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareInputPoints(const Point &p1, const Point &p2);

/**
 * Kikeresi az elek kozul a szomszedosakat
 * @param p a kiindulopont
 * @param edges az elek
 * @param e a kuszobertek
 * @return a szomszedos pontokhoz vezeto elek
 * @since 1.3
 */
std::vector<Edge> findAdjacentPoints(Point& p, const std::vector<Edge>& edges, double e);

/**
 * Visszaadja a ket pont kozott levo el sulyat
 * @param p1 az eslo pont
 * @param p2 a szomszedos pont
 * @param edges az elek
 * @param e a kuszobertek
 * @return az el sulya
 */
double penalty(const Point &p1, const Point &p2, std::vector<Edge> &edges, double e);

/**
 * Visszaadja az alatamsaztando pontokat
 * @param A a rendezett bemeno pontok
 * @param D a tavolsag vagy suly kuszoberteke
 * @param edges az elek
 * @param e a kuszobertek
 * @return az alatamsaztando pontok listaja
 * @since 1.3
 */
std::vector<Point> supportPointDetection(std::vector<Point> &A, double D, std::vector<Edge> &edges, double e);

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
 * @param input_file_name az eredeti alakzat neve
 * @param count a szamlalo erteke
 * @param points a pontok
 * @since 1.3
 */
void writePoints(const std::string& output_file_name, const std::string& input_file_name, int count, std::vector<Point>& points);