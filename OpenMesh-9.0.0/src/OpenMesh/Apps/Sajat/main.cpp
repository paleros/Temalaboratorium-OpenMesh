/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Created by peros on 2023.10.23.
 *
 * Budapesti Muszaki es Gazdasagtudomanyi Egyetem (BME)
 * Villamosmernoki es Informatikai Kar (VIK)
 * Iranyitastechnika es Informatika Tanszek (IIT)
 * 1. Temalaboratorium: 3D nyomtatas kulso alatamasztas
 * 2. Onallo laboratorium: 3D nyomtatas - tartostrukturak generalasa es mas gyakorlati problemak
 * 3. Szakdolgozat: 3D nyomtatas - tartostrukturak generalasa es mas gyakorlati problemak
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
 * 3.1. Feladatresz
 * Feladat leirasa: Fentrol indulva lefele huzunk tamaszokat ha bizonyos szogben metszik egymast, akkor osszekotjuk.
 * Csoportositani kell valahogyan.
 *
 * Felhasznalt anyagok: OpenMesh Documentation, gpytoolbox.org, digitalocean.com, w3schools.com, stackoverflow.com,
 *                      geeksforgeeks.org, GitHub Copilot, ChatGTP, Microsoft Copilot
 *                      [2020, Jang et al] Free-floating support structure generation
 *                      [2019, Zhang et al] Local barycenter based efficient tree-support generation for 3D printing
 *
 * @author Eros Pal
 * @consulant Dr. Salvi Peter
 * @since 2024.04.03.
 * ---------------------------------------------------------------------------------------------------------------------
*/

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "auxiliary.h"
#include "supportPoints.h"
#include "columnMain.h"
#include "treeMain.h"

/**
 * Makrok definialasa --------------------------------------------------------------------------------------------------
 */
/**
 * Az alatamasztas tipusanak beallitasa
 */
//#define GRID_SUPPORT /// Oszlop alatamasztas
#define TREE_SUPPORT /// Fa alatamasztas


/**
 * A demozhato alakzatok
 * FONTOS: csak akkor mukodik, ha az alakzat haromszogekbol epul fel!
 */
//#define TEST_BUNNY
//#define TEST_DIAMOND
//#define TEST_SPHERE
//#define TEST_LUCY
//#define TEST_FANDISK
#define TEST_T
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
#ifdef TEST_FANDISK
    std::string inputFile = "models/fandisk.obj";
#endif
#ifdef TEST_T
    std::string inputFile = "models/t.obj";
#endif

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;
    /// A fa csoporitasi erteke
    int groupingValue;

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
#ifdef TEST_FANDISK
    double l = 0.1;
    diameter = 0.05;
    groupingValue = 5;
#endif
#ifdef TEST_T
    double l = 0.1;
    diameter = 0.05;
    groupingValue = 5;
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

    /// Az alatamasztas tipusa "oszlop"
#ifdef GRID_SUPPORT
    columnSupportGenerated(meshObject, inputFile, supportPointsAll, intersectPoints, diameter, l, e);
#endif //GRID_SUPPORT

    /// Az alatamasztas tipusa "fa"
#ifdef TREE_SUPPORT

    treeSupportGenerated(meshObject, inputFile, supportPointsAll, intersectPoints, diameter, l, e, groupingValue);
#endif //TREE_SUPPORT

    writeEndLog();
    return 0;
}


