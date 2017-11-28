//
//   Project Name:        Kratos

//   Last Modified by:    $Author: pryzhakov $
//   Date:                $Date: 2008-11-26 15:05:54 $
//   Revision:            $Revision: 1.11 $
//
//



// System includes


// External includes 


// Project includes
#include "includes/define.h"


#include "includes/deprecated_variables.h"
#include "geometries/triangle_2d_3.h"
#include "geometries/tetrahedra_3d_4.h"
#include "geometries/point_3d.h"
#include "geometries/point_2d.h"

#include "geometries/triangle_3d_3.h"
#include "geometries/quadrilateral_2d_4.h"
#include "geometries/hexahedra_3d_8.h"
#include "geometries/line_2d.h"
#include "geometries/line_2d_3.h"

#include "ULF_application.h"
#include "includes/variables.h"




namespace Kratos
{

/*	KRATOS_CREATE_VARIABLE(double, NODAL_AREA)
	KRATOS_CREATE_VARIABLE(double, NODAL_H)
	KRATOS_CREATE_VARIABLE(double, IS_STRUCTURE)
	KRATOS_CREATE_VARIABLE(double, IS_FLUID)
	KRATOS_CREATE_VARIABLE(double, IS_BOUNDARY)
	KRATOS_CREATE_VARIABLE(double, IS_FREE_SURFACE)
	KRATOS_CREATE_VARIABLE(double, IS_FREE_SURFACE)
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(NORMAL_TO_WALL)
*/
//KRATOS_CREATE_VARIABLE(double, IS_LAGRANGIAN_INLET)


//KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(PRESSURE_FORCE)
//KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DISP_FRAC)


KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(PRESSURE_FORCE)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DISP_FRAC)

KRATOS_CREATE_VARIABLE(int, PATCH_INDEX)
KRATOS_CREATE_VARIABLE(double, TAUONE)
KRATOS_CREATE_VARIABLE(double, TAUTWO)
KRATOS_CREATE_VARIABLE(double, NODAL_LENGTH)
KRATOS_CREATE_VARIABLE(double, CURVATURE)
KRATOS_CREATE_VARIABLE(double, TRIPLE_POINT)
KRATOS_CREATE_VARIABLE(double, CONTACT_ANGLE_STATIC)
KRATOS_CREATE_VARIABLE( double, SURFTENS_COEFF ) 
KRATOS_CREATE_VARIABLE( double, MEAN_CURVATURE )
KRATOS_CREATE_VARIABLE( double, GAUSSIAN_CURVATURE )
KRATOS_CREATE_VARIABLE( double, PRINCIPAL_CURVATURE_1 )
KRATOS_CREATE_VARIABLE( double, PRINCIPAL_CURVATURE_2 ) 
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( VISCOUS_STRESSX )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( VISCOUS_STRESSY )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( VISCOUS_STRESSZ ) 
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( PRINCIPAL_DIRECTION_1 ) 
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( PRINCIPAL_DIRECTION_2 )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( NORMAL_GEOMETRIC )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( ADHESION_FORCE )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( NORMAL_EQ )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( NORMAL_CL_EQ )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( NORMAL_TP )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( NORMAL_CL )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( SOLID_FRACTION_GRADIENT )
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( SOLID_FRACTION_GRADIENT_PROJECTED )
KRATOS_CREATE_VARIABLE(double, SUBSCALE_PRESSURE)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(SUBSCALE_VELOCITY)
KRATOS_CREATE_VARIABLE( double, ERASE_FLAG )


KRATOS_CREATE_VARIABLE(double, PRESSURE_OLD_IT)
KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(VAUX)

KratosULFApplication::KratosULFApplication():

    mUpdatedLagrangianFluid2D(0, Element::GeometryType::Pointer(new Triangle2D3<Node<3> >(Element::GeometryType::PointsArrayType(3)))),
    mUpdatedLagrangianFluid3D(0, Element::GeometryType::Pointer(new Tetrahedra3D4 <Node<3> >(Element::GeometryType::PointsArrayType(4)))),
    mUpdatedLagrangianFluid2Dinc(0, Element::GeometryType::Pointer(new Triangle2D3<Node<3> >(Element::GeometryType::PointsArrayType(3)))),
    mUpdatedLagrangianFluid3Dinc(0, Element::GeometryType::Pointer(new Tetrahedra3D4 <Node<3> >(Element::GeometryType::PointsArrayType(4)))),
    mUlfAxisym(0, Element::GeometryType::Pointer(new Triangle2D3<Node<3> >(Element::GeometryType::PointsArrayType(3)))),
    //new one - mix of frac step and ulf_inc
    mUlfFrac2D(0, Element::GeometryType::Pointer(new Triangle2D3<Node<3> >(Element::GeometryType::PointsArrayType(3)))),
    mUlfFrac3D(0, Element::GeometryType::Pointer(new Tetrahedra3D4 <Node<3> >(Element::GeometryType::PointsArrayType(4)))),
    mPointNeumann3D(0, Element::GeometryType::Pointer(new Point3D <Node<3> >(Element::GeometryType::PointsArrayType(1)))),
    mPointNeumann2D(0, Element::GeometryType::Pointer(new Point2D <Node<3> >(Element::GeometryType::PointsArrayType(1)))),
    mPointNeumannAxisym(0, Element::GeometryType::Pointer(new Point2D <Node<3> >(Element::GeometryType::PointsArrayType(1)))),
    mSurfaceTension2D(0, Element::GeometryType::Pointer(new Triangle2D3<Node<3> >(Element::GeometryType::PointsArrayType(3)))),
    mSurfaceTension3D(0, Element::GeometryType::Pointer(new Tetrahedra3D4<Node<3> >(Element::GeometryType::PointsArrayType(4))))
  
    

{}


void KratosULFApplication::Register()
{
    // calling base class register to register Kratos components
    KratosApplication::Register();
    std::cout << "Initializing Kratos ULFApplication... " << std::endl;

    /*		KRATOS_REGISTER_VARIABLE( NODAL_AREA)
    		KRATOS_REGISTER_VARIABLE( NODAL_H)
    		KRATOS_REGISTER_VARIABLE( IS_STRUCTURE)
    		KRATOS_REGISTER_VARIABLE( IS_FLUID)
    		KRATOS_REGISTER_VARIABLE( IS_BOUNDARY)
    		KRATOS_REGISTER_VARIABLE( IS_FREE_SURFACE)
    		KRATOS_REGISTER_VARIABLE( IS_FREE_SURFACE)
    		KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(NORMAL_TO_WALL)
    */
    //	KRATOS_REGISTER_VARIABLE(IS_LAGRANGIAN_INLET)

    //KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(PRESSURE_FORCE)
    //KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(DISP_FRAC)
    
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(PRESSURE_FORCE)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(DISP_FRAC)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(VAUX)
    KRATOS_REGISTER_VARIABLE( PATCH_INDEX )
    KRATOS_REGISTER_VARIABLE(TAUONE)
    KRATOS_REGISTER_VARIABLE(TAUTWO)
    KRATOS_REGISTER_VARIABLE(NODAL_LENGTH)
    KRATOS_REGISTER_VARIABLE(CURVATURE)
    KRATOS_REGISTER_VARIABLE( TRIPLE_POINT)
    KRATOS_REGISTER_VARIABLE( CONTACT_ANGLE )
    KRATOS_REGISTER_VARIABLE( CONTACT_ANGLE_STATIC )
    KRATOS_REGISTER_VARIABLE( SURFTENS_COEFF ) 
    KRATOS_REGISTER_VARIABLE( MEAN_CURVATURE )
    KRATOS_REGISTER_VARIABLE( GAUSSIAN_CURVATURE )
    KRATOS_REGISTER_VARIABLE( PRINCIPAL_CURVATURE_1 )
    KRATOS_REGISTER_VARIABLE( PRINCIPAL_CURVATURE_2 ) 
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( VISCOUS_STRESSX )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( VISCOUS_STRESSY )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( VISCOUS_STRESSZ ) 
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( PRINCIPAL_DIRECTION_1 ) 
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( PRINCIPAL_DIRECTION_2 )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( NORMAL_GEOMETRIC )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( ADHESION_FORCE )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( NORMAL_EQ )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( NORMAL_CL_EQ )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( NORMAL_TP )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( NORMAL_CL )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( SOLID_FRACTION_GRADIENT )
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS( SOLID_FRACTION_GRADIENT_PROJECTED )
    KRATOS_REGISTER_VARIABLE(SUBSCALE_PRESSURE)
    KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(SUBSCALE_VELOCITY)
    KRATOS_REGISTER_VARIABLE( ERASE_FLAG )
    
    
    KRATOS_REGISTER_VARIABLE(PRESSURE_OLD_IT)

    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianFluid2D", mUpdatedLagrangianFluid2D);
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianFluid3D", mUpdatedLagrangianFluid3D);
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianFluid2Dinc", mUpdatedLagrangianFluid2Dinc);
    KRATOS_REGISTER_ELEMENT("UpdatedLagrangianFluid3Dinc", mUpdatedLagrangianFluid3Dinc);
    KRATOS_REGISTER_ELEMENT("UlfAxisym", mUlfAxisym);
    //
    KRATOS_REGISTER_ELEMENT("UlfFrac2D", mUlfFrac2D);
    KRATOS_REGISTER_ELEMENT("UlfFrac3D", mUlfFrac3D);
    KRATOS_REGISTER_CONDITION("PointNeumann3D", mPointNeumann3D);
    KRATOS_REGISTER_CONDITION("PointNeumann2D", mPointNeumann2D);
    KRATOS_REGISTER_CONDITION("PointNeumannAxisym", mPointNeumannAxisym);
    
    KRATOS_REGISTER_ELEMENT("SurfaceTension2D3N",mSurfaceTension2D); //this is the name the element should have according to the naming convention
    KRATOS_REGISTER_ELEMENT("SurfaceTension3D4N",mSurfaceTension3D); //this is the name the element should have according to the naming convention
    KRATOS_REGISTER_ELEMENT("SurfaceTension2D",mSurfaceTension2D);
    KRATOS_REGISTER_ELEMENT("SurfaceTension3D",mSurfaceTension3D);

}



} // namespace Kratos. 
