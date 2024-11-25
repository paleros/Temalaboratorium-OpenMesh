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
#include <string>
#include "OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh"

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
 * Visszaadja a legnagyobb y erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb y erteke
 * @since 3.1
 */
double getMaxY(std::vector<Point> &supportPointsAll);

/**
* Visszaadja a legkisebb y erteket
* @param mesh az alakzat
* @return a legkisebb y erteke
 * @since 4.1
*/
double getMinY(MyMesh &mesh);

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

/**
 * DFS algoritmus, ami bejarja a grafot
 * @param node a kezdo csucs
 * @param graph a graf
 * @param visited a mar latogatott pontok
 * @param tree a fa
 * @since 3.1
 */
void dfs(const Point& node, const std::map<Point, std::vector<Edge>>& graph, std::set<Point>& visited, Tree& tree);

/**
 * Szetvalasztja a tamasz fakat
 * @param supportTree a tamasz fal elei
 * @param trees a tamasz fakra bontva
 * @since 3.1
 */
void separateTree(std::vector<Edge>& supportTree, std::vector<Tree>& trees);

/**
 * A f atamasz henger elemeit generalja le es irja ki a fajlba
 * @param file a kimeneti fajl
 * @param i az aktualis el
 * @param dUp a henger felso atmeroje
 * @param dDown a henger also atmeroje
 * @param n a mar kiirt pontok szama
 * @return a mar kiirt pontok szama
 * @since 3.1
 */
int calculateAndWriteCylinder(std::ofstream &file, Edge i, double dUp, double dDown, int n);

/**
 * A ket ponttal adott szakaszon halad feljebb
 * @param A a felso pont a szakaszon
 * @param B az also pont a szakaszon
 * @param distance az uj pont tavolsaga a B ponttol
 * @return az uj pont
 * @since 3.1
 */
Point upAlongTheSection(Point A, Point B, double distance);

/**
 * A fa tamasz eleit megvastagitja es kiirja a fajlba
 * @param outputFileName a kimeneti fajl neve
 * @param inputFileName a bemeneti fajl neve
 * @param supportTree a tamasz fa elei
 * @param minDiameter a tamasz minimalis vastagsaga
 * @param minY a legkisebb y ertek
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 3.1
 */
void
writeSupportTreeDynamic(const std::string &outputFileName, const std::string &inputFileName, std::vector<Tree> &trees,
                        double minY, double minDiameter, bool isFinish);

/**
 * Egy haromszog es egy szakasz metszespontjat adja vissza
 * @param p1 a szakasz egyik pontja
 * @param p2 a szakasz masik pontja
 * @param a a haromszog egyik pontja
 * @param b a haromszog masik pontja
 * @param c a haromszog harmadik pontja
 * @param intersect a metszespont
 * @since 4.1
 */
void intersectRayTriangle(Point& p1, Point& p2, Point& a, Point& b, Point& c, Point& intersect);

/**
 * Megnezi, hogy az el az alakzaton belul fut-e
 * @param meshObject az alakzat
 * @param p1 az el elso pontja
 * @param p2 az el masodik pontja
 * @return belul fut-e az el az alakzaton
 * @since 4.1
 */
bool isItInside(MyMesh &meshObject, Point &p1, Point &p2);

#endif //OPENMESH_TREEAUXILIARY_H