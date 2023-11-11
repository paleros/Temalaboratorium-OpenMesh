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
 */
void readMesh(const std::string& file, MyMesh& mesh);

/**
 * A pontok koordinatainak tarolasara szolgalo struktura
 * [x, y, z]
 */
struct Point{
    double coordinates[3];
};

/**
 * Iranyitott elek tarolasara szolgalo struktura
 * p1 -> p2
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
 */
double area(double x1, double z1, double x2, double z2, double x3, double z3);

/**
* Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
* Elsonek az x koordinata alapjan, majd az z, majd az y koordinata alapjan
* @param p1 az eslo pont
* @param p2 a masodik pont
* @return az elso elem elobbre valo-e vagy sem
*/
bool comparePoints(const Point& p1, const Point& p2);

/**
 * Visszaadja a ket pont kozotti vektor sulyat
 * @param actualPoint aktualis pont
 * @param adjacentPoint szomszedos pont
 * @return 0 ha a ket pont kozti vektor szoge hatarerteken beluli, egyebkent a szog erteke lesz a suly, -1 ha alatta van
 */
double thisEdgeLeadsToPoint(const Point &actualPoint, const Point &adjacentPoint, double l);

/**
 * Kitorli a rossz, hibas pontokat (zajt)
 * (Egymast koveto pont paroknak meg kell egyeznie az x es z koordinatajuknak)
 * @param intersect_points a pontok listaja
 */
void deleteWrongPoints(std::vector<Point>& intersect_points);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param p a keresendo pont
 * @returnbenne van-e
 */
bool isIncluded(std::vector<Point> &intersect_points, const Point &p, double l);

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param output_file_name a kimeneti file neve
 * @param input_file_name a bemeneti file neve
 * @param edges a kiirando elek
 */
void writeInputEdges(const std::string& output_file_name, const std::string& input_file_name, std::vector<Edge>& edges);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param p a keresendo pont
 * @return visszaadja az y koordinatat, ha nincs benne, akkopr 0.0
 */
double getY(std::vector<Point> &intersect_points, const Point &p, double l);