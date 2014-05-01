#ifndef __MATHFUNCTIONS_H__
#define __MATHFUNCTIONS_H__

/*LICENSE_START*/ 
/*
 *  Copyright (c) 2014, Washington University School of Medicine
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>

namespace cifti {

/**
 * Various mathematical functions.
 */
//NOTE: in CiftiLib, we only reference matrixToQuatern and quaternToMatrix, both used only in NiftiHeader when using volume files, not cifti files
class MathFunctions {
public:
    static int64_t combinations(
                    const int64_t n,
                    const int64_t k);

    static int64_t permutations(
                    const int64_t n,
                    const int64_t k);

    static int64_t factorial(const int64_t n);

    static bool normalVector(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3],
                    float normalVectorOut[3]);

    static bool normalVector(
                    const double v1[3],
                    const double v2[3],
                    const double v3[3],
                    double normalVectorOut[3]);

    static void normalVectorDirection(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3],
                    float directionOut[3]);

    static void crossProduct(
                    const float v1[],
                    const float v2[],
                    float resultOut[]);

    static void crossProduct(
                             const double v1[],
                             const double v2[],
                             double resultOut[]);
    
    static void normalizedCrossProduct(
                    const float x1[],
                                 const float x2[],
                                 float resultOut[]);

    static float normalizeVector(
                    float vectorsAll[],
                    const int32_t offset);

    static float normalizeVector(float vectorInOut[3]);

    static double normalizeVector(double vectorInOut[3]);

    static float vectorLength(const float vector[3]);

    static float vectorLength(
                    const float vectorsAll[],
                    const int32_t offset);

    static double vectorLength(const double vector[3]);

    static float distanceSquared3D(
                    const float p1[3],
                    const float p2[3]);

    static float distanceSquared3D(
                    const float xyzAll[],
                    const int32_t offsetCoord1,
                    const int32_t offsetCoord2);

    static float distance3D(
                    const float p1[3],
                    const float p2[3]);

    static double distanceSquared3D(
                    const double p1[3],
                    const double p2[3]);

    static double distance3D(
                    const double p1[3],
                    const double p2[3]);

    static double distanceSquared2D(const double x1,
                                    const double y1,
                                    const double x2,
                                    const double y2);
    
    static void subtractVectors(
                    const float v1[3],
                    const float v2[3],
                    float resultOut[3]);

    static void addVectors(
                    const float v1[3],
                    const float v2[3],
                    float resultOut[3]);
    
    static void createUnitVector(
                    const float startXYZ[3],
                    const float endXYZ[3],
                    float unitVectorOut[3]);

    static void createUnitVector(
                                 const double startXYZ[3],
                                 const double endXYZ[3],
                                 double unitVectorOut[3]);
    
    static float dotProduct(
                    const float p1[3],
                    const float p2[3]);

    static float dotProduct(
                            const double p1[3],
                            const double p2[3]);
    
    static float triangleArea(
                    const float v1[3],
                    const float v2[3],
                    const float v3[3]);

    static float triangleArea(const double v1[3],
                              const double v2[3],
                              const double v3[3]);
    
    static float triangleArea(
                    const float xyzAll[],
                    const int32_t offsetCoord1,
                    const int32_t offsetCoord2,
                    const int32_t offsetCoord3);

    static float triangleAreaSigned2D(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static float triangleAreaSigned3D(
                    const float referenceNormal[3],
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static bool lineIntersection2D(
                    const float p1[3],
                    const float p2[3],
                    const float q1[3],
                    const float q2[3],
                    const float tolerance,
                    float intersectionOut[3]);

    static bool rayIntersectPlane(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3],
                    const float rayOrigin[3],
                    const float rayVector[3],
                    float intersectionXYZandDistance[3]);

    static void projectPoint(
                    const float pt[3],
                    const float origin[3],
                    const float normal[3],
                              float projectedPointOut[3]);

    static float signedDistanceFromPlane(
                    const float planeNormal[3],
                    const float pointInPlane[3],
                    const float queryPoint[3]);

    static int32_t limitRange(
                    const int32_t value,
                    const int32_t minimumValue,
                    const int32_t maximumValue);

    static float limitRange(
                    const float value,
                    const float minimumValue,
                    const float maximumValue);

    static double limitRange(
                    const double value,
                    const double minimumValue,
                    const double maximumValue);

    static float distanceToLine3D(
                    const float p1[3],
                    const float p2[3],
                    const float point[3]);

    static bool arraysEqual(
                    const float a[],
                    const float b[],
                    const int32_t numElements);

    static void averageOfThreeCoordinates(
                    const float c1[3],
                    const float c2[3],
                    const float c3[3],
                    float outputAverage[3]);

    static void averageOfThreeCoordinates(
                    const float xyzAll[],
                    const int32_t offsetCoord1,
                    const int32_t offsetCoord2,
                    const int32_t offsetCoord3,
                    float outputAverage[],
                    const int32_t outputOffset);

    static float angle(
                    const float p1[3],
                    const float p2[3],
                    const float p3[3]);

    static float signedAngle(
                    const float pi[3],
                    const float pj[3],
                    const float pk[3],
                    const float n[3]);

    static bool isOddNumber(const int32_t number);

    static bool isEvenNumber(const int32_t number);
    
    static bool isNaN(const float number);
    
    static bool isPosInf(const float number);
    
    static bool isNegInf(const float number);
    
    ///true if either inf or -inf
    static bool isInf(const float number);
    
    ///true only if not NaN, inf, or -inf
    static bool isNumeric(const float number);

    static bool compareArrays(
                    const float a1[],
                    const float a2[],
                    const int32_t numElemets,
                    const float tolerance);

    static int32_t clamp(
                    const int32_t value,
                    const int32_t minimum,
                    const int32_t maximum);

    static float clamp(
                    const float value,
                    const float minimum,
                    const float maximum);

    static float toRadians(float angle);
    
    static float toDegrees(float radians);
    
    ///greatest common divisor
    static uint32_t gcd(uint32_t num1, uint32_t num2);
    
    /**
     * Is the value very, very close to zero?
     * @param value
     *    Value to test.
     * @return true if approximately zero, else false.
     */
    static inline bool isZero(const float value) {
        if (value >  0.00001) return false;
        if (value < -0.00001) return false;
        return true;
    }
    
    ///convert quaternion to rotation matrix
    static void quaternToMatrix(const float cijk[4], float matrix[3][3]);
    
    ///convert quaternion to rotation matrix
    static void quaternToMatrix(const double cijk[4], double matrix[3][3]);
    
    ///try to convert 3x3 matrix to quaternion (return false if not a rotation matrix)
    static bool matrixToQuatern(const float matrix[3][3], float cijk[4]);

    ///try to convert 3x3 matrix to quaternion (return false if not a rotation matrix)
    static bool matrixToQuatern(const double matrix[3][3], double cijk[4]);
    
    static double remainder(const double numerator,
                            const double denominator);
    
    static double round(const double value);
    
};

} // namespace

#endif // __MATHFUNCTIONS_H__
