/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Created by peros on 2023.10.23.
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
 * @since 2024.04.03.
 * ---------------------------------------------------------------------------------------------------------------------
*/

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "auxiliary.h"
#include "columnAuxiliary.h"
#include "treeAuxiliary.h"
#include "supportPoints.h"

/**
 * Makrok definialasa --------------------------------------------------------------------------------------------------
 */
/**
 * Az alatamasztas tipusanak beallitasa
 */
#define COLUMN_SUPPORT
//#define TREE_SUPPORT


/**
 * A demozhato alakzatok
 * FONTOS: csak akkor mukodik, ha az alakzat haromszogekbol epul fel!
 */
#define TEST_BUNNY
//#define TEST_DIAMOND
//#define TEST_SPHERE
//#define TEST_LUCY
/**
 * ---------------------------------------------------------------------------------------------------------------------
 */

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

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;

/// A racspont osztas leptek merete es maximum kiterjedese
#ifdef TEST_BUNNY
    double l = 0.005; /// A sugarak kozti tavolsag
    diameter = 0.002;
#endif
#ifdef TEST_DIAMOND
    double l = 0.15;
    diameter = 0.08;
#endif
#ifdef TEST_SPHERE
    double l = 0.3;
    diameter = 0.15;
#endif

#ifdef TEST_LUCY
    double l = 40;
    diameter = 20;
    //swapYZ(meshObject);
    //writeMesh("models/lucy.obj", meshObject);
#endif

    /// Valtozok inicializalasa

    /// Az alakzat
    MyMesh meshObject;

    /// A maximalis suly
    double maxWeight = M_PI / 4 * 3 *2;

    /// A metszespontok x, y, es z koordinatait tarolja
    std::vector<Point> intersectPoints;

    /// Szamlalo
    int count = 0;

    /// A szamitasi hibak korrekcios erteke
    double e = l / 100;

    /// A bemeneti pontpok kozotti elek tarolasara szolgalo tomb
    std::vector<Edge> edges;

    /// Az alatamasztando pontok kiszamitasa
    /// @since 1.3
    std::vector<Point> inputPoints;

    std::vector<Point> supportPointsAll;

    writeLog("\tBasic parameters set");

    /// Beolvassuk az alakzatot es kiszamoljuk az alatamasztando pontokat
    /// @since 2.2
    supportPointsGenerated(diameter, l, e, inputFile, intersectPoints, count, meshObject, edges,
                           inputPoints, supportPointsAll, maxWeight);









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
            if(std::abs(intersectPoints[i].coordinates[0] - supportPoint.coordinates[0]) <= e &&
               std::abs(intersectPoints[i].coordinates[2] - supportPoint.coordinates[2]) <= e){
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


