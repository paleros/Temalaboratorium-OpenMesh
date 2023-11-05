//
// Created by peros on 2023.11.04..
//
/**
 * A segedfuggvenyeket tarolo file
 *
 * @author Eros Pal
 * @since 2023.11.04.
 */

#include <string>
#include <fstream>
#include <tuple>
#include "auxiliary.h"
#include "OpenMesh/Core/IO/MeshIO.hh"

/**
 * Beolvassa a megadott filet
 * @param file a beolvasando file
 * @param mesh a mesh, amibe tarolni kell az adatokat
 */
void readMesh(const std::string& file, MyMesh& mesh){
    if(!OpenMesh::IO::read_mesh(mesh, file)){
        std::cerr << "Error: Cannot read mesh from " << file << std::endl;
        exit(1);
    }
}

/**
 * A parameterkent kapott bemeneti es kimeneti pontokat osszekoti fuggolegesen es kiirja a .obj fileba
 * @param file_name a kimeneti file neve
 * @param intersect_points a metszespontok koordinatai
 */
void writeInternalLines(const std::string& output_file_name, const std::string& input_file_name, std::vector<Point>& intersect_points){
    std::ofstream file(output_file_name);
    if(!file){
        std::cout << "Error: The file " << output_file_name << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file <<  "# Internal lines generated from " << input_file_name << " by peros\n";
    int k = 1;
    for(int i = 0; i < (int)intersect_points.size(); i++){
        file << "v " << intersect_points[i].coordinates[0] << " " << intersect_points[i].coordinates[1] << " " << intersect_points[i].coordinates[2] << "\n";
        i++;
        file << "v " << intersect_points[i].coordinates[0] << " " << intersect_points[i].coordinates[1] << " " << intersect_points[i].coordinates[2] << "\n";
        file << "l " << k << " " << k+1 << "\n";
        k = k + 2;
    }
    file.close();
}

/**
 * Kiszamolja a haromszog teruletet
 * @param x1 a kerdeses pont x koordinataja
 * @param y1 a kerdeses pont y koordinataja
 * @param z1 a kerdeses pont z koordinataja
 * @param x2 az masodik pont x koordinataja
 * @param y2 az masodik pont y koordinataja
 * @param z2 az masodik pont z koordinataja
 * @param x3 az harmadik pont x koordinataja
 * @param y3 az harmadik pont y koordinataja
 * @param z3 az harmadik pont z koordinataja
 * @return a haromszog terulete
 */
double area(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
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
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az x koordinata alapjan, majd az z, majd az y koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 */
bool comparePoints(const Point& p1, const Point& p2) {
    if(p1.coordinates[0] < p2.coordinates[0]){
        return true;
    } else if(p1.coordinates[0] == p2.coordinates[0]){
        if(p1.coordinates[2] < p2.coordinates[2]){
            return true;
        } else if(p1.coordinates[2] == p2.coordinates[2]){
            if(p1.coordinates[1] < p2.coordinates[1]){
                return true;
            }
        }
    }
    return false;
}

/**
 * Visszaadja a ket pont kozotti vektor sulyat
 * @param actualPoint aktualis pont
 * @param adjacentPoint szomszedos pont
 * @return 0 ha a ket pont kozti vektor szoge hatarerteken beluli, egyebkent a szog erteke lesz a suly, -1 ha alatta van
 */
double thisEdgeLeadsToPoint(const Point& actualPoint, const Point& adjacentPoint){
    if(actualPoint.coordinates[1] > adjacentPoint.coordinates[1]){
        return -1;
    }
    //double phi0 = M_PI;
    double phi0 = 1;
    Point p{};
    p.coordinates[0] = adjacentPoint.coordinates[0] - actualPoint.coordinates[0];
    p.coordinates[1] = adjacentPoint.coordinates[1] - actualPoint.coordinates[1];
    p.coordinates[2] = adjacentPoint.coordinates[2] - actualPoint.coordinates[2];
    double phi = atan2(p.coordinates[2], p.coordinates[0]);
    if(phi > phi0 || phi < -phi0){
        return std::abs(phi);
    }
    return 0;
}

/**
 * Kitorli a rossz, hibas pontokat (zajt)
 * (Egymast koveto pont paroknak meg kell egyeznie az x es z koordinatajuknak)
 * @param intersect_points a pontok listaja
 */
void deleteWrongPoints(std::vector<Point>& intersect_points) {
    for (int i = 0; i < (int) intersect_points.size() - 1; i++) {
        if (intersect_points[i].coordinates[0] != intersect_points[i + 1].coordinates[0] ||
            intersect_points[i].coordinates[2] != intersect_points[i + 1].coordinates[2]) {
            intersect_points.erase(intersect_points.begin() + i);
            i--;
        } else {
            i++;
        }
    }
}

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param p a keresendo pont
 * @return benne van-e a tombben
 */
bool isIncluded(std::vector<Point>& intersect_points, const Point& p){
    for (auto & intersect_point : intersect_points){
        if (intersect_point.coordinates[0] == p.coordinates[0] &&
            intersect_point.coordinates[2] == p.coordinates[2]){
            return true;
        }
    }
    return false;
}

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param output_file_name a kimeneti file neve
 * @param input_file_name a bemeneti file neve
 * @param edges a kiirando elek
 */
void writeInputEdges(const std::string& output_file_name, const std::string& input_file_name, std::vector<Edge>& edges){
    std::ofstream file(output_file_name);
    if(!file){
        std::cout << "Error: The file " << output_file_name << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file <<  "# Input edges generated from " << input_file_name << " by peros\n";
    int k = 1;
    for(auto & edge : edges){
        file << "v " << edge.p1.coordinates[0] << " " << edge.p1.coordinates[1] << " " << edge.p1.coordinates[2] << "\n";
        file << "v " << edge.p2.coordinates[0] << " " << edge.p2.coordinates[1] << " " << edge.p2.coordinates[2] << "\n";
        file << "l " << k << " " << k+1 << "\n";
        k = k + 2;
    }
    file.close();
}