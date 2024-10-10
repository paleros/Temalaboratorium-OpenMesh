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


/**
 * Az alakzat optimalis forgatasa
 * @param meshObject az alakzat
 * @since 4.1
 */
void findOptimalSide(MyMesh &meshObject){

    double optimalTheta;    /// A z tengely koruli forgatas optimalis szoge
    double optimalPhi;      /// A y tengely koruli forgatas optimalis szoge

    MyMesh newMesh = meshObject;

    double bestValue;   /// Az eddig legoptimalisabb forgatas erteke

    for (int theta = 0; theta < 360; theta += 10){
        for (int phi = 0; phi < 360; phi += 10){

            rotateMesh(newMesh, theta, phi);

            double value = calculateValue(newMesh);

            if (value < bestValue){
                bestValue = value;
                optimalTheta = theta;
                optimalPhi = phi;
            }
        }
    }

    /// Az optimalis forgatas elvegzese
    rotateMesh(meshObject, optimalTheta, optimalPhi);
    writeLog("\tFind optimal rotation.");
}