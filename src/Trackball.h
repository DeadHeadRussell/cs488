#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__

#include "Algebra.h"

Matrix4x4 TrackballRotation(float new_x, float old_x, float new_y, float old_y,
                            float diameter);

#endif

