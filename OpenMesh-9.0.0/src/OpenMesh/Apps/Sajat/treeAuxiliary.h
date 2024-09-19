//
// Created by peros on 2024.04.03.
//
/**
 * A fa tamasz fuggvenyeit tartalmazo header file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#ifndef OPENMESH_TREEAUXILIARY_H
#define OPENMESH_TREEAUXILIARY_H
#include "auxiliary.h"
#include "columnAuxiliary.h"
#include <string>
#include "OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh"

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
Point* getNeighbour(int x, int y, int z, Point* p, std::vector<Point> &supportPointsAll, double l, double angle);

/**
 * A ket kapott pont kozotti tavolsagot adja vissza
 * @param p1 az elso pont
 * @param p2 a masodik pont
 * @return a ket pont kozotti tavolsag
 * @since 3.1
 */
double getDistance(Point &p1, Point &p2);

/**
 * A parameterkent kapott pontokat osszehasonlitja az y, x es z koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsYXZAll(const Point &p1, const Point &p2);

/**
 * A parameterkent kapott pontokat osszehasonlitja az x, y es z koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsXYZAll(const Point &p1, const Point &p2);

/**
 * A parameterkent kapott pontokat osszehasonlitja az z, x es y koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsZXYAll(const Point &p1, const Point &p2);

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
double dot(double Ax, double Ay, double Az, double Bx, double By, double Bz);

/**
 * Visszaadja a legnagyobb x erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb x erteke
 * @since 3.1
 */
double getMaxX(std::vector<Point> &supportPointsAll);

/**
 * Visszaadja a legnagyobb z erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb z erteke
 * @since 3.1
 */
double getMaxZ(std::vector<Point> &supportPointsAll);

/**
 * Visszaadja a legnagyobb y erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb y erteke
 * @since 3.1
 */
double getMaxY(std::vector<Point> &supportPointsAll);

/**
* Visszaadja a legkisebb y erteket
* @param supportPointsAll a pontok
* @return a legkisebb y erteke
 * @since 3.1
*/
double getMinY(std::vector<Point> &supportPointsAll);

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
                       double l, double groupingValue);

/**
 * A ket pont altam meghatarozott szakasz metszi-e az alakzatot
 * @param neighbourPoint a szakasz egyik pontja
 * @param lowestPoint a szakasz masik, also pontja
 * @param meshObject az alakzat
 * @param e a hibahatar
 * return a metszespont
 * @since 3.1
 */
Point passTheModel(Point& neighbourPoint, Point& lowestPoint, MyMesh& meshObject, double e);

#endif //OPENMESH_TREEAUXILIARY_H