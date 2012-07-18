//---------------------------------------------------------------------------
//
// CS488 -- Introduction to Computer Graphics
//
// algebra.hpp/algebra.cpp
//
// Classes and functions for manipulating points, vectors, matrices, 
// and colours.  You probably won't need to modify anything in these
// two files.
//
// University of Waterloo Computer Graphics Lab / 2003
//
//---------------------------------------------------------------------------

#ifndef CS488_ALGEBRA_HPP
#define CS488_ALGEBRA_HPP

#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Point2D
{
public:
  Point2D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
  }
  Point2D(double x, double y)
  { 
    v_[0] = x;
    v_[1] = y;
  }
  Point2D(const Point2D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
  }

  Point2D& operator =(const Point2D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

private:
  double v_[2];
};

class Point3D
{
public:
  Point3D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
  }
  Point3D(double x, double y, double z)
  { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
  }
  Point3D(const Point3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
  }

  Point3D& operator =(const Point3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

private:
  double v_[3];
};

class Vector3D
{
public:
  Vector3D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
  }
  Vector3D(double x, double y, double z)
  { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
  }
  Vector3D(const Vector3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
  }

  Vector3D& operator =(const Vector3D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

  double Dot(const Vector3D& other) const
  {
    return v_[0]*other.v_[0] + v_[1]*other.v_[1] + v_[2]*other.v_[2];
  }

  double Length2() const
  {
    return v_[0]*v_[0] + v_[1]*v_[1] + v_[2]*v_[2];
  }
  double Length() const
  {
    return sqrt(Length2());
  }

  double Normalize();

  Vector3D Cross(const Vector3D& other) const
  {
    return Vector3D(
                    v_[1]*other[2] - v_[2]*other[1],
                    v_[2]*other[0] - v_[0]*other[2],
                    v_[0]*other[1] - v_[1]*other[0]);
  }

private:
  double v_[3];
};

class Matrix3x3 {
 public:
  Matrix3x3() {
    // Construct an identity matrix
    std::fill(v_, v_ + 9, 0.0);
    v_[0] = 1.0;
    v_[4] = 1.0;
    v_[8] = 1.0;
  }

  Matrix3x3(const Matrix3x3& other) {
    std::copy(other.v_, other.v_ + 9, v_);
  }

  Matrix3x3(const Vector3D row1, const Vector3D row2, const Vector3D row3) {
    v_[0] = row1[0]; 
    v_[1] = row1[1]; 
    v_[2] = row1[2]; 

    v_[3] = row2[0]; 
    v_[4] = row2[1]; 
    v_[5] = row2[2]; 

    v_[6] = row3[0]; 
    v_[7] = row3[1]; 
    v_[8] = row3[2]; 
  }

  Matrix3x3(double* vals) {
    std::copy(vals, vals + 9, (double*)v_);
  }

  Matrix3x3& operator=(const Matrix3x3& other) {
    std::copy(other.v_, other.v_ + 9, v_);
    return *this;
  }

  Vector3D GetRow(size_t row) const {
    return Vector3D(v_[row * 3], v_[row * 3 + 1], v_[row * 3 + 2]);
  }

  double* GetRow(size_t row) {
    return (double*)v_ + row * 3;
  }

  Vector3D GetColumn(size_t col) const {
    return Vector3D(v_[col], v_[col + 3], v_[col + 6]);
  }

  Vector3D operator[](size_t row) const {
    return GetRow(row);
  }

  double* operator[](size_t row) {
    return GetRow(row);
  }

  Matrix3x3 Transpose() const {
    return Matrix3x3(GetColumn(0), GetColumn(1), GetColumn(2));
  }

  Matrix3x3 Invert() const;

  const double* Begin() const {
    return (double*)v_;
  }

  const double* End() const {
    return Begin() + 9;
  }

private:
  double v_[9];
};

inline Vector3D operator *(double s, const Vector3D& v)
{
  return Vector3D(s*v[0], s*v[1], s*v[2]);
}

inline Vector3D operator +(const Vector3D& a, const Vector3D& b)
{
  return Vector3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline Point3D operator +(const Point3D& a, const Vector3D& b)
{
  return Point3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline Vector3D operator -(const Point3D& a, const Point3D& b)
{
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D operator -(const Vector3D& a, const Vector3D& b)
{
  return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D operator -(const Vector3D& a)
{
  return Vector3D(-a[0], -a[1], -a[2]);
}

inline Point3D operator -(const Point3D& a, const Vector3D& b)
{
  return Point3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D Cross(const Vector3D& a, const Vector3D& b) 
{
  return a.Cross(b);
}

inline Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b) {
  Matrix3x3 ret;

  for(size_t i = 0; i < 3; ++i) {
    Vector3D row = a.GetRow(i);
		
    for(size_t j = 0; j < 3; ++j) {
      ret[i][j] = row[0] * b[0][j] + row[1] * b[1][j] + row[2] * b[2][j];
    }
  }

  return ret;
}

inline Vector3D operator*(const Matrix3x3& M, const Vector3D& v) {
  return Vector3D(
                  v[0] * M[0][0] + v[1] * M[0][1] + v[2] * M[0][2],
                  v[0] * M[1][0] + v[1] * M[1][1] + v[2] * M[1][2],
                  v[0] * M[2][0] + v[1] * M[2][1] + v[2] * M[2][2]);
}

inline std::ostream& operator <<(std::ostream& os, const Point2D& p)
{
  return os << "p<" << p[0] << "," << p[1] << ">";
}

inline std::ostream& operator <<(std::ostream& os, const Point3D& p)
{
  return os << "p<" << p[0] << "," << p[1] << "," << p[2] << ">";
}

inline std::ostream& operator <<(std::ostream& os, const Vector3D& v)
{
  return os << "v<" << v[0] << "," << v[1] << "," << v[2] << ">";
}

class Matrix4x4;

class Vector4D
{
public:
  Vector4D()
  {
    v_[0] = 0.0;
    v_[1] = 0.0;
    v_[2] = 0.0;
    v_[3] = 0.0;
  }
  Vector4D(double x, double y, double z, double w)
  { 
    v_[0] = x;
    v_[1] = y;
    v_[2] = z;
    v_[3] = w;
  }
  Vector4D(const Vector4D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
  }
  Vector4D(const Vector3D& other)
  {
    v_[0] = other[0];
    v_[1] = other[1];
    v_[2] = other[2];
    v_[3] = 0;
  }

  Vector4D& operator =(const Vector4D& other)
  {
    v_[0] = other.v_[0];
    v_[1] = other.v_[1];
    v_[2] = other.v_[2];
    v_[3] = other.v_[3];
    return *this;
  }

  double& operator[](size_t idx) 
  {
    return v_[ idx ];
  }
  double operator[](size_t idx) const 
  {
    return v_[ idx ];
  }

private:
  double v_[4];
};

class Matrix4x4
{
public:
  Matrix4x4()
  {
    // Construct an identity matrix
    std::fill(v_, v_+16, 0.0);
    v_[0] = 1.0;
    v_[5] = 1.0;
    v_[10] = 1.0;
    v_[15] = 1.0;
  }
  Matrix4x4(const Matrix4x4& other)
  {
    std::copy(other.v_, other.v_+16, v_);
  }
  Matrix4x4(const Vector4D row1, const Vector4D row2, const Vector4D row3, 
             const Vector4D row4)
  {
    v_[0] = row1[0]; 
    v_[1] = row1[1]; 
    v_[2] = row1[2]; 
    v_[3] = row1[3]; 

    v_[4] = row2[0]; 
    v_[5] = row2[1]; 
    v_[6] = row2[2]; 
    v_[7] = row2[3]; 

    v_[8] = row3[0]; 
    v_[9] = row3[1]; 
    v_[10] = row3[2]; 
    v_[11] = row3[3]; 

    v_[12] = row4[0]; 
    v_[13] = row4[1]; 
    v_[14] = row4[2]; 
    v_[15] = row4[3]; 
  }
  Matrix4x4(double *vals)
  {
    std::copy(vals, vals + 16, (double*)v_);
  }

  Matrix4x4& operator=(const Matrix4x4& other)
  {
    std::copy(other.v_, other.v_+16, v_);
    return *this;
  }

  Matrix4x4& operator=(const Matrix3x3& other)
  {
    const double* M = other.Begin();
    v_[0] = M[0];
    v_[1] = M[1];
    v_[2] = M[2];
    v_[3] = 0;

    v_[4] = M[3];
    v_[5] = M[4];
    v_[6] = M[6];
    v_[7] = 0;
    
    v_[8] = M[7];
    v_[9] = M[8];
    v_[10] = M[9];
    v_[11] = 0;

    v_[12] = v_[13] = v_[14] = 0;
    v_[15] = 1;

    return *this;
  }

  Vector4D GetRow(size_t row) const
  {
    return Vector4D(v_[4*row], v_[4*row+1], v_[4*row+2], v_[4*row+3]);
  }
  double* GetRow(size_t row) 
  {
    return (double*)v_ + 4*row;
  }

  Vector4D GetColumn(size_t col) const
  {
    return Vector4D(v_[col], v_[4+col], v_[8+col], v_[12+col]);
  }

  Vector4D operator[](size_t row) const
  {
    return GetRow(row);
  }

  double* operator[](size_t row) 
  {
    return GetRow(row);
  }

  Matrix4x4 Transpose() const
  {
    return Matrix4x4(GetColumn(0), GetColumn(1), 
                      GetColumn(2), GetColumn(3));
  }
  Matrix4x4 Invert() const;

  const double* Begin() const {
    return (double*)v_;
  }

  const double* End() const {
    return Begin() + 16;
  }

private:
  double v_[16];
};

inline Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
  Matrix4x4 ret;

  for(size_t i = 0; i < 4; ++i) {
    Vector4D row = a.GetRow(i);
		
    for(size_t j = 0; j < 4; ++j) {
      ret[i][j] = row[0] * b[0][j] + row[1] * b[1][j] + 
        row[2] * b[2][j] + row[3] * b[3][j];
    }
  }

  return ret;
}

inline Vector3D operator*(const Matrix4x4& M, const Vector3D& v)
{
  return Vector3D(
                  v[0] * M[0][0] + v[1] * M[0][1] + v[2] * M[0][2],
                  v[0] * M[1][0] + v[1] * M[1][1] + v[2] * M[1][2],
                  v[0] * M[2][0] + v[1] * M[2][1] + v[2] * M[2][2]);
}

inline Vector4D operator*(const Matrix4x4& M, const Vector4D& v)
{
  return Vector4D(
                  v[0] * M[0][0] + v[1] * M[0][1] + v[2] * M[0][2] + v[3] * M[0][3],
                  v[0] * M[1][0] + v[1] * M[1][1] + v[2] * M[1][2] + v[3] * M[1][3],
                  v[0] * M[2][0] + v[1] * M[2][1] + v[2] * M[2][2] + v[3] * M[2][3],
                  v[0] * M[3][0] + v[1] * M[3][1] + v[2] * M[3][2] + v[3] * M[3][3]);
}

inline Point3D operator*(const Matrix4x4& M, const Point3D& p)
{
  return Point3D(
                 p[0] * M[0][0] + p[1] * M[0][1] + p[2] * M[0][2] + M[0][3],
                 p[0] * M[1][0] + p[1] * M[1][1] + p[2] * M[1][2] + M[1][3],
                 p[0] * M[2][0] + p[1] * M[2][1] + p[2] * M[2][2] + M[2][3]);
}

inline Vector3D TransformNormal(const Matrix4x4& M, const Vector3D& n)
{
  return Vector3D(
                  n[0] * M[0][0] + n[1] * M[1][0] + n[2] * M[2][0],
                  n[0] * M[0][1] + n[1] * M[1][1] + n[2] * M[2][1],
                  n[0] * M[0][2] + n[1] * M[1][2] + n[2] * M[2][2]);
}

inline std::ostream& operator <<(std::ostream& os, const Vector4D& v)
{
  return os << "v<" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << ">";
}

inline std::ostream& operator <<(std::ostream& os, const Matrix4x4& M)
{
  return os << "[" << M[0][0] << " " << M[0][1] << " " 
            << M[0][2] << " " << M[0][3] << "]" << std::endl
            << "[" << M[1][0] << " " << M[1][1] << " " 
            << M[1][2] << " " << M[1][3] << "]" << std::endl
            << "[" << M[2][0] << " " << M[2][1] << " " 
            << M[2][2] << " " << M[2][3] << "]" << std::endl
            << "[" << M[3][0] << " " << M[3][1] << " " 
            << M[3][2] << " " << M[3][3] << "]";
}

class Colour
{
public:
  Colour(double r, double g, double b)
    : r_(r)
    , g_(g)
    , b_(b)
  {}
  Colour(double c)
    : r_(c)
    , g_(c)
    , b_(c)
  {}
  Colour(const Colour& other)
    : r_(other.r_)
    , g_(other.g_)
    , b_(other.b_)
  {}

  Colour& operator =(const Colour& other)
  {
    r_ = other.r_;
    g_ = other.g_;
    b_ = other.b_;
    return *this;
  }

  double R() const 
  { 
    return r_;
  }
  double G() const 
  { 
    return g_;
  }
  double B() const 
  { 
    return b_;
  }

private:
  double r_;
  double g_;
  double b_;
};

inline Colour operator *(double s, const Colour& a)
{
  return Colour(s*a.R(), s*a.G(), s*a.B());
}

inline Colour operator *(const Colour& a, const Colour& b)
{
  return Colour(a.R()*b.R(), a.G()*b.G(), a.B()*b.B());
}

inline Colour operator +(const Colour& a, const Colour& b)
{
  return Colour(a.R()+b.R(), a.G()+b.G(), a.B()+b.B());
}

inline std::ostream& operator <<(std::ostream& os, const Colour& c)
{
  return os << "c<" << c.R() << "," << c.G() << "," << c.B() << ">";
}

#endif

