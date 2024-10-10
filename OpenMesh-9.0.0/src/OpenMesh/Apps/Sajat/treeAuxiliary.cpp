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
        if (t > 0 && t <= 1) {  // Azert felemas a feltetel, hogy a fenti vege ne befolyasolja

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

            if (dotProduct(normal, cross1) >= 0 && dotProduct(normal, cross2) >= 0 && dotProduct(normal, cross3) >= 0) {
                if (intersection.coordinates[1] > lowestPoint.coordinates[1]) {
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
    badPoint.coordinates[0] = 0;
    badPoint.coordinates[1] = 0;
    badPoint.coordinates[2] = 0;
    badPoint.e = -1;
    return badPoint;

}

/**
 * A fa tamasz eleit megvastagitja es kiirja a fajlba
 * @param outputFileName a kimeneti fajl neve
 * @param inputFileName a bemeneti fajl neve
 * @param supportTree a tamasz fa elei
 * @param diameter a tamasz vastagsaga
 * @param minY a legkisebb y ertek
 * @since 3.1
 */
void writeSupportTree(const std::string &outputFileName, const std::string &inputFileName,
                      std::vector<Edge> &supportTree, double diameter, double minY){

    double D = diameter;    /// Az also vastagsag
    double H = supportTree[0].p1.coordinates[1] - minY;     /// A fa maximum magassaga
    double del = diameter/10;   /// A minimalis atmero

    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

    int n = 0;

    for (auto & i : supportTree){
        //TODO a henger aljanak szelessege fuggjon a maximalis aktualis magassagtol
        double dUp = del + D - D * (i.p1.coordinates[1] - minY) / H;
        double dDown = del + D - D * (i.p2.coordinates[1] - minY) / H;

        /// A korlap pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
        double deltaX1, deltaX2, deltaX3, deltaZ1, deltaZ2, deltaZ3;
        /// A masodik korlap pont
        double xNull, zNull;
        /// A korlap kozeppontja
        double x, y, z;

        /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
        x = i.p2.coordinates[0];
        z = i.p2.coordinates[2];
        zNull = i.p2.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * (dDown / 2);
        xNull = i.p2.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * (dDown / 2);
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = i.p2.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * (dDown / 2);
        xNull = i.p2.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * (dDown / 2);
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = i.p2.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * (dDown / 2);
        xNull = i.p2.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * (dDown / 2);
        deltaX3 = xNull - x;
        deltaZ3 = zNull - z;
        y = i.p2.coordinates[1];

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
        x = i.p1.coordinates[0];
        z = i.p1.coordinates[2];
        zNull = i.p1.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * (dUp / 2);
        xNull = i.p1.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * (dUp / 2);
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = i.p1.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * (dUp / 2);
        xNull = i.p1.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * (dUp / 2);
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = i.p1.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * (dUp / 2);
        xNull = i.p1.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * (dUp / 2);
        deltaX3 = xNull - x;
        deltaZ3 = zNull - z;
        y = i.p1.coordinates[1];

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
    }
    file.close();
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

    for (const auto& edge : graph.at(node)) {
        const Point& neighbor = (edge.p1 == node) ? edge.p2 : edge.p1;

        if (visited.find(neighbor) == visited.end()) {
            tree.edges.push_back(edge);
            dfs(neighbor, graph, visited, tree);
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
        double max = tree.edges[0].p1.coordinates[1];
        double min = tree.edges[0].p2.coordinates[1];

        for (auto & edge : tree.edges){
            if (edge.p1.coordinates[1] > max){
                max = edge.p1.coordinates[1];
            }
            if (edge.p2.coordinates[1] < min){
                min = edge.p2.coordinates[1];
            }
        }
        tree.height = max - min;
        //TODO a magassagot szerintem nem jol szamolja
    }
}

/**
 * A f atamasz henger elemeit generalja le es irja ki a fajlba
 * @param file a kimeneti fajl
 * @param i az aktualis el
 * @param dUp a henger felso atmeroje
 * @param dDown a henger also atmeroje
 * @param n a mar kiirt pontok szama
 * @param yUpCorrection a felso korlap pontjainak y koordinata korrigalasa
 * @param yDownCorrection  az also korlap pontjainak y koordinata korrigalasa
 * @param minY a targyasztal szintje
 * @return a mar kiirt pontok szama
 * @since 3.1
 */
int calculateAndWriteCylinder(std::ofstream &file, Edge i, double dUp, double dDown, int n,
                              double yUpCorrection, double yDownCorrection, double minY){
/// A korlap pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
    double deltaX1, deltaX2, deltaX3, deltaZ1, deltaZ2, deltaZ3;
    /// A masodik korlap pont
    double xNull, zNull;
    /// A korlap kozeppontja
    double x, y, z;

    /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
    x = i.p2.coordinates[0];
    z = i.p2.coordinates[2];
    zNull = i.p2.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * (dDown / 2);
    xNull = i.p2.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * (dDown / 2);
    deltaX1 = xNull - x;
    deltaZ1 = zNull - z;
    zNull = i.p2.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * (dDown / 2);
    xNull = i.p2.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * (dDown / 2);
    deltaX2 = xNull - x;
    deltaZ2 = zNull - z;
    zNull = i.p2.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * (dDown / 2);
    xNull = i.p2.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * (dDown / 2);
    deltaX3 = xNull - x;
    deltaZ3 = zNull - z;
    y = i.p2.coordinates[1] + yDownCorrection;

    /// Ha a talajt erinti a tamasz, akkor rovidebb a talp miatt
    if(i.e == -2 && yUpCorrection == 0){
        y = minY + dDown *2;
    }

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
    x = i.p1.coordinates[0];
    z = i.p1.coordinates[2];
    zNull = i.p1.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * (dUp / 2);
    xNull = i.p1.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * (dUp / 2);
    deltaX1 = xNull - x;
    deltaZ1 = zNull - z;
    zNull = i.p1.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * (dUp / 2);
    xNull = i.p1.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * (dUp / 2);
    deltaX2 = xNull - x;
    deltaZ2 = zNull - z;
    zNull = i.p1.coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * (dUp / 2);
    xNull = i.p1.coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * (dUp / 2);
    deltaX3 = xNull - x;
    deltaZ3 = zNull - z;
    y = i.p1.coordinates[1] + yUpCorrection;

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
 * A fa tamasz eleit megvastagitja es kiirja a fajlba
 * @param outputFileName a kimeneti fajl neve
 * @param inputFileName a bemeneti fajl neve
 * @param supportTree a tamasz fa elei
 * @param minDiameter a tamasz minimalis vastagsaga
 * @param minY a legkisebb y ertek
 * @since 3.1
 */
void writeSupportTreeDynamic(const std::string &outputFileName, const std::string &inputFileName,
                             std::vector<Tree> &trees, double minY, double minDiameter) {

    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

    int n = 0;

    for (auto &tree : trees) {

        double D = tree.height / 12;    /// Az also vastagsag
        double H = tree.height;     /// A fa maximum magassaga
        double del = D/10;   /// A minimalis atmero

        for (auto &i: tree.edges) {
            //TODO a henger szelessege nem mindig jo

            double dUp = del + D - D * (i.p1.coordinates[1] - minY) / H;
            double dDown = del + D - D * (i.p2.coordinates[1] - minY) / H;

            /// A minimalis atmero
            if (dUp < minDiameter) {
                dUp = minDiameter;
            }
            if (dDown < minDiameter) {
                dDown = minDiameter;
            }

            n = calculateAndWriteCylinder(file, i, dUp, dDown, n, 0, 0, minY);

            if (i.e == -1) {    /// Ha nem metszi az alakzatot, es nem a talajra tamaszt
                continue;
            } else if(i.e == -2){   /// Ha a talajra tamaszt

                dUp = del + D - D * (i.p2.coordinates[1] - minY) / H;
                dDown = dDown * 2;

                /// A minimalis atmero
                if (dUp < minDiameter) {
                    dUp = minDiameter;
                }

                n = calculateAndWriteCylinder(file, i, dUp, dDown, n, -i.p1.coordinates[1] + minY + dDown,
                                              0, minY);

            } else {    /// Azaz a tamasz metszi az alakzatot
                //TODO alakzat metszese eseten a tamasz elvekonyitasa
            }
        }
    }
    file.close();
}