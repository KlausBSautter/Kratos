//   
//   Project Name:        KratosSolidMechanicsApplication $      
//   Last modified by:    $Author:            JMCarbonell $ 
//   Date:                $Date:                July 2013 $
//   Revision:            $Revision:                  0.0 $
//
//

// System includes
#include <iostream>

// External includes
#include<cmath>

// Project includes
#include "includes/properties.h"
#include "custom_constitutive/linear_elastic_3D_law.hpp"

#include "solid_mechanics_application.h"

namespace Kratos
{

  //******************************CONSTRUCTOR*******************************************
  //************************************************************************************

  LinearElastic3DLaw::LinearElastic3DLaw()
  : HyperElastic3DLaw()
  {
  }

  //******************************COPY CONSTRUCTOR**************************************
  //************************************************************************************

  LinearElastic3DLaw::LinearElastic3DLaw(const LinearElastic3DLaw& rOther)
  : HyperElastic3DLaw()
  {
  }
  
  //********************************CLONE***********************************************
  //************************************************************************************

  ConstitutiveLaw::Pointer LinearElastic3DLaw::Clone() const
  {
    LinearElastic3DLaw::Pointer p_clone(new LinearElastic3DLaw(*this));
    return p_clone;
  }
  
  //*******************************DESTRUCTOR*******************************************
  //************************************************************************************

  LinearElastic3DLaw::~LinearElastic3DLaw()
  {
  }


  //************* COMPUTING  METHODS
  //************************************************************************************
  //************************************************************************************


  //*****************************MATERIAL RESPONSES*************************************
  //************************************************************************************


  void  LinearElastic3DLaw::CalculateMaterialResponsePK2 (Parameters& rValues)
  {

     //-----------------------------//
   
    //a.-Check if the constitutive parameters are passed correctly to the law calculation
    CheckParameters(rValues);
    
    //b.- Get Values to compute the constitutive law:
    const Properties& MaterialProperties  = rValues.GetMaterialProperties();
    const Matrix& DeformationGradientF    = rValues.GetDeformationGradientF();
    //const double& DeterminantF            = rValues.GetDeterminantF(); 

    Vector& StrainVector                  = rValues.GetStrainVector();
    Matrix& DeformationGradientF0         = rValues.GetDeformationGradientF0();
    //double& DeterminantF0                 = rValues.GetDeterminantF0(); 

    Vector& StressVector                  = rValues.GetStressVector();
    Matrix& ConstitutiveMatrix            = rValues.GetConstitutiveMatrix();      

    //-----------------------------//

    //0.- Voigt size
    Flags &Options=rValues.GetOptions();

    //1.- Lame constants
    const double& YoungModulus          = MaterialProperties[YOUNG_MODULUS];
    const double& PoissonCoefficient    = MaterialProperties[POISSON_RATIO];

    //2.-Total Deformation Gradient
    Matrix F0 = prod(DeformationGradientF,DeformationGradientF0);

    //3.-Determinant of the Total Deformation Gradient
    //double detF0 = DeterminantF0 * DeterminantF;
   
    //4.-Right Cauchy Green
    Matrix RightCauchyGreen = prod(trans(F0),F0);

    //5.-Inverse of the Right Cauchy-Green tensor C:
    double Trace_C=0;
    Matrix InverseRightCauchyGreen ( 2 , 2 );
    MathUtils<double>::InvertMatrix( RightCauchyGreen, InverseRightCauchyGreen, Trace_C);

    //6.-Green-Lagrange Strain:
    if(Options.Is( ConstitutiveLaw::COMPUTE_STRAIN ))
      {
	//E= 0.5*(FT*F-1)
	this->CalculateGreenLagrangeStrain(RightCauchyGreen,StrainVector);
      }

    //7.-Calculate Total PK2 stress   
   
    if( Options.Is( ConstitutiveLaw::COMPUTE_STRESS ) ){
	  
      this->CalculateLinearElasticMatrix( ConstitutiveMatrix, YoungModulus, PoissonCoefficient );

      this->CalculateStress( StrainVector, ConstitutiveMatrix, StressVector );		

    }
    else if(  Options.IsNot( ConstitutiveLaw::COMPUTE_STRESS ) && Options.Is( ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR ) ){

      this->CalculateLinearElasticMatrix( ConstitutiveMatrix, YoungModulus, PoissonCoefficient );

    }

    // std::cout<<" Constitutive "<<ConstitutiveMatrix<<std::endl;
    // std::cout<<" Strain "<<StrainVector<<std::endl;
    // std::cout<<" Stress "<<StressVector<<std::endl;
		
  }


  //************************************************************************************
  //************************************************************************************

  
  void LinearElastic3DLaw::CalculateMaterialResponseKirchhoff (Parameters& rValues)
  {

    //-----------------------------//
   
    //a.-Check if the constitutive parameters are passed correctly to the law calculation
    CheckParameters(rValues);
    
    //b.- Get Values to compute the constitutive law:
    const Properties& MaterialProperties  = rValues.GetMaterialProperties();
    const Matrix&   DeformationGradientF  = rValues.GetDeformationGradientF();
    //const double&   DeterminantF          = rValues.GetDeterminantF(); 

    Vector& StrainVector                  = rValues.GetStrainVector();
    Matrix& DeformationGradientF0         = rValues.GetDeformationGradientF0();
    //double& DeterminantF0                 = rValues.GetDeterminantF0(); 

    Vector& StressVector                  = rValues.GetStressVector();
    Matrix& ConstitutiveMatrix            = rValues.GetConstitutiveMatrix();      

    //-----------------------------//

    //0.- Voigt size
    Flags &Options=rValues.GetOptions();

    //1.- Lame constants
    const double& YoungModulus          = MaterialProperties[YOUNG_MODULUS];
    const double& PoissonCoefficient    = MaterialProperties[POISSON_RATIO];

    //2.-Total Deformation Gradient
    Matrix F0 = prod(DeformationGradientF,DeformationGradientF0);

    //3.-Determinant of the Total Deformation Gradient
    //double detF0 = DeterminantF0 * DeterminantF;
        
    //4.-Left Cauchy-Green tensor b
    Matrix LeftCauchyGreen = prod(F0,trans(F0));

    //6.-Almansi Strain:
    if(Options.Is( ConstitutiveLaw::COMPUTE_STRAIN ))
      {
	// e= 0.5*(1-invbT*invb)   
	this->CalculateAlmansiStrain(LeftCauchyGreen,StrainVector);
	std::cout<<" compute strain "<<std::endl;
      }
 
    //7.-Calculate total Kirchhoff stress   
   
    if( Options.Is( ConstitutiveLaw::COMPUTE_STRESS ) ){
	  
      this->CalculateLinearElasticMatrix( ConstitutiveMatrix, YoungModulus, PoissonCoefficient );

      this->CalculateStress( StrainVector, ConstitutiveMatrix, StressVector );		

    }
    else if(  Options.IsNot( ConstitutiveLaw::COMPUTE_STRESS ) && Options.Is( ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR ) ){

      this->CalculateLinearElasticMatrix( ConstitutiveMatrix, YoungModulus, PoissonCoefficient );

    }

    // std::cout<<" Strain "<<StrainVector<<std::endl;
    // std::cout<<" Stress "<<StressVector<<std::endl;
    // std::cout<<" Constitutive "<<ConstitutiveMatrix<<std::endl;

  }
  


  //***********************COMPUTE TOTAL STRESS PK2*************************************
  //************************************************************************************


  void LinearElastic3DLaw::CalculateStress( const Vector & rStrainVector,
					   const Matrix & rConstitutiveMatrix,
					   Vector& rStressVector )
  {
      
    //1.-2nd Piola Kirchhoff StressVector increment
    rStressVector = prod(rConstitutiveMatrix,rStrainVector);


  }



  //***********************COMPUTE ALGORITHMIC CONSTITUTIVE MATRIX**********************
  //************************************************************************************


  void LinearElastic3DLaw::CalculateLinearElasticMatrix( Matrix& rConstitutiveMatrix, 
							const double &rYoungModulus, 
							const double &rPoissonCoefficient )
  {
    rConstitutiveMatrix.clear();

    //plane strain constitutive matrix:
    rConstitutiveMatrix ( 0 , 0 ) = (rYoungModulus*(1.0-rPoissonCoefficient)/((1.0+rPoissonCoefficient)*(1.0-2*rPoissonCoefficient)));
    rConstitutiveMatrix ( 1 , 1 ) = rConstitutiveMatrix ( 0 , 0 );
    rConstitutiveMatrix ( 2 , 2 ) = rConstitutiveMatrix ( 0 , 0 );

    rConstitutiveMatrix ( 3 , 3 ) = rConstitutiveMatrix ( 0 , 0 )*(1-2*rPoissonCoefficient)/(2.0*(1.0-rPoissonCoefficient));
    rConstitutiveMatrix ( 4 , 4 ) = rConstitutiveMatrix ( 3 , 3 );
    rConstitutiveMatrix ( 5 , 5 ) = rConstitutiveMatrix ( 3 , 3 );

    rConstitutiveMatrix ( 0 , 1 ) = rConstitutiveMatrix ( 0 , 0 )*rPoissonCoefficient/(1.0-rPoissonCoefficient);
    rConstitutiveMatrix ( 1 , 0 ) = rConstitutiveMatrix ( 0 , 1 );

    rConstitutiveMatrix ( 0 , 2 ) = rConstitutiveMatrix ( 0 , 1 );
    rConstitutiveMatrix ( 2 , 0 ) = rConstitutiveMatrix ( 0 , 1 );

    rConstitutiveMatrix ( 1 , 2 ) = rConstitutiveMatrix ( 0 , 1 );
    rConstitutiveMatrix ( 2 , 1 ) = rConstitutiveMatrix ( 0 , 1 );

  }


  

  //******************CHECK CONSISTENCY IN THE CONSTITUTIVE LAW*************************
  //************************************************************************************

  bool LinearElastic3DLaw::CheckParameters(Parameters& rValues)
  {
    return rValues.CheckAllParameters();
  }



  int LinearElastic3DLaw::Check(const Properties& rProperties, 
			       const GeometryType& rGeometry, 
			       const ProcessInfo& rCurrentProcessInfo)
  {

    if(YOUNG_MODULUS.Key() == 0 || rProperties[YOUNG_MODULUS]<= 0.00)
      KRATOS_ERROR(std::invalid_argument,"YOUNG_MODULUS has Key zero or invalid value ","");

    const double& nu = rProperties[POISSON_RATIO];
    const bool check = bool( (nu >0.499 && nu<0.501 ) || (nu < -0.999 && nu > -1.01 ) );

    if(POISSON_RATIO.Key() == 0 || check==true) 
      KRATOS_ERROR(std::invalid_argument,"POISSON_RATIO has Key zero invalid value ","");

	  
    if(DENSITY.Key() == 0 || rProperties[DENSITY]<0.00)
      KRATOS_ERROR(std::invalid_argument,"DENSITY has Key zero or invalid value ","");

	  	    
    return 0;
	    
  }
    
} // Namespace Kratos
