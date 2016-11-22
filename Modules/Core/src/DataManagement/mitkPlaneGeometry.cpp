/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "mitkPlaneGeometry.h"
#include "mitkPlaneOperation.h"
#include "mitkInteractionConst.h"
#include "mitkLine.h"

#include <itkSpatialOrientationAdapter.h>

#include <vtkTransform.h>

#include <vnl/vnl_cross.h>

namespace mitk
{
  PlaneGeometry::PlaneGeometry()
    : Superclass(),
    m_ReferenceGeometry( nullptr )
  {
    Initialize();
  }

  PlaneGeometry::~PlaneGeometry()
  {
  }

  PlaneGeometry::PlaneGeometry(const PlaneGeometry& other)
    : Superclass(other),
    m_ReferenceGeometry( other.m_ReferenceGeometry )
  {
  }

  void
    PlaneGeometry::EnsurePerpendicularNormal(mitk::AffineTransform3D *transform)
  {
    // Ensure column(2) of transform to be perpendicular to plane, keep length and handedness.

    Matrix3D::InternalMatrixType mx = transform->GetMatrix().GetVnlMatrix();

    ScalarType length = mx.get_column(2).two_norm();

    if (length == 0)
    {
      length = 1;
    }

    /// Negative determinant means left-handedness.
    if (vnl_determinant(mx) < 0)
    {
      length *= -1;
    }

    /// Cross-product creates right-handed normal.
    VnlVector normal = vnl_cross_3d(mx.get_column(0), mx.get_column(1));
    normal.normalize();
    /// Restore length and handedness.
    normal *= length;

    Matrix3D matrix = transform->GetMatrix();
    matrix.GetVnlMatrix().set_column(2, normal);
    transform->SetMatrix(matrix);
  }

    void PlaneGeometry::CheckIndexToWorldTransform(mitk::AffineTransform3D* transform)
  {
    EnsurePerpendicularNormal(transform);
  }

  void
    PlaneGeometry::CheckBounds(const BoundingBox::BoundsArrayType &bounds)
  {
    // error: unused parameter 'bounds'
    // this happens in release mode, where the assert macro is defined empty
    // hence we "use" the parameter:
    (void)bounds;

    //currently the unit rectangle must be starting at the origin [0,0]
    assert(bounds[0]==0);
    assert(bounds[2]==0);
    //the unit rectangle must be two-dimensional
    assert(bounds[1]>0);
    assert(bounds[3]>0);
  }

  void
    PlaneGeometry::IndexToWorld( const Point2D &pt_units, Point2D &pt_mm ) const
  {
    pt_mm[0] = GetExtentInMM(0) / GetExtent(0)*pt_units[0];
    pt_mm[1] = GetExtentInMM(1) / GetExtent(1)*pt_units[1];
  }

  void PlaneGeometry::WorldToIndex( const Point2D &pt_mm, Point2D &pt_units ) const
  {
    pt_units[0] = pt_mm[0] * (1.0 / (GetExtentInMM(0) / GetExtent(0)));
    pt_units[1] = pt_mm[1] * (1.0 / (GetExtentInMM(1) / GetExtent(1)));
  }

  void PlaneGeometry::IndexToWorld( const Point2D & /*atPt2d_units*/,
    const Vector2D &vec_units, Vector2D &vec_mm) const
  {
    MITK_WARN<<"Warning! Call of the deprecated function PlaneGeometry::IndexToWorld(point, vec, vec). Use PlaneGeometry::IndexToWorld(vec, vec) instead!";
    this->IndexToWorld(vec_units, vec_mm);
  }

  void PlaneGeometry::IndexToWorld(const Vector2D &vec_units, Vector2D &vec_mm) const
  {
    vec_mm[0] = (GetExtentInMM(0) / GetExtent(0)) * vec_units[0];
    vec_mm[1] = (GetExtentInMM(1) / GetExtent(1)) * vec_units[1];
  }

  void
    PlaneGeometry::WorldToIndex( const Point2D & /*atPt2d_mm*/,
    const Vector2D &vec_mm, Vector2D &vec_units) const
  {
    MITK_WARN<<"Warning! Call of the deprecated function PlaneGeometry::WorldToIndex(point, vec, vec). Use PlaneGeometry::WorldToIndex(vec, vec) instead!";
    this->WorldToIndex(vec_mm, vec_units);
  }

  void
    PlaneGeometry::WorldToIndex( const Vector2D &vec_mm, Vector2D &vec_units) const
  {
    vec_units[0] = vec_mm[0] * (1.0 / (GetExtentInMM(0) / GetExtent(0)));
    vec_units[1] = vec_mm[1] * (1.0 / (GetExtentInMM(1) / GetExtent(1)));
  }

  void
    PlaneGeometry::InitializeStandardPlane( mitk::ScalarType width,
    ScalarType height, const Vector3D & spacing,
    PlaneGeometry::PlaneOrientation planeorientation,
    ScalarType zPosition, bool top, bool frontside, bool rotated )
  {
    AffineTransform3D::Pointer transform;

    transform = AffineTransform3D::New();
    AffineTransform3D::MatrixType matrix;
    AffineTransform3D::MatrixType::InternalMatrixType &vnlmatrix =
      matrix.GetVnlMatrix();

    vnlmatrix.set_identity();
    vnlmatrix(0,0) = spacing[0];
    vnlmatrix(1,1) = spacing[1];
    vnlmatrix(2,2) = spacing[2];
    transform->SetIdentity();
    transform->SetMatrix(matrix);

    InitializeStandardPlane(width, height, transform.GetPointer(),
      planeorientation, zPosition, top, frontside, rotated);
  }

  void
    PlaneGeometry::InitializeStandardPlane( mitk::ScalarType width,
    ScalarType height, const AffineTransform3D* transform,
    PlaneGeometry::PlaneOrientation planeorientation, ScalarType zPosition, bool top,
    bool frontside, bool rotated )
  {
    Superclass::Initialize();

    //construct standard view
    Point3D origin;
    VnlVector rightDV(3), bottomDV(3), normal(3);
    origin.Fill(0);
    int normalDirection;
    switch(planeorientation)
    {
    case Axial:
      if(frontside)
      {
        if(rotated==false)
        {
          FillVector3D(origin,   0,  0, zPosition);
          FillVector3D(rightDV,  1,  0,         0);
          FillVector3D(bottomDV, 0,  1,         0);
        }
        else
        {
          FillVector3D(origin,   width,  height, zPosition);
          FillVector3D(rightDV,     -1,       0,         0);
          FillVector3D(bottomDV,     0,      -1,         0);
        }
      }
      else
      {
        if(rotated==false)
        {
          FillVector3D(origin,   width,  0, zPosition);
          FillVector3D(rightDV,     -1,  0,         0);
          FillVector3D(bottomDV,     0,  1,         0);
        }
        else
        {
          FillVector3D(origin,   0,  height, zPosition);
          FillVector3D(rightDV,  1,       0,         0);
          FillVector3D(bottomDV, 0,      -1,         0);
        }
      }
      normalDirection = 2;
      break;
    case Frontal:
      if(frontside)
      {
        if(rotated==false)
        {
          FillVector3D(origin,   0, zPosition, 0);
          FillVector3D(rightDV,  1, 0,         0);
          FillVector3D(bottomDV, 0, 0,         1);
        }
        else
        {
          FillVector3D(origin,   width, zPosition, height);
          FillVector3D(rightDV,     -1,         0,      0);
          FillVector3D(bottomDV,     0,         0,     -1);
        }
      }
      else
      {
        if(rotated==false)
        {
          FillVector3D(origin,    width, zPosition,  0);
          FillVector3D(rightDV,      -1,         0,  0);
          FillVector3D(bottomDV,      0,         0,  1);
        }
        else
        {
          FillVector3D(origin,   0, zPosition,  height);
          FillVector3D(rightDV,  1,         0,       0);
          FillVector3D(bottomDV, 0,         0,      -1);
        }
      }
      normalDirection = 1;
      break;
    case Sagittal:
      if(frontside)
      {
        if(rotated==false)
        {
          FillVector3D(origin,   zPosition, 0, 0);
          FillVector3D(rightDV,  0,         1, 0);
          FillVector3D(bottomDV, 0,         0, 1);
        }
        else
        {
          FillVector3D(origin,   zPosition, width, height);
          FillVector3D(rightDV,          0,    -1,      0);
          FillVector3D(bottomDV,         0,     0,     -1);
        }
      }
      else
      {
        if(rotated==false)
        {
          FillVector3D(origin,   zPosition,  width, 0);
          FillVector3D(rightDV,          0,     -1, 0);
          FillVector3D(bottomDV,         0,      0, 1);
        }
        else
        {
          FillVector3D(origin,   zPosition,  0, height);
          FillVector3D(rightDV,          0,  1,      0);
          FillVector3D(bottomDV,         0,  0,     -1);
        }
      }
      normalDirection = 0;
      break;
    default:
      itkExceptionMacro("unknown PlaneOrientation");
    }

    FillVector3D(normal, 0, 0, 0);
    normal[normalDirection] = top ? 1 : -1;

    if ( transform != nullptr )
    {
      origin = transform->TransformPoint( origin );
      rightDV = transform->TransformVector( rightDV );
      bottomDV = transform->TransformVector( bottomDV );
      normal = transform->TransformVector( normal );
    }

    ScalarType bounds[6]= { 0, width, 0, height, 0, 1 };
    this->SetBounds( bounds );

    AffineTransform3D::Pointer planeTransform = AffineTransform3D::New();
    Matrix3D matrix;
    matrix.GetVnlMatrix().set_column(0, rightDV);
    matrix.GetVnlMatrix().set_column(1, bottomDV);
    matrix.GetVnlMatrix().set_column(2, normal);
    planeTransform->SetMatrix(matrix);
    planeTransform->SetOffset(this->GetIndexToWorldTransform()->GetOffset());
    this->SetIndexToWorldTransform(planeTransform);

    this->SetOrigin(origin);
  }

  void
    PlaneGeometry::InitializeStandardPlane( const BaseGeometry *geometry3D,
    PlaneOrientation planeorientation, ScalarType zPosition, bool top,
    bool frontside, bool rotated )
  {
    this->SetReferenceGeometry( geometry3D );

    ScalarType width, height;

    // Inspired by:
    // http://www.na-mic.org/Wiki/index.php/Coordinate_System_Conversion_Between_ITK_and_Slicer3

    mitk::AffineTransform3D::MatrixType matrix = geometry3D->GetIndexToWorldTransform()->GetMatrix();

    matrix.GetVnlMatrix().normalize_columns();
    mitk::AffineTransform3D::MatrixType::InternalMatrixType inverseMatrix = matrix.GetInverse();

    /// The index of the sagittal, coronal and axial axes in the reference geometry.
    int axes[3];
    /// The direction of the sagittal, coronal and axial axes in the reference geometry.
    /// +1 means that the direction is straight, i.e. greater index translates to greater
    /// world coordinate. -1 means that the direction is inverted.
    int directions[3];
    ScalarType extents[3];
    ScalarType spacings[3];
    for (int i = 0; i < 3; ++i)
    {
      int dominantAxis = itk::Function::Max3(
          inverseMatrix[0][i],
          inverseMatrix[1][i],
          inverseMatrix[2][i]
      );
      axes[i] = dominantAxis;
      directions[i] = itk::Function::Sign(inverseMatrix[dominantAxis][i]);
      extents[i] = geometry3D->GetExtent(dominantAxis);
      spacings[i] = geometry3D->GetSpacing()[dominantAxis];
    }

    // matrix(column) = inverseTransformMatrix(row) * flippedAxes * spacing
    matrix[0][0] = inverseMatrix[axes[0]][0] * directions[0] * spacings[0];
    matrix[1][0] = inverseMatrix[axes[0]][1] * directions[0] * spacings[0];
    matrix[2][0] = inverseMatrix[axes[0]][2] * directions[0] * spacings[0];
    matrix[0][1] = inverseMatrix[axes[1]][0] * directions[1] * spacings[1];
    matrix[1][1] = inverseMatrix[axes[1]][1] * directions[1] * spacings[1];
    matrix[2][1] = inverseMatrix[axes[1]][2] * directions[1] * spacings[1];
    matrix[0][2] = inverseMatrix[axes[2]][0] * directions[2] * spacings[2];
    matrix[1][2] = inverseMatrix[axes[2]][1] * directions[2] * spacings[2];
    matrix[2][2] = inverseMatrix[axes[2]][2] * directions[2] * spacings[2];

    /// The new origin is the bottom left back corner in the world coordinate system.
    Point3D origin = geometry3D->GetOrigin();
    for (int i = 0; i < 3; ++i)
    {
      /// The distance of the origin from the bottom left back corner in voxels.
      double offset = directions[i] > 0 ? 0.0 : extents[i];

      if (geometry3D->GetImageGeometry())
      {
        offset += directions[i] * 0.5;
      }

      for (int j = 0; j < 3; ++j)
      {
        origin[j] -= offset * matrix[j][i];
      }
    }

    switch(planeorientation)
    {
    case Axial:
      width  = extents[0];
      height = extents[1];
      break;
    case Frontal:
      width  = extents[0];
      height = extents[2];
      break;
    case Sagittal:
      width  = extents[1];
      height = extents[2];
      break;
    default:
      itkExceptionMacro("unknown PlaneOrientation");
    }

    ScalarType bounds[6]= { 0, width, 0, height, 0, 1 };
    this->SetBounds( bounds );

    AffineTransform3D::Pointer transform = AffineTransform3D::New();
    transform->SetMatrix(matrix);
    transform->SetOffset(origin.GetVectorFromOrigin());

    InitializeStandardPlane( width, height,
      transform,
      planeorientation, zPosition, top, frontside, rotated );
  }

  void
    PlaneGeometry::InitializeStandardPlane( const BaseGeometry *geometry3D,
    bool top, PlaneOrientation planeorientation, bool frontside, bool rotated )
  {
    /// The index of the sagittal, coronal and axial axes in world coordinate system.
    int worldAxis;
    switch(planeorientation)
    {
    case Axial:
      worldAxis = 2;
      break;
    case Frontal:
      worldAxis = 1;
      break;
    case Sagittal:
      worldAxis = 0;
      break;
    default:
      itkExceptionMacro("unknown PlaneOrientation");
    }

    // Inspired by:
    // http://www.na-mic.org/Wiki/index.php/Coordinate_System_Conversion_Between_ITK_and_Slicer3

    mitk::AffineTransform3D::ConstPointer affineTransform = geometry3D->GetIndexToWorldTransform();
    mitk::AffineTransform3D::MatrixType matrix = affineTransform->GetMatrix();
    matrix.GetVnlMatrix().normalize_columns();
    mitk::AffineTransform3D::MatrixType::InternalMatrixType inverseMatrix = matrix.GetInverse();

    /// The index of the sagittal, coronal and axial axes in the reference geometry.
    int dominantAxis = itk::Function::Max3(
        inverseMatrix[0][worldAxis],
        inverseMatrix[1][worldAxis],
        inverseMatrix[2][worldAxis]);

    ScalarType zPosition = top ? 0.5 : geometry3D->GetExtent(dominantAxis) - 0.5;

    InitializeStandardPlane( geometry3D, planeorientation,
      zPosition, top, frontside, rotated );
  }

  void
    PlaneGeometry::InitializeStandardPlane( const Vector3D &rightVector,
    const Vector3D &downVector, const Vector3D *spacing )
  {
    InitializeStandardPlane( rightVector.GetVnlVector(),
      downVector.GetVnlVector(), spacing );
  }

  void
    PlaneGeometry::InitializeStandardPlane( const VnlVector& rightVector,
    const VnlVector &downVector, const Vector3D *spacing )
  {
    ScalarType width  = rightVector.magnitude();
    ScalarType height = downVector.magnitude();

    InitializeStandardPlane( width, height, rightVector, downVector, spacing );
  }

  void
    PlaneGeometry::InitializeStandardPlane( mitk::ScalarType width,
    ScalarType height, const Vector3D &rightVector, const Vector3D &downVector,
    const Vector3D *spacing )
  {
    InitializeStandardPlane(
      width, height,
      rightVector.GetVnlVector(), downVector.GetVnlVector(),
      spacing );
  }

  void
    PlaneGeometry::InitializeStandardPlane(
    mitk::ScalarType width, ScalarType height,
    const VnlVector &rightVector, const VnlVector &downVector,
    const Vector3D *spacing )
  {
    assert(width > 0);
    assert(height > 0);

    VnlVector rightDV = rightVector; rightDV.normalize();
    VnlVector downDV  = downVector;  downDV.normalize();
    VnlVector normal  = vnl_cross_3d(rightVector, downVector);
    normal.normalize();

    if(spacing!=nullptr)
    {
      rightDV *= (*spacing)[0];
      downDV  *= (*spacing)[1];
      normal  *= (*spacing)[2];
    }

    AffineTransform3D::Pointer transform = AffineTransform3D::New();
    Matrix3D matrix;
    matrix.GetVnlMatrix().set_column(0, rightDV);
    matrix.GetVnlMatrix().set_column(1, downDV);
    matrix.GetVnlMatrix().set_column(2, normal);
    transform->SetMatrix(matrix);
    transform->SetOffset(this->GetIndexToWorldTransform()->GetOffset());

    ScalarType bounds[6] = { 0, width, 0, height, 0, 1 };
    this->SetBounds( bounds );

    this->SetIndexToWorldTransform( transform );
  }

  void
    PlaneGeometry::InitializePlane( const Point3D &origin,
    const Vector3D &normal )
  {
    VnlVector rightVectorVnl(3), downVectorVnl;

    if( Equal( normal[1], 0.0f ) == false )
    {
      FillVector3D( rightVectorVnl, 1.0f, -normal[0]/normal[1], 0.0f );
      rightVectorVnl.normalize();
    }
    else
    {
      FillVector3D( rightVectorVnl, 0.0f, 1.0f, 0.0f );
    }
    downVectorVnl = vnl_cross_3d( normal.GetVnlVector(), rightVectorVnl );
    downVectorVnl.normalize();

    InitializeStandardPlane( rightVectorVnl, downVectorVnl );

    SetOrigin(origin);
  }

  void
    PlaneGeometry::SetMatrixByVectors( const VnlVector &rightVector,
    const VnlVector &downVector, ScalarType thickness )
  {
    VnlVector normal = vnl_cross_3d(rightVector, downVector);
    normal.normalize();
    normal *= thickness;

    AffineTransform3D::Pointer transform = AffineTransform3D::New();
    Matrix3D matrix;
    matrix.GetVnlMatrix().set_column(0, rightVector);
    matrix.GetVnlMatrix().set_column(1, downVector);
    matrix.GetVnlMatrix().set_column(2, normal);
    transform->SetMatrix(matrix);
    transform->SetOffset(this->GetIndexToWorldTransform()->GetOffset());
    SetIndexToWorldTransform(transform);
  }

  Vector3D
    PlaneGeometry::GetNormal() const
  {
    Vector3D frontToBack;
    frontToBack.SetVnlVector( this->GetIndexToWorldTransform()
      ->GetMatrix().GetVnlMatrix().get_column(2) );

    return frontToBack;
  }

  VnlVector
    PlaneGeometry::GetNormalVnl() const
  {
    return  this->GetIndexToWorldTransform()
      ->GetMatrix().GetVnlMatrix().get_column(2);
  }

  ScalarType
    PlaneGeometry::DistanceFromPlane( const Point3D &pt3d_mm ) const
  {
    return fabs(SignedDistance( pt3d_mm ));
  }

  ScalarType
    PlaneGeometry::SignedDistance( const Point3D &pt3d_mm ) const
  {
    return SignedDistanceFromPlane(pt3d_mm);
  }

  //Function from Geometry2D
  //  mitk::ScalarType
  //  PlaneGeometry::SignedDistance(const mitk::Point3D& pt3d_mm) const
  //{
  //  Point3D projectedPoint;
  //  Project(pt3d_mm, projectedPoint);
  //  Vector3D direction = pt3d_mm-projectedPoint;
  //  ScalarType distance = direction.GetNorm();

  //  if(IsAbove(pt3d_mm) == false)
  //    distance*=-1.0;

  //  return distance;
  //}

  bool
    PlaneGeometry::IsAbove( const Point3D &pt3d_mm , bool considerBoundingBox) const
  {
    if(considerBoundingBox)
    {
      Point3D pt3d_units;
      BaseGeometry::WorldToIndex(pt3d_mm, pt3d_units);
      return (pt3d_units[2] > this->GetBoundingBox()->GetBounds()[4]);
    }
    else
      return SignedDistanceFromPlane(pt3d_mm) > 0;
  }

  bool
    PlaneGeometry::IntersectionLine(
    const PlaneGeometry* plane, Line3D& crossline ) const
  {
    Vector3D normal = this->GetNormal();
    normal.Normalize();

    Vector3D planeNormal = plane->GetNormal();
    planeNormal.Normalize();

    Vector3D direction = itk::CrossProduct( normal, planeNormal );

    if ( direction.GetSquaredNorm() < eps )
      return false;

    crossline.SetDirection( direction );

    double N1dN2 = normal * planeNormal;
    double determinant = 1.0 - N1dN2 * N1dN2;

    Vector3D origin = this->GetOrigin().GetVectorFromOrigin();
    Vector3D planeOrigin = plane->GetOrigin().GetVectorFromOrigin();

    double d1 = normal * origin;
    double d2 = planeNormal * planeOrigin;

    double c1 = ( d1 - d2 * N1dN2 ) / determinant;
    double c2 = ( d2 - d1 * N1dN2 ) / determinant;

    Vector3D p = normal * c1 + planeNormal * c2;
    crossline.GetPoint().GetVnlVector() = p.GetVnlVector();

    return true;
  }

  unsigned int
    PlaneGeometry::IntersectWithPlane2D(
    const PlaneGeometry* plane, Point2D& lineFrom, Point2D &lineTo ) const
  {
    Line3D crossline;
    if ( this->IntersectionLine( plane, crossline ) == false )
      return 0;

    Point2D  point2;
    Vector2D direction2;

    this->Map( crossline.GetPoint(), point2 );
    this->Map( crossline.GetPoint(), crossline.GetDirection(), direction2 );

    return
      Line3D::RectangleLineIntersection(
      0, 0, GetExtentInMM(0), GetExtentInMM(1),
      point2, direction2, lineFrom, lineTo );
  }

  double PlaneGeometry::Angle( const PlaneGeometry *plane ) const
  {
    return angle(plane->GetMatrixColumn(2), GetMatrixColumn(2));
  }

  double PlaneGeometry::Angle( const Line3D &line ) const
  {
    return vnl_math::pi_over_2
      - angle( line.GetDirection().GetVnlVector(), GetMatrixColumn(2) );
  }

  bool PlaneGeometry::IntersectionPoint(
    const Line3D &line, Point3D &intersectionPoint ) const
  {
    Vector3D planeNormal = this->GetNormal();
    planeNormal.Normalize();

    Vector3D lineDirection = line.GetDirection();
    lineDirection.Normalize();

    double t = planeNormal * lineDirection;
    if ( fabs( t ) < eps )
    {
      return false;
    }

    Vector3D diff;
    diff = this->GetOrigin() - line.GetPoint();
    t = ( planeNormal * diff ) / t;

    intersectionPoint = line.GetPoint() + lineDirection * t;
    return true;
  }

  bool
    PlaneGeometry::IntersectionPointParam( const Line3D &line, double &t ) const
  {
    Vector3D planeNormal = this->GetNormal();

    Vector3D lineDirection = line.GetDirection();

    t = planeNormal * lineDirection;

    if ( fabs( t ) < eps )
    {
      return false;
    }

    Vector3D diff;
    diff = this->GetOrigin() - line.GetPoint();
    t = ( planeNormal * diff  ) / t;
    return true;
  }

  bool
    PlaneGeometry::IsParallel( const PlaneGeometry *plane ) const
  {
    return ( (Angle(plane) < 10.0 * mitk::sqrteps ) || ( Angle(plane) > ( vnl_math::pi - 10.0 * sqrteps ) ) ) ;
  }

  bool
    PlaneGeometry::IsOnPlane( const Point3D &point ) const
  {
    return Distance(point) < eps;
  }

  bool
    PlaneGeometry::IsOnPlane( const Line3D &line ) const
  {
    return ( (Distance( line.GetPoint() ) < eps)
      && (Distance( line.GetPoint2() ) < eps) );
  }

  bool
    PlaneGeometry::IsOnPlane( const PlaneGeometry *plane ) const
  {
    return ( IsParallel( plane ) && (Distance( plane->GetOrigin() ) < eps) );
  }

  Point3D
    PlaneGeometry::ProjectPointOntoPlane( const Point3D& pt ) const
  {
    ScalarType len = this->GetNormalVnl().two_norm();
    return pt - this->GetNormal() * this->SignedDistanceFromPlane( pt ) / len;
  }

  itk::LightObject::Pointer
    PlaneGeometry::InternalClone() const
  {
    Self::Pointer newGeometry = new PlaneGeometry(*this);
    newGeometry->UnRegister();
    return newGeometry.GetPointer();
  }

  void
    PlaneGeometry::ExecuteOperation( Operation *operation )
  {
    vtkTransform *transform = vtkTransform::New();
    transform->SetMatrix( this->GetVtkMatrix());

    switch ( operation->GetOperationType() )
    {
    case OpORIENT:
      {
        mitk::PlaneOperation *planeOp = dynamic_cast< mitk::PlaneOperation * >( operation );
        if ( planeOp == nullptr )
        {
          return;
        }

        Point3D center = planeOp->GetPoint();

        Vector3D orientationVector = planeOp->GetNormal();
        Vector3D defaultVector;
        FillVector3D( defaultVector, 0.0, 0.0, 1.0 );

        Vector3D rotationAxis = itk::CrossProduct( orientationVector, defaultVector );
        //double rotationAngle = acos( orientationVector[2] / orientationVector.GetNorm() );

        double rotationAngle = atan2( (double) rotationAxis.GetNorm(), (double) (orientationVector * defaultVector) );
        rotationAngle *= 180.0 / vnl_math::pi;

        transform->PostMultiply();
        transform->Identity();
        transform->Translate( center[0], center[1], center[2] );
        transform->RotateWXYZ( rotationAngle, rotationAxis[0], rotationAxis[1], rotationAxis[2] );
        transform->Translate( -center[0], -center[1], -center[2] );
        break;
      }
    case OpRESTOREPLANEPOSITION:
      {
        RestorePlanePositionOperation *op = dynamic_cast< mitk::RestorePlanePositionOperation* >(operation);
        if(op == nullptr)
        {
          return;
        }

        AffineTransform3D::Pointer transform2 = AffineTransform3D::New();
        Matrix3D matrix;
        matrix.GetVnlMatrix().set_column(0, op->GetTransform()->GetMatrix().GetVnlMatrix().get_column(0));
        matrix.GetVnlMatrix().set_column(1, op->GetTransform()->GetMatrix().GetVnlMatrix().get_column(1));
        matrix.GetVnlMatrix().set_column(2, op->GetTransform()->GetMatrix().GetVnlMatrix().get_column(2));
        transform2->SetMatrix(matrix);
        Vector3D offset = op->GetTransform()->GetOffset();
        transform2->SetOffset(offset);

        this->SetIndexToWorldTransform(transform2);
        ScalarType bounds[6] = {0, op->GetWidth(), 0, op->GetHeight(), 0 ,1 };
        this->SetBounds(bounds);
        this->Modified();
        transform->Delete();
        return;
      }
    default:
      Superclass::ExecuteOperation( operation );
      transform->Delete();
      return;
    }

    this->SetVtkMatrixDeepCopy(transform);
    this->Modified();
    transform->Delete();
  }

  void PlaneGeometry::PrintSelf( std::ostream& os, itk::Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);
    os << indent << " ScaleFactorMMPerUnitX: "
      << GetExtentInMM(0) / GetExtent(0) << std::endl;
    os << indent << " ScaleFactorMMPerUnitY: "
      << GetExtentInMM(1) / GetExtent(1) << std::endl;
    os << indent << " Normal: " << GetNormal() << std::endl;
  }

  bool PlaneGeometry::Map(const mitk::Point3D &pt3d_mm, mitk::Point2D &pt2d_mm) const
  {
    assert(this->IsBoundingBoxNull()==false);

    Point3D pt3d_units;
    Superclass::WorldToIndex(pt3d_mm, pt3d_units);
    pt2d_mm[0] = pt3d_units[0] * GetExtentInMM(0) / GetExtent(0);
    pt2d_mm[1] = pt3d_units[1] * GetExtentInMM(1) / GetExtent(1);
    pt3d_units[2]=0;
    return const_cast<BoundingBox*>(this->GetBoundingBox())->IsInside(pt3d_units);
  }

  void
    PlaneGeometry::Map(const mitk::Point2D &pt2d_mm, mitk::Point3D &pt3d_mm) const
  {
    //pt2d_mm is measured from the origin of the world geometry (at leats it called form BaseRendere::Mouse...Event)
    Point3D pt3d_units;
    pt3d_units[0] = pt2d_mm[0] / (GetExtentInMM(0) / GetExtent(0));
    pt3d_units[1] = pt2d_mm[1] / (GetExtentInMM(1) / GetExtent(1));
    pt3d_units[2]=0;
    //pt3d_units is a continuos index. We divided it with the Scale Factor (= spacing in x and y) to convert it from mm to index units.
    //
    pt3d_mm = GetIndexToWorldTransform()->TransformPoint(pt3d_units);
    //now we convert the 3d index to a 3D world point in mm. We could have used IndexToWorld as well as GetITW->Transform...
  }

  void
    PlaneGeometry::SetSizeInUnits(mitk::ScalarType width, mitk::ScalarType height)
  {
    ScalarType bounds[6]={0, width, 0, height, 0, 1};
    ScalarType extent, newextentInMM;
    if(GetExtent(0)>0)
    {
      extent = GetExtent(0);
      if(width>extent)
        newextentInMM = GetExtentInMM(0)/width*extent;
      else
        newextentInMM = GetExtentInMM(0)*extent/width;
      SetExtentInMM(0, newextentInMM);
    }
    if(GetExtent(1)>0)
    {
      extent = GetExtent(1);
      if(width>extent)
        newextentInMM = GetExtentInMM(1)/height*extent;
      else
        newextentInMM = GetExtentInMM(1)*extent/height;
      SetExtentInMM(1, newextentInMM);
    }
    SetBounds(bounds);
  }

  bool
    PlaneGeometry::Project(
    const mitk::Point3D &pt3d_mm, mitk::Point3D &projectedPt3d_mm) const
  {
    assert(this->IsBoundingBoxNull()==false);

    Point3D pt3d_units;
    Superclass::WorldToIndex(pt3d_mm, pt3d_units);
    pt3d_units[2] = 0;
    projectedPt3d_mm = GetIndexToWorldTransform()->TransformPoint(pt3d_units);
    return const_cast<BoundingBox*>(this->GetBoundingBox())->IsInside(pt3d_units);
  }

  bool
    PlaneGeometry::Project(const mitk::Vector3D &vec3d_mm, mitk::Vector3D &projectedVec3d_mm) const
  {
    assert(this->IsBoundingBoxNull()==false);

    Vector3D vec3d_units;
    Superclass::WorldToIndex(vec3d_mm, vec3d_units);
    vec3d_units[2] = 0;
    projectedVec3d_mm = GetIndexToWorldTransform()->TransformVector(vec3d_units);
    return true;
  }

  bool
    PlaneGeometry::Project(const mitk::Point3D & atPt3d_mm,
    const mitk::Vector3D &vec3d_mm, mitk::Vector3D &projectedVec3d_mm) const
  {
    MITK_WARN << "Deprecated function! Call Project(vec3D,vec3D) instead.";
    assert(this->IsBoundingBoxNull()==false);

    Vector3D vec3d_units;
    Superclass::WorldToIndex(atPt3d_mm, vec3d_mm, vec3d_units);
    vec3d_units[2] = 0;
    projectedVec3d_mm = GetIndexToWorldTransform()->TransformVector(vec3d_units);

    Point3D pt3d_units;
    Superclass::WorldToIndex(atPt3d_mm, pt3d_units);
    return const_cast<BoundingBox*>(this->GetBoundingBox())->IsInside(pt3d_units);
  }

  bool
    PlaneGeometry::Map(const mitk::Point3D & atPt3d_mm,
    const mitk::Vector3D &vec3d_mm, mitk::Vector2D &vec2d_mm) const
  {
    Point2D pt2d_mm_start, pt2d_mm_end;
    Point3D pt3d_mm_end;
    bool inside=Map(atPt3d_mm, pt2d_mm_start);
    pt3d_mm_end = atPt3d_mm+vec3d_mm;
    inside&=Map(pt3d_mm_end, pt2d_mm_end);
    vec2d_mm=pt2d_mm_end-pt2d_mm_start;
    return inside;
  }

  void
    PlaneGeometry::Map(const mitk::Point2D &/*atPt2d_mm*/,
    const mitk::Vector2D &/*vec2d_mm*/, mitk::Vector3D &/*vec3d_mm*/) const
  {
    //@todo implement parallel to the other Map method!
    assert(false);
  }

  void
    PlaneGeometry::SetReferenceGeometry( const mitk::BaseGeometry *geometry )
  {
    m_ReferenceGeometry = geometry;
  }

  const mitk::BaseGeometry *
    PlaneGeometry::GetReferenceGeometry() const
  {
    return m_ReferenceGeometry;
  }

  bool
    PlaneGeometry::HasReferenceGeometry() const
  {
    return ( m_ReferenceGeometry != nullptr );
  }
} // namespace
