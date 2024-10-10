//
// Created by peros on 2024.04.03.
//
/**
 * A beolvasast es az alatamasztando pontokat kiszamolo fuggvenyeket tarolo file
 *
 * @author Eros Pal
 * @since 2024.04.03.
 */

#include <string>
#include <vector>
#include "supportPoints.h"

/**
 * Az alakzat beolvasasaert es az alatamasztando pontokert felelos fuggveny
 * @param diameter az alatamasztas vastagsaga
 * @param l a sugarak kozti tavolsag
 * @param e a double ertekek kovetkezteben megjeleno hiba korrekcios erteke
 * @param inputFile a bemeneti file
 * @param intersectPoints a metszespontok
 * @param count szamlalo
 * @param meshObject az alakzat
 * @param edges az alakzaton belul futo elek
 * @param inputPoints a bemeneti pontok
 * @param supportPointsAll az osszes alatamasztando pont
 * @param maxWeight a maximalis suly
 */
void supportPointsGenerated(double l, double e, const std::string &inputFile, std::vector<Point> &intersectPoints,
                            int &count, MyMesh &meshObject, std::vector<Edge> &edges, std::vector<Point> &inputPoints,
                            std::vector<Point> &supportPointsAll, double maxWeight) {

    readMesh(inputFile, meshObject);
    writeStartLog(inputFile);

    /// Vegigmegy az osszes tarolt haromszogon
    for(MyMesh::FaceIter fi = meshObject.faces_begin(); fi != meshObject.faces_end(); fi++){
        MyMesh::FaceHandle fh = *fi;

        /// Az aktualis haromszog csucspontjai
        Point p1, p2, p3;
        p1.coordinates[0] = 0;
        p1.coordinates[1] = 0;
        p1.coordinates[2] = 0;
        p1.e = e;
        p2.coordinates[0] = 0;
        p2.coordinates[1] = 0;
        p2.coordinates[2] = 0;
        p2.e = e;
        p3.coordinates[0] = 0;
        p3.coordinates[1] = 0;
        p3.coordinates[2] = 0;
        p3.e = e;
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
        double x = std::floor(minX / l) * l + e;
        while(x <= maxX){
            double z = std::floor(minZ / l) * l + e;
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
                    if (b1 >= 0 && b2 >= 0 && b3 >= 0) {
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
    deleteWrongPoints(intersectPoints, e);

    /// A kiiras a fileba
    writeInternalLines("outputs/1-internalLines.obj", inputFile, intersectPoints, "# Internal lines generated from ");
    writeLog("\tInternal lines written to file");

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
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0];
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] - l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] - l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2];
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2];
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] - l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] + l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0];
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] + l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
        adjacentPoint.coordinates[0] = intersectPoints[i].coordinates[0] + l;
        adjacentPoint.coordinates[1] = intersectPoints[i].coordinates[1];
        adjacentPoint.coordinates[2] = intersectPoints[i].coordinates[2] + l;
        if(isIncluded(intersectPoints, adjacentPoint, l)) {
            adjacentPoint.coordinates[1] = getY(intersectPoints, adjacentPoint, l);
            weight = thisEdgeLeadsToPoint(intersectPoints[i], adjacentPoint, l);
            if (weight != -1 && weight != 0) {
                edges.emplace_back(intersectPoints[i], adjacentPoint, weight, e);
            }
        }
    }

    writeInputEdges("outputs/2-inputEdges.obj", inputFile, edges);
    writeLog("\tInput edges written to file");

    for(auto & edge : edges){
        inputPoints.push_back(edge.p1);
        inputPoints.push_back(edge.p2);
    }
    std::sort(inputPoints.begin(), inputPoints.end(), comparePoints);
    for(int i = 0; i < (int)inputPoints.size(); i++){
        if(std::abs(inputPoints[i].coordinates[0] - inputPoints[i+1].coordinates[0]) <= e &&
           std::abs(inputPoints[i].coordinates[1] - inputPoints[i+1].coordinates[1]) <= e &&
           std::abs(inputPoints[i].coordinates[2] - inputPoints[i+1].coordinates[2]) <= e){
            inputPoints.erase(inputPoints.begin() + i);
            i--;
        }
    }

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
}