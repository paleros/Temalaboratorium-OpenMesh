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
 * @param FILE a beolvasando file
 * @param mesh a mesh, amibe tarolni kell az adatokat
 * @since 1.1
 */
void readMesh(const std::string& FILE, MyMesh& mesh);

/**
 * A pontok koordinatainak tarolasara szolgalo struktura
 * [x, y, z]
 * @since 1.1
 */
struct Point{
    double _coordinates[3]{};
    double _weight = -1.0;
    double _e{};

    bool operator==(const Point& OTHER) const {
        return std::abs(_coordinates[0] - OTHER._coordinates[0]) <= _e &&
               std::abs(_coordinates[1] - OTHER._coordinates[1]) <= _e &&
               std::abs(_coordinates[2] - OTHER._coordinates[2]) <= _e;
    }

    /// @since 3.1
    bool operator!=(const Point& OTHER) const {
        return  !(std::abs(_coordinates[0] - OTHER._coordinates[0]) <= _e &&
                  std::abs(_coordinates[1] - OTHER._coordinates[1]) <= _e &&
                  std::abs(_coordinates[2] - OTHER._coordinates[2]) <= _e);
    }

    Point operator-(const Point& OTHER) const {
        Point p;
        p._coordinates[0] = _coordinates[0] - OTHER._coordinates[0];
        p._coordinates[1] = _coordinates[1] - OTHER._coordinates[1];
        p._coordinates[2] = _coordinates[2] - OTHER._coordinates[2];
        return p;
    }

    Point operator+(const Point& OTHER) const {
        Point p;
        p._coordinates[0] = _coordinates[0] + OTHER._coordinates[0];
        p._coordinates[1] = _coordinates[1] + OTHER._coordinates[1];
        p._coordinates[2] = _coordinates[2] + OTHER._coordinates[2];
        return p;
    }

    Point operator*(const double& OTHER) const {
        Point p;
        p._coordinates[0] = _coordinates[0] * OTHER;
        p._coordinates[1] = _coordinates[1] * OTHER;
        p._coordinates[2] = _coordinates[2] * OTHER;
        return p;
    }

    bool operator<(const Point& OTHER) const {
        if (std::abs(_coordinates[0] - OTHER._coordinates[0]) > _e) {
            return _coordinates[0] < OTHER._coordinates[0];
        }
        if (std::abs(_coordinates[1] - OTHER._coordinates[1]) > _e) {
            return _coordinates[1] < OTHER._coordinates[1];
        }
        return _coordinates[2] < OTHER._coordinates[2];
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
        return std::abs(p1._coordinates[0] - other.p1._coordinates[0]) <= e &&
               std::abs(p1._coordinates[1] - other.p1._coordinates[1]) <= e &&
               std::abs(p1._coordinates[2] - other.p1._coordinates[2]) <= e &&
               std::abs(p2._coordinates[0] - other.p2._coordinates[0]) <= e &&
               std::abs(p2._coordinates[1] - other.p2._coordinates[1]) <= e &&
               std::abs(p2._coordinates[2] - other.p2._coordinates[2]) <= e &&
                std::abs(weight - other.weight) <= e;
    }

    bool operator!=(const Edge& other) const {
        return  !(std::abs(p1._coordinates[0] - other.p1._coordinates[0]) <= e &&
                  std::abs(p1._coordinates[1] - other.p1._coordinates[1]) <= e &&
                  std::abs(p1._coordinates[2] - other.p1._coordinates[2]) <= e &&
                  std::abs(p2._coordinates[0] - other.p2._coordinates[0]) <= e &&
                  std::abs(p2._coordinates[1] - other.p2._coordinates[1]) <= e &&
                  std::abs(p2._coordinates[2] - other.p2._coordinates[2]) <= e &&
                std::abs(weight - other.weight) <= e);
    }

    /**
     * Az elek kezdopont magassag szerinti rendezesehez szukseges fuggveny
     */
    public: static bool sort(Edge e1, Edge e2) {
        return e1.p1._coordinates[1] > e2.p1._coordinates[1];
    }

};

/**
 * A parameterkent kapott bemeneti es kimeneti pontokat osszekoti fuggolegesen es kiirja a .obj fileba
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @param INPUT_FILE_NAME a bemeneti file neve
 * @param intersect_points a metszespontok koordinatai
 * @param desc a leiras
 * @param is_finish igaz, ha a vegso kiiratasrol van szo
 * @since 1.1
 */
void writeInternalLines(const std::string &OUTPUT_FILE_NAME, const std::string &INPUT_FILE_NAME,
                        std::vector<Point> &intersect_points, const std::string &desc, bool is_finish);

/**
 * Kiszamolja a haromszog teruletet
 * @param x_1 a kerdeses pont x koordinataja
 * @param z_1 a kerdeses pont z koordinataja
 * @param x_2 az masodik pont x koordinataja
 * @param z_2 az masodik pont z koordinataja
 * @param x_3 az harmadik pont x koordinataja
 * @param z_3 az harmadik pont z koordinataja
 * @return a haromszog terulete
 * @since 1.1
 */
double area(double x_1, double z_1, double x_2, double z_2, double x_3, double z_3);

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az x koordinata alapjan, majd az z, majd az y koordinata alapjan
 * @param p_1 az eslo pont
 * @param p_2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.1
 */
bool comparePoints(Point& p_1, Point& p_2);

/**
 * Visszaadja a ket pont kozotti vektor sulyat
 * @param ACTUAL_POINT aktualis pont
 * @param ADJACENT_POINT szomszedos pont
 * @return 0 ha a ket pont kozti vektor szoge hatarerteken beluli, egyebkent a szog erteke lesz a suly, -1 ha alatta van
 * @since 1.2
 */
double thisEdgeLeadsToPoint(const Point &ACTUAL_POINT, const Point &ADJACENT_POINT, double l);

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
 * @param P a keresendo pont
 * @param l a kuszobertek
 * @return benne van-e
 * @since 1.2
 */
bool isIncluded(std::vector<Point> &intersect_points, const Point &P, double l);

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @param INPUT_FILE_NAME a bemeneti file neve
 * @param edges a kiirando elek
 * @param is_finish igaz, ha a vegso kiiratasrol van szo
 * @since 1.2
 */
void writeInputEdges(const std::string &OUTPUT_FILE_NAME, const std::string &INPUT_FILE_NAME, std::vector<Edge> &edges,
                     bool is_finish);

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param P a keresendo pont
 * @return visszaadja az y koordinatat, ha nincs benne, akkopr 0.0
 * @since 1.2
 */
double getY(std::vector<Point> &intersect_points, const Point &P, double l);

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param P_1 az eslo pont
 * @param P_2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareInputPointsYXZ(const Point &P_1, const Point &P_2);

/**
 * Osszehasonlitja a ket kapott el kezdopontjanak koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareEdgesInputPoints(const Edge& E_1, const Edge& E_2);

/**
 * A parameterkent kapott eleket sorbarendezi a rendezo fuggveny alapjan
 * @param edges az elek
 * @param COMPARE a rendezo fuggveny
 * @since 4.1
 */
 void sort(std::vector<Edge> &edges, const std::function<bool(const Edge&, const Edge&)>& COMPARE);

/**
 * Kikeresi az elek kozul a pontot és visszaadja az indexet
 * @param points a pontok halmaza
 * @param p a keresett pont
 * @return
 */
int findPoint(std::vector<Point> &points, Point &p);

/**
 * Beallitja a pontoknak, hogy milyen a sulyuk
 * @param edges az elek
 * @param input_points a pontok
 * @param max_weight a maximalis suly
 * @return a pontok listaja, aminke a sulyuk nem tul nagy
 * @since 1.3
 */
std::vector<Point>
setWeightAllPointsAndGetSupportPoints(std::vector<Edge> &edges, std::vector<Point> &input_points, double max_weight);

/**
 * A parameterkent kapott pontokat kiirja a .obj fileba
 * @param file_name a kimeneti file neve
 * @param INPUT_FILE_NAME az eredeti alakzat neve
 * @param count a szamlalo erteke
 * @param points a pontok
 * @param is_finish igaz, ha a vegso kiiratasrol van szo
 * @since 1.3
 */
void
writePoints(const std::string &OUTPUT_FILE_NAME, const std::string &INPUT_FILE_NAME, int count, std::vector<Point> &points,
            bool is_finish);

/**
 * Kiirja a futtatas adatait a konzolra
 * @param INPUT_FILE_NAME
 */
void writeStartLog(const std::string &INPUT_FILE_NAME);

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
 * @param p_1 az elso pont
 * @param p_2 a masodik pont
 * @return a keresztszorzat
 * @since 3.1
 */
Point crossProduct(Point p_1, Point p_2);

/**
 * Skalaris szorzatot szamol ket pont kozott
 * @param p_1 az elso pont
 * @param p_2 a masodik pont
 * @return a skalaris szorzat
 * @since 3.1
 */
double dotProduct(Point &p_1, Point &p_2);

/**
 * Az alatamasztas fait tarolja el
 * @since 3.1
 */
struct Tree{
    std::vector<Edge> edges;
    double height;
};

/**
 * Megkeresi a minimum es a maximum ertekeket
 * @param mesh_object az alakzat
 * @param COORDINATE melyik tengely menten keressen
 * @return a minimum es a maximum ertekek egy vektorban
 * @since 4.1
 */
std::vector<double> findMinMax(MyMesh &mesh_object, const std::string& COORDINATE);

/**
 * Kiszamolja az alatamasztas ellenorzesi tavolsagat
 * @param l az alatamasztas ellenorzesi tavolsaga
 * @param diameter atamasz atmeroje
 * @param mesh_object az alakzat
 * @since 4.1
 */
void calculateDiameterAndL(double &l, double &diameter, MyMesh &mesh_object);

/**
 * A ket kapott pont kozotti tavolsagot adja vissza
 * @param p_1 az elso pont
 * @param p_2 a masodik pont
 * @return a ket pont kozotti tavolsag
 * @since 3.1
 */
double getDistance(Point &p_1, Point &p_2);

/**
 * Kiszamolja az elek ossztavolsagat
 * @param support_lines az alatamasztasi elek
 * @return az ossz tavolsag
 * @since 4.1
 */
double calculatePoint(std::vector<Point>& support_lines);

/**
 * Kiszamolja az elek ossztavolsagat
 * @param support_lines az alatamasztasi elek
 * @return az ossz tavolsag
 * @since 4.1
 */
double calculatePoint(std::vector<Edge>& support_lines);

/**
 * Kiirja a mesht a megadott fileba
 * @param mesh_object az alakzat
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @since 4.1
 */
void writeMesh(MyMesh &mesh_object, const std::string &OUTPUT_FILE_NAME);

/**
 * Az alatamasztas tipusa
 */
enum class SupportType {
    COLUMN,
    TREE
};

/**
 * Az optimalis forgatast kereso algoritmus tipusa
 */
enum class AlgorithmType {
    NELDERMEAD,
    DIRECT,
    NONE
};

#endif //OPENMESH_AUXILIARY_H