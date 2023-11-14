/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Created by peros on 2023.10.23..
 *
 * Budapesti Muszaki es Gazdasagtudomanyi Egyetem
 * Villamosmernoki es Informatikai Kar
 * Itanyitastechnika es Informatika Tanszek
 * Temalaboratorium: 3D nyomtatas kulso alatamasztas
 *
 * 1. Feladatresz
 *
 * Feladat leirasa: egy fajbol betolt egy 3d alakzatot. Ezt bizonyos idokozonkent egy-egy fuggoleges vonallal elmetszi
 * es kiszamolja, hogy a vonal melyik ponokon metszi el. A bemeno és kimeno pontokat összekoti egy vonallal, ez megy a
 * kimeneti fileba. Tehat a vegeredmegy egy olyan fajl lesz, amiben függöleges vonalak vannak azokon a helyeken, ahol
 * az eredeti test belsejeben halad.
 *
 * 2. Feladatresz
 *
 * Feladat leirasa: a bemeneti pontokat a szomszedokkal ossze kell kotni, a szerint, hogy feljebb vannak-e.
 * Mindegyik el gyakorlatilag egy iranyitott vektor, aminek sulya is van, aszerint, hogy milyen meredeken halad felfele.
 * A kimenet egy elhalo a pontok kozott.
 *
 * 3. Feladatresz
 *
 * Feladat leirasa: az eleknel sulyt szamolunk a pontokra rekurzivan. A tamasz csak bizonyos sulyig tartja meg
 * utana uj pont kell. Ezeket a megtartott ponthalmazokat irjuk ki a tamaszponttal.
 *
 * Felhasznalt anyagok: OpenMesh Documentation, gpytoolbox.org, digitalocean.com, w3schools.com, stackoverflow.com,
 *                      geeksforgeeks.org, GitHub Copilot, ChatGTP,
 *                      [2020, Jang et al] Free-floating support structure generation
 *
 * @author Eros Pal
 * @since 2023.10.23.
 * ---------------------------------------------------------------------------------------------------------------------
*/

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "auxiliary.h"

/**
 * Makrok definialasa
 */
/// A tesztelheto alakzatok
//#define TEST_CUBE
#define TEST_BUNNY
//#define TEST_DIAMOND

/**
 * A feladat megvalositasa
 * @return
 * @since 1.1
 */
int main(){

    /// A be es kimeneti fileok nevei
#ifdef TEST_BUNNY
    std::string input_file = "bunny.obj";
#endif
#ifdef TEST_CUBE
    std::string input_file = "cube.obj";
#endif
#ifdef TEST_DIAMOND
    std::string input_file = "diamond.obj";
#endif

    /// A racspont osztas leptek merete es maximum kiterjedese
#ifdef TEST_BUNNY
    double l = 0.002;
#endif
#ifdef TEST_CUBE
    double l = 0.1;
#endif
#ifdef TEST_DIAMOND
    double l = 0.1;
#endif

    MyMesh mesh_object;
    readMesh(input_file, mesh_object);
    double maxWeight = 3;

    /// A metszespontok x, y, es z koordinatait tarolja
    std::vector<Point> intersect_points;

    int count = 0;

    /// Vegigmegy az osszes tarolt haromszogon
    for(MyMesh::FaceIter fi = mesh_object.faces_begin(); fi != mesh_object.faces_end(); fi++){
        MyMesh::FaceHandle fh = *fi;

        //Ezt a reszt nem szettem ki kulon fuggvenybe, mert percekkel(!) novelte a lefutasi idot
        /// Az aktualis haromszog csucspontjai
        Point p1, p2, p3;
        p1.coordinates[0] = 0;
        p1.coordinates[1] = 0;
        p1.coordinates[2] = 0;
        p1.e = l/100;
        p2.coordinates[0] = 0;
        p2.coordinates[1] = 0;
        p2.coordinates[2] = 0;
        p2.e = l/100;
        p3.coordinates[0] = 0;
        p3.coordinates[1] = 0;
        p3.coordinates[2] = 0;
        p3.e = l/100;
        int c =0;

        /// Kiaszamoljuk az aktualis haromszog csucspontjait
        for (MyMesh::FaceVertexIter fvi = mesh_object.fv_iter(fh); fvi.is_valid(); ++fvi) {
            MyMesh::VertexHandle vh = *fvi;
            // Biztos van elegansabb megoldas, nekem ez jutott most eszembe
            if(c == 0) {
                p1.coordinates[0] = mesh_object.point(vh)[0];
                p1.coordinates[1] = mesh_object.point(vh)[1];
                p1.coordinates[2] = mesh_object.point(vh)[2];
            } else if(c == 1) {
                p2.coordinates[0] = mesh_object.point(vh)[0];
                p2.coordinates[1] = mesh_object.point(vh)[1];
                p2.coordinates[2] = mesh_object.point(vh)[2];
            } else if(c == 2) {
                p3.coordinates[0] = mesh_object.point(vh)[0];
                p3.coordinates[1] = mesh_object.point(vh)[1];
                p3.coordinates[2] = mesh_object.point(vh)[2];
            }
            c++;
        }


        /// Kiszamoljuk a haromszog maximum x es z koordinatait
        double max_x = fmax(fmax(p1.coordinates[0], p2.coordinates[0]), p3.coordinates[0]);
        double max_z = fmax(fmax(p1.coordinates[2], p2.coordinates[2]), p3.coordinates[2]);
        double min_x = fmin(fmin(p1.coordinates[0], p2.coordinates[0]), p3.coordinates[0]);
        double min_z = fmin(fmin(p1.coordinates[2], p2.coordinates[2]), p3.coordinates[2]);

        /// Vegigmegyunk a potencialis racspontokon
        double x = std::floor(min_x/l)*l;
        while(x <= max_x){
            double z = std::floor(min_z/l)*l;
            while(z <= max_z) {
                /// Kiszamoljuk a haromszogek teruleteit
                double A = area(p1.coordinates[0], p1.coordinates[2],
                                p2.coordinates[0], p2.coordinates[2],
                                p3.coordinates[0], p3.coordinates[2]);

                if (A != 0) {
                    double A1 = area(x, z,
                                     p2.coordinates[0], p2.coordinates[2],
                                     p3.coordinates[0], p3.coordinates[2]);
                    double A2 = area(p1.coordinates[0], p1.coordinates[2],
                                     x, z,
                                     p3.coordinates[0], p3.coordinates[2]);
                    double A3 = area(p1.coordinates[0], p1.coordinates[2],
                                     p2.coordinates[0], p2.coordinates[2],
                                     x, z);

                    /// A b ertekek szamitasa
                    double b1 = A1 / A;
                    double b2 = A2 / A;
                    double b3 = A3 / A;
                    /// Ha nem negativ akkor a haromszogon belul van, tehat metszi
                    if (b1 > 0 && b2 > 0 && b3 > 0) {
                        double y = p1.coordinates[1] * b1 + p2.coordinates[1] * b2 + p3.coordinates[1] * b3;
                        Point p;
                        p.coordinates[0] = x;
                        p.coordinates[1] = y;
                        p.coordinates[2] = z;
                        intersect_points.push_back(p);
                        count++;
                    }
                }
                z = z + l;
                /// Ha 0, akkor tenyleg legyen nulla
                if (std::abs(z) < 1e-10) {
                    z = 0.0;
                }
            }
            x = x + l;
            /// Ha 0, akkor tenyleg legyen nulla
            if (std::abs(x) < 1e-10) {
                x = 0.0;
            }
        }

    }

    /// A metszespontok rendezese
    std::sort(intersect_points.begin(), intersect_points.end(), comparePoints);

    deleteWrongPoints(intersect_points);

    /// A bemeneti pontpok kozotti elek tarolasara szolgalo tomb
    /// @since 1.2
    std::vector<Edge> edges;

    /// A bemeneti pontok kozotti elek kiszamitasa
    for(int i = 0; i < (int)intersect_points.size(); i = i + 2){

        /// Vegigmegyunk az osszes lehetseges szomszedos ponton
        Point adjacentPoint{};
        double weight;
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2] - l;
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0];
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2] - l;
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2] - l;
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2];
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2];
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2] + l;
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0];
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2] + l;
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
        adjacentPoint.coordinates[0] = intersect_points[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersect_points[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersect_points[i].coordinates[2] + l;
        if(isIncluded(intersect_points, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersect_points, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersect_points[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.push_back(Edge{intersect_points[i], adjacentPoint, weight});
            }
        }
    }

    std::vector<Point> inputPoints;
    for(int i = 0; i < (int)intersect_points.size(); i = i + 2){
        inputPoints.push_back(intersect_points[i]);
    }


    std::vector<Point> inputPoints2;
    int c = 1;
    //--------------------------------------------------------------------------------------------
    inputPoints2 = setWeightAllPoint(edges, maxWeight, l/100);

    ///Az eredeti listabol kitorli azokat a pontokat, amiket mar alatamasztottunk
    for (const auto& point : inputPoints2) {
        auto iterator = std::find(inputPoints.begin(), inputPoints.end(), point);
        if (iterator != inputPoints.end()) {
            inputPoints.erase(iterator);
        }
    }

    std::string fileName = "output3";
    std::string obj = ".obj";
    fileName += std::to_string(c);
    fileName += obj;
    writePoints(fileName, input_file, c, inputPoints2);




/*    std::sort(inputPoints.begin(), inputPoints.end(), compareInputPoints);
    std::vector<Point> supportPoints;
    //supportPoints = supportPointDetection(inputPoints, d,edges, l/100);
    ///--------------------------------------------------------------------------------------------
//ezt a reszt a masik file fuggvenybol hoztam at hogy gyorsabb legyen, de meg nem jo
    std::vector<Point> B;
    std::vector<double> dist(inputPoints.size());
    std::set<int> Q;
//TODO nem teljesen ertem mit csinal
    for(int i = 0; i < (int)inputPoints.size(); i++){

        if(!inputPoints[i].weight) {
            continue;
        }
        B.push_back(inputPoints[i]);

        Q.clear();
        for(int j = 0; j < (int)inputPoints.size(); i++){
            dist[j] = INFINITY;
            Q.insert(j);
        }

        dist[i] = 0;

        while (!Q.empty()) {
            int u = *Q.begin();

            if(dist[u] > d){
                break;
            }
            Q.erase(u);

            for(int v = 0; v < (int)inputPoints.size(); v++){
                double d = dist[u] + penalty(inputPoints[u], inputPoints[v], edges, l/100);
                if(d < dist[v]){
                    Q.erase(v);
                    dist[v] = d;
                    Q.insert(v);
                }
            }

            inputPoints[u].weight = false;
        }
    }
*/



    //TODO kiszepiteni a kododt

    /// A kiiras a fileba
    writeInternalLines("output1.obj", input_file, intersect_points);
    writeInputEdges("output2.obj", input_file, edges);

    return 0;
}


