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
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az x koordinata alapjan, majd az z, majd az y koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.1
 */
bool comparePoints(const Point& p1, const Point& p2) {
    if(p2.coordinates[0] - p1.coordinates[0] > p1.e){
        return true;
    } else if(std::abs(p1.coordinates[0] - p2.coordinates[0]) <= p1.e){
        if(p2.coordinates[2] - p1.coordinates[2] > p1.e){
            return true;
        } else if(std::abs(p1.coordinates[2] - p2.coordinates[2]) <= p1.e){
            if(p2.coordinates[1] - p1.coordinates[1] > p1.e){
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
 * @param l a hibahatarhoz az osztas
 * @return 0 ha a ket pont kozti vektor szoge hatarerteken beluli, egyebkent a szog erteke lesz a suly, -1 ha alatta van
 * @since 1.2
 */
double thisEdgeLeadsToPoint(const Point &actualPoint, const Point &adjacentPoint, double l) {
    double e = l / 100;
    if(actualPoint.coordinates[1] - adjacentPoint.coordinates[1] >= e){
        return -1;
    }
    /// A hatarertek
    double theta0 = M_PI / 4; // 45 fok

    Point p{};
    p.coordinates[0] = adjacentPoint.coordinates[0] - actualPoint.coordinates[0];
    p.coordinates[1] = adjacentPoint.coordinates[1] - actualPoint.coordinates[1];
    p.coordinates[2] = adjacentPoint.coordinates[2] - actualPoint.coordinates[2];
    double theta = atan(sqrt(pow(p.coordinates[0], 2) + pow(p.coordinates[2], 2)) / p.coordinates[1]);
    theta = fabs(theta);
    if(theta >= theta0){
        return theta;
    }
    return 0;
}

/**
 * Kitorli a rossz, hibas pontokat (zajt)
 * (Egymast koveto pont paroknak meg kell egyeznie az x es z koordinatajuknak)
 * @param intersectPoints a pontok listaja
 * @param e a hibahatar
 * @since 1.1
 */
void deleteWrongPoints(std::vector<Point> &intersectPoints, double e) {
    for (int i = 0; i < (int) intersectPoints.size() - 1; i++) {
        if (std::abs(intersectPoints[i].coordinates[0] - intersectPoints[i + 1].coordinates[0]) > e ||
            std::abs(intersectPoints[i].coordinates[2] - intersectPoints[i + 1].coordinates[2]) > e) {
            intersectPoints.erase(intersectPoints.begin() + i);
            i--;
        } else {
            i++;
        }
    }
}

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersectPoints a pontok tombje
 * @param p a keresendo pont
 * @param l a hibahatarhoz az osztas egysege
 * @return benne van-e
 * @since 1.2
 */
bool isIncluded(std::vector<Point> &intersectPoints, const Point &p, double l) {
    /// A bemeneti pontokat es a kimeneti pontokat kulon valogatja
    std::vector<double> inputPointsY;
    std::vector<double> outputPointsY;
    double e = l / 100; /// A hibahatar

    for(int i = 0; i < (int)intersectPoints.size(); i++) {
        if (std::abs(intersectPoints[i].coordinates[0] - p.coordinates[0]) <= e &&
            std::abs(intersectPoints[i].coordinates[2] - p.coordinates[2]) <= e &&
            (std::abs(intersectPoints[i].coordinates[1] - p.coordinates[1]) <= e ||
             intersectPoints[i].coordinates[1] - p.coordinates[1] >= -e)) {
            if (i % 2 == 0) {
                inputPointsY.push_back(intersectPoints[i].coordinates[1]);
            } else {
                outputPointsY.push_back(intersectPoints[i].coordinates[1]);
            }
        }
    }

    if (!inputPointsY.empty() && !outputPointsY.empty()){
        /// Megkeresi a legkisebb y koordinataju bemeneti pontot, ami nagyobb mint a parameterkent kapott pont y koordinataja
        double minYInput;
        std::sort(inputPointsY.begin(), inputPointsY.end());
        minYInput = inputPointsY[0];

        double minYOutput;
        std::sort(outputPointsY.begin(), outputPointsY.end());
        minYOutput = outputPointsY[0];

        if ((minYOutput - minYInput) >= -e){
            return true;
        }
    }
    return false;
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
 * Kikeresi a parameterkent kapott pont a tombben (csal x-t es z-t vizsgalja)
 * @param intersectPoints a pontok tombje
 * @param p a keresendo pont
 * @param l a hibahatarhoz az osztas egysege
 * @return visszaadja az y koordinatat a legkisebb, de az aktualisnal magasabb pontrol, ha nincs benne, akkor 0.0
 * @since 1.2
 */
double getY(std::vector<Point> &intersectPoints, const Point &p, double l) {
    std::vector<double> inputPointsY;
    std::vector<double> outputPointsY;
    double e = l / 100; /// A hibahatar

    for(int i = 0; i < (int)intersectPoints.size(); i++) {
        if ((std::abs(intersectPoints[i].coordinates[0] - p.coordinates[0]) <= e &&
             std::abs(intersectPoints[i].coordinates[2] - p.coordinates[2]) <= e) &&
            (std::abs(intersectPoints[i].coordinates[1] - p.coordinates[1]) <= e ||
             intersectPoints[i].coordinates[1] - p.coordinates[1] >= -e)) {

            if (i % 2 == 0) {
                inputPointsY.push_back(intersectPoints[i].coordinates[1]);
            } else {
                outputPointsY.push_back(intersectPoints[i].coordinates[1]);
            }
        }
    }

    if (!inputPointsY.empty() && !outputPointsY.empty()){
        /// Megkeresi a legkisebb y koordinataju bemeneti pontot, ami nagyobb mint a parameterkent kapott pont y koordinataja
        double minYInput;
        std::sort(inputPointsY.begin(), inputPointsY.end());
        minYInput = inputPointsY[0];

        double minYOutput;
        std::sort(outputPointsY.begin(), outputPointsY.end());
        minYOutput = outputPointsY[0];

        if ((minYOutput - minYInput) >= -e){
            return minYInput;
        }
    }
    return 0.0;
}

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareInputPoints(const Point &p1, const Point &p2) {
    if((p1.coordinates[1] - p2.coordinates[1]) < -p1.e){
        return true;
    } else if(std::abs(p1.coordinates[1] - p2.coordinates[1]) <= p1.e){
        if((p1.coordinates[0] - p2.coordinates[0]) < -p1.e){
            return true;
        } else if(std::abs(p1.coordinates[0] - p2.coordinates[0]) <= p1.e){
            if((p1.coordinates[2] - p2.coordinates[2]) < -p1.e){
                return true;
            }
        }
    }
    return false;
}

/**
 * Osszehasonlitja a ket kapott el kezdopontjanak koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param e1 az eslo el
 * @param e2 a masodik el
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareEdgesInputPoints(const Edge& e1, const Edge& e2) {
    if((e1.p1.coordinates[1] - e2.p1.coordinates[1]) < -e1.p1.e){
        return true;
    } else if(std::abs(e1.p1.coordinates[1] - e2.p1.coordinates[1]) <= -e1.p1.e){
        if((e1.p1.coordinates[0] - e2.p1.coordinates[0]) < -e1.p1.e){
            return true;
        } else if(std::abs(e1.p1.coordinates[0] - e2.p1.coordinates[0]) <= -e1.p1.e){
            if((e1.p1.coordinates[2] - e2.p1.coordinates[2]) < -e1.p1.e){
                return true;
            } else if(std::abs(e1.p2.coordinates[1] - e2.p2.coordinates[1]) <= -e1.p1.e){
                if((e1.p2.coordinates[0] - e2.p2.coordinates[0]) < -e1.p1.e){
                    return true;
                } else if(std::abs(e1.p2.coordinates[0] - e2.p2.coordinates[0]) <= -e1.p1.e){
                    if((e1.p2.coordinates[2] - e2.p2.coordinates[2]) < -e1.p1.e){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
 * Kikeresi az elek kozul a pontot és visszaadja az indexet
 * @param points a pontok halmaza
 * @param p a keresett pont
 * @return
 */
int findPoint(std::vector<Point> &points, const Point &p) {
    for(int i = 0; i < (int)points.size(); i++){
        if(points[i] == p){
            return i;
        }
    }
    return -1;
}

/**
 * Beallitja a pontoknak, hogy milyen a sulyuk
 * @param edges az elek
 * @param inputPoints a pontok
 * @param maxWeight a maximalis suly
 * @return a pontok listaja, aminke a sulyuk nem tul nagy
 * @since 1.3
 */
std::vector<Point>
setWeightAllPointsAndGetSupportPoints(std::vector<Edge> &edges, std::vector<Point> &inputPoints, double maxWeight) {
    std::sort(edges.begin(), edges.end(), compareEdgesInputPoints);
    /// Kiszamolja minden pontra a sulyt
    for(auto & edge : edges){
        if(findPoint(inputPoints, edge.p1) == -1 || findPoint(inputPoints, edge.p2) == -1){
            continue;
        }
        double weightP2Actual = inputPoints[findPoint(inputPoints, edge.p2)].weight;
        double weightP1Actual = inputPoints[findPoint(inputPoints, edge.p1)].weight;
        double weightP2New = inputPoints[findPoint(inputPoints, edge.p1)].weight + edge.weight;
        double weightEdge = edge.weight;
        if(weightP2Actual == -1){
            if(weightP1Actual == -1){
                inputPoints[findPoint(inputPoints, edge.p2)].weight = weightEdge;
                inputPoints[findPoint(inputPoints, edge.p1)].weight = 0;
            }else{
                inputPoints[findPoint(inputPoints, edge.p2)].weight = weightP2New;
            }
        }else{
            if(weightP1Actual == -1) {
                inputPoints[findPoint(inputPoints, edge.p2)].weight = weightEdge;
                inputPoints[findPoint(inputPoints, edge.p1)].weight = 0;
            }else {
                if (weightP2Actual - weightP2New > edge.p1.e) {
                    inputPoints[findPoint(inputPoints, edge.p2)].weight = weightP2New;
                }
            }
        }
    }

    /// Felveszi a csucspontokat a listaba
    std::vector<Point> supportPoints;
    for(auto & edge : edges){
        if(findPoint(inputPoints, edge.p1) == -1 || findPoint(inputPoints, edge.p2) == -1){
            continue;
        }
        supportPoints.push_back(inputPoints[findPoint(inputPoints, edge.p1)]);
        supportPoints.push_back(inputPoints[findPoint(inputPoints, edge.p2)]);
    }
    /// Kitorli a duplikatumokat
    std::sort(supportPoints.begin(), supportPoints.end(), compareInputPoints);
    for (int i = 0; i < (int)supportPoints.size() -1; i++){
        if (supportPoints[i] == supportPoints[i + 1]){
            if (supportPoints[i].weight == -1){
                supportPoints.erase(supportPoints.begin() + i);
                i--;
            } else {
                if (supportPoints[i + 1].weight == -1){
                    supportPoints.erase(supportPoints.begin() + i + 1);
                    i--;
                } else {
                    if (supportPoints[i].weight < supportPoints[i + 1].weight){
                        supportPoints.erase(supportPoints.begin() + i + 1);
                        i--;
                    } else {
                        supportPoints.erase(supportPoints.begin() + i);
                        i--;
                    }
                }
            }
        }
    }

    /// Kitorli azokat a pontokat, amiknek mar tul nagy a sulya
    for (int j = 0; j < (int)supportPoints.size(); j++){
        if (supportPoints[j].weight > maxWeight){
            supportPoints.erase(supportPoints.begin() + j);
        }
    }

    return supportPoints;
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
 * A parameterkent kapott pontok kozott haromszog alapu hasabokat csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a haromszog merete
 * @param minY a legkisebb y koordinata
 * @since 1.5
 */
void generateAndWriteSupportLines(const std::string &outputFileName, const std::string &inputFileName,
                                  std::vector<Point> &points, double diameter, double minY) {
    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

    int n = 1;
    double a2 = std::cos(M_PI/6) * diameter;
    double k = std::sin(M_PI/6) * diameter;
    for (int i = 0; i < (int)points.size(); i++){
        if(i % 2 != 0){
            if (points[i].coordinates[1] >= minY + a2*2) {
                file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] + a2 * 2 << " "
                     << points[i].coordinates[2] - diameter << "\n";
                file << "v " << points[i].coordinates[0] + a2 << " " << points[i].coordinates[1] + a2 * 2 << " "
                     << points[i].coordinates[2] + k << "\n";
                file << "v " << points[i].coordinates[0] - a2 << " " << points[i].coordinates[1] + a2 * 2 << " "
                     << points[i].coordinates[2] + k << "\n";
            } else {
                file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] << " "
                     << points[i].coordinates[2] - diameter << "\n";
                file << "v " << points[i].coordinates[0] + a2 << " " << points[i].coordinates[1] << " "
                     << points[i].coordinates[2] + k << "\n";
                file << "v " << points[i].coordinates[0] - a2 << " " << points[i].coordinates[1] << " "
                     << points[i].coordinates[2] + k << "\n";
            }

            /// Sok szamolas es probalkozas eredmenye
            file << "f " << n-4 << " " << n-3 << " " << n+1 << "\n";
            file << "f " << n-4 << " " << n+1 << " " << n << "\n";
            file << "f " << n-3 << " " << n-2 << " " << n+2 << "\n";
            file << "f " << n-3 << " " << n+2 << " " << n+1 << "\n";
            file << "f " << n-4 << " " << n-2 << " " << n+2 << "\n";
            file << "f " << n-4 << " " << n+2 << " " << n << "\n";
            n = n + 3;

            file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] << " " << points[i].coordinates[2] << "\n";
            if (points[i].coordinates[1] >= minY + a2*2) {

                file << "f " << n-3 << " " << n-2 << " " << n << "\n";
                file << "f " << n-2 << " " << n-1 << " " << n << "\n";
                file << "f " << n-1 << " " << n-3 << " " << n << "\n";
            }
            n = n + 1;

        } else {
            if (points[i].coordinates[1] >= minY + a2*2){
                file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] - a2*2 << " "
                     << points[i].coordinates[2] - diameter << "\n";
                file << "v " << points[i].coordinates[0] + a2 << " " << points[i].coordinates[1] - a2*2 << " "
                     << points[i].coordinates[2] + k << "\n";
                file << "v " << points[i].coordinates[0] - a2 << " " << points[i].coordinates[1] - a2*2 << " "
                     << points[i].coordinates[2] + k << "\n";
            } else {
                file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] << " "
                     << points[i].coordinates[2] - diameter << "\n";
                file << "v " << points[i].coordinates[0] + a2 << " " << points[i].coordinates[1] << " "
                     << points[i].coordinates[2] + k << "\n";
                file << "v " << points[i].coordinates[0] - a2 << " " << points[i].coordinates[1] << " "
                     << points[i].coordinates[2] + k << "\n";
            }

            n = n + 3;

            file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] << " " << points[i].coordinates[2] << "\n";
            if (points[i].coordinates[1] >= minY + a2*2) {

                file << "f " << n-3 << " " << n-2 << " " << n << "\n";
                file << "f " << n-2 << " " << n-1 << " " << n << "\n";
                file << "f " << n-1 << " " << n-3 << " " << n << "\n";
            }
            n = n + 1;

        }
    }
    file.close();
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
 * A parameterkent kapott pontok kozott hengereket csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a henger atmeroje
 * @param minY a legkisebb y koordinata
 * @since 2.1
 */
void generateAndWriteSupportCylinder(const std::string &outputFileName, const std::string &inputFileName,
                                     std::vector<Point> &points, double diameter, double minY) {
    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

    int n = 0;
    /// A henger felso csucsanak magassaga
    double a1 = std::cos(M_PI / 6) * diameter;
    /// A henger also kiszelesedesenek magassaganak kilogo resze
    double a2 = std::cos(M_PI/6) * diameter;
    /// A henger sugara
    double r = diameter/2;
    /// A korlep pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
    double deltaX1, deltaX2, deltaX3, deltaX4, deltaZ1, deltaZ2, deltaZ3, deltaZ4;
    /// A masodik korlap pont
    double xNull, zNull;
    /// A korlap kozeppontja
    double x, y, z;

    for (int i = 0; i < (int)points.size(); i++){
        /// Az also es felso pontok kulonvalasztasa
        if(i % 2 != 0){
            /// Az also pontok

            x = points[i].coordinates[0];
            y = points[i].coordinates[1] + a1*2;
            z = points[i].coordinates[2];

            /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r;
            deltaX1 = xNull - x;
            deltaZ1 = zNull - z;
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r;
            deltaX2 = xNull - x;
            deltaZ2 = zNull - z;
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r;
            deltaX3 = xNull - x;
            deltaZ3 = zNull - z;

            /// Elso negyed pontjai
            file << "v " << x + r << " " << y << " " << z << "\n"; /// 1
            file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
            file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
            file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
            /// Masodik negyed pontjai
            file << "v " << x << " " << y << " " << z + r << "\n"; /// 5
            file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
            file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
            file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
            /// Harmadik negyed pontjai
            file << "v " << x - r << " " << y << " " << z << "\n"; /// 9
            file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
            file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
            file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
            /// Negyedik negyed pontjai
            file << "v " << x << " " << y << " " << z - r << "\n"; /// 13
            file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
            file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
            file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

            /// A henger oldalanak kirajzolasa
            for (int j = 1; j < 16; j++){
                file << "f " << j + n << " " << j + n + 16 << " " << j + n + 1 << "\n";
                file << "f " << j + n + 1 + 16 << " " << j + n + 16 << " " << j + n + 1 << "\n";
            }
            file << "f " << 16 + n << " " << 16 + n + 16 << " " << 1 + n << "\n";
            file << "f " << 1 + 16 + n << " " << 16 + 16 + n << " " << 1 + n << "\n";

            /// A tamasz csucs kirajzolasa
            file << "v " << points[i-1].coordinates[0] << " " << points[i-1].coordinates[1] << " " << points[i-1].coordinates[2] << "\n";
            if (points[i-1].coordinates[1] >= minY + a2*2) {
                for (int j = 1; j < 16; j++) {
                    file << "f " << j + n << " " << 16 + n + 17 << " " << j + n + 1 << "\n";
                }
                file << "f " << 16 + n << " " << 16 + n + 17 << " " << 1 + n << "\n";
            }

            /// A tamasz also szelesitesenek kirajzolasa

            /// Kiszamoljuk a henger alapjanak szamito legalso, szelesebb "kort", ami egy 16 szog lesz
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r*2;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r*2;
            deltaX1 = xNull - x;
            deltaZ1 = zNull - z;
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r*2;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r*2;
            deltaX2 = xNull - x;
            deltaZ2 = zNull - z;
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r*2;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r*2;
            deltaX3 = xNull - x;
            deltaZ3 = zNull - z;

            /// Elso negyed pontjai
            file << "v " << x + r*2 << " " << y -a1*2 << " " << z << "\n"; /// 1
            file << "v " << x + deltaX1 << " " << y -a1*2 << " " << z + deltaZ1 << "\n"; /// 2
            file << "v " << x + deltaX2 << " " << y -a1*2 << " " << z + deltaZ2 << "\n"; /// 3
            file << "v " << x + deltaX3 << " " << y -a1*2 << " " << z + deltaZ3 << "\n"; /// 4
            /// Masodik negyed pontjai
            file << "v " << x << " " << y -a1*2 << " " << z + r*2 << "\n"; /// 5
            file << "v " << x - deltaX3 << " " << y -a1*2 << " " << z + deltaZ3 << "\n"; /// 6
            file << "v " << x - deltaX2 << " " << y -a1*2 << " " << z + deltaZ2 << "\n"; /// 7
            file << "v " << x - deltaX1 << " " << y -a1*2 << " " << z + deltaZ1 << "\n"; /// 8
            /// Harmadik negyed pontjai
            file << "v " << x - r*2 << " " << y -a1*2 << " " << z << "\n"; /// 9
            file << "v " << x - deltaX1 << " " << y -a1*2 << " " << z - deltaZ1 << "\n"; /// 10
            file << "v " << x - deltaX2 << " " << y -a1*2 << " " << z - deltaZ2 << "\n"; /// 11
            file << "v " << x - deltaX3 << " " << y -a1*2 << " " << z - deltaZ3 << "\n"; /// 12
            /// Negyedik negyed pontjai
            file << "v " << x << " " << y -a1*2 << " " << z - r*2 << "\n"; /// 13
            file << "v " << x + deltaX3 << " " << y -a1*2 << " " << z - deltaZ3 << "\n"; /// 14
            file << "v " << x + deltaX2 << " " << y -a1*2 << " " << z - deltaZ2 << "\n"; /// 15
            file << "v " << x + deltaX1 << " " << y -a1*2 << " " << z - deltaZ1 << "\n"; /// 16

            /// A kiszelesites oldalanak kirajzolasa
            for (int j = 1; j < 16; j++){
                file << "f " << j + n + 16 + 17 << " " << j + n + 16 << " " << j + n + 1 + 16 + 17 << "\n";
                file << "f " << j + n + 1 + 16 << " " << j + n + 16 << " " << j + n + 1 + 16 + 17 << "\n";
            }
            file << "f " << 16 + n + 16 + 17 << " " << 16 + n + 16 << " " << 1 + n + 16 + 17 << "\n";
            file << "f " << 1 + 16 + n << " " << 16 + 16 + n << " " << 1 + n + 16 + 17 << "\n";

            n = n + 16*2 + 1 + 16;
        } else {
            /// A felso pontok

            x = points[i].coordinates[0];
            y = points[i].coordinates[1] - a1*2;
            z = points[i].coordinates[2];

            /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r;
            deltaX1 = xNull - x;
            deltaZ1 = zNull - z;
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r;
            deltaX2 = xNull - x;
            deltaZ2 = zNull - z;
            zNull = points[i].coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r;
            xNull = points[i].coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r;
            deltaX3 = xNull - x;
            deltaZ3 = zNull - z;

            /// Elso negyed pontjai
            file << "v " << x + r << " " << y << " " << z << "\n"; /// 1
            file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
            file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
            file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
            /// Masodik negyed pontjai
            file << "v " << x << " " << y << " " << z + r << "\n"; /// 5
            file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
            file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
            file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
            /// Harmadik negyed pontjai
            file << "v " << x - r << " " << y << " " << z << "\n"; /// 9
            file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
            file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
            file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
            /// Negyedik negyed pontjai
            file << "v " << x << " " << y << " " << z - r << "\n"; /// 13
            file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
            file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
            file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

        }
    }
    file.close();
}

/**
 * A parameterkent kapott pontok kozotti alatamasztasok kozott generalja a keresztmerevitoket
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter az alatamasztas atmeroje
 * @param minY a legkisebb y koordinata
 * @param l a kuszobertek
 * @since 2.1.2
 */
void generateAndWriteSupportCrossBrace(const std::string &outputFileName, const std::string &inputFileName,
                                     std::vector<Point> &points, double diameter, double minY, double l) {

    double a1 = std::cos(M_PI / 6) * diameter;
    double e = l / 100;

    /// A keresztmerevitok lehetseges kozeppontjai
    std::vector<Point> crossBraceEndPoints;
    /// Az aktualis vizsgalando pont
    Point p;
    /// A magassag novelese
    double epsilon = 0;

    /// A keresztmerevitok lehetseges kozeppontjainak kiszamitasa
    for (int i = 1; i < (int)points.size(); i = i + 2){
        p.coordinates[0] = points[i].coordinates[0];
        p.coordinates[1] = points[i].coordinates[1] + a1*2 + epsilon;
        p.coordinates[2] = points[i].coordinates[2];

        /// A keresztmerevitok kozeppontjai benne vannak e az egyenes szakaszban
        while(p.coordinates[1] >= (minY + a1*2) && p.coordinates[1] <= (points[i-1].coordinates[1] - a1*2)){
            crossBraceEndPoints.push_back(p);
            epsilon = epsilon + l;
            p.coordinates[0] = points[i].coordinates[0];
            p.coordinates[1] = points[i].coordinates[1] + a1*2 + epsilon;
            p.coordinates[2] = points[i].coordinates[2];
        }
        epsilon = 0;
    }

    /// A keresztmerevitok egyenesekke alakitasa, ahova kell
    std::vector<Edge> crossBraceEdges;
    /*for(int i = 0; i < (int)crossBraceEndPoints.size(); i++){
        for(int j = 0; j < (int)crossBraceEndPoints.size(); j++){
            if(i != j){
                if(std::abs(crossBraceEndPoints[i].coordinates[1] - crossBraceEndPoints[j].coordinates[1] + l) <= e){
                    if(std::abs(crossBraceEndPoints[i].coordinates[0] - crossBraceEndPoints[j].coordinates[0] + l) <= e
                        || std::abs(crossBraceEndPoints[i].coordinates[0] - crossBraceEndPoints[j].coordinates[0] - l) <= e){
                        if(std::abs(crossBraceEndPoints[i].coordinates[2] - crossBraceEndPoints[j].coordinates[2] + l) <= e
                            || std::abs(crossBraceEndPoints[i].coordinates[2] - crossBraceEndPoints[j].coordinates[2] - l) <= e){
                            crossBraceEdges.emplace_back(crossBraceEndPoints[i], crossBraceEndPoints[j], 0);
                        }
                    }
                }
            }
        }
    }*/

    for(int i = 0; i < (int)crossBraceEndPoints.size(); i++){
        for(int j = 0; j < (int)crossBraceEndPoints.size(); j++){
            if(i != j){
                double h = std::abs(crossBraceEndPoints[i].coordinates[1] - crossBraceEndPoints[j].coordinates[1]);
                double d1 = std::abs(crossBraceEndPoints[i].coordinates[0] - crossBraceEndPoints[j].coordinates[0]);
                double d2 = std::abs(crossBraceEndPoints[i].coordinates[2] - crossBraceEndPoints[j].coordinates[2]);
                if(((h / d1 == 1) && (d2 == 0)) || ((h / d2 == 1) && (d1 == 0))){
                    crossBraceEdges.emplace_back(crossBraceEndPoints[i], crossBraceEndPoints[j], 0);
                }
            }
        }
    }

    //writeInputEdges(outputFileName, inputFileName, crossBraceEdges);

    /// A keresztmerevito hengerek kirajzolasa

    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

    int n = 0;
    /// A henger sugara
    double r = diameter/2;
    /// A korlep pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
    double deltaX1, deltaX2, deltaX3, deltaX4, deltaZ1, deltaZ2, deltaZ3, deltaZ4;
    /// A masodik korlap pont
    double xNull, zNull;
    /// A korlap kozeppontja
    double x, y, z;
    for(int i = 0; i < (int)crossBraceEdges.size(); i++){
        /// Az also pontok

        x = crossBraceEdges[i].p1.coordinates[0];
        y = crossBraceEdges[i].p1.coordinates[1];
        z = crossBraceEdges[i].p1.coordinates[2];

        /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
        zNull = crossBraceEdges[i].p1.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r;
        xNull = crossBraceEdges[i].p1.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r;
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = crossBraceEdges[i].p1.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r;
        xNull = crossBraceEdges[i].p1.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r;
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = crossBraceEdges[i].p1.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r;
        xNull = crossBraceEdges[i].p1.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r;
        deltaX3 = xNull - x;
        deltaZ3 = zNull - z;

        /// Elso negyed pontjai
        file << "v " << x + r << " " << y << " " << z << "\n"; /// 1
        file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
        file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
        file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
        /// Masodik negyed pontjai
        file << "v " << x << " " << y << " " << z + r << "\n"; /// 5
        file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
        file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
        file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
        /// Harmadik negyed pontjai
        file << "v " << x - r << " " << y << " " << z << "\n"; /// 9
        file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
        file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
        file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
        /// Negyedik negyed pontjai
        file << "v " << x << " " << y << " " << z - r << "\n"; /// 13
        file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
        file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
        file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

        /// A felso pontok

        x = crossBraceEdges[i].p2.coordinates[0];
        y = crossBraceEdges[i].p2.coordinates[1];
        z = crossBraceEdges[i].p2.coordinates[2];

        /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
        zNull = crossBraceEdges[i].p2.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r;
        xNull = crossBraceEdges[i].p2.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r;
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = crossBraceEdges[i].p2.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r;
        xNull = crossBraceEdges[i].p2.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r;
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = crossBraceEdges[i].p2.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r;
        xNull = crossBraceEdges[i].p2.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r;
        deltaX3 = xNull - x;
        deltaZ3 = zNull - z;

        /// Elso negyed pontjai
        file << "v " << x + r << " " << y << " " << z << "\n"; /// 1
        file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
        file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
        file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
        /// Masodik negyed pontjai
        file << "v " << x << " " << y << " " << z + r << "\n"; /// 5
        file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
        file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
        file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
        /// Harmadik negyed pontjai
        file << "v " << x - r << " " << y << " " << z << "\n"; /// 9
        file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
        file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
        file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
        /// Negyedik negyed pontjai
        file << "v " << x << " " << y << " " << z - r << "\n"; /// 13
        file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
        file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
        file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

        /// A henger oldalanak kirajzolasa
        for (int j = 1; j < 16; j++){
            file << "f " << j + n << " " << j + n + 16 << " " << j + n + 1 << "\n";
            file << "f " << j + n + 1 + 16 << " " << j + n + 16 << " " << j + n + 1 << "\n";
        }
        file << "f " << 16 + n << " " << 16 + n + 16 << " " << 1 + n << "\n";
        file << "f " << 1 + 16 + n << " " << 16 + 16 + n << " " << 1 + n << "\n";
        n = n + 16*2;
    }

}