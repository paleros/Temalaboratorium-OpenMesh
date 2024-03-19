/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Created by peros on 2023.10.23..
 *
 * Budapesti Muszaki es Gazdasagtudomanyi Egyetem (BME)
 * Villamosmernoki es Informatikai Kar (VIK)
 * Iranyitastechnika es Informatika Tanszek (IIT)
 * 1. Temalaboratorium: 3D nyomtatas kulso alatamasztas
 * 2. Onallo laboratorium: 3D nyomtatas - tartostrukturak generalasa es mas gyakorlati problemak
 *
 * 1.1. Feladatresz
 * Feladat leirasa: egy fajbol betolt egy 3d alakzatot. Ezt bizonyos idokozonkent egy-egy fuggoleges vonallal elmetszi
 * es kiszamolja, hogy a vonal melyik ponokon metszi el. A bemeno és kimeno pontokat összekoti egy vonallal, ez megy a
 * kimeneti fileba. Tehat a vegeredmegy egy olyan fajl lesz, amiben függöleges vonalak vannak azokon a helyeken, ahol
 * az eredeti test belsejeben halad.
 *
 * 1.2. Feladatresz
 * Feladat leirasa: a bemeneti pontokat a szomszedokkal ossze kell kotni, a szerint, hogy feljebb vannak-e.
 * Mindegyik el gyakorlatilag egy iranyitott vektor, aminek sulya is van, aszerint, hogy milyen meredeken halad felfele.
 * A kimenet egy elhalo a pontok kozott.
 *
 * 1.3. Feladatresz
 * Feladat leirasa: az eleknel sulyt szamolunk a pontokra rekurzivan. A tamasz csak bizonyos sulyig tartja meg
 * utana uj pont kell. Ezeket a megtartott ponthalmazokat irjuk ki a tamaszponttal.
 *
 * 1.4. Feladatresz
 * Feladat leirasa: a tamaszpontokból fuggoleges egyeneseket huzunk az elekzet aljaig.
 *
 * 1.5. Feladatresz
 * Feladat leirasa: a tamasz egyeneseket atalakitjuk harpmszog hasab alakzatta, hogy nyomtathato legyen.
 *
 * 2.1. Feladatresz
 * Feladat leirasa: a tamasz egyeneseket atalakitjuk henger alakzatta, hogy nyomtathato legyen. Ezt kovetoen a
 * tamaszokat osszekotjuk a stabilitas erdekeben.
 *
 * Felhasznalt anyagok: OpenMesh Documentation, gpytoolbox.org, digitalocean.com, w3schools.com, stackoverflow.com,
 *                      geeksforgeeks.org, GitHub Copilot, ChatGTP,
 *                      [2020, Jang et al] Free-floating support structure generation
 *
 * @author Eros Pal
 * @consulant Dr. Salvi Peter
 * @since 2024.02.17.
 * ---------------------------------------------------------------------------------------------------------------------
*/

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "auxiliary.h"

/**
 * Makrok definialasa
 */
/// A tesztelheto alakzatok
//#define TEST_BUNNY
//#define TEST_DIAMOND
#define TEST_SPHERE
//#define TEST_LUCY
//#define TEST_GYMNAST

/**
 * A feladat megvalositasa
 * @return
 * @since 1.1
 */
int main(){

    /// A be es kimeneti fileok nevei
#ifdef TEST_BUNNY
    std::string inputFile = "models/bunny.obj";
#endif
#ifdef TEST_DIAMOND
    std::string inputFile = "models/diamond.obj";
#endif
#ifdef TEST_SPHERE
    std::string inputFile = "models/sphere.obj";
#endif
#ifdef TEST_LUCY
    std::string inputFile = "models/lucy.obj";
#endif
#ifdef TEST_GYMNAST
    std::string inputFile = "models/gymnast.obj";
#endif


    /// A fajl beolvasasa
    MyMesh meshObject;
    readMesh(inputFile, meshObject);
    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter = 0.001;

    /// A racspont osztas leptek merete es maximum kiterjedese
#ifdef TEST_BUNNY
    double l = 0.006;
    diameter = 0.002;
#endif
#ifdef TEST_DIAMOND
    double l = 0.15; /// A sugarak kozti tavolsag
    diameter = 0.08;
    //swapYZ(meshObject);
    //writeMesh("models/diamond.obj", meshObject);
#endif
#ifdef TEST_SPHERE
    double l = 0.1;
    diameter = 0.05;
#endif

#ifdef TEST_LUCY
    double l = 40;
    diameter = 20;
    //swapYZ(meshObject);
    //writeMesh("models/lucy.obj", meshObject);
#endif
#ifdef TEST_GYMNAST
    double l = 0.2;
    diameter = 0.06;
    //swapYZ(meshObject);
    //writeMesh("models/lucy.obj", meshObject);
#endif

    writeStartLog(inputFile);
    double maxWeight = M_PI / 4 * 3 *2;

    /// A metszespontok x, y, es z koordinatait tarolja
    std::vector<Point> intersectPoints;

    int count = 0;

    writeLog("\tBasic parameters set");
    /// Vegigmegy az osszes tarolt haromszogon
    for(MyMesh::FaceIter fi = meshObject.faces_begin(); fi != meshObject.faces_end(); fi++){
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
        for (MyMesh::FaceVertexIter fvi = meshObject.fv_iter(fh); fvi.is_valid(); ++fvi) {
            MyMesh::VertexHandle vh = *fvi;
            // Biztos van elegansabb megoldas, nekem ez jutott most eszembe
            if(c == 0) {
                p1.coordinates[0] = meshObject.point(vh)[0];
                p1.coordinates[1] = meshObject.point(vh)[1];
                p1.coordinates[2] = meshObject.point(vh)[2];
            } else if(c == 1) {
                p2.coordinates[0] = meshObject.point(vh)[0];
                p2.coordinates[1] = meshObject.point(vh)[1];
                p2.coordinates[2] = meshObject.point(vh)[2];
            } else if(c == 2) {
                p3.coordinates[0] = meshObject.point(vh)[0];
                p3.coordinates[1] = meshObject.point(vh)[1];
                p3.coordinates[2] = meshObject.point(vh)[2];
            }
            c++;
        }


        /// Kiszamoljuk a haromszog maximum x es z koordinatait
        double maxX = fmax(fmax(p1.coordinates[0], p2.coordinates[0]), p3.coordinates[0]);
        double maxZ = fmax(fmax(p1.coordinates[2], p2.coordinates[2]), p3.coordinates[2]);
        double minX = fmin(fmin(p1.coordinates[0], p2.coordinates[0]), p3.coordinates[0]);
        double minZ = fmin(fmin(p1.coordinates[2], p2.coordinates[2]), p3.coordinates[2]);

        /// Vegigmegyunk a potencialis racspontokon
        double x = std::floor(minX / l) * l;
        while(x <= maxX){
            double z = std::floor(minZ / l) * l;
            while(z <= maxZ) {
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
                        intersectPoints.push_back(p);
                        count++;
                    }
                }
                z = z + l;
            }
            x = x + l;
        }

    }

    /// A metszespontok rendezese
    std::sort(intersectPoints.begin(), intersectPoints.end(), comparePoints);
    deleteWrongPoints(intersectPoints, l / 100);

    /// A kiiras a fileba
    writeInternalLines("outputs/1-internalLines.obj", inputFile, intersectPoints, "# Internal lines generated from ");
    writeLog("\tInternal lines written to file");

    /// A bemeneti pontpok kozotti elek tarolasara szolgalo tomb
    /// @since 1.2
    std::vector<Edge> edges;


    /// A bemeneti pontok kozotti elek kiszamitasa
    for(int i = 0; i < (int)intersectPoints.size(); i = i + 2){

        /// Vegigmegyunk az osszes lehetseges szomszedos ponton
        Point adjacentPoint{};
        double weight;
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] - l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0];
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] - l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] - l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2];
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2];
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] + l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0];
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] + l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] + l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, l/100);
            }
        }
    }

    writeInputEdges("outputs/2-inputEdges.obj", inputFile, edges);
    writeLog("\tInput edges written to file");

    /// Az alatamasztando pontok kiszamitasa
    /// @since 1.3
    std::vector<Point> inputPoints;
    for(auto & edge : edges){
        inputPoints.push_back(edge.p1);
        inputPoints.push_back(edge.p2);
    }
    std::sort(inputPoints.begin(), inputPoints.end(), comparePoints);
    for(int i = 0; i < (int)inputPoints.size(); i++){
        if(std::abs(inputPoints[i].coordinates[0] - inputPoints[i+1].coordinates[0]) <= l/100 &&
            std::abs(inputPoints[i].coordinates[1] - inputPoints[i+1].coordinates[1]) <= l/100 &&
            std::abs(inputPoints[i].coordinates[2] - inputPoints[i+1].coordinates[2]) <= l/100){
            inputPoints.erase(inputPoints.begin() + i);
            i--;
        }
    }

    std::vector<Point> supportPointsAll;
    int c = 1;
    while ((int)edges.size() > 0) {
        std::vector<Point> supportPoints;
        supportPoints = setWeightAllPointsAndGetSupportPoints(edges, inputPoints, maxWeight);

        /// Az elek listajabol kitorli azokat, amelyeket alatamasztottunk mar
        for (const auto & supportPoint : supportPoints){
            for(int j = 0; j < (int)edges.size(); j++){
                if(supportPoint == edges[j].p1){
                    edges.erase(edges.begin() + j);
                    j--;
                }
            }
        }

        for(int i = 0; i < (int) supportPoints.size(); i++){
            if(supportPoints[i].weight != 0){
                supportPoints.erase(supportPoints.begin() + i);
                i--;
            }
        }

        /// A pontok sulyait nullazza (illetve -1-eli)
        for (auto &inputPoint : inputPoints) {
            inputPoint.weight = -1;
        }

        if((int)supportPoints.size() == 0){
            break;
        }
        c++;
        for (const auto & supportPoint : supportPoints) {
            supportPointsAll.push_back(supportPoint);
        }
    }

    /// Kiirjuk az alatamasztando pontokat
    writePoints("outputs/3-supportedPoints.obj", inputFile, 0, supportPointsAll);
    writeLog("\tSupportedPoints written to file");


    /// Az alatamasztando pontokbol egyeneseket huzunk a legalso pont y koordinataja szerinti sikra
    /// @since 1.4

    std::sort(supportPointsAll.begin(), supportPointsAll.end(), compareInputPoints);
    std::vector<Point> supportLines;
    /// Kiszamoljuk, hogy meddig kell az egyeneseket huzni
    double minY = std::numeric_limits<double>::max(); // Kezdeti érték a minimális y koordinátának
    for (MyMesh::VertexIter v_it = meshObject.vertices_begin(); v_it != meshObject.vertices_end(); ++v_it) {
        MyMesh::Point p = meshObject.point(*v_it);
        if (p[1] < minY) {
            minY = p[1];
        }
    }

    for (const auto & supportPoint : supportPointsAll) {
        Point p;
        p.coordinates[0] = supportPoint.coordinates[0];
        p.coordinates[1] = supportPoint.coordinates[1];
        p.coordinates[2] = supportPoint.coordinates[2];
        p.e = supportPoint.e;
        supportLines.push_back(p);
        p.coordinates[1] = minY;

        /// Ha a metszespontok kozott van olyan kimeno pont, amelyik alacsonyabb, akkor azt a pontot csak addig huzzuk le
        for(int i = 1; i < (int)intersectPoints.size(); i = i + 2){
            if(std::abs(intersectPoints[i].coordinates[0] - supportPoint.coordinates[0]) <= l / 100 &&
               std::abs(intersectPoints[i].coordinates[2] - supportPoint.coordinates[2]) <= l / 100){
                if(intersectPoints[i].coordinates[1] > p.coordinates[1] && intersectPoints[i].coordinates[1] < supportPoint.coordinates[1]){
                    p.coordinates[1] = intersectPoints[i].coordinates[1];
                }
            }
        }
        supportLines.push_back(p);
    }

    writeInternalLines("outputs/4-supportLines.obj", inputFile, supportLines, "# Support lines generated from ");
    writeLog("\tSupportLines written to file");


    /// Az alatamasztando pontokat atalakitjuk alazatta, hogy nyomtathato legyen
    /// @since 1.5
    generateAndWriteSupportLines("outputs/5-triangleSupportObjects.obj", inputFile, supportLines, diameter, minY);
    writeLog("\tTriangleSupportObjects written to file");
    /// @since 2.1
    generateAndWriteSupportCylinder("outputs/6-cylinderSupportObjects.obj", inputFile, supportLines, diameter, minY);
    writeLog("\tCylinderSupportObjects written to file");
    generateAndWriteSupportCrossBrace("outputs/7-diagonalSupportObjects.obj", inputFile, supportLines, diameter, l, meshObject);
    writeLog("\tDiagonalsupportObjects written to file");

    writeEndLog();
    return 0;
}


