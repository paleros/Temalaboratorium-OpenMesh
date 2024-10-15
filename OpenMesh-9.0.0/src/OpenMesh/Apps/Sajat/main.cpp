/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Created by peros on 2023.10.23.
 *
 * Budapesti Muszaki es Gazdasagtudomanyi Egyetem (BME)
 * Villamosmernoki es Informatikai Kar (VIK)
 * Iranyitastechnika es Informatika Tanszek (IIT)
 *
 * I. Temalaboratorium: 3D nyomtatas kulso alatamasztas
 * II. Onallo laboratorium: 3D nyomtatas - tartostrukturak generalasa es mas gyakorlati problemak
 * III. Szakdolgozat: Tamaszstruktura generalasa additiv megmunkalashoz
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
#include "rotationMain.h"

/**
 * Alapbeallitasok definialasa --------------------------------------------------------------------------------------------------
 */
/**
 * Az alatamasztas tipusanak beallitasa
 */
bool isTreeSupport = true;    /// Fa alatamasztas
//bool isTreeSupport = false;    /// Oszlop alatamasztas


/**
 * Optimalis fogatas keresese
 */
//bool findOptimalRotation = false;    /// Optimalis forgatas nelkul
bool findOptimalRotation = true;    /// Optimalis forgatassal
/**
 * A demozhato alakzatok
 * FONTOS: csak akkor mukodik, ha az alakzat haromszogekbol epul fel!
 */
//std::string inputFile = "models/bunny.obj";
//std::string inputFile = "models/diamond.obj";
//std::string inputFile = "models/sphere.obj";
//std::string inputFile = "models/lucy.obj";
std::string inputFile = "models/fandisk.obj";
//std::string inputFile = "models/t.obj";
/**
 * ---------------------------------------------------------------------------------------------------------------------
 */

/**
 * A feladat megvalositasa
 * @return
 * @since 1.1
 */
int main(){

    run(inputFile, isTreeSupport, findOptimalRotation);

    writeEndLog();
    return 0;
}


