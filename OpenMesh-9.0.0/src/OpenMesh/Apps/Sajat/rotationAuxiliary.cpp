//
// Created by peros on 2024.10.08.
//

#include "rotationAuxiliary.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "auxiliary.h"
#include "treeMain.h"
#include "columnMain.h"
#include "supportPoints.h"

/**
 * Az alakzat forgatasa a z tengely korul
 * @param meshObject az alakzat
 * @param theta szog a z tengely koruli forgatashoz
 * @since 4.1
 */
void rotateZ(MyMesh &meshObject, double theta){
    for (MyMesh::VertexIter v_it = meshObject.vertices_begin(); v_it != meshObject.vertices_end(); ++v_it){
        MyMesh::Point p = meshObject.point(*v_it);
        double x = p[0];
        double y = p[1];
        double z = p[2];

        double xNew = x * cos(theta) - y * sin(theta);
        double yNew = x * sin(theta) + y * cos(theta);

        meshObject.set_point(*v_it, MyMesh::Point(xNew, yNew, z));
    }
}

/**
 * Az alakzat forgatasa a x tengely korul
 * @param meshObject az alakzat
 * @param phi szog a x tengely koruli forgatashoz
 * @since 4.1
 */
void rotateX(MyMesh &meshObject, double theta){
    for (MyMesh::VertexIter v_it = meshObject.vertices_begin(); v_it != meshObject.vertices_end(); ++v_it){
        MyMesh::Point p = meshObject.point(*v_it);
        double x = p[0];
        double y = p[1];
        double z = p[2];

        double yNew = y * cos(theta) - z * sin(theta);
        double zNew = y * sin(theta) + z * cos(theta);

        meshObject.set_point(*v_it, MyMesh::Point(x, yNew, zNew));
    }
}

/**
 * Az alakzat forgatasa a tengelyek korul
 * @param meshObject az alakzat
 * @param theta szog az x tengely koruli forgatashoz
 * @param phi szog a z tengely koruli forgatashoz
 * @since 4.1
 */
void rotateMesh(MyMesh &meshObject, double theta, double phi){
    /// Az alakzat forgatasa az x tengely korul
    rotateX(meshObject, theta);

    /// Az alakzat forgatasa a z tengely korul
    rotateZ(meshObject, phi);
}