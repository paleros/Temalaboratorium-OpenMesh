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
#include <nelder-mead.hh>

double getPoint(const std::vector<double> &angles) {

    /// Az alatamasztas oszlopanak vastagsagahoz
    double diameter;
    /// A fa csoporitasi erteke
    int groupingValue;





    /// Az alakzat fogatasa
    //rotateMesh(meshObject, angles[0], angles[1]);

    //TODO a pontok kiszamitasa

}


/**
 * Az alakzat optimalis forgatasat megkereso fuggveny
 * Nelder-Mead algoritmussal
 * @param meshObject az alakzat
 * @since 4.1
 */
void findOptimalSide(MyMesh &meshObject){

    /// A kezdeti szogek
    std::vector<double> angles = {0, 0};

    //NelderMead::optimize(getPoint, angles, 10, 1e-5, 1);

        rotateMesh(meshObject, angles[0], angles[1]);
        writeLog("\tFind optimal rotation.");
}