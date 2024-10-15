//
// Created by peros on 2024.10.08.
//
/**
 * Az alakzat optimalis forgatasat vegzo fuggveny
 *
 * @author Eros Pal
 * @since 2024.10.08.
 */

#include "rotationMain.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "auxiliary.h"
#include "supportPoints.h"
#include "columnMain.h"
#include "treeMain.h"
#include "rotationAuxiliary.h"
#include "nelder-mead.hh"

/**
 * Kiszamitja hogy a megadott elforgatassal mennyire jo a tamasztas
 * @param angles az elforgatas szoge
 * @param inputFile a bemeneti fajl
 * @param isTreeSupport az alatamasztas tipusa
 * @return
 */
double getPoint(std::vector<double> &angles, std::string& inputFile, bool isTreeSupport) {

    /// Valtozok inicializalasa

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;
    /// A fa csoporitasi erteke
    int groupingValue = 5;
    /// A racs tavolsaga
    double l;

    /// Az alakzat
    MyMesh meshObject;

    /// A maximalis suly
    double maxWeight = M_PI / 4 * 3 *2;

    /// A metszespontok x, y, es z koordinatait tarolja
    std::vector<Point> intersectPoints;

    /// Szamlalo
    int count = 0;

    /// A bemeneti pontpok kozotti elek tarolasara szolgalo tomb
    std::vector<Edge> edges;

    /// Az alatamasztando pontok kiszamitasa
    /// @since 1.3
    std::vector<Point> inputPoints;

    std::vector<Point> supportPointsAll;

    writeLog("\tBasic parameters set");

    /// Beolvassuk az alakzatot es kiszamoljuk az alatamasztando pontokat
    /// @since 2.2
    readMesh(inputFile, meshObject);

    rotateMesh(meshObject, angles[0], angles[1]);

    calculateDiameterAndL(l, diameter, meshObject);

    /// A szamitasi hibak korrekcios erteke
    double e = l / 100;

    supportPointsGenerated(l, e, inputFile, intersectPoints, count, meshObject, edges,
                           inputPoints, supportPointsAll, maxWeight);

    /// Az alatamasztas pontszamanak kiszamitasa
    double point = 0;

    /// Az alatamasztas tipusa "oszlop"
    if (!isTreeSupport){
        point = columnSupportGenerated(meshObject, inputFile, supportPointsAll, intersectPoints, diameter, l, e);
    }

    /// Az alatamasztas tipusa "fa"
    if (isTreeSupport){
        point = treeSupportGenerated(meshObject, inputFile, supportPointsAll, diameter, l, e, groupingValue);
    }

    return point;
}

/**
 * Az alakzat optimalis forgatasat megkereso fuggveny
 * Nelder-Mead algoritmussal
 * @param inputFile a bemeneti fajl
 * @param isTreeSupport az alatamasztas tipusa
 * @return az optimalis forgatas
 * @since 4.1
 */
std::vector<double> optimalSide(std::string& inputFile, bool isTreeSupport){

    /// Igy csak az elso parametert tudja valtoztatni
    auto function = [&inputFile, isTreeSupport](const std::vector<double>& angles) {
        return getPoint(const_cast<std::vector<double>&>(angles), inputFile, isTreeSupport);
    };

    /// A kezdeti szogek
    std::vector<double> angles = {0, 0};

    NelderMead::optimize(function, angles, 100, 0, 1);

    writeLog("\tFind optimal rotation");
    return angles;
}

/**
 * A PROGRAM FUTTATASA
 * @param inputFile a bemeneti file
 * @param isTreeSupport az alatamasztas tipusa
 * @param findOptimalSide az optimalis oldal keresese
 * @since 4.1
 */
void run(std::string inputFile, bool isTreeSupport, bool findOptimalSide) {
    writeStartLog(inputFile);

    std::vector<double> angles = {0, 0};
    if (findOptimalSide){
        angles = optimalSide(inputFile, isTreeSupport);
    }
    /// Valtozok inicializalasa

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;
    /// A fa csoporitasi erteke
    int groupingValue = 5;
    /// A racs tavolsaga
    double l;

    /// Az alakzat
    MyMesh meshObject;

    /// A maximalis suly
    double maxWeight = M_PI / 4 * 3 *2;

    /// A metszespontok x, y, es z koordinatait tarolja
    std::vector<Point> intersectPoints;

    /// Szamlalo
    int count = 0;

    /// A bemeneti pontpok kozotti elek tarolasara szolgalo tomb
    std::vector<Edge> edges;

    /// Az alatamasztando pontok kiszamitasa
    /// @since 1.3
    std::vector<Point> inputPoints;

    std::vector<Point> supportPointsAll;

    writeLog("\tBasic parameters set");

    /// Beolvassuk az alakzatot es kiszamoljuk az alatamasztando pontokat
    /// @since 2.2
    readMesh(inputFile, meshObject);

    /// Az alakzat elforgatasa optimalis helyzetbe
    rotateMesh(meshObject, angles[0], angles[1]);

    writeMesh(meshObject, "outputs/object.obj");

    calculateDiameterAndL(l, diameter, meshObject);

    /// A szamitasi hibak korrekcios erteke
    double e = l / 100;

    supportPointsGenerated(l, e, inputFile, intersectPoints, count, meshObject, edges,
                           inputPoints, supportPointsAll, maxWeight);

    /// Az alatamasztas tipusa "oszlop"
    if (!isTreeSupport){
        columnSupportGenerated(meshObject, inputFile, supportPointsAll, intersectPoints, diameter, l, e);
    }

    /// Az alatamasztas tipusa "fa"
    if (isTreeSupport){
        treeSupportGenerated(meshObject, inputFile, supportPointsAll, diameter, l, e, groupingValue);
    }
}