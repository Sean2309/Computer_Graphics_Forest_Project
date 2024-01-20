//----------------------------------------------------------------------------------------
/**
 * \file       spline.cpp
 * \author     Miroslav Miksik & Jaroslav Sloup & Petr Felkel
 * \date       2013
 * \brief      Utility functions and stuff tasks concerning animation curves.
*/
//----------------------------------------------------------------------------------------

#include "spline.h"

//**************************************************************************************************
/// Checks whether vector is zero-length or not.
bool isVectorNull(const glm::vec3 &vect) {

  return !vect.x && !vect.y && !vect.z;
}

//**************************************************************************************************
/// Align (rotate and move) current coordinate system to given parameters.
/**
 This function works similarly to \ref gluLookAt, however, it is used for object transform
 rather than for view transform. Therefore, this function does not compute the computed matrix inverse.
 The current coordinate system is moved in such a way that the origin is moved
 to the \a position. Object's local front (-Z) direction is rotated to the \a front and
 object's local up (+Y) direction is rotated so that angle between its local up direction and
 \a up vector is minimum.

 \param[in]  position           Position of the origin.
 \param[in]  front              Front direction.
 \param[in]  up                 Up vector.
 */
glm::mat4 alignObject(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &up) {

  glm::vec3 z = -glm::normalize(front);

  if (isVectorNull(z))
    z = glm::vec3(0.0, 0.0, 1.0);

  glm::vec3 x = glm::normalize(glm::cross(up, z));

  if (isVectorNull(x))
    x =  glm::vec3(1.0, 0.0, 0.0);

  glm::vec3 y = glm::cross(z, x);
  //mat4 matrix = mat4(1.0f);
  glm::mat4 matrix = glm::mat4(
      x.x,        x.y,        x.z,        0.0,
      y.x,        y.y,        y.z,        0.0,
      z.x,        z.y,        z.z,        0.0,
      position.x, position.y, position.z, 1.0
  );

  return matrix;
}



/// Number of control points of the animation curve.
const size_t  curveSize   = 12;

/// Control points of the animation curve.
glm::vec3 curveData[] = {
  glm::vec3( 0.00, 0.0,  0.0),

  glm::vec3(-0.33,  0.35, 0.0),
  glm::vec3(-0.66,  0.35, 0.0),
  glm::vec3(-1.00,  0.0, 0.0),
  glm::vec3(-0.66, -0.35, 0.0),
  glm::vec3(-0.33, -0.35, 0.0),

  glm::vec3( 0.00,  0.0, 0.0),

  glm::vec3( 0.33,  0.35, 0.0),
  glm::vec3( 0.66,  0.35, 0.0),
  glm::vec3( 1.00,  0.0, 0.0),
  glm::vec3( 0.66, -0.35, 0.0),
  glm::vec3( 0.33, -0.35, 0.0)
};

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
) {
  glm::vec3 result(0.0, 0.0, 0.0);

  // evaluate point on a curve segment (defined by the control points P0...P3)
  // for the given value of parameter t

  const float t2 = t*t;
  const float t3 = t*t2;

  result = P0 * (-      t3 + 2.0f*t2 - t       )
      + P1 * (  3.0f*t3 - 5.0f*t2     + 2.0f)
      + P2 * (- 3.0f*t3 + 4.0f*t2 + t       )
      + P3 * (       t3 -      t2           );

  result *= 0.5f;

  return result;
}

//**************************************************************************************************
/// Evaluates a first derivative of Catmull-Rom curve segment.
/**
  \param[in] P0       First control point of the curve segment.
  \param[in] P1       Second control point of the curve segment.
  \param[in] P2       Third control point of the curve segment.
  \param[in] P3       Fourth control point of the curve segment.
  \param[in] t        Curve segment parameter. Must be within range [0, 1].
  \return             First derivative of the curve for parameter \a t.
*/
glm::vec3 evaluateCurveSegment_1stDerivative(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t
) {
  glm::vec3 result(1.0, 0.0, 0.0);

  // evaluate first derivative for a point on a curve segment (defined by the control points P0...P3)
  // for the given value of parameter t

  const float t2 = t*t;

  result = P0 * (- 3.0f*t2 +  4.0f*t - 1.0f)
      + P1 * (  9.0f*t2 - 10.0f*t       )
      + P2 * (- 9.0f*t2 +  8.0f*t + 1.0f)
      + P3 * (  3.0f*t2 -  2.0f*t       );

  result *= 0.5f;

  return result;
}

//**************************************************************************************************
/// Evaluates a position on a closed curve composed of Catmull-Rom segments.
/**
  \param[in] points   Array of curve control points.
  \param[in] count    Number of curve control points.
  \param[in] t        Parameter for which the position shall be evaluated.
  \return             Position on the curve for parameter \a t.
  \note               Although the range of the parameter is from [0, \a count] (outside the range
                      the curve is periodic), one must presume any value (even negative).
*/
glm::vec3 evaluateClosedCurve(
    const glm::vec3 points[],
    const size_t    count,
    const float     t
) {
  glm::vec3 result(0.0, 0.0, 0.0);

  // based on the value of parameter t first find corresponding segment and its control points => i
  // and then call evaluateCurveSegment function with proper parameters to get a point on a closed curve

  float param = cyclic_clamp(t, 0.0f, float(count));
  size_t index = size_t(param);

  result = evaluateCurveSegment(
      points[(index-1+count)%count],
      points[(index        )%count],
      points[(index+1      )%count],
      points[(index+2      )%count],
      param-floor(param)
      );

  return result;
}

//**************************************************************************************************
/// Evaluates a first derivative of a closed curve composed of Catmull-Rom segments.
/**
  \param[in] points   Array of curve control points.
  \param[in] count    Number of curve control points.
  \param[in] t        Parameter for which the derivative shall be evaluated.
  \return             First derivative of the curve for parameter \a t.
  \note               Although the range of the parameter is from [0, \a count] (outside the range
                      the curve is periodic), one must presume any value (even negative).
*/
glm::vec3 evaluateClosedCurve_1stDerivative(
    const glm::vec3 points[],
    const size_t    count,
    const float     t
) {
  glm::vec3 result(1.0, 0.0, 0.0);

  // based on the value of parameter t first find corresponding curve segment and its control points => i
  // and then call evaluateCurveSegment_1stDerivative function with proper parameters
  // to get a derivative for the given point on a closed curve

  float param = cyclic_clamp(t, 0.0f, float(count));
  size_t index = size_t(param);

  result = evaluateCurveSegment_1stDerivative(
      points[(index-1+count)%count],
      points[(index        )%count],
      points[(index+1      )%count],
      points[(index+2      )%count],
      param-floor(param)
      );

  return result;
}

//**************************************************************************************************
/// Curve validity test points.
glm::vec3 curveTestPoints[] = {
  glm::vec3(-9.0f, 0.0f, -5.0f),
  glm::vec3(-3.0f, 0.0f,  5.0f),
  glm::vec3( 3.0f, 4.0f, -5.0f),
  glm::vec3( 9.0f, 0.0f,  0.0f),
};

/// Correct result for curve position in range [0, 1] with step 0.05.
const glm::vec3 curveTestGoldfile[] = {
  glm::vec3(-3.000000, 0.000000,  5.000000),
  glm::vec3(-2.700000, 0.119250,  4.933437),
  glm::vec3(-2.400000, 0.274000,  4.742500),
  glm::vec3(-2.100000, 0.459750,  4.440312),
  glm::vec3(-1.800000, 0.672000,  4.040000),
  glm::vec3(-1.500000, 0.906250,  3.554688),
  glm::vec3(-1.200000, 1.158000,  2.997500),
  glm::vec3(-0.900000, 1.422750,  2.381562),
  glm::vec3(-0.600000, 1.696000,  1.720000),
  glm::vec3(-0.300000, 1.973250,  1.025937),
  glm::vec3( 0.000000, 2.250000,  0.312500),
  glm::vec3( 0.300000, 2.521750, -0.407188),
  glm::vec3( 0.600000, 2.784000, -1.120000),
  glm::vec3( 0.900000, 3.032250, -1.812812),
  glm::vec3( 1.200000, 3.262000, -2.472499),
  glm::vec3( 1.500000, 3.468750, -3.085938),
  glm::vec3( 1.800000, 3.648000, -3.640001),
  glm::vec3( 2.100000, 3.795250, -4.121563),
  glm::vec3( 2.400000, 3.906000, -4.517500),
  glm::vec3( 2.700000, 3.975750, -4.814687),
  glm::vec3( 3.000000, 4.000000, -5.000000)
};
/// Correct result for curve 1st derivative in range [0, 1] with step 0.05.
const glm::vec3 curveTestGoldfile_1stDerivative[] = {
  glm::vec3(6.000000, 2.000000,   0.000000),
  glm::vec3(6.000000, 2.755000, - 2.618750),
  glm::vec3(6.000000, 3.420000, - 4.975000),
  glm::vec3(6.000000, 3.995000, - 7.068750),
  glm::vec3(6.000000, 4.480000, - 8.900000),
  glm::vec3(6.000000, 4.875000, -10.468750),
  glm::vec3(6.000000, 5.180000, -11.775001),
  glm::vec3(6.000000, 5.395000, -12.818750),
  glm::vec3(6.000000, 5.520000, -13.599999),
  glm::vec3(6.000000, 5.555000, -14.118751),
  glm::vec3(6.000000, 5.500000, -14.375000),
  glm::vec3(6.000000, 5.355000, -14.368751),
  glm::vec3(6.000000, 5.120000, -14.100000),
  glm::vec3(6.000000, 4.795001, -13.568751),
  glm::vec3(6.000000, 4.380000, -12.775001),
  glm::vec3(6.000000, 3.875000, -11.718750),
  glm::vec3(6.000000, 3.279999, -10.399999),
  glm::vec3(6.000000, 2.595000, - 8.818749),
  glm::vec3(6.000000, 1.820001, - 6.975001),
  glm::vec3(6.000000, 0.955000, - 4.868751),
  glm::vec3(6.000000, 0.000000, - 2.500000)
};

/// True is the curve test passed.
bool curveValid;
/// Positive if the curve 1st derivative test passed, zero if failed or negative if the test has not been taken.
int  curve1stDerivativeValid;
