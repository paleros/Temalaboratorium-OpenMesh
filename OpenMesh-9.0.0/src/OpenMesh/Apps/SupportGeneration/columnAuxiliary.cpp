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
 * A parameterkent kapott pontok kozott haromszog alapu hasabokat csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a haromszog merete
 * @param minY a legkisebb y koordinata
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 1.5
 */
void generateAndWriteSupportLines(const std::string &outputFileName, const std::string &inputFileName,
                                  std::vector<Point> &points, double diameter, double minY, bool isFinish) {
    if(isFinish) {
        std::ofstream file(outputFileName);
        if (!file) {
            std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

        int n = 1;
        double a2 = std::cos(M_PI / 6) * diameter;
        double k = std::sin(M_PI / 6) * diameter;
        for (int i = 0; i < (int) points.size(); i++) {
            if (i % 2 != 0) {
                if (points[i]._coordinates[1] >= minY + a2 * 2) {
                    file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] + a2 * 2 << " "
                         << points[i]._coordinates[2] - diameter << "\n";
                    file << "v " << points[i]._coordinates[0] + a2 << " " << points[i]._coordinates[1] + a2 * 2 << " "
                         << points[i]._coordinates[2] + k << "\n";
                    file << "v " << points[i]._coordinates[0] - a2 << " " << points[i]._coordinates[1] + a2 * 2 << " "
                         << points[i]._coordinates[2] + k << "\n";
                } else {
                    file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] << " "
                         << points[i]._coordinates[2] - diameter << "\n";
                    file << "v " << points[i]._coordinates[0] + a2 << " " << points[i]._coordinates[1] << " "
                         << points[i]._coordinates[2] + k << "\n";
                    file << "v " << points[i]._coordinates[0] - a2 << " " << points[i]._coordinates[1] << " "
                         << points[i]._coordinates[2] + k << "\n";
                }

                /// Sok szamolas es probalkozas eredmenye
                file << "f " << n - 4 << " " << n - 3 << " " << n + 1 << "\n";
                file << "f " << n - 4 << " " << n + 1 << " " << n << "\n";
                file << "f " << n - 3 << " " << n - 2 << " " << n + 2 << "\n";
                file << "f " << n - 3 << " " << n + 2 << " " << n + 1 << "\n";
                file << "f " << n - 4 << " " << n - 2 << " " << n + 2 << "\n";
                file << "f " << n - 4 << " " << n + 2 << " " << n << "\n";
                n = n + 3;

                file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] << " "
                     << points[i]._coordinates[2] << "\n";
                if (points[i]._coordinates[1] >= minY + a2 * 2) {

                    file << "f " << n - 3 << " " << n - 2 << " " << n << "\n";
                    file << "f " << n - 2 << " " << n - 1 << " " << n << "\n";
                    file << "f " << n - 1 << " " << n - 3 << " " << n << "\n";
                }
                n = n + 1;

            } else {
                if (points[i]._coordinates[1] >= minY + a2 * 2) {
                    file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] - a2 * 2 << " "
                         << points[i]._coordinates[2] - diameter << "\n";
                    file << "v " << points[i]._coordinates[0] + a2 << " " << points[i]._coordinates[1] - a2 * 2 << " "
                         << points[i]._coordinates[2] + k << "\n";
                    file << "v " << points[i]._coordinates[0] - a2 << " " << points[i]._coordinates[1] - a2 * 2 << " "
                         << points[i]._coordinates[2] + k << "\n";
                } else {
                    file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] << " "
                         << points[i]._coordinates[2] - diameter << "\n";
                    file << "v " << points[i]._coordinates[0] + a2 << " " << points[i]._coordinates[1] << " "
                         << points[i]._coordinates[2] + k << "\n";
                    file << "v " << points[i]._coordinates[0] - a2 << " " << points[i]._coordinates[1] << " "
                         << points[i]._coordinates[2] + k << "\n";
                }

                n = n + 3;

                file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] << " "
                     << points[i]._coordinates[2] << "\n";
                if (points[i]._coordinates[1] >= minY + a2 * 2) {

                    file << "f " << n - 3 << " " << n - 2 << " " << n << "\n";
                    file << "f " << n - 2 << " " << n - 1 << " " << n << "\n";
                    file << "f " << n - 1 << " " << n - 3 << " " << n << "\n";
                }
                n = n + 1;

            }
        }
        file.close();
    }
}

/**
 * A parameterkent kapott pontok kozott hengereket csinal es kiirja a .obj fileba
 * @param outputFileName a kimeneti file neve
 * @param inputFileName a bemeneti file neve
 * @param points a pontok
 * @param diameter a henger atmeroje
 * @param minY a legkisebb y koordinata
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 2.1
 */
void generateAndWriteSupportCylinder(const std::string &outputFileName, const std::string &inputFileName,
                                     std::vector<Point> &points, double diameter, double minY, bool isFinish) {
    if (isFinish) {
        std::ofstream file(outputFileName);
        if (!file) {
            std::cout << "Error: The file " << outputFileName << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << "# Support objects generated from " << inputFileName << " by BTMLYV\n";

        int n = 0;
        double e = points[0]._e;
        /// A henger felso csucsanak magassaga
        double a1 = std::cos(M_PI / 6) * diameter;
        /// A henger also kiszelesedesenek magassaganak kilogo resze
        double a2 = std::cos(M_PI / 6) * diameter;
        /// A henger sugara
        double r = diameter / 2;
        /// A korlep pontjainak koordinata elterese a kozepponthoz kepest (elso negyed)
        double deltaX1, deltaX2, deltaX3, deltaZ1, deltaZ2, deltaZ3;
        /// A masodik korlap pont
        double xNull, zNull;
        /// A korlap kozeppontja
        double x, y, z;

        for (int i = 0; i < (int) points.size(); i++) {
            /// Az also es felso pontok kulonvalasztasa
            if (std::abs(points[i]._coordinates[1] - points[i + 1]._coordinates[1]) > a1 * 2 * 2) { /// Ha nem tul kicsi
                if (i % 2 != 0) {
                    /// Az also pontok
                    x = points[i]._coordinates[0];
                    y = points[i]._coordinates[1] + a1 * 2;
                    z = points[i]._coordinates[2];

                    /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
                    zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r;
                    xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r;
                    deltaX1 = xNull - x;
                    deltaZ1 = zNull - z;
                    zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r;
                    xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r;
                    deltaX2 = xNull - x;
                    deltaZ2 = zNull - z;
                    zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r;
                    xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r;
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
                    file << "v " << points[i - 1]._coordinates[0] << " " << points[i - 1]._coordinates[1] << " "
                         << points[i - 1]._coordinates[2] << "\n";
                    if (points[i - 1]._coordinates[1] >= minY + a2 * 2) {
                        for (int j = 1; j < 16; j++) {
                            file << "f " << j + n << " " << 16 + n + 17 << " " << j + n + 1 << "\n";
                        }
                        file << "f " << 16 + n << " " << 16 + n + 17 << " " << 1 + n << "\n";
                    }
                    n = n + 16 * 2 + 1;

                    /// A tamasz also szelesitesenek kirajzolasa
                    if (std::abs(points[i]._coordinates[1] - minY) <= e) {
                        /// Kiszamoljuk a henger alapjanak szamito legalso, szelesebb "kort", ami egy 16 szog lesz
                        zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r * 2;
                        xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r * 2;
                        deltaX1 = xNull - x;
                        deltaZ1 = zNull - z;
                        zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r * 2;
                        xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r * 2;
                        deltaX2 = xNull - x;
                        deltaZ2 = zNull - z;
                        zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r * 2;
                        xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r * 2;
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
                        file << "v " << points[i]._coordinates[0] << " " << points[i]._coordinates[1] << " "
                             << points[i]._coordinates[2] << "\n";
                        for (int j = 1; j < 16; j++) {
                            file << "f " << j + n - 16 << " " << n + 16 + 1 - 16 << " " << j + n + 1 - 16 << "\n";
                        }
                        file << "f " << 16 + n - 16 << " " << n + 16 + 1 - 16 << " " << 1 + n - 16 << "\n";

                        n = n + 1;
                    }
                } else {
                    /// A felso pontok

                    x = points[i]._coordinates[0];
                    y = points[i]._coordinates[1] - a1 * 2;
                    z = points[i]._coordinates[2];

                    /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
                    zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r;
                    xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r;
                    deltaX1 = xNull - x;
                    deltaZ1 = zNull - z;
                    zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r;
                    xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r;
                    deltaX2 = xNull - x;
                    deltaZ2 = zNull - z;
                    zNull = points[i]._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r;
                    xNull = points[i]._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r;
                    deltaX3 = xNull - x;
                    deltaZ3 = zNull - z;
                    if (y < points[i + 1]._coordinates[1]) {
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
                    A._coordinates[0] = meshObject.point(vh)[0];
                    A._coordinates[1] = meshObject.point(vh)[1];
                    A._coordinates[2] = meshObject.point(vh)[2];
                } else if(c == 1) {
                    B._coordinates[0] = meshObject.point(vh)[0];
                    B._coordinates[1] = meshObject.point(vh)[1];
                    B._coordinates[2] = meshObject.point(vh)[2];
                } else if(c == 2) {
                    C._coordinates[0] = meshObject.point(vh)[0];
                    C._coordinates[1] = meshObject.point(vh)[1];
                    C._coordinates[2] = meshObject.point(vh)[2];
                }
                c++;
            }
            P._coordinates[0] = edges[i].p1._coordinates[0];
            P._coordinates[1] = edges[i].p1._coordinates[1];
            P._coordinates[2] = edges[i].p1._coordinates[2];
            Q._coordinates[0] = edges[i].p2._coordinates[0];
            Q._coordinates[1] = edges[i].p2._coordinates[1];
            Q._coordinates[2] = edges[i].p2._coordinates[2];

            /// Kiszamoljuk a haromszog normalvektorat
            Point normal;
            normal._coordinates[0] = (B._coordinates[1] - A._coordinates[1]) * (C._coordinates[2] - A._coordinates[2]) -
                                     (B._coordinates[2] - A._coordinates[2]) * (C._coordinates[1] - A._coordinates[1]);
            normal._coordinates[1] = (B._coordinates[2] - A._coordinates[2]) * (C._coordinates[0] - A._coordinates[0]) -
                                     (B._coordinates[0] - A._coordinates[0]) * (C._coordinates[2] - A._coordinates[2]);
            normal._coordinates[2] = (B._coordinates[0] - A._coordinates[0]) * (C._coordinates[1] - A._coordinates[1]) -
                                     (B._coordinates[1] - A._coordinates[1]) * (C._coordinates[0] - A._coordinates[0]);

            /// Kiszamoljuk az egyenes iranyvektorat
            Point direction;
            direction._coordinates[0] = Q._coordinates[0] - P._coordinates[0];
            direction._coordinates[1] = Q._coordinates[1] - P._coordinates[1];
            direction._coordinates[2] = Q._coordinates[2] - P._coordinates[2];

            /// A nevezo nem lehet 0
            double denominator = normal._coordinates[0] * direction._coordinates[0]
                                 + normal._coordinates[1] * direction._coordinates[1]
                                 + normal._coordinates[2] * direction._coordinates[2];

            double t = (normal._coordinates[0] * (A._coordinates[0] - P._coordinates[0])
                        + normal._coordinates[1] * (A._coordinates[1] - P._coordinates[1])
                        + normal._coordinates[2] * (A._coordinates[2] - P._coordinates[2])) / denominator;

            Point intersectionPoint;
            intersectionPoint._coordinates[0] = P._coordinates[0] + t * direction._coordinates[0];
            intersectionPoint._coordinates[1] = P._coordinates[1] + t * direction._coordinates[1];
            intersectionPoint._coordinates[2] = P._coordinates[2] + t * direction._coordinates[2];

            double maxX = std::max(P._coordinates[0], Q._coordinates[0]);
            double minX = std::min(P._coordinates[0], Q._coordinates[0]);
            double maxY = std::max(P._coordinates[1], Q._coordinates[1]);
            double minY = std::min(P._coordinates[1], Q._coordinates[1]);
            double maxZ = std::max(P._coordinates[2], Q._coordinates[2]);
            double minZ = std::min(P._coordinates[2], Q._coordinates[2]);

            /// Ha az atlo nem parhuzamos a haromszog normalvektoral es nem a szakaszon van
            if (std::abs(denominator - 0) > e &&
                intersectionPoint._coordinates[0] > maxX &&
                intersectionPoint._coordinates[0] < minX &&
                intersectionPoint._coordinates[1] > maxY &&
                intersectionPoint._coordinates[1] < minY &&
                intersectionPoint._coordinates[2] > maxZ &&
                intersectionPoint._coordinates[2] < minZ) {
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
 * @param isFinish igaz, ha a vegso kiiratasrol van szo
 * @since 2.1.2
 */
void generateAndWriteSupportCrossBrace(const std::string &outputFileName, const std::string &inputFileName,
                                       std::vector<Point> &points, double diameter, double l,
                                       OpenMesh::PolyMesh_ArrayKernelT<> meshObject, bool isFinish) {

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
        p1._coordinates[0] = points[i]._coordinates[0];
        p1._coordinates[1] = points[i]._coordinates[1] + a1 * 2;
        p1._coordinates[2] = points[i]._coordinates[2];
        p2._coordinates[0] = points[i - 1]._coordinates[0];
        p2._coordinates[1] = points[i - 1]._coordinates[1] - a1 * 2;
        p2._coordinates[2] = points[i - 1]._coordinates[2];
        supportEdges.emplace_back(p1, p2, 0, l/100);
    }
    std::sort(supportEdges.begin(), supportEdges.end(), Edge::sort);

    /// A keresztmerevitok kiszamitasa
    for(int i = 0; i < (int)supportEdges.size(); i++){ /// Vegig megy az osszes tamaszon
        for(int j = 0; j < (int)supportEdges.size(); j++){

            double epsilon = 0;
            double distance, dX, dY;
            dX = std::abs(supportEdges[i].p1._coordinates[0] - supportEdges[j].p1._coordinates[0]);
            dY = std::abs(supportEdges[i].p1._coordinates[2] - supportEdges[j].p1._coordinates[2]);
            distance = dX + dY;
            double startY;
            if (supportEdges[i].p1._coordinates[1] > supportEdges[j].p1._coordinates[1]) {
                startY = supportEdges[i].p1._coordinates[1];
            } else {
                startY = supportEdges[j].p1._coordinates[1];
            }

            /// Ha a ket tamasz nincs tul messze, illetve ha parhuzamosak az x es z tengelyre az atlok
            if (distance <= 6 * l && ((dX != 0 && dY == 0) || (dX == 0 && dY != 0))) {
                /// Felfele mozog a tamaszok tavolsaga alapjan, hogy ne legyen felesleges kereszt merevito
                while ((startY + epsilon - supportEdges[i].p2._coordinates[1]) <= e) {

                    if (((startY + epsilon + distance - supportEdges[j].p2._coordinates[1]) <= e) &&
                        ((startY + epsilon + distance - supportEdges[j].p1._coordinates[1]) >= e)) {


                        Point actualPoint = supportEdges[i].p1;
                        actualPoint._coordinates[1] = startY + epsilon;

                        Point neighbourPoint = supportEdges[j].p1;
                        neighbourPoint._coordinates[1] = startY + epsilon + distance;

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
            double a1X = crossBraceEdges[i].p1._coordinates[0];
            double a1Y = crossBraceEdges[i].p1._coordinates[1];
            double a1Z = crossBraceEdges[i].p1._coordinates[2];
            double a2X = crossBraceEdges[i].p2._coordinates[0];
            double a2Y = crossBraceEdges[i].p2._coordinates[1];
            double a2Z = crossBraceEdges[i].p2._coordinates[2];
            double t1X = supportEdge.p1._coordinates[0];
            double t1Z = supportEdge.p1._coordinates[2];
            double t2X = supportEdge.p2._coordinates[0];
            double t2Y = supportEdge.p2._coordinates[1];
            double t2Z = supportEdge.p2._coordinates[2];

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
    if (isFinish) {
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

        x = crossBraceEdge.p1._coordinates[0];
        y = crossBraceEdge.p1._coordinates[1];
        z = crossBraceEdge.p1._coordinates[2];

        /// Kiszamoljuk a henger alapjanak szamito felso "kort", ami egy 16 szog lesz
        zNull = crossBraceEdge.p1._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r;
        xNull = crossBraceEdge.p1._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r;
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = crossBraceEdge.p1._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r;
        xNull = crossBraceEdge.p1._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r;
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = crossBraceEdge.p1._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r;
        xNull = crossBraceEdge.p1._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r;
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

        x = crossBraceEdge.p2._coordinates[0];
        y = crossBraceEdge.p2._coordinates[1];
        z = crossBraceEdge.p2._coordinates[2];

        /// Kiszamoljuk a henger alapjanak szamito also "kort", ami egy 16 szog lesz
        zNull = crossBraceEdge.p2._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 1)) * r;
        xNull = crossBraceEdge.p2._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 1)) * r;
        deltaX1 = xNull - x;
        deltaZ1 = zNull - z;
        zNull = crossBraceEdge.p2._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 2)) * r;
        xNull = crossBraceEdge.p2._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 2)) * r;
        deltaX2 = xNull - x;
        deltaZ2 = zNull - z;
        zNull = crossBraceEdge.p2._coordinates[2] + std::cos(M_PI / 2 - (M_PI / 8 * 3)) * r;
        xNull = crossBraceEdge.p2._coordinates[0] + std::sin(M_PI / 2 - (M_PI / 8 * 3)) * r;
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
    file.close();
    }
}