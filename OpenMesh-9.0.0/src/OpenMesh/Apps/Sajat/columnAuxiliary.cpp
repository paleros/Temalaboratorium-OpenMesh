//
// Created by peros on 2024.04.03.
//
/**
 * Az oszlopos tamasz fuggvenyeit tartalmazo file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#include <string>
#include <fstream>
#include <queue>
#include <utility>
#include "OpenMesh/Core/IO/MeshIO.hh"
#include "columnAuxiliary.h"

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
 * Elorebb a kisebb y koordinataju pontok kerulnek
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareInputPointsYXZ(const Point &p1, const Point &p2) {
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
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az x koordinata alapjan, majd az y, majd a z koordinata alapjan
 * @param p1 az eslo pont
 * @param p2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 3.1
 */
bool compareInputPointsXYZ(const Point &p1, const Point &p2) {
    if((p1.coordinates[0] - p2.coordinates[0]) < -p1.e){
        return true;
    } else if(std::abs(p1.coordinates[0] - p2.coordinates[0]) <= p1.e){
        if((p1.coordinates[1] - p2.coordinates[1]) < -p1.e){
            return true;
        } else if(std::abs(p1.coordinates[1] - p2.coordinates[1]) <= p1.e){
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
    std::sort(supportPoints.begin(), supportPoints.end(), compareInputPointsYXZ);
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
    double e = points[0].e;
    /// A henger felso csucsanak magassaga
    double a1 = std::cos(M_PI / 6) * diameter;
    /// A henger also kiszelesedesenek magassaganak kilogo resze
    double a2 = std::cos(M_PI/6) * diameter;
    /// A henger sugara
    double r = diameter/2;
    /// A korlep pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
    double deltaX1, deltaX2, deltaX3, deltaZ1, deltaZ2, deltaZ3;
    /// A masodik korlap pont
    double xNull, zNull;
    /// A korlap kozeppontja
    double x, y, z;

    for (int i = 0; i < (int)points.size(); i++){
        /// Az also es felso pontok kulonvalasztasa
        if(std::abs(points[i].coordinates[1] - points[i+1].coordinates[1]) > a1 * 2 * 2) { /// Ha nem tul kicsi
            if (i % 2 != 0) {
                /// Az also pontok
                x = points[i].coordinates[0];
                y = points[i].coordinates[1] + a1 * 2;
                z = points[i].coordinates[2];

                /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
                zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r;
                xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r;
                deltaX1 = xNull - x;
                deltaZ1 = zNull - z;
                zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r;
                xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r;
                deltaX2 = xNull - x;
                deltaZ2 = zNull - z;
                zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r;
                xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r;
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
                for (int j = 1; j < 16; j++) {
                    file << "f " << j + n << " " << j + n + 16 << " " << j + n + 1 << "\n";
                    file << "f " << j + n + 1 + 16 << " " << j + n + 16 << " " << j + n + 1 << "\n";
                }
                file << "f " << 16 + n << " " << 16 + n + 16 << " " << 1 + n << "\n";
                file << "f " << 1 + 16 + n << " " << 16 + 16 + n << " " << 1 + n << "\n";

                /// A tamasz csucs kirajzolasa
                file << "v " << points[i - 1].coordinates[0] << " " << points[i - 1].coordinates[1] << " "
                     << points[i - 1].coordinates[2] << "\n";
                if (points[i - 1].coordinates[1] >= minY + a2 * 2) {
                    for (int j = 1; j < 16; j++) {
                        file << "f " << j + n << " " << 16 + n + 17 << " " << j + n + 1 << "\n";
                    }
                    file << "f " << 16 + n << " " << 16 + n + 17 << " " << 1 + n << "\n";
                }
                n = n + 16 * 2 + 1;

                /// A tamasz also szelesitesenek kirajzolasa
                if (std::abs(points[i].coordinates[1] - minY) <= e) {
                    /// Kiszamoljuk a henger alapjanak szamito legalso, szelesebb "kort", ami egy 16 szog lesz
                    zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r * 2;
                    xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r * 2;
                    deltaX1 = xNull - x;
                    deltaZ1 = zNull - z;
                    zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r * 2;
                    xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r * 2;
                    deltaX2 = xNull - x;
                    deltaZ2 = zNull - z;
                    zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r * 2;
                    xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r * 2;
                    deltaX3 = xNull - x;
                    deltaZ3 = zNull - z;
                    y = y - a1 * 2;

                    /// Elso negyed pontjai
                    file << "v " << x + r * 2 << " " << y << " " << z << "\n"; /// 1
                    file << "v " << x + deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 2
                    file << "v " << x + deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 3
                    file << "v " << x + deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 4
                    /// Masodik negyed pontjai
                    file << "v " << x << " " << y << " " << z + r * 2 << "\n"; /// 5
                    file << "v " << x - deltaX3 << " " << y << " " << z + deltaZ3 << "\n"; /// 6
                    file << "v " << x - deltaX2 << " " << y << " " << z + deltaZ2 << "\n"; /// 7
                    file << "v " << x - deltaX1 << " " << y << " " << z + deltaZ1 << "\n"; /// 8
                    /// Harmadik negyed pontjai
                    file << "v " << x - r * 2 << " " << y << " " << z << "\n"; /// 9
                    file << "v " << x - deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 10
                    file << "v " << x - deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 11
                    file << "v " << x - deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 12
                    /// Negyedik negyed pontjai
                    file << "v " << x << " " << y << " " << z - r * 2 << "\n"; /// 13
                    file << "v " << x + deltaX3 << " " << y << " " << z - deltaZ3 << "\n"; /// 14
                    file << "v " << x + deltaX2 << " " << y << " " << z - deltaZ2 << "\n"; /// 15
                    file << "v " << x + deltaX1 << " " << y << " " << z - deltaZ1 << "\n"; /// 16

                    /// A kiszelesites oldalanak kirajzolasa
                    for (int j = 1; j < 16; j++) {
                        file << "f " << j + n << " " << j + n - 17 << " " << j + n + 1 << "\n";
                        file << "f " << j + n - 16 << " " << j + n - 17 << " " << j + n + 1 << "\n";
                    }
                    file << "f " << 16 + n << " " << n - 1 << " " << 1 + n << "\n";
                    file << "f " << n - 16 << " " << n - 1 << " " << 1 + n << "\n";

                    n = n + 16;
                } else {
                    file << "v " << points[i].coordinates[0] << " " << points[i].coordinates[1] << " "
                         << points[i].coordinates[2] << "\n";
                    for (int j = 1; j < 16; j++) {
                        file << "f " << j + n - 16 << " " << n + 16 + 1 - 16 << " " << j + n + 1 - 16 << "\n";
                    }
                    file << "f " << 16 + n - 16 << " " << n + 16 + 1 - 16 << " " << 1 + n - 16 << "\n";

                    n = n + 1;
                }
            } else {
                /// A felso pontok

                x = points[i].coordinates[0];
                y = points[i].coordinates[1] - a1 * 2;
                z = points[i].coordinates[2];

                /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
                zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r;
                xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r;
                deltaX1 = xNull - x;
                deltaZ1 = zNull - z;
                zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r;
                xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r;
                deltaX2 = xNull - x;
                deltaZ2 = zNull - z;
                zNull = points[i].coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r;
                xNull = points[i].coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r;
                deltaX3 = xNull - x;
                deltaZ3 = zNull - z;
                if (y < points[i + 1].coordinates[1]) {
                    y = y + a1 * 2;
                }

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
        } else {
            i++;
        }
    }
    file.close();
}

/**
 * Ha egy atlo metszi az alakzatot, akkor azt torli
 * @param edges az atlok
 * @param meshObject az alakzat
 * @param l hibahatar
 * @since 2.1.3
 */
void deleteWrongDiagonals(std::vector<Edge>& edges, OpenMesh::PolyMesh_ArrayKernelT<> meshObject, double l){
    double e = l / 100;
    /// Vegigmegyunk az eleken
    for (int i = 0; i < (int)edges.size(); i++){
        /// A haromszog csucspontjai es az atlo ket vegpontja
        Point A, B, C, P, Q;
        /// Vegigmegyunk az alakzatot alkoto haromszogeken
        for(MyMesh::FaceIter fi = meshObject.faces_begin(); fi != meshObject.faces_end(); fi++){
            MyMesh::FaceHandle fh = *fi;
            int c = 0;
            /// Vegigmegyunk a haromszog csucspontjain
            for (MyMesh::FaceVertexIter fvi = meshObject.fv_iter(fh); fvi.is_valid(); ++fvi) {
                MyMesh::VertexHandle vh = *fvi;

                if(c == 0) {
                    A.coordinates[0] = meshObject.point(vh)[0];
                    A.coordinates[1] = meshObject.point(vh)[1];
                    A.coordinates[2] = meshObject.point(vh)[2];
                } else if(c == 1) {
                    B.coordinates[0] = meshObject.point(vh)[0];
                    B.coordinates[1] = meshObject.point(vh)[1];
                    B.coordinates[2] = meshObject.point(vh)[2];
                } else if(c == 2) {
                    C.coordinates[0] = meshObject.point(vh)[0];
                    C.coordinates[1] = meshObject.point(vh)[1];
                    C.coordinates[2] = meshObject.point(vh)[2];
                }
                c++;
            }
            P.coordinates[0] = edges[i].p1.coordinates[0];
            P.coordinates[1] = edges[i].p1.coordinates[1];
            P.coordinates[2] = edges[i].p1.coordinates[2];
            Q.coordinates[0] = edges[i].p2.coordinates[0];
            Q.coordinates[1] = edges[i].p2.coordinates[1];
            Q.coordinates[2] = edges[i].p2.coordinates[2];

            /// Kiszamoljuk a haromszog normalvektorat
            Point normal;
            normal.coordinates[0] = (B.coordinates[1] - A.coordinates[1]) * (C.coordinates[2] - A.coordinates[2]) -
                                    (B.coordinates[2] - A.coordinates[2]) * (C.coordinates[1] - A.coordinates[1]);
            normal.coordinates[1] = (B.coordinates[2] - A.coordinates[2]) * (C.coordinates[0] - A.coordinates[0]) -
                                    (B.coordinates[0] - A.coordinates[0]) * (C.coordinates[2] - A.coordinates[2]);
            normal.coordinates[2] = (B.coordinates[0] - A.coordinates[0]) * (C.coordinates[1] - A.coordinates[1]) -
                                    (B.coordinates[1] - A.coordinates[1]) * (C.coordinates[0] - A.coordinates[0]);

            /// Kiszamoljuk az egyenes iranyvektorat
            Point direction;
            direction.coordinates[0] = Q.coordinates[0] - P.coordinates[0];
            direction.coordinates[1] = Q.coordinates[1] - P.coordinates[1];
            direction.coordinates[2] = Q.coordinates[2] - P.coordinates[2];

            /// A nevezo nem lehet 0
            double denominator = normal.coordinates[0] * direction.coordinates[0]
                                 + normal.coordinates[1] * direction.coordinates[1]
                                 + normal.coordinates[2] * direction.coordinates[2];

            double t = (normal.coordinates[0] * (A.coordinates[0] - P.coordinates[0])
                        + normal.coordinates[1] * (A.coordinates[1] - P.coordinates[1])
                        + normal.coordinates[2] * (A.coordinates[2] - P.coordinates[2])) / denominator;

            Point intersectionPoint;
            intersectionPoint.coordinates[0] = P.coordinates[0] + t * direction.coordinates[0];
            intersectionPoint.coordinates[1] = P.coordinates[1] + t * direction.coordinates[1];
            intersectionPoint.coordinates[2] = P.coordinates[2] + t * direction.coordinates[2];

            double maxX = std::max(P.coordinates[0], Q.coordinates[0]);
            double minX = std::min(P.coordinates[0], Q.coordinates[0]);
            double maxY = std::max(P.coordinates[1], Q.coordinates[1]);
            double minY = std::min(P.coordinates[1], Q.coordinates[1]);
            double maxZ = std::max(P.coordinates[2], Q.coordinates[2]);
            double minZ = std::min(P.coordinates[2], Q.coordinates[2]);

            /// Ha az atlo nem parhuzamos a haromszog normalvektoral es nem a szakaszon van
            if (std::abs(denominator - 0) > e &&
                intersectionPoint.coordinates[0] > maxX &&
                intersectionPoint.coordinates[0] < minX &&
                intersectionPoint.coordinates[1] > maxY &&
                intersectionPoint.coordinates[1] < minY &&
                intersectionPoint.coordinates[2] > maxZ &&
                intersectionPoint.coordinates[2] < minZ) {
                edges.erase(edges.begin() + i);
                i--;
                break;
            }
        }
    }
}

/**
 * A parameterkent kapott pontok kozotti alatamasztasok kozott generalja a keresztmerevitoket
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter az alatamasztas atmeroje
 * @param l a kuszobertek
 * @param meshObject az alakzat
 * @since 2.1.2
 */
void generateAndWriteSupportCrossBrace(const std::string &outputFileName, const std::string &inputFileName,
                                       std::vector<Point> &points, double diameter, double l,
                                       OpenMesh::PolyMesh_ArrayKernelT<> meshObject) {

    /// @since 2.1.3
    /// A tamaszok csucsa
    double a1 = std::cos(M_PI / 6) * diameter;
    /// A hibahatar
    double e = l / 100;

    /// A keresztmerevitok lehetseges kozeppontjai
    std::vector<Point> crossBraceEndPoints;

    /// A keresztmerevitok egyenesekke alakitasa, ahova kell
    std::vector<Edge> crossBraceEdges;

    /// A tamaszok ket vegpontjabol elek generalasa a konnyebb hasznalathoz
    std::vector<Edge> supportEdges;
    for (int i = 1; i < (int)points.size(); i++){
        Point p1;
        Point p2;
        p1.coordinates[0] = points[i].coordinates[0];
        p1.coordinates[1] = points[i].coordinates[1] + a1*2;
        p1.coordinates[2] = points[i].coordinates[2];
        p2.coordinates[0] = points[i-1].coordinates[0];
        p2.coordinates[1] = points[i-1].coordinates[1] - a1*2;
        p2.coordinates[2] = points[i-1].coordinates[2];
        supportEdges.emplace_back(p1, p2, 0, l/100);
    }
    std::sort(supportEdges.begin(), supportEdges.end(), Edge::sort);

    /// A keresztmerevitok kiszamitasa
    for(int i = 0; i < (int)supportEdges.size(); i++){ /// Vegig megy az osszes tamaszon
        for(int j = 0; j < (int)supportEdges.size(); j++){

            double epsilon = 0;
            double distance, dX, dY;
            dX = std::abs(supportEdges[i].p1.coordinates[0] - supportEdges[j].p1.coordinates[0]);
            dY = std::abs(supportEdges[i].p1.coordinates[2] - supportEdges[j].p1.coordinates[2]);
            distance = dX + dY;
            double startY;
            if (supportEdges[i].p1.coordinates[1] > supportEdges[j].p1.coordinates[1]) {
                startY = supportEdges[i].p1.coordinates[1];
            } else {
                startY = supportEdges[j].p1.coordinates[1];
            }

            /// Ha a ket tamasz nincs tul messze, illetve ha parhuzamosak az x es z tengelyre az atlok
            if (distance <= 6 * l && ((dX != 0 && dY == 0) || (dX == 0 && dY != 0))) {
                /// Felfele mozog a tamaszok tavolsaga alapjan, hogy ne legyen felesleges kereszt merevito
                while ((startY + epsilon - supportEdges[i].p2.coordinates[1]) <= e) {

                    if (((startY + epsilon + distance - supportEdges[j].p2.coordinates[1]) <= e) &&
                        ((startY + epsilon + distance - supportEdges[j].p1.coordinates[1]) >= e)) {


                        Point actualPoint = supportEdges[i].p1;
                        actualPoint.coordinates[1] = startY + epsilon;

                        Point neighbourPoint = supportEdges[j].p1;
                        neighbourPoint.coordinates[1] = startY + epsilon + distance;

                        double storagedDistance;
                        if(dX != 0) {
                            storagedDistance = dX;
                        } else {
                            storagedDistance = -dY;
                        }
                        crossBraceEdges.emplace_back(actualPoint, neighbourPoint, storagedDistance, l / 100);
                        epsilon = epsilon + distance;
                    } else {
                        break;
                    }
                }
            }
        }
    }

    /// Kitoroljuk azokat az atlokat, amik elmetszenek tamaszokat
    for(int i = 0; i < (int)crossBraceEdges.size(); i++){
        for(auto & supportEdge : supportEdges){
            double a1X = crossBraceEdges[i].p1.coordinates[0];
            double a1Y = crossBraceEdges[i].p1.coordinates[1];
            double a1Z = crossBraceEdges[i].p1.coordinates[2];
            double a2X = crossBraceEdges[i].p2.coordinates[0];
            double a2Y = crossBraceEdges[i].p2.coordinates[1];
            double a2Z = crossBraceEdges[i].p2.coordinates[2];
            double t1X = supportEdge.p1.coordinates[0];
            double t1Z = supportEdge.p1.coordinates[2];
            double t2X = supportEdge.p2.coordinates[0];
            double t2Y = supportEdge.p2.coordinates[1];
            double t2Z = supportEdge.p2.coordinates[2];

            /// Ha az atlo ket vege kozott van egy tamasz
//TODO nem minden esetben torli ki a felesleges atlokat neha meg tul sokat is
            if(((((a1X - t1X < -e) && (t1X - a2X < -e)) ||
                 ((a2X - t1X < -e) && (t1X - a1X < -e))) &&
                ((std::abs(a1Z - t1Z) <= e) && (std::abs(a1Z - a2Z) <= e))) ||
               ((((a1Z - t1Z < -e) && (t1Z - a2Z < -e)) ||
                 ((a2Z - t1Z < -e) && (t1Z - a1X < -e))) &&
                ((std::abs(a1X - t1X) <= e) && (std::abs(a1X - a2X) <= e)))) {

                if( (((a2Y - a1Y) * (t2X - a1X)) / (a2X - a1X) + a1Y - t2Y) <= e ||
                    (((a1Y - a2Y) * (t2X - a2X)) / (a1X - a2X) + a2Y - t2Y) <= e ||
                    (((a2Y - a1Y) * (t2Z - a1Z)) / (a2Z - a1Z) + a1Y - t2Y) <= e ||
                    (((a1Y - a2Y) * (t2Z - a2Z)) / (a1Z - a2Z) + a2Y - t2Y) <= e
                        ) {
                    crossBraceEdges.erase(crossBraceEdges.begin() + i);
                    i--;
                    break;
                }
            }
        }
    }

    deleteWrongDiagonals(crossBraceEdges, std::move(meshObject), l);

    /// A keresztmerevito hengerek kirajzolasa
    /// @since 2.1.2

    std::ofstream file(outputFileName);
    if(!file){
        std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
        exit(1);
    }
    /// A kimeneti file fejlece
    file << "# Diagonal support objects generated from " << inputFileName << " by BTMLYV\n";

    int n = 0;
    /// A henger sugara
    double r = diameter/2;
    /// A korlep pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
    double deltaX1, deltaX2, deltaX3, deltaZ1, deltaZ2, deltaZ3;
    /// A masodik korlap pont
    double xNull, zNull;
    /// A korlap kozeppontja
    double x, y, z;
    for(auto & crossBraceEdge : crossBraceEdges){
        /// Az also pontok

        x = crossBraceEdge.p1.coordinates[0];
        y = crossBraceEdge.p1.coordinates[1];
        z = crossBraceEdge.p1.coordinates[2];

        /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
        zNull = crossBraceEdge.p1.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r;
        xNull = crossBraceEdge.p1.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r;
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = crossBraceEdge.p1.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r;
        xNull = crossBraceEdge.p1.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r;
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = crossBraceEdge.p1.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r;
        xNull = crossBraceEdge.p1.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r;
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

        x = crossBraceEdge.p2.coordinates[0];
        y = crossBraceEdge.p2.coordinates[1];
        z = crossBraceEdge.p2.coordinates[2];

        /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
        zNull = crossBraceEdge.p2.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 1))*r;
        xNull = crossBraceEdge.p2.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 1))*r;
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = crossBraceEdge.p2.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 2))*r;
        xNull = crossBraceEdge.p2.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 2))*r;
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = crossBraceEdge.p2.coordinates[2] + std::cos(M_PI/2 - (M_PI/8 * 3))*r;
        xNull = crossBraceEdge.p2.coordinates[0] + std::sin(M_PI/2 - (M_PI/8 * 3))*r;
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