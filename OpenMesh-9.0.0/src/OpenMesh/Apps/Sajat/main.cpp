/**
 * Created by peros on 2023.10.23..
 *
 * Budapesti Muszaki es Gazdasagtudomanyi Egyetem
 * Villamosmernoki es Informatikai Kar
 * Itanyitastechnika es Informatika Tanszek
 * Temalaboratorium: 3D nyomtatas kulso alatamasztas
 *
 * 1. Feladatresz
 *
 * Feladat leirasa: egy fajbol betolt egy 3d alakzatot. Ezt bizonyos idokozonkent egy-egy fuggoleges vonallal elmetszi es
 * kiszamolja, hogy a vonal melyik ponokon metszi el. A bemeno és kimeno pontokat összekoti egy vonallal, ez megy a
 * kimeneti fileba. Tehat a vegeredmegy egy olyan fajl lesz, amiben függöleges vonalak vannak azokon a helyeken, ahol
 * az eredeti test belsejeben halad.
 *
 * 2. Feladatresz
 *
 * Feladat leirasa: a bemeneti pontokat a szomszedokkal ossze kell kotni, a szerint, hogy feljebb vannak-e.
 * Mindegyik el gyakorlatilag egy iranyitott vektor, aminek sulya is van, aszerint, hogy milyen meredeken halad felfele.
 * A kimenet egy elhalo a pontok kozott.
 *
 * Felhasznalt anyagok: OpenMesh Documentation, gpytoolbox.org, digitalocean.com, w3schools.com, stackoverflow.com,
 *                      geeksforgeeks.org, GitHub Copilot, ChatGTP
 *
 * @author Eros Pal
 * @since 2023.10.23.
*/

#include <iostream>
#include <fstream>
#include <tuple>
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

    std::string output_file = "output.obj";

    /// A racspont osztas leptek merete es maximum kiterjedese
#ifdef TEST_BUNNY
    double l = 0.005;
    double max = 0.2;
#endif
#ifdef TEST_CUBE
    double l = 0.1;
    double max = 1;
#endif
#ifdef TEST_DIAMOND
    double l = 0.1;
    double max = 2;
#endif

    MyMesh mesh_object;
    readMesh(input_file, mesh_object);


    /// A metszespontok x, y, es z koordinatait tarolja
    std::vector<Point> intersect_points((int)(max/l)*(int)(max/l)*4);

    int count = 0;

    /// Vegigmegy az osszes tarolt haromszogon
    for(MyMesh::FaceIter fi = mesh_object.faces_begin(); fi != mesh_object.faces_end(); fi++){
        MyMesh::FaceHandle fh = *fi;

        /// Az aktualis haromszog csucspontjai
        Point p1 = {0,0,0};
        Point p2 = {0,0,0};
        Point p3 = {0,0,0};
        int c =0;

        /// Kiaszamoljuk az aktualis haromszog csucspontjait
        for (MyMesh::FaceVertexIter fvi = mesh_object.fv_iter(fh); fvi.is_valid(); ++fvi) {
            MyMesh::VertexHandle vh = *fvi;
            // Biztos van elegansabb megoldas, nekem ez jutott most eszembe
            if(c == 0) {
                p1 = {mesh_object.point(vh)[0],
                      mesh_object.point(vh)[1],
                      mesh_object.point(vh)[2]};
            } else if(c == 1) {
                p2 = {mesh_object.point(vh)[0],
                      mesh_object.point(vh)[1],
                      mesh_object.point(vh)[2]};
            } else if(c == 2) {
                p3 = {mesh_object.point(vh)[0],
                      mesh_object.point(vh)[1],
                      mesh_object.point(vh)[2]};
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
                double A = area(p1.coordinates[0], 0,p1.coordinates[2],
                                p2.coordinates[0], 0, p2.coordinates[2],
                                p3.coordinates[0], 0, p3.coordinates[2]);

                if (A != 0) {
                    double A1 = area(x, 0, z,
                                     p2.coordinates[0], 0, p2.coordinates[2],
                                     p3.coordinates[0], 0, p3.coordinates[2]);
                    double A2 = area(p1.coordinates[0], 0, p1.coordinates[2],
                                     x, 0, z,
                                     p3.coordinates[0], 0, p3.coordinates[2]);
                    double A3 = area(p1.coordinates[0], 0, p1.coordinates[2],
                                     p2.coordinates[0], 0, p2.coordinates[2],
                                     x, 0, z);

                    /// A b ertekek szamitasa
                    double b1 = A1 / A;
                    double b2 = A2 / A;
                    double b3 = A3 / A;
                    /// Ha nem negativ akkor a haromszogon belul van, tehat metszi
                    if (b1 > 0 && b2 > 0 && b3 > 0) {
                        double y = p1.coordinates[1] * b1 + p2.coordinates[1] * b2 + p3.coordinates[1] * b3;
                        intersect_points[count].coordinates[0] = x;
                        intersect_points[count].coordinates[1] = y;
                        intersect_points[count].coordinates[2] = z;
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
/*    std::vector<Edge> edges((int)(max/l)*(int)(max/l)*4);

    int ii = 0;
    /// A bemeneti pontok kozotti elek kiszamitasa
    for(int i = 0; i < (int)intersect_points.size(); i = i+2){
        int k;
        /// Vegigmegyunk az osszes szomszedos ponton
        for(int j = 0; j < 8; j++) {
            // Ezt biztos lehetne szebben
            switch (j) {
                case 0:
                    k = (int)(i-((max/l)-3));
                    break;
                case 1:
                    k = (int)(i-((max/l)-2));
                    break;
                case 2:
                    k = (int)(i-((max/l)-1));
                    break;
                case 3:
                    k = (int)i-1;
                    break;
                case 4:
                    k = (int)i+1;
                    break;
                case 5:
                    k = (int)(i+((max/l)-1));
                    break;
                case 6:
                    k = (int)(i+((max/l)));
                    break;
                case 7:
                    k = (int)(i+((max/l)+1));
                    break;
                default:
                    break;
            }
            if (thisEdgeLeadsToPoint(intersect_points[i], intersect_points[k]) != -1) {
                edges[ii].p1 = intersect_points[i];
                edges[ii].p2 = intersect_points[k];
                edges[ii].weight = thisEdgeLeadsToPoint(intersect_points[i], intersect_points[k]);
                //TODO valami malloc hiba van!
                ii++;
            }
        }
    }
    //TODO fuggvenyekbe attenni mindent

    //TODO a pontokat ossze kell kotni es ki kell rajzolni

    //TODO a debuggolashoz
    for (auto & edge : edges) {
        if (edge.weight != 0) {
            std::cout << edge.p1.coordinates[0] << " " << edge.p1.coordinates[1] << " " << edge.p1.coordinates[2] << " " << edge.p2.coordinates[0] << " " << edge.p2.coordinates[1] << " " << edge.p2.coordinates[2] << " " << edge.weight << std::endl;
        }
    }

*/


    /// A kiiras a fileba
    writeVertices("output.obj", input_file, intersect_points);

    return 0;
}


