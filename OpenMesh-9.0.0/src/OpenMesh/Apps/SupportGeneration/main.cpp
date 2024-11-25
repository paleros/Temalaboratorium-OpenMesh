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
 * 4.1. Feladatresz
 * Feladat leirasa: az alakzatnak keresi a legjobb forgatasat, hogy minel kevesebb tamaszt kelljen hasznalni.
 *
 * Felhasznalt anyagok: OpenMesh Documentation, gpytoolbox.org, digitalocean.com, w3schools.com, stackoverflow.com,
 *                      geeksforgeeks.org, GitHub Copilot, ChatGTP, Microsoft Copilot
 *                      [2020, Jang et al] Free-floating support structure generation
 *                      [2019, Zhang et al] Local barycenter based efficient tree-support generation for 3D printing
 *                      [2015, Etair et al] Orientation analysis of 3D objects toward minimal support volume in 3D-printing
 *
 * @author Eros Pal
 * @consulant Dr. Salvi Peter
 * @since 2024.10.21.
 * ---------------------------------------------------------------------------------------------------------------------
*/

#include "auxiliary.h"
#include "rotationMain.h"

/**
 * Alapbeallitasok definialasa -----------------------------------------------------------------------------------------
 */
/**
 * Az alatamasztas tipusanak beallitasa
 */

SupportType supportType = SupportType::TREE;    /// Fa alatamasztas
//SupportType supportType = SupportType::COLUMN;    /// Oszlop alatamasztas

/**
 * Optimalis fogatas keresese
 */
//AlgorithmType algorithmType = AlgorithmType::NELDERMEAD;      /// Nelder-Mead algoritmus
//AlgorithmType algorithmType = AlgorithmType::DIRECT;          /// Direct algoritmus
AlgorithmType algorithmType = AlgorithmType::NONE;            /// Forgatas nelkul

/**
 * A demozhato alakzatok
 * FONTOS: csak akkor mukodik, ha az alakzat haromszogekbol epul fel! (fontos hogy jo legyen az alakzat!)
 */
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/bunny.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/diamond.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/sphere.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/lucy.obj";
std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/fandisk.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/t.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/T.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/cube.obj";
//std::string inputFile = "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/src/OpenMesh/Apps/SupportGeneration/models/cow.obj";
/**
 * ---------------------------------------------------------------------------------------------------------------------
 */

/**
 * A feladat megvalositasa
 * @return
 * @since 4.1
 */
int main(){
    /// A futtatasi ido kiirasa
    auto start = std::chrono::high_resolution_clock::now();

    run(inputFile, supportType, algorithmType);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Program running time: " << duration.count() << " second" << std::endl;

    writeEndLog();
    return 0;
}


