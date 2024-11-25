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
#include <map>
#include <set>
#include "OpenMesh/Core/IO/MeshIO.hh"
#include "treeAuxiliary.h"
#include "auxiliary.h"

/**
 * A parameterkent kapott pontokat osszehasonlitja az y, x es z koordinatak alapjan
 * A nagyobb kerul elore
 * @param p1 az egyik pont
 * @param p2 a masik pont
 * @return igaz, ha az elso pont a nagyobb
 * @since 3.1
 */
bool compareInputPointsYXZAll(const Point &p1, const Point &p2){
    if (p1._coordinates[1] > p2._coordinates[1]) {
        return true;
    } else if (p1._coordinates[1] == p2._coordinates[1]) {
        if (p1._coordinates[0] > p2._coordinates[0]) {
            return true;
        } else if (p1._coordinates[0] == p2._coordinates[0]) {
            return p1._coordinates[2] > p2._coordinates[2];
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
 * Visszaadja a legnagyobb y erteket
 * @param supportPointsAll a pontok
 * @return a legnagyobb y erteke
 * @since 3.1
 */
double getMaxY(std::vector<Point> &supportPointsAll){
    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPointsYXZAll);
    return supportPointsAll[0]._coordinates[1];
}

/**
* Visszaadja a legkisebb y erteket
* @param mesh az alakzat
* @return a legkisebb y erteke
 * @since 4.1
*/
double getMinY(MyMesh &mesh){
    double minY = 1000000;
    for(auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it){
        if(mesh.point(*v_it)[1] < minY){
            minY = mesh.point(*v_it)[1];
        }
    }
    return minY;
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
        if (i._coordinates[0] <= actualPoint._coordinates[0] &&
            i._coordinates[0] >= actualPoint._coordinates[0] - l * groupingValue &&
            //i.coordinates[1] <= actualPoint.coordinates[1] &&
            i._coordinates[1] > actualPoint._coordinates[1] - l * groupingValue &&
            i._coordinates[2] <= actualPoint._coordinates[2] &&
            i._coordinates[2] >= actualPoint._coordinates[2] - l * groupingValue) {
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
    neighbourPoint._e = e;
    lowestPoint._e = e;

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
                a._coordinates[0] = meshObject.point(vh)[0];
                a._coordinates[1] = meshObject.point(vh)[1];
                a._coordinates[2] = meshObject.point(vh)[2];
                a._e = e;
            } else if (counter == 1) {
                b._coordinates[0] = meshObject.point(vh)[0];
                b._coordinates[1] = meshObject.point(vh)[1];
                b._coordinates[2] = meshObject.point(vh)[2];
                b._e = e;
            } else if (counter == 2) {
                c._coordinates[0] = meshObject.point(vh)[0];
                c._coordinates[1] = meshObject.point(vh)[1];
                c._coordinates[2] = meshObject.point(vh)[2];
                c._e = e;
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
        direction._e = e;

        /// Az egyenes parametere ahol metszi a haromszog sikjat
        double t = (d - dotProduct(normal, neighbourPoint)) / dotProduct(normal, direction);

        /// Ha a szakasz ket vegpontja kozott van az egyenesen a metszespont
        if (t > 0 && t <= 1) {  // Azert felemas a feltetel, hogy a fenti vege ne befolyasolja

            /// Az metszespont kiszamitasa
            Point intersection;
            intersection = neighbourPoint + (direction * t);
            intersection._e = e;

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

            if (dotProduct(normal, cross1) >= 0 && dotProduct(normal, cross2) >= 0 && dotProduct(normal, cross3) >= 0) {
                if (intersection._coordinates[1] > lowestPoint._coordinates[1]) {
                    intersectPoints.push_back(intersection);
                }
            }

        }
    }

    /// A legkozelebbi metszespont kell nekunk
    for(auto &intersectPoint : intersectPoints){
        if(getDistance(neighbourPoint, intersectPoint) < getDistance(neighbourPoint, lowestPoint)
           && getDistance(neighbourPoint, intersectPoint) >= 2 * e){
            lowestPoint = intersectPoint;
            return intersectPoint;
        }
    }
    Point badPoint;
    badPoint._coordinates[0] = 0;
    badPoint._coordinates[1] = 0;
    badPoint._coordinates[2] = 0;
    badPoint._e = -1;
    return badPoint;

}

/**
 * DFS algoritmus, ami bejarja a grafot
 * @param node a kezdo csucs
 * @param graph a graf
 * @param visited a mar latogatott pontok
 * @param tree a fa
 * @since 3.1
 */
void dfs(const Point& node, const std::map<Point, std::vector<Edge>>& graph, std::set<Point>& visited, Tree& tree){
    visited.insert(node);

    auto it = graph.find(node);
    if (it != graph.end()) {
        for (const auto& edge : it->second) {
            const Point& neighbor = (edge.p1 == node) ? edge.p2 : edge.p1;

            if (visited.find(neighbor) == visited.end()) {
                tree.edges.push_back(edge);
                dfs(neighbor, graph, visited, tree);
            }
        }
    }
}

/**
 * Szetvalasztja a tamasz fakat
 * @param supportTree a tamasz fal elei
 * @param trees a tamasz fakra bontva
 * @since 3.1
 */
void separateTree(std::vector<Edge>& supportTree, std::vector<Tree>& trees){

    /// Felepiti a grafot
    std::map<Point, std::vector<Edge>> graph;
    for (const auto& edge : supportTree) {
        graph[edge.p1].push_back(edge);
        graph[edge.p2].push_back(edge);
    }

    /// A mar meglatogatott pontok
    std::set<Point> visited;

    /// DFS algoritmus, ami bejarja a grafot
    for (const auto& edge : supportTree) {

        /// Szakasz kezdopont eseteben
        if (visited.find(edge.p1) == visited.end()) {
            Tree tree;
            tree.height = 0;

            dfs(edge.p1, graph, visited, tree);

            trees.push_back(tree);
        }

        /// Szakasz vegpont eseteben
        if (visited.find(edge.p2) == visited.end()) {
            Tree tree;
            tree.height = 0;
            dfs(edge.p2, graph, visited, tree);
            trees.push_back(tree);
        }
    }

    /// A fa magassaganak kiszamitasa
    for (auto & tree : trees){
        if (tree.edges.empty()){
            tree.height = 0;
            continue;
        }
        double max = tree.edges[0].p1._coordinates[1];
        double min = tree.edges[0].p2._coordinates[1];

        for (auto & edge : tree.edges){
            if (edge.p1._coordinates[1] > max){
                max = edge.p1._coordinates[1];
            }
            if (edge.p2._coordinates[1] < min){
                min = edge.p2._coordinates[1];
            }
        }
        tree.height = max - min;
    }
}

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
int calculateAndWriteCylinder(std::ofstream &file, Edge i, double dUp, double dDown, int n) {
/// A korlap pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
    double deltaX1, deltaX2, deltaX3, deltaZ1, deltaZ2, deltaZ3;
    /// A masodik korlap pont
    double xNull, zNull;
    /// A korlap kozeppontja
    double x, y, z;

    /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
    x = i.p2._coordinates[0];
    z = i.p2._coordinates[2];
    zNull = i.p2._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * (dDown / 2);
    xNull = i.p2._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * (dDown / 2);
    deltaX1 = xNull - x;
    deltaZ1 = zNull - z;
    zNull = i.p2._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * (dDown / 2);
    xNull = i.p2._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * (dDown / 2);
    deltaX2 = xNull - x;
    deltaZ2 = zNull - z;
    zNull = i.p2._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * (dDown / 2);
    xNull = i.p2._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * (dDown / 2);
    deltaX3 = xNull - x;
    deltaZ3 = zNull - z;
    y = i.p2._coordinates[1];

    /// Elso negyed pontjai
    file << "v " << x + (dDown / 2) << " " << y << " " << z << "\n"; /// 1
    file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
    file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
    file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
    /// Masodik negyed pontjai
    file << "v " << x << " " << y << " " << z + (dDown / 2) << "\n"; /// 5
    file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
    file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
    file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
    /// Harmadik negyed pontjai
    file << "v " << x - (dDown / 2) << " " << y << " " << z << "\n"; /// 9
    file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
    file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
    file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
    /// Negyedik negyed pontjai
    file << "v " << x << " " << y << " " << z - (dDown / 2) << "\n"; /// 13
    file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
    file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
    file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

    /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
    x = i.p1._coordinates[0];
    z = i.p1._coordinates[2];
    zNull = i.p1._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * (dUp / 2);
    xNull = i.p1._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * (dUp / 2);
    deltaX1 = xNull - x;
    deltaZ1 = zNull - z;
    zNull = i.p1._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * (dUp / 2);
    xNull = i.p1._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * (dUp / 2);
    deltaX2 = xNull - x;
    deltaZ2 = zNull - z;
    zNull = i.p1._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * (dUp / 2);
    xNull = i.p1._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * (dUp / 2);
    deltaX3 = xNull - x;
    deltaZ3 = zNull - z;
    y = i.p1._coordinates[1];

    /// Elso negyed pontjai
    file << "v " << x + (dUp / 2) << " " << y << " " << z << "\n"; /// 1
    file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
    file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
    file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
    /// Masodik negyed pontjai
    file << "v " << x << " " << y << " " << z + (dUp / 2) << "\n"; /// 5
    file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
    file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
    file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
    /// Harmadik negyed pontjai
    file << "v " << x - (dUp / 2) << " " << y << " " << z << "\n"; /// 9
    file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
    file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
    file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
    /// Negyedik negyed pontjai
    file << "v " << x << " " << y << " " << z - (dUp / 2) << "\n"; /// 13
    file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
    file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
    file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

    for (int j = 1; j < 16; j++) {
        file << "f " << j + n << " " << j + n + 16 << " " << j + n + 1 << "\n";
        file << "f " << j + n + 1 + 16 << " " << j + n + 16 << " " << j + n + 1 << "\n";
    }
    file << "f " << n + 16 << " " << n + 32 << " " << n + 1 << "\n";
    file << "f " << n + 1 + 16 << " " << n + 32 << " " << n + 1 << "\n";

    n = n + 32;
    return n;
}

/**
 * A ket ponttal adott szakaszon halad feljebb
 * @param A a felso pont a szakaszon
 * @param B az also pont a szakaszon
 * @param distance az uj pont tavolsaga a B ponttol
 * @return az uj pont
 * @since 3.1
 */
Point upAlongTheSection(Point A, Point B, double distance){
    double ABDistance = getDistance(A, B);
    double ux = -(B._coordinates[0] - A._coordinates[0]) / ABDistance;
    double uy = -(B._coordinates[1] - A._coordinates[1]) / ABDistance;
    double uz = -(B._coordinates[2] - A._coordinates[2]) / ABDistance;

    Point C;
    C._coordinates[0] = A._coordinates[0] + distance * ux;
    C._coordinates[1] = A._coordinates[1] + distance * uy;
    C._coordinates[2] = A._coordinates[2] + distance * uz;
    return C;
}

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
                        double minY, double minDiameter, bool isFinish) {

    if (isFinish) {
        std::ofstream file(outputFileName);
        if (!file) {
            std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

        int n = 0;

        for (auto &tree: trees) {

            double D = tree.height / 12;    /// Az also vastagsag
            double H = tree.height;     /// A fa maximum magassaga
            double del = D / 10;   /// A minimalis atmero

            for (auto &i: tree.edges) {

                double dUp = del + D - D * (i.p1._coordinates[1] - minY) / H;
                double dDown = del + D - D * (i.p2._coordinates[1] - minY) / H;

                /// A minimalis atmero
                if (dUp < minDiameter) {
                    dUp = minDiameter;
                }
                if (dDown < minDiameter) {
                    dDown = minDiameter;
                }

                /// Ha kozvetlenul tamasztja az alakzatot
                if (i.p1._weight == -2 || i.p2._weight == -2) {
                    dUp = minDiameter;
                }

                if (i.e == -1) {    /// Ha nem metszi az alakzatot, es nem a talajra tamaszt

                    n = calculateAndWriteCylinder(file, i, dUp, dDown, n);

                } else if (i.e == -2) {   /// Ha a talajra tamaszt

                    dDown = dDown * 1.5;
                    Edge edge = i;
                    edge.p1 = upAlongTheSection(i.p2, i.p1, -dDown);
                    edge.p2 = i.p2;
                    edge.e = 0;
                    double dUpNew = del + D - D * (edge.p1._coordinates[1] - minY) / H;

                    /// A minimalis atmero
                    if (dUpNew < minDiameter) {
                        dUpNew = minDiameter;
                    }

                    i.p2 = edge.p1;
                    /// Kiirja a roviditett tamaszt
                    n = calculateAndWriteCylinder(file, i, dUp, dUpNew, n);
                    /// Kiirja a szelesitest
                    n = calculateAndWriteCylinder(file, edge, dUpNew, dDown, n);

                } else {    /// Azaz a tamasz metszi az alakzatot

                    dDown = dDown * 2;
                    Edge edge = i;
                    edge.p1 = upAlongTheSection(i.p2, i.p1, -dDown);
                    edge.p2 = i.p2;
                    edge.e = 0;
                    double dUpNew = del + D - D * (edge.p1._coordinates[1] - minY) / H;

                    /// A minimalis atmero
                    if (dUpNew < minDiameter) {
                        dUpNew = minDiameter;
                    }

                    i.p2 = edge.p1;
                    /// Kiirja a roviditett tamaszt
                    n = calculateAndWriteCylinder(file, i, dUp, dUpNew, n);
                    /// Kiirja a szelesitest
                    n = calculateAndWriteCylinder(file, edge, dUpNew, minDiameter, n);
                }
            }
        }
        file.close();
        writeLog("\tTreeSupportObjects written to file");
    }
}

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
void intersectRayTriangle(Point& p1, Point& p2, Point& a, Point& b, Point& c, Point& intersect) {
    Point normal = crossProduct(b - a, c - a);
    double d = dotProduct(normal, a);

    Point direction = p2 - p1;

    double t = (d - dotProduct(normal, p1)) / dotProduct(normal, direction);

    Point intersection = p1 + (direction * t);

    Point AB = b - a;
    Point BC = c - b;
    Point CA = a - c;

    Point AI = intersection - a;
    Point BI = intersection - b;
    Point CI = intersection - c;

    Point cross1 = crossProduct(AB, AI);
    Point cross2 = crossProduct(BC, BI);
    Point cross3 = crossProduct(CA, CI);

    if (dotProduct(normal, cross1) >= 0 && dotProduct(normal, cross2) >= 0 && dotProduct(normal, cross3) >= 0) {
        intersection._weight = 0;
        intersect = intersection;
    } else {
        Point badPoint;
        badPoint._coordinates[0] = 0;
        badPoint._coordinates[1] = 0;
        badPoint._coordinates[2] = 0;
        badPoint._weight = -1;
        intersect = badPoint;
    }
}

/**
 * Megnezi, hogy az el az alakzaton belul fut-e
 * @param meshObject az alakzat
 * @param p1 az el elso pontja
 * @param p2 az el masodik pontja
 * @return belul fut-e az el az alakzaton
 * @since 4.1
 */
bool isItInside(MyMesh &meshObject, Point &p1, Point &p2) {
    std::vector<Point> intersectPoints;

    for (auto f_it = meshObject.faces_begin(); f_it != meshObject.faces_end(); ++f_it) {
        /// A haromszog csucspontjainak kinyerese
        MyMesh::FaceVertexIter fv_it = meshObject.fv_iter(*f_it);
        Point a;
        a._coordinates[0] = meshObject.point(*fv_it)[0];
        a._coordinates[1] = meshObject.point(*fv_it)[1];
        a._coordinates[2] = meshObject.point(*fv_it)[2];
        ++fv_it;
        Point b;
        b._coordinates[0] = meshObject.point(*fv_it)[0];
        b._coordinates[1] = meshObject.point(*fv_it)[1];
        b._coordinates[2] = meshObject.point(*fv_it)[2];
        ++fv_it;
        Point c;
        c._coordinates[0] = meshObject.point(*fv_it)[0];
        c._coordinates[1] = meshObject.point(*fv_it)[1];
        c._coordinates[2] = meshObject.point(*fv_it)[2];

        /// Metszespont szamitasa
        Point intersect;
        intersectRayTriangle(p1, p2, a, b, c, intersect);
        if (intersect._weight == 0) {
            intersectPoints.push_back(intersect);
        }
    }


    int counter1 = 0;
    int counter2 = 0;

    /// Kiszamolja, hogy az adott szakasz hany ponton metszi az alakzatot es azt milyen aranyban elosztva a szakaszon
    for (auto & intersectPoint : intersectPoints){
        Point v, w;
        v._coordinates[0] = p2._coordinates[0] - p1._coordinates[0];
        v._coordinates[1] = p2._coordinates[1] - p1._coordinates[1];
        v._coordinates[2] = p2._coordinates[2] - p1._coordinates[2];
        w._coordinates[0] = intersectPoint._coordinates[0] - p1._coordinates[0];
        w._coordinates[1] = intersectPoint._coordinates[1] - p1._coordinates[1];
        w._coordinates[2] = intersectPoint._coordinates[2] - p1._coordinates[2];

        double dot = dotProduct(v, w);
        double len_sq = dotProduct(v, v);

        double t = dot / len_sq;

        double e = p1._e;
        if (t <= e) {
            counter1++;
        } else if (t >= 1 -e) {
            counter2++;
        }
    }

    /// Ha paros a metszespontok szama  a ket ponton kivul egy oldalt, akkor kivul van a szakasz, kulonben belul
    if (counter1 % 2 == 0 && counter2 % 2 == 0) {
        return false;
    } else {
        return true;
    }
}
//v -0.890486 5.49017 0.00936974
//v -0.890486 -0.0448766 0.00936974