#include "Trackball.h"

#include <cmath>

// The following functions were taken (and slightly modified) from the provided demo code

void vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ) {
  long  nXOrigin, nYOrigin;
  float fNewVecX, fNewVecY, fNewVecZ,    /* Vector corresponding to new mouse location */
        fOldVecX, fOldVecY, fOldVecZ,    /* Vector corresponding to old mouse location */
        fLength;

  /* Vector pointing from center of virtual trackball to
   * new mouse position
   */
  fNewVecX  = fNewX * 2.0 / fDiameter;
  fNewVecY  = fNewY * 2.0 / fDiameter;
  fNewVecZ  = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

  /* If the Z component is less than 0, the mouse point
   * falls outside of the trackball which is interpreted
   * as rotation about the Z axis.
   */
  if (fNewVecZ < 0.0) {
    fLength = sqrt(1.0 - fNewVecZ);
    fNewVecZ  = 0.0;
    fNewVecX /= fLength;
    fNewVecY /= fLength;
  } else {
    fNewVecZ = sqrt(fNewVecZ);
  }

  /* Vector pointing from center of virtual trackball to
   * old mouse position
   */
  fOldVecX  = fOldX * 2.0 / fDiameter;
  fOldVecY  = fOldY * 2.0 / fDiameter;
  fOldVecZ  = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);

  /* If the Z component is less than 0, the mouse point
   * falls outside of the trackball which is interpreted
   * as rotation about the Z axis.
   */
  if (fOldVecZ < 0.0) {
    fLength = sqrt(1.0 - fOldVecZ);
    fOldVecZ  = 0.0;
    fOldVecX /= fLength;
    fOldVecY /= fLength;
  } else {
    fOldVecZ = sqrt(fOldVecZ);
  }

  /* Generate rotation vector by calculating cross product:
   *
   * fOldVec x fNewVec.
   *
   * The rotation vector is the axis of rotation
   * and is non-unit length since the length of a crossproduct
   * is related to the angle between fOldVec and fNewVec which we need
   * in order to perform the rotation.
  */
  *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
  *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
  *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

Matrix4x4 TrackballRotation(float newX, float oldX, float newY, float oldY, float diameter) {

  float fVecX, fVecY, fVecZ;

  vCalcRotVec(newX, newY, oldX, oldY, diameter, &fVecX, &fVecY, &fVecZ);

  float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;
  Matrix4x4 mNewMat;

  /* Find the length of the vector which is the angle of rotation
   * (in radians)
   */
  fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

  /* If the vector has zero length - return the identity matrix */
  if (fRadians > -0.000001 && fRadians < 0.000001) {
    return mNewMat;
  }

  /* Normalize the rotation vector now in preparation for making
   * rotation matrix.
   */
  fInvLength = 1 / fRadians;
  fNewVecX   = fVecX * fInvLength;
  fNewVecY   = fVecY * fInvLength;
  fNewVecZ   = fVecZ * fInvLength;

  /* Create the arbitrary axis rotation matrix */
  double dSinAlpha = sin(fRadians);
  double dCosAlpha = cos(fRadians);
  double dT = 1 - dCosAlpha;

  mNewMat = Matrix4x4(
    Vector4D(dCosAlpha + fNewVecX * fNewVecX * dT,
             fNewVecX * fNewVecY * dT + fNewVecZ * dSinAlpha,
             fNewVecX * fNewVecZ * dT - fNewVecY * dSinAlpha,
             0),
    Vector4D(fNewVecX * fNewVecY * dT - dSinAlpha * fNewVecZ,
             dCosAlpha + fNewVecY * fNewVecY * dT,
             fNewVecY * fNewVecZ * dT + dSinAlpha * fNewVecX,
             0),
    Vector4D(fNewVecZ * fNewVecX * dT + dSinAlpha * fNewVecY,
             fNewVecZ * fNewVecY * dT - dSinAlpha * fNewVecX,
             dCosAlpha + fNewVecZ * fNewVecZ * dT,
             0),
    Vector4D(0, 0, 0, 1));

  return mNewMat;
}

