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
#include "OpenMesh/Core/IO/MeshIO.hh"
#include <utility>
#include "auxiliary.h"
#include "supportPoints.h"
#include "columnMain.h"
#include "treeMain.h"
#include "rotationAuxiliary.h"
#include "nelder-mead.hh"
#include "direct.hh"

/**
 * Kiszamitja hogy a megadott elforgatassal mennyire jo a tamasztas
 * @param angles az elforgatas szoge
 * @param mesh az alakzat
 * @param inputFile a bemeneti fajl
 * @param supportType az alatamasztas tipusa
 * @param roundNumber a aktualis kor szama
 * @return
 */
double getOptimalValue(std::vector<double> &angles, MyMesh mesh, std::string &inputFile, SupportType supportType,
                       int &roundNumber) {
   /// Valtozok inicializalasa

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;
    /// A fa csoporitasi erteke
    int groupingValue = 5;
    /// A racs tavolsaga
    double l;

    /// Az alakzat
    MyMesh meshObject;
    meshObject = std::move(mesh);

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

    /// Beolvassuk az alakzatot es kiszamoljuk az alatamasztando pontokat
    /// @since 2.2
    readMesh(inputFile, meshObject);

    rotateMesh(meshObject, angles[0], angles[1]);

    calculateDiameterAndL(l, diameter, meshObject);

    /// A szamitasi hibak korrekcios erteke
    double e = l / 100;

    supportPointsGenerated(l, e, inputFile, intersectPoints, count, meshObject, edges,
                           inputPoints, supportPointsAll, maxWeight, false);

    /// Az alatamasztas pontszamanak kiszamitasa
    double value;

    /// Az alatamasztas tipusa "oszlop"
    if (supportType == SupportType::COLUMN) {
        value = columnSupportGenerated(meshObject, inputFile, supportPointsAll, intersectPoints, diameter, l, e, false);
    }

    /// Az alatamasztas tipusa "fa"
    if (supportType == SupportType::TREE) {
        value = treeSupportGenerated(meshObject, inputFile, supportPointsAll, diameter, l, e, groupingValue, false);
    }

    /// Logolja a probalkozasok szamat es a forgatas szoget
    std::string log = " Rotate object x: " + std::to_string(angles[0]) + "\tz:\t" + std::to_string(angles[1]) +
                      "\t(rad)\tsumSupportLength: " + std::to_string(value) + "\t\tround: " + std::to_string(roundNumber);

    writeLog(log);
    roundNumber++;

    return value;
}

/**
 * Az alakzat optimalis forgatasat megkereso fuggveny
 * Nelder-Mead vagy Direct algoritmussal
 * @param inputFile a bemeneti fajl
 * @param mesh az alakzat
 * @param supportType az alatamasztas tipusa
 * @param algorithmType az optimalizalasi algoritmus
 * @return az optimalis forgatas
 * @since 4.1
 */
std::vector<double> optimalSide(std::string &inputFile, MyMesh &mesh, SupportType supportType, AlgorithmType algorithmType) {
    int roundNumber = 1;

    /// Igy csak az elso parametert tudja valtoztatni
    auto function = [mesh, &inputFile, supportType, &roundNumber](std::vector<double> angles) {
        return getOptimalValue(angles, mesh, inputFile, supportType, roundNumber);
    };

    /// A kezdeti szogek
    std::vector<double> angles = {0, 0};
    std::vector<double> maxAngles = {2*M_PI, 2*M_PI};

    if (algorithmType == AlgorithmType::NELDERMEAD) {

        /// Nelder-Mead algoritmus
        writeLog("\tUse Nelder-Mead algorithm");
        NelderMead::optimize(function, angles, 100, 0.0001, M_PI-0.4);

    }else if(algorithmType == AlgorithmType::DIRECT){

        /// Direct algoritmus
        writeLog("\tUse Direct algorithm");
        angles = DIRECT::optimize(function, angles, maxAngles, 100, 0.0001);

    }else{
        std::cout << "Error: algorithm not found!" << std::endl;
        exit(1);
    }

    writeLog("\tFind optimal rotation");
    return angles;
}

/**
 * A PROGRAM FUTTATASA
 * @param inputFile a bemeneti file
 * @param supportType az alatamasztas tipusa
 * @param algorithmType az optimalis oldal keresese
 * @since 4.1
 */
void run(std::string inputFile, SupportType supportType, AlgorithmType algorithmType) {
    writeStartLog(inputFile);

    /// Az alakzat
    MyMesh meshObject;

    /// Beolvassuk az alakzatot es kiszamoljuk az alatamasztando pontokat
    /// @since 2.2
    readMesh(inputFile, meshObject);

    std::vector<double> angles = {0, 0};
    if (algorithmType != AlgorithmType::NONE) {

        angles = optimalSide(inputFile, meshObject, supportType, algorithmType);
        writeLog("\tOptimal rotation found: x: " + std::to_string(angles[0]) + " z: " + std::to_string(angles[1]) +
                 " (rad)");
    }
    /// Valtozok inicializalasa

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;
    /// A fa csoporitasi erteke
    int groupingValue = 5;
    /// A racs tavolsaga
    double l;

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
                           inputPoints, supportPointsAll, maxWeight, true);

    /// Az alatamasztas tipusa "oszlop"
    if (supportType == SupportType::COLUMN){
        columnSupportGenerated(meshObject, inputFile, supportPointsAll, intersectPoints, diameter/2, l, e, true);
    }

    /// Az alatamasztas tipusa "fa"
    if (supportType == SupportType::TREE){
        treeSupportGenerated(meshObject, inputFile, supportPointsAll, diameter, l, e, groupingValue, true);
    }
}