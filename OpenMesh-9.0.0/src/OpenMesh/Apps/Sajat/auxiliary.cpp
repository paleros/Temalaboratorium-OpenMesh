//
// Created by peros on 2023.11.04.
//
/**
 * A segedfuggvenyeket es valtozokat tarolo file
 *
 * @author Eros Pal
 * @since 2023.11.04.
 */

#include <string>
#include <fstream>
#include <queue>
#include "auxiliary.h"
#include "OpenMesh/Core/IO/MeshIO.hh"

/**
 * Beolvassa a megadott filet
 * @param file a beolvasando file
 * @param mesh a mesh, amibe tarolni kell az adatokat
 * @since 1.1
 */
void readMesh(const std::string& file, MyMesh& mesh){
    if(!OpenMesh::IO::read_mesh(mesh, file)){
        std::cerr << "Error: Cannot read mesh from " << file << std::endl;
        exit(1);
    }
}

/**
 * A parameterkent kapott bemeneti es kimeneti pontokat osszekoti fuggolegesen es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param intersectPoints a metszespontok koordinatai
 * @param desc a leiras
 * @since 1.1
 */
void writeInternalLines(const std::string &outputFileName, const std::string &inputFileName,
                        std::vector<Point> &intersectPoints, const std::string &desc) {
    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << desc << inputFileName << " by BTMLYV\n";
    int k = 1;
    for(int i = 0; i < (int)intersectPoints.size(); i++){
        file << "v " << intersectPoints[i].coordinates[0] << " " << intersectPoints[i].coordinates[1] << " " << intersectPoints[i].coordinates[2] << "\n";
        i++;
        file << "v " << intersectPoints[i].coordinates[0] << " " << intersectPoints[i].coordinates[1] << " " << intersectPoints[i].coordinates[2] << "\n";
        file << "l " << k << " " << k+1 << "\n";
        k = k + 2;
    }
    file.close();
}

/**
 * Kiszamolja a haromszog teruletet
 * @param x1 a kerdeses pont x koordinataja
 * @param z1 a kerdeses pont z koordinataja
 * @param x2 az masodik pont x koordinataja
 * @param z2 az masodik pont z koordinataja
 * @param x3 az harmadik pont x koordinataja
 * @param z3 az harmadik pont z koordinataja
 * @return a haromszog terulete
 * @since 1.1
 */
double area(double x1, double z1, double x2, double z2, double x3, double z3) {
    double v1[2];
    v1[0] = x1 - x2;
    v1[1] = z1 - z2;

    double v2[2];
    v2[0] = x1 - x3;
    v2[1] = z1 - z3;

    double A_2 = v1[0] * v2[1] - v2[0] * v1[1];
    return A_2 / 2;
}

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param edges a kiirando elek
 * @since 1.2
 */
void writeInputEdges(const std::string& outputFileName, const std::string& inputFileName, std::vector<Edge>& edges){
    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Input edges generated from " << inputFileName << " by BTMLYV\n";
    int k = 1;
    for(auto & edge : edges){
        file << "v " << edge.p1.coordinates[0] << " " << edge.p1.coordinates[1] << " " << edge.p1.coordinates[2] << "\n";
        file << "v " << edge.p2.coordinates[0] << " " << edge.p2.coordinates[1] << " " << edge.p2.coordinates[2] << "\n";
        file << "l " << k << " " << k+1 << "\n";
        k = k + 2;
    }
    file.close();
}

/**
 * A parameterkent kapott pontokat kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName az eredeti alakzat neve
 * @param count a szamlalo erteke
 * @param points a pontok
 * @since 1.3
 */
void writePoints(const std::string& outputFileName, const std::string& inputFileName, int count, std::vector<Point>& points){
    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Supported points No. " << count << " generated from " << inputFileName << " by BTMLYV\n";
    for(auto & point : points){
        file << "v " << point.coordinates[0] << " " << point.coordinates[1] << " " << point.coordinates[2] << "\n";
    }
    file.close();
}

/**
 * A kapott mesh-ben minden pontnal az y es a z koordinatat felcsereli
 * @param mesh a mesh
 * @since 1.5
*/
void swapYZ(MyMesh& mesh){
    for(auto & vertex : mesh.vertices()){
        float y = mesh.point(vertex)[1];
        mesh.point(vertex)[1] = mesh.point(vertex)[2];
        mesh.point(vertex)[2] = y;
    }
}

/**
 * A parameterkent kapott mesht kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param mesh a mesh
 */
void writeMesh(const std::string& outputFileName, MyMesh& mesh){
    if(!OpenMesh::IO::write_mesh(mesh, outputFileName)){
        std::cerr << "Error: Cannot write mesh to " << outputFileName << std::endl;
        exit(1);
    }
}

/**
 * Kiirja a futtatas adatait a konzolra
 * @param inputFileName
 */
void writeStartLog(const std::string &inputFileName) {
    std::cout << "   _____                              _      _____                           _   _             \n"
                 "  / ____|                            | |    / ____|                         | | (_)            \n"
                 " | (___  _   _ _ __  _ __   ___  _ __| |_  | |  __  ___ _ __   ___ _ __ __ _| |_ _  ___  _ __  \n"
                 "  \\___ \\| | | | '_ \\| '_ \\ / _ \\| '__| __| | | |_ |/ _ \\ '_ \\ / _ \\ '__/ _` | __| |/ _ \\| '_ \\ \n"
                 "  ____) | |_| | |_) | |_) | (_) | |  | |_  | |__| |  __/ | | |  __/ | | (_| | |_| | (_) | | | |\n"
                 " |_____/ \\__,_| .__/| .__/ \\___/|_|   \\__|  \\_____|\\___|_| |_|\\___|_|  \\__,_|\\__|_|\\___/|_| |_|\n"
                 "              | |   | |                                                                        \n"
                 "              |_|   |_|                                                                        " << std::endl;
    std::cout << " for 3D printing" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\nThe input file is: " << inputFileName << std::endl;
    std::cout << "Log:-------------------------------------------------------------------------------------------" << std::endl;
}

/**
 * Kiirja hogy keszen van
 */
void writeEndLog() {
    std::cout << "-----------------------------------------------------------------------------------------------\n" << std::endl;
    std::cout << "\x1B[32m   _____                      _      _           _ \n"
                 "  / ____|                    | |    | |         | |\n"
                 " | |     ___  _ __ ___  _ __ | | ___| |_ ___  __| |\n"
                 " | |    / _ \\| '_ ` _ \\| '_ \\| |/ _ \\ __/ _ \\/ _` |\n"
                 " | |___| (_) | | | | | | |_) | |  __/ ||  __/ (_| |\n"
                 "  \\_____\\___/|_| |_| |_| .__/|_|\\___|\\__\\___|\\__,_|\n"
                 "                       | |                         \n"
                 "                       |_|                         \033[0m" << std::endl;
}

/**
 * Kiirja a logot a konsolra
 * @param log a kiirando szoveg
 */
void writeLog(const std::string &log) {
    /// Az aktuális idő lekérdezése
    auto now = std::chrono::system_clock::now();
    std::time_t nowC = std::chrono::system_clock::to_time_t(now);
    std::tm* localNow = std::localtime(&nowC);

    std::cout << std::put_time(localNow, "%Y/%m/%d %H:%M:%S") << log << std::endl;
}