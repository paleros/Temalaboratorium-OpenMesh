//
// Created by peros on 2024.10.08.
//

#ifndef OPENMESH_ROTATIONAUXILIARY_H
#define OPENMESH_ROTATIONAUXILIARY_H
#include "auxiliary.h"

/**
 * Az alakzat forgatasa a z tengely korul
 * @param meshObject az alakzat
 * @param theta szog a z tengely koruli forgatashoz
 * @since 4.1
 */
void rotateZ(MyMesh &meshObject, double theta);

/**
 * Az alakzat forgatasa a y tengely korul
 * @param meshObject az alakzat
 * @param phi szog a y tengely koruli forgatashoz
 * @since 4.1
 */
void rotateY(MyMesh &meshObject, double phi);

/**
 * Az alakzat forgatasa a tengelyek korul
 * @param meshObject az alakzat
 * @param theta szog a z tengely koruli forgatashoz
 * @param phi szog a y tengely koruli forgatashoz
 * @since 4.1
 */
void rotateMesh(MyMesh &meshObject, double theta, double phi);

#endif //OPENMESH_ROTATIONAUXILIARY_H
