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
void writeVertices(const std::string& output_file_name, const std::string& input_file_name,std::vector<Point>& intersect_points);

/**
 * Kiszamolja a haromszog teruletet
 * @param x1 a kerdeses pont x koordinataja
 * @param y1 a kerdeses pont y koordinataja
 * @param z1 a kerdeses pont z koordinataja
 * @param x2 az masodik pont x koordinataja
 * @param y2 az masodik pont y koordinataja
 * @param z2 az masodik pont z koordinataja
 * @param x3 az harmadik pont x koordinataja
 * @param y3 az harmadik pont y koordinataja
 * @param z3 az harmadik pont z koordinataja
 * @return a haromszog terulete
 */
 double area(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);

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
double thisEdgeLeadsToPoint(const Point& actualPoint, const Point& adjacentPoint);

/**
 * Kitorli a rossz, hibas pontokat (zajt)
 * (Egymast koveto pont paroknak meg kell egyeznie az x es z koordinatajuknak)
 * @param intersect_points a pontok listaja
 */
void deleteWrongPoints(std::vector<Point>& intersect_points);

