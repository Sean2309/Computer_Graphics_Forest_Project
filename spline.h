//----------------------------------------------------------------------------------------
/**
 * \file       spline.h
 * \author     Miroslav Miksik & Jaroslav Sloup & Petr Felkel
 * \date       2013
 * \brief      Utility functions and stuff tasks concerning animation curves.
*/
//----------------------------------------------------------------------------------------
#ifndef __SPLINE_H
#define __SPLINE_H

#include "pgr.h" // glm

//**************************************************************************************************
/// Checks whether vector is zero-length or not.
bool isVectorNull(const glm::vec3 &vect);

//**************************************************************************************************
/// Align (rotate and move) current coordinate system to given parameters.
/**
 This function works similarly to \ref gluLookAt, however it is used for object transform
 rather than for view transform. Therefore, this function does not compute the computed matrix inverse.
 The current coordinate system is moved in such a way that the origin is moved
 to the \a position. Object's local front (-Z) direction is rotated to the \a front and
 object's local up (+Y) direction is rotated so that angle between its local up direction and
 \a up vector is minimum.

 \param[in]  position           Position of the origin.
 \param[in]  front              Front direction.
 \param[in]  up                 Up vector.
 */
glm::mat4 alignObject(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &up);


extern glm::vec3 curveData[];
extern const size_t  curveSize;


//**************************************************************************************************
/// Cyclic clamping of a value.
/**
 Makes sure that \a value is not outside the interval [\a minBound, \a maxBound].
 If \a value is outside the interval it is treated as a periodic value with the period equal to the size
 of the interval. A necessary number of periods are added/subtracted to fit the value to the interval.

 \param[in]  value              Value to be clamped.
 \param[in]  minBound           Minimum bound of value.
 \param[in]  maxBound           Maximum bound of value.
 \return                        Value within range [minBound, maxBound].
 \pre                           \a minBound is not greater that \maxBound.
*/
template <typename T>
T cyclic_clamp(const T value, const T minBound, const T maxBound) {

  T amp = maxBound-minBound;
  T val = fmod(value-minBound, amp);

  if (val < T(0))
    val += amp;

  return val+minBound;
}

//**************************************************************************************************
/// Evaluates a position on Catmull-Rom curve segment.
/**
  \param[in] P0       First control point of the curve segment.
  \param[in] P1       Second control point of the curve segment.
  \param[in] P2       Third control point of the curve segment.
  \param[in] P3       Fourth control point of the curve segment.
  \param[in] t        Curve segment parameter. Must be within range [0, 1].
  \return             Position on the curve for parameter \a t.
*/
glm::vec3 evaluateCurveSegment(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t
);
//**************************************************************************************************
/// Evaluates a fist derivative of Catmull-Rom curve segment.
/**
  \param[in] P0       First control point of the curve segment.
  \param[in] P1       Second control point of the curve segment.
  \param[in] P2       Third control point of the curve segment.
  \param[in] P3       Founrth control point of the curve segment.
  \param[in] t        Curve segment parameter. Must be within range [0, 1].
  \return             First derivative of the curve for parameter \a t.
*/
glm::vec3 evaluateCurveSegment_1stDerivative(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t
);
//**************************************************************************************************
/// Evaluates a position on a closed curve composed of Catmull-Rom segments.
/**
  \param[in] points   Array of curve control points.
  \param[in] count    Number of curve control points.
  \param[in] t        Parameter for which the position shall be evaluated.
  \return             Position on the curve for parameter \a t.
  \note               Although the range of the paramter is from [0, \a count] (outside the range
                      the curve is periodic) one must presume any value (even negative).
*/
glm::vec3 evaluateClosedCurve(
    const glm::vec3 points[],
    const size_t    count,
    const float     t
);
//**************************************************************************************************
/// Evaluates a first derivative of a closed curve composed of Catmull-Rom segments.
/**
  \param[in] points   Array of curve control points.
  \param[in] count    Number of curve control points.
  \param[in] t        Parameter for which the derivative shall be evaluated.
  \return             First derivative of the curve for parameter \a t.
  \note               Although the range of the paramter is from [0, \a count] (outside the range
                      the curve is periodic) one must presume any value (even negative).
*/
glm::vec3 evaluateClosedCurve_1stDerivative(
    const glm::vec3 points[],
    const size_t    count,
    const float     t
);

//**************************************************************************************************
/// Curve validity test points.
extern glm::vec3 curveTestPoints[];
/// Correct result for curve position in range [0, 1] with step 0.05.
extern const glm::vec3 curveTestGoldfile[];
/// Correct result for curve 1st derivative in range [0, 1] with step 0.05.
extern const glm::vec3 curveTestGoldfile_1stDerivative[];
/// True is the curve test passed.
extern bool curveValid;
/// Positive if the curve 1st derivative test passed, zero if failed or negative if the test has not been taken.
extern int  curve1stDerivativeValid;

//**************************************************************************************************
/// Tests against goldfile whether the Catmull-Rom curve segment is correctly computed (tasks 1 and 3).
/**
 \param[in]  evaluator                  Functor evaluating the position on the curve. The functor is of type
                                        <tt>(CVector3D& P0, CVector3D& P1, CVector3D& P2, CVector3D& P3, const float param)</tt>.
 \param[in]  evaluator_1stDerivative    Functor evaluating the 1st derivative of the curve. The functor is of type
                                        <tt>(CVector3D& P0, CVector3D& P1, CVector3D& P2, CVector3D& P3, const float param)</tt>.
*/
template <class CurveSegmentEvaluator, class CurveSegmentEvaluator_1stDerivative>
void testCurve(CurveSegmentEvaluator evaluator, CurveSegmentEvaluator_1stDerivative evaluator_1stDerivative) {

  bool c1dValid = true;
  bool c1dNull = true;
  curveValid = true;

#define COMPARISON_ACCURACY 1e-5f

  for(int i=0; i<=20; i++) {
    float t = i/20.f;
    glm::vec3 c = evaluator(curveTestPoints[0], curveTestPoints[1], curveTestPoints[2], curveTestPoints[3], t);
    glm::vec3 d = evaluator_1stDerivative(curveTestPoints[0], curveTestPoints[1], curveTestPoints[2], curveTestPoints[3], t);

	if (curveValid && glm::length(c-curveTestGoldfile[i]) > COMPARISON_ACCURACY)
      curveValid = false;
	if (c1dNull && glm::length(d) > COMPARISON_ACCURACY)
      c1dNull = false;
	if (c1dValid && glm::length(d-curveTestGoldfile_1stDerivative[i]) > COMPARISON_ACCURACY)
      c1dValid = false;
  }
  curve1stDerivativeValid = c1dNull ? -1 : (c1dValid ? 1 : 0);

#undef COMPARISON_ACCURACY

  if(curveValid) {
    printf("Spline segment is VALID\n");
  }
  else {
    printf("Spline segment is INVALID\n");
  }

  if(curve1stDerivativeValid) {
    printf("Spline segment 1st derivative is VALID\n");
  }
  else {
    printf("Spline segment 1st derivative is INVALID\n");
  }
}

#endif // __SPLINE_H
