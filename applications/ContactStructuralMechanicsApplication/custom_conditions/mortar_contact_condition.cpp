// KRATOS  ___|  |       |       |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//           | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License: BSD License
//   license: structural_mechanics_application/license.txt
//
//  Main authors:  Mohamed Khalil
//                 Vicente Mataix Ferrándiz
//

// System includes

// External includes

// Project includes
#include "custom_conditions/mortar_contact_condition.h"
#include "custom_utilities/contact_utilities.h"
#include "utilities/math_utils.h"
#include "structural_mechanics_application.h"
#include "structural_mechanics_application_variables.h"
#include "custom_utilities/structural_mechanics_math_utilities.hpp"

/* Includes of particular contact conditions */
#include "contact_2D_2N_2N.hpp"
// #include "contact_3D_3N_3N.hpp"
// #include "contact_3D_4N_4N.hpp"

#include <algorithm>

namespace Kratos 
{
/**
 * Flags related to the condition computation
 */
KRATOS_CREATE_LOCAL_FLAG( MortarContactCondition, COMPUTE_RHS_VECTOR,     0 );
KRATOS_CREATE_LOCAL_FLAG( MortarContactCondition, COMPUTE_LHS_MATRIX,     1 );
KRATOS_CREATE_LOCAL_FLAG( MortarContactCondition, COMPUTE_RHS_VECTOR_WITH_COMPONENTS, 2 );
KRATOS_CREATE_LOCAL_FLAG( MortarContactCondition, COMPUTE_LHS_MATRIX_WITH_COMPONENTS, 3 );

/************************************* CONSTRUCTOR *********************************/
/***********************************************************************************/

MortarContactCondition::MortarContactCondition(
    IndexType NewId,
    GeometryType::Pointer pGeometry ) :
    Condition( NewId, pGeometry )
{
    //DO NOT ADD DOFS HERE!!!
}

/************************************* CONSTRUCTOR *********************************/
/***********************************************************************************/

MortarContactCondition::MortarContactCondition( 
    IndexType NewId,
    GeometryType::Pointer pGeometry,
    PropertiesType::Pointer pProperties ) :
    Condition( NewId, pGeometry, pProperties )
{
    mThisIntegrationMethod = GetGeometry().GetDefaultIntegrationMethod();

    //DO NOT ADD DOFS HERE!!!
}

/************************************* CONSTRUCTOR *********************************/
/***********************************************************************************/

MortarContactCondition::MortarContactCondition( 
    MortarContactCondition const& rOther ) :
    Condition( rOther )
{
    //DO NOT ADD DOFS HERE!!!
}

/************************************* OPERATIONS **********************************/
/***********************************************************************************/

Condition::Pointer MortarContactCondition::Create( 
    IndexType NewId,
    NodesArrayType const& rThisNodes,
    PropertiesType::Pointer pProperties ) const
{
    return Condition::Pointer( new MortarContactCondition( NewId, GetGeometry().Create( rThisNodes ), pProperties ) );
}

/***********************************************************************************/
/***********************************************************************************/

Condition::Pointer MortarContactCondition::Create(
    IndexType NewId,
    GeometryType::Pointer pGeom,
    PropertiesType::Pointer pProperties) const
{
    return Condition::Pointer( new MortarContactCondition( NewId, pGeom, pProperties ) );
}

/************************************* DESTRUCTOR **********************************/
/***********************************************************************************/

MortarContactCondition::~MortarContactCondition( )
{
}

//************************** STARTING - ENDING  METHODS ***************************//
/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::Initialize( ) 
{
    KRATOS_TRY;
    
    if( this->Has(INTEGRATION_ORDER_CONTACT) )
    {
        if (this->GetValue(INTEGRATION_ORDER_CONTACT) == 1)
        {
            mThisIntegrationMethod = GeometryData::GI_EXTENDED_GAUSS_1;
        }
        else if (this->GetValue(INTEGRATION_ORDER_CONTACT) == 2)
        {
            mThisIntegrationMethod = GeometryData::GI_EXTENDED_GAUSS_2;
        }
        else if (this->GetValue(INTEGRATION_ORDER_CONTACT) == 3)
        {
            mThisIntegrationMethod = GeometryData::GI_EXTENDED_GAUSS_3;
        }
        else if (this->GetValue(INTEGRATION_ORDER_CONTACT) == 4)
        {
            mThisIntegrationMethod = GeometryData::GI_EXTENDED_GAUSS_4;
        }
        else if (this->GetValue(INTEGRATION_ORDER_CONTACT) == 5)
        {
            mThisIntegrationMethod = GeometryData::GI_EXTENDED_GAUSS_5;
        }
        else
        {
            std::cout << "The number of integration points is not defined.  INTEGRATION_ORDER_CONTACT: "<< this->GetValue(INTEGRATION_ORDER_CONTACT) << std::endl;
            std::cout << "Options are: 1, 2, 3, 4, 5  " << std::endl;
            std::cout << "Taking default number of integration points (INTEGRATION_ORDER_CONTACT = 1)  " << std::endl;
            mThisIntegrationMethod = GetGeometry().GetDefaultIntegrationMethod();
        }
    }
    else
    {
        mThisIntegrationMethod = GetGeometry().GetDefaultIntegrationMethod();
    }

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::InitializeSolutionStep( ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY;
    
    // Populate the vector of master elements
    std::vector<contact_container> * all_containers = this->GetValue(CONTACT_CONTAINERS);
    mThisMasterElements.clear();
    mThisMasterElements.resize( all_containers->size( ) );
    
    for ( unsigned int i_cond = 0; i_cond < all_containers->size(); ++i_cond )
    {
        mThisMasterElements[i_cond] = (*all_containers)[i_cond].condition;
    }
    
    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::InitializeNonLinearIteration( ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY;
    
    // Populate the vector of master elements
    std::vector<contact_container> *& all_containers = this->GetValue(CONTACT_CONTAINERS);
    
    double ActiveCheckFactor = 0.005;
    if( GetProperties().Has(ACTIVE_CHECK_FACTOR) )
    {
        ActiveCheckFactor = GetProperties().GetValue(ACTIVE_CHECK_FACTOR);
    }
    
    for ( unsigned int i_cond = 0; i_cond < all_containers->size(); ++i_cond )
    {
        Condition::Pointer pCond = (*all_containers)[i_cond].condition;
    
        // Fill the condition
        ContactUtilities::ContactContainerFiller((*all_containers)[i_cond], pCond->GetGeometry().Center(), GetGeometry(), pCond->GetGeometry(), 
 this->GetValue(NORMAL), pCond->GetValue(NORMAL), ActiveCheckFactor, mThisIntegrationMethod);
    }
    
    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::FinalizeNonLinearIteration( ProcessInfo& rCurrentProcessInfo )
{
    // TODO: Add things if needed
}

/***********************************************************************************/
/***********************************************************************************/

const Vector MortarContactCondition::LagrangeMultiplierShapeFunctionValue(
    const double xi_local,
    const double eta_local
    )
{
    // NOTE: For more information look at the thesis of Popp page 93/236
    
    // Working space dimension
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    
    // Number of slave nodes
    const unsigned int number_nodes_slave = GetGeometry( ).PointsNumber();
    
    Vector Phi = ZeroVector( number_nodes_slave );
            
    if (dimension == 2)
    {
        if (number_nodes_slave == 2) // First order
        {
            Phi[0] = ( 0.5 * ( 1.0 - 3.0 * xi_local ) );
            Phi[1] = ( 0.5 * ( 1.0 + 3.0 * xi_local ) );
        }
        else if (number_nodes_slave == 3) // Second order
        {
            array_1d<double,3> aux_coordinates = ZeroVector(3);
            aux_coordinates[0] = xi_local;
            Vector Ncontainer = ZeroVector(3);
            Ncontainer = GetGeometry().ShapeFunctionsValues(Ncontainer, aux_coordinates);

            Phi[0] = Ncontainer(0) -  3.0/4.0 * Ncontainer(2) + 0.5;
            Phi[1] = Ncontainer(1) -  3.0/4.0 * Ncontainer(2) + 0.5;
            Phi[2] = 5.0/2.0 * Ncontainer(2) - 1.0;
        }
    }
    else if (dimension == 3)
    {
        if (number_nodes_slave == 3) // Triangle
        {
            Phi[0] = 3.0 - 4.0 * xi_local - 4.0 * eta_local;
            Phi[1] = 4.0 * xi_local  - 1.0;
            Phi[2] = 4.0 * eta_local - 1.0;
        }
        else if (number_nodes_slave == 4) // Quadrilateral
        {
            array_1d<double,3> aux_coordinates = ZeroVector(3);
            aux_coordinates[0] =  xi_local;
            aux_coordinates[1] = eta_local;
            Vector Ncontainer = ZeroVector(3);
            Ncontainer = GetGeometry().ShapeFunctionsValues(Ncontainer, aux_coordinates);

            Phi[0] =   4.0 * Ncontainer(0) - 2.0 * Ncontainer(1) + 1.0 * Ncontainer(2) - 2.0 * Ncontainer(3);
            Phi[1] = - 2.0 * Ncontainer(0) + 4.0 * Ncontainer(1) - 2.0 * Ncontainer(2) + 1.0 * Ncontainer(3);
            Phi[2] =   1.0 * Ncontainer(0) - 2.0 * Ncontainer(1) + 4.0 * Ncontainer(2) - 2.0 * Ncontainer(3);
            Phi[3] = - 2.0 * Ncontainer(0) + 1.0 * Ncontainer(1) - 2.0 * Ncontainer(2) + 4.0 * Ncontainer(3);
        }
        else
        {
            KRATOS_THROW_ERROR( std::logic_error,  " CONDITION can not supply the required number_nodes_slave: ", number_nodes_slave );
        }
    }
        
    return Phi;
}

/***********************************************************************************/
/***********************************************************************************/

const Matrix MortarContactCondition::LagrangeMultiplierShapeFunctionLocalGradient( 
    const double xi_local, 
    const double eta_local 
    )
{
    // Working space dimension
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    
    // Number of slave nodes
    const unsigned int number_nodes_slave = GetGeometry( ).PointsNumber( );
    const unsigned int local_dimension_slave = GetGeometry( ).LocalSpaceDimension( );
    
    Matrix DPhi_De = ZeroMatrix( number_nodes_slave, local_dimension_slave );
            
    if (dimension == 2)
    {
        if (number_nodes_slave == 2) // First order
        {
            DPhi_De( 0, 0 ) = - 3.0 / 2.0;
            DPhi_De( 1, 0 ) = + 3.0 / 2.0;
        }
        else if (number_nodes_slave == 3) // Second order
        {
            array_1d<double,3> aux_coordinates = ZeroVector(3);
            aux_coordinates[0] = xi_local;
            Matrix DNcontainer = ZeroMatrix(number_nodes_slave, local_dimension_slave);
            DNcontainer = GetGeometry().ShapeFunctionsLocalGradients( DNcontainer , aux_coordinates );
            
            DPhi_De( 0, 0 ) = DNcontainer(0, 0) -  3.0/4.0 * DNcontainer(2, 0);
            DPhi_De( 1, 0 ) = DNcontainer(1, 0) -  3.0/4.0 * DNcontainer(2, 0);
            DPhi_De( 2, 0 ) = 5.0/2.0 * DNcontainer(2, 0);
        }
    }
    else if (dimension == 3)
    {
        if (number_nodes_slave == 3) // Triangle
        {
            DPhi_De( 0, 0 ) = - 4.0;
            DPhi_De( 1, 0 ) =   4.0;
            DPhi_De( 2, 0 ) =   4.0;
            
            DPhi_De( 0, 1 ) = - 4.0;
            DPhi_De( 1, 1 ) =   0.0;
            DPhi_De( 2, 1 ) =   4.0;
        }
        else if (number_nodes_slave == 4) // Quadrilateral
        {
            array_1d<double,3> aux_coordinates = ZeroVector(3);
            aux_coordinates[0] =  xi_local;
            aux_coordinates[1] = eta_local;
            Matrix DNcontainer = ZeroMatrix(number_nodes_slave, local_dimension_slave);
            DNcontainer = GetGeometry().ShapeFunctionsLocalGradients(DNcontainer, aux_coordinates);

            DPhi_De( 0, 0 ) =   4.0 * DNcontainer( 0, 0 ) - 2.0 * DNcontainer( 1, 0 ) + 1.0 * DNcontainer( 2, 0 ) - 2.0 * DNcontainer( 3, 0 );
            DPhi_De( 1, 0 ) = - 2.0 * DNcontainer( 0, 0 ) + 4.0 * DNcontainer( 1, 0 ) - 2.0 * DNcontainer( 2, 0 ) + 1.0 * DNcontainer( 3, 0 );
            DPhi_De( 2, 0 ) =   1.0 * DNcontainer( 0, 0 ) - 2.0 * DNcontainer( 1, 0 ) + 4.0 * DNcontainer( 2, 0 ) - 2.0 * DNcontainer( 3, 0 );
            DPhi_De( 3, 0 ) = - 2.0 * DNcontainer( 0, 0 ) + 1.0 * DNcontainer( 1, 0 ) - 2.0 * DNcontainer( 2, 0 ) + 4.0 * DNcontainer( 3, 0 );
            DPhi_De( 0, 1 ) =   4.0 * DNcontainer( 0, 1 ) - 2.0 * DNcontainer( 1, 1 ) + 1.0 * DNcontainer( 2, 1 ) - 2.0 * DNcontainer( 3, 1 );
            DPhi_De( 1, 1 ) = - 2.0 * DNcontainer( 0, 1 ) + 4.0 * DNcontainer( 1, 1 ) - 2.0 * DNcontainer( 2, 1 ) + 1.0 * DNcontainer( 3, 1 );
            DPhi_De( 2, 1 ) =   1.0 * DNcontainer( 0, 1 ) - 2.0 * DNcontainer( 1, 1 ) + 4.0 * DNcontainer( 2, 1 ) - 2.0 * DNcontainer( 3, 1 );
            DPhi_De( 3, 1 ) = - 2.0 * DNcontainer( 0, 1 ) + 1.0 * DNcontainer( 1, 1 ) - 2.0 * DNcontainer( 2, 1 ) + 4.0 * DNcontainer( 3, 1 );
        }
        else
        {
            KRATOS_THROW_ERROR( std::logic_error,  " CONDITION can not supply the required number_nodes_slave: ", number_nodes_slave );
        }
    }

    return DPhi_De;
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateLocalSystem( 
    std::vector<MatrixType>& rLeftHandSideMatrices,
    const std::vector<Variable<MatrixType> >& rLHSVariables,
    std::vector<VectorType>& rRightHandSideVectors,
    const std::vector<Variable<VectorType> >& rRHSVariables,
    ProcessInfo& rCurrentProcessInfo 
    )
{    
    // Create local system components
    LocalSystemComponents LocalSystem;

    // Calculation flags
    LocalSystem.CalculationFlags.Set(MortarContactCondition::COMPUTE_LHS_MATRIX_WITH_COMPONENTS, true);
    LocalSystem.CalculationFlags.Set(MortarContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS, true);

    //Initialize sizes for the system components:
    if ( rLHSVariables.size( ) != rLeftHandSideMatrices.size( ) )
    {
        rLeftHandSideMatrices.resize( rLHSVariables.size( ) );
    }

    if ( rRHSVariables.size( ) != rRightHandSideVectors.size( ) )
    {
        rRightHandSideVectors.resize( rRHSVariables.size( ) );
    }

    LocalSystem.CalculationFlags.Set(MortarContactCondition::COMPUTE_LHS_MATRIX, true);
    for ( unsigned int i = 0; i < rLeftHandSideMatrices.size( ); i++ )
    {
        // Note: rRightHandSideVectors.size() > 0
        this->InitializeSystemMatrices( rLeftHandSideMatrices[i], rRightHandSideVectors[0],LocalSystem.CalculationFlags );
    }

    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_RHS_VECTOR, true );
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_LHS_MATRIX, false ); // Temporarily only
    for ( unsigned int i = 0; i < rRightHandSideVectors.size( ); i++ )
    {
        // Note: rLeftHandSideMatrices.size() > 0
        this->InitializeSystemMatrices( rLeftHandSideMatrices[0], rRightHandSideVectors[i], LocalSystem.CalculationFlags  );
    }
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_LHS_MATRIX, true ); // Reactivated again

    // Set Variables to Local system components
    LocalSystem.SetLeftHandSideMatrices( rLeftHandSideMatrices );
    LocalSystem.SetRightHandSideVectors( rRightHandSideVectors );

    LocalSystem.SetLeftHandSideVariables( rLHSVariables );
    LocalSystem.SetRightHandSideVariables( rRHSVariables );

    // Calculate condition system
    this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );

}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateLocalSystem(
    MatrixType& rLeftHandSideMatrix,
    VectorType& rRightHandSideVector,
    ProcessInfo& rCurrentProcessInfo 
    )
{
    KRATOS_TRY;
    
    // Create local system components
    LocalSystemComponents LocalSystem;

    // Calculation flags
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_LHS_MATRIX, true );
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_RHS_VECTOR, true );

    // Initialize sizes for the system components:
    this->InitializeSystemMatrices( rLeftHandSideMatrix, rRightHandSideVector, LocalSystem.CalculationFlags );
    
    // Set Variables to Local system components
    LocalSystem.SetLeftHandSideMatrix( rLeftHandSideMatrix );
    LocalSystem.SetRightHandSideVector( rRightHandSideVector );

    // Calculate condition system
    this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateLeftHandSide( 
    MatrixType& rLeftHandSideMatrix,
    ProcessInfo& rCurrentProcessInfo 
    )
{
    // Create local system components
    LocalSystemComponents LocalSystem;

    // Calculation flags
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_LHS_MATRIX, true );

    VectorType RightHandSideVector = Vector( );

    // Initialize sizes for the system components:
    this->InitializeSystemMatrices( rLeftHandSideMatrix, RightHandSideVector, LocalSystem.CalculationFlags );

    // Set Variables to Local system components
    LocalSystem.SetLeftHandSideMatrix( rLeftHandSideMatrix );
    LocalSystem.SetRightHandSideVector( RightHandSideVector );

    // Calculate condition system
    this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateLeftHandSide( 
    std::vector< MatrixType >& rLeftHandSideMatrices,
    const std::vector< Variable< MatrixType > >& rLHSVariables,
    ProcessInfo& rCurrentProcessInfo 
    )
{
    // Create local system components
    LocalSystemComponents LocalSystem;

    // Calculation flags
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_LHS_MATRIX, true );
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_LHS_MATRIX_WITH_COMPONENTS, true );

    VectorType RightHandSideVector = Vector( );

    // Initialize size for the system components
    for( unsigned int i = 0; i < rLeftHandSideMatrices.size( ); i++ )
    {
        this->InitializeSystemMatrices( rLeftHandSideMatrices[i], RightHandSideVector, LocalSystem.CalculationFlags );
    }

    LocalSystem.SetLeftHandSideMatrices( rLeftHandSideMatrices );
    LocalSystem.SetRightHandSideVector( RightHandSideVector );

    this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateRightHandSide( 
    VectorType& rRightHandSideVector,
    ProcessInfo& rCurrentProcessInfo 
    )
{
    // Create local system components
    LocalSystemComponents LocalSystem;

    // Calculation flags
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_RHS_VECTOR, true);

    MatrixType LeftHandSideMatrix = Matrix( );

    // Initialize size for the system components
    this->InitializeSystemMatrices( LeftHandSideMatrix, rRightHandSideVector,LocalSystem.CalculationFlags);

    //Set Variables to Local system components
    LocalSystem.SetLeftHandSideMatrix( LeftHandSideMatrix );
    LocalSystem.SetRightHandSideVector( rRightHandSideVector );

    //Calculate condition system
    this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateRightHandSide( 
    std::vector< VectorType >& rRightHandSideVectors,
    const std::vector< Variable< VectorType > >& rRHSVariables,
    ProcessInfo& rCurrentProcessInfo )
{
    // Create local system components
    LocalSystemComponents LocalSystem;

    // Calculation flags
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_RHS_VECTOR, true );
    LocalSystem.CalculationFlags.Set( MortarContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS, true );

    MatrixType LeftHandSideMatrix = Matrix( );

    // Initialize size for the system components
    for( unsigned int i = 0; i < rRightHandSideVectors.size(); i++ )
    {
        this->InitializeSystemMatrices( LeftHandSideMatrix, rRightHandSideVectors[i], LocalSystem.CalculationFlags );
    }

    // Set Variables to Local system components
    LocalSystem.SetLeftHandSideMatrix( LeftHandSideMatrix );
    LocalSystem.SetRightHandSideVectors( rRightHandSideVectors );

    // Calculate condition system
    this->CalculateConditionSystem( LocalSystem, rCurrentProcessInfo );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::InitializeSystemMatrices( 
    MatrixType& rLeftHandSideMatrix,
    VectorType& rRightHandSideVector,
    Flags& rCalculationFlags 
    )
{
    const unsigned int condition_size = this->CalculateConditionSize( );
    
    // Resizing as needed the LHS
    if ( rCalculationFlags.Is( MortarContactCondition::COMPUTE_LHS_MATRIX ) ) // Calculation of the matrix is required
    {
        if ( rLeftHandSideMatrix.size1() != condition_size )
        {
            rLeftHandSideMatrix.resize( condition_size, condition_size, false );
        }
        noalias( rLeftHandSideMatrix ) = ZeroMatrix( condition_size, condition_size ); // Resetting LHS
    }

    // Resizing as needed the RHS
    if ( rCalculationFlags.Is( MortarContactCondition::COMPUTE_RHS_VECTOR ) ) // Calculation of the matrix is required
    {
        if ( rRightHandSideVector.size() != condition_size )
        {
            rRightHandSideVector.resize( condition_size, false );
        }
        rRightHandSideVector = ZeroVector( condition_size ); // Resetting RHS
    }
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateMassMatrix( 
    MatrixType& rMassMatrix, 
    ProcessInfo& rCurrentProcessInfo
    )
{
    KRATOS_TRY;
    
    rMassMatrix.resize(0, 0, false);

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateDampingMatrix( 
    MatrixType& rDampingMatrix,
    ProcessInfo& rCurrentProcessInfo
    )
{
    KRATOS_TRY;

    rDampingMatrix.resize(0, 0, false);

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

const unsigned int MortarContactCondition::CalculateConditionSize( )
{
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();

    unsigned int condition_size = 0;
        
    for ( unsigned int i_master_elem = 0; i_master_elem < mThisMasterElements.size( ); ++i_master_elem )
    {
        condition_size += mThisMasterElements[i_master_elem]->GetGeometry( ).PointsNumber( );
        condition_size += 2 * GetGeometry( ).PointsNumber( );
    }
  
    condition_size *= dimension;
    
    return condition_size;
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateConditionSystem( 
    LocalSystemComponents& rLocalSystem,
    const ProcessInfo& rCurrentProcessInfo
    )
{
    KRATOS_TRY;
    
    // Create and initialize condition variables:
    GeneralVariables Variables;
    
    // Initialize the current contact data
    ContactData CurrentContactData;

    // Slave segment info
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    const unsigned int number_nodes_slave        = GetGeometry().PointsNumber( );
    const unsigned int number_of_elements_master = mThisMasterElements.size( );
    
    // Reading integration points
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry( ).IntegrationPoints( mThisIntegrationMethod );
    
    double ActiveCheckFactor = 0.005;
    if( GetProperties().Has(ACTIVE_CHECK_FACTOR) )
    {
        ActiveCheckFactor = GetProperties().GetValue(ACTIVE_CHECK_FACTOR);
    }
    
    this->InitializeContactData(CurrentContactData, rCurrentProcessInfo);
    
    for (unsigned int PairIndex = 0; PairIndex < number_of_elements_master; ++PairIndex)
    {
        const unsigned int num_nodes_master = mThisMasterElements[PairIndex]->GetGeometry( ).PointsNumber( );
        
        // Initialize general variables for the current master element
        this->InitializeGeneralVariables( Variables, rCurrentProcessInfo, PairIndex );
        
        // Update the contact data
        this->UpdateContactData(CurrentContactData, PairIndex);
         
        // Master segment info
        const GeometryType& current_master_element = Variables.GetMasterElement( );
        
        for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); PointNumber++ )
        {
            // Calculate the kinematic variables
            this->CalculateKinematics( Variables, PointNumber, PairIndex, integration_points );
        
            // Calculate the gap in the integration node and check tolerance
            const double integration_point_gap = inner_prod(CurrentContactData.Gaps, Variables.N_Slave);
            
            double dist_tol = ActiveCheckFactor * GetGeometry().Length();
            dist_tol = (dist_tol <= ActiveCheckFactor * current_master_element.Length()) ? (ActiveCheckFactor * current_master_element.Length()):dist_tol;
            
            if (integration_point_gap <= dist_tol)
            {
                
                KRATOS_WATCH(integration_point_gap);
                
                const double IntegrationWeight = integration_points[PointNumber].Weight();
                
                // Calculation of the matrix is required
                if ( rLocalSystem.CalculationFlags.Is( MortarContactCondition::COMPUTE_LHS_MATRIX ) ||
                        rLocalSystem.CalculationFlags.Is( MortarContactCondition::COMPUTE_LHS_MATRIX_WITH_COMPONENTS ) )
                {
                    // Contributions to stiffness matrix calculated on the reference config
                    this->CalculateAndAddLHS( rLocalSystem, Variables, CurrentContactData, PairIndex, IntegrationWeight );
                }

                // Calculation of the vector is required
                if ( rLocalSystem.CalculationFlags.Is( MortarContactCondition::COMPUTE_RHS_VECTOR ) ||
                        rLocalSystem.CalculationFlags.Is( MortarContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS ) )
                {
                    // Contribution to previous step contact force and residuals vector
                    this->CalculateAndAddRHS( rLocalSystem, Variables, CurrentContactData, PairIndex, IntegrationWeight );
                }
            }
        }
    }
    
    // TODO: Add the update of the integrated contact gap
    
//     MatrixType& rLeftHandSideMatrix = rLocalSystem.GetLeftHandSideMatrix( );   
//     KRATOS_WATCH(rLeftHandSideMatrix);
//     KRATOS_WATCH(rRightHandSideVector);
    
    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::InitializeGeneralVariables(
    GeneralVariables& rVariables,
    const ProcessInfo& rCurrentProcessInfo,
    const unsigned int& rMasterElementIndex
    )
{
    // Working space dimension
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();

    // Resize according to the integration order
    const unsigned int& integration_point_number = GetGeometry().IntegrationPointsNumber( mThisIntegrationMethod );
    
    // Slave segment info
    GeometryType& current_slave_element = this->GetGeometry();
    const unsigned int number_nodes_slave = current_slave_element.PointsNumber();
    const unsigned int local_dimension_slave = current_slave_element.LocalSpaceDimension();

    // Master segment info
    GeometryType& current_master_element = mThisMasterElements[rMasterElementIndex]->GetGeometry();
    const unsigned int number_nodes_master = current_master_element.PointsNumber();
    const unsigned int local_dimension_master = current_master_element.LocalSpaceDimension();
    
    // Slave element info
    rVariables.Initialize(local_dimension_master, local_dimension_slave, number_nodes_master, number_nodes_slave, dimension, integration_point_number );

    rVariables.SetMasterElement( current_master_element );
    rVariables.SetMasterElementIndex( rMasterElementIndex );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::InitializeContactData(
    ContactData& rContactData,
    const ProcessInfo& rCurrentProcessInfo
    )
{
    // Working space dimension
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();

    // Slave segment info
    GeometryType& current_slave_element = this->GetGeometry();
    const unsigned int number_nodes_slave = current_slave_element.PointsNumber();
    
    // Slave element info
    rContactData.Initialize(GetGeometry(), number_nodes_slave, dimension );
    
    /* Set Delta time */
    rContactData.Dt = rCurrentProcessInfo[DELTA_TIME];
    
    /* NORMALS AND LM */ 
    array_1d<double,3> tangent1 = ZeroVector(3);
    array_1d<double,3> tangent2 = ZeroVector(3);
    
   if (dimension == 2)
   {
       tangent2[2] = 1.0;
   }
    
    for (unsigned int iNode = 0; iNode < number_nodes_slave; iNode++)
    {
        array_1d<double,3> normal = GetGeometry()[iNode].GetValue(NORMAL);
        
        if (dimension == 3)
        {
            // TODO: Finish this!!!
        }
        
        MathUtils<double>::CrossProduct(tangent1, normal, tangent2);
        
        array_1d<double,3> lm = GetGeometry()[iNode].FastGetSolutionStepValue(VECTOR_LAGRANGE_MULTIPLIER, 0);
        
        for (unsigned int iDof = 0; iDof < dimension; iDof++)
        {
            rContactData.NormalsSlave(iNode, iDof) = normal[iDof]; 
            rContactData.LagrangeMultipliers(iNode, iDof) = lm[iDof];
            
            rContactData.Tangent1Slave(iNode, iDof) = tangent1[iDof];
            if (dimension == 3)
            {
                rContactData.Tangent2Slave(iNode, iDof) = tangent2[iDof];
            }
        }
    }
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::UpdateContactData(
    ContactData& rContactData,
    const unsigned int& rMasterElementIndex
    )
{
    // Working space dimension
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();

    // Slave segment info
    GeometryType& current_slave_element = this->GetGeometry();
    const unsigned int number_nodes_slave = current_slave_element.PointsNumber();
    
    // Master segment info
    GeometryType& current_master_element = mThisMasterElements[rMasterElementIndex]->GetGeometry();
    const unsigned int number_nodes_master = current_master_element.PointsNumber();
    
    // Slave element info
    rContactData.UpdateMasterPair(current_master_element, number_nodes_master, dimension );
    
    /* NORMALS AND GAPS */
    for (unsigned int iNode = 0; iNode < number_nodes_master; iNode++)
    {
        array_1d<double,3> normal = current_master_element[iNode].GetValue(NORMAL);
        
        for (unsigned int iDof = 0; iDof < dimension; iDof++)
        {
            rContactData.NormalsMaster(iNode, iDof) = normal[iDof]; 
        }
    }
    
    for (unsigned int iNode = 0; iNode < number_nodes_slave; iNode++)
    {
        array_1d<double,3> normal = GetGeometry()[iNode].GetValue(NORMAL);
        
        PointType projected_global;
        ContactUtilities::ProjectDirection(current_master_element, GetGeometry()[iNode], projected_global, rContactData.Gaps(iNode), normal ); // NOTE: This is not the CPP, so the solution can be wrong
    }
}

/*********************************COMPUTE KINEMATICS*********************************/
/************************************************************************************/

void MortarContactCondition::CalculateKinematics( 
    GeneralVariables& rVariables,
    const double& rPointNumber,
    const unsigned int& rPairIndex,
    const GeometryType::IntegrationPointsArrayType& integration_points
    )
{
    KRATOS_TRY;
    
    /* DEFINITIONS */
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    GeometryType& slave_nodes  = GetGeometry( );
    GeometryType& master_nodes = rVariables.GetMasterElement( );
    const unsigned int number_of_master_nodes = master_nodes.PointsNumber( );
    const unsigned int number_of_slave_nodes  =  slave_nodes.PointsNumber( );

    /* LOCAL COORDINATES */
    const PointType& local_point = integration_points[rPointNumber].Coordinates();
    
    /* RESIZE MATRICES AND VECTORS */
    rVariables.Phi_LagrangeMultipliers.resize( number_of_slave_nodes, false );
    rVariables.N_Master.resize( number_of_master_nodes, false );
    rVariables.N_Slave.resize( number_of_slave_nodes, false );

    rVariables.DN_De_Master.resize( number_of_master_nodes, master_nodes.LocalSpaceDimension( ), false );
    rVariables.DN_De_Slave.resize( number_of_slave_nodes, slave_nodes.LocalSpaceDimension( ), false );
    rVariables.DPhi_De_LagrangeMultipliers.resize( number_of_slave_nodes, slave_nodes.LocalSpaceDimension( ), false );
    
    /*  POPULATE MATRICES AND VECTORS */
    
    /// SLAVE CONDITION ///
    
    // SHAPE FUNCTIONS 
    rVariables.N_Slave = slave_nodes.ShapeFunctionsValues( rVariables.N_Slave, local_point.Coordinates() );
    rVariables.Phi_LagrangeMultipliers= LagrangeMultiplierShapeFunctionValue( local_point.Coordinate(1), local_point.Coordinate(2) );
    
    // SHAPE FUNCTION DERIVATIVES
    rVariables.DN_De_Slave  =  slave_nodes.ShapeFunctionsLocalGradients( rVariables.DN_De_Slave , local_point );
    rVariables.DPhi_De_LagrangeMultipliers = LagrangeMultiplierShapeFunctionLocalGradient( local_point.Coordinate(1), local_point.Coordinate(2) );
    
    // MASTER CONDITION
    this->MasterShapeFunctionValue( rVariables, local_point);
    
    /* CALCULATE JACOBIAN AND JACOBIAN DETERMINANT */
    slave_nodes.Jacobian( rVariables.j_Slave, local_point.Coordinates() );
    if ( dimension == 2)
    {
        rVariables.DetJSlave = norm_2( column( rVariables.j_Slave, 0 ) );
    }
    else
    {
        Matrix aux_matrix = prod(rVariables.j_Slave,trans(rVariables.j_Slave));
        StructuralMechanicsMathUtilities::DetMat3x3(aux_matrix,rVariables.DetJSlave);
        rVariables.DetJSlave = std::sqrt(rVariables.DetJSlave);
    }

    /* POPULATE DIRECTIONAL DERTIVATIVES RELATED TO KINEMATICS */
    // TODO
    
    KRATOS_CATCH( "" );
}
 
/***********************************************************************************/
/*************** METHODS TO CALCULATE THE CONTACT CONDITION MATRICES ***************/
/***********************************************************************************/

void MortarContactCondition::MasterShapeFunctionValue(
    GeneralVariables& rVariables,
    const PointType& local_point 
    )
{    
    GeometryType& master_seg = rVariables.GetMasterElement( );
    
    rVariables.N_Master = ZeroVector(master_seg.PointsNumber());
    
    double aux_dist = 0.0;
    PointType projected_gp_global;

    array_1d<double,3> normal = ZeroVector(3);
    for( unsigned int iNode = 0; iNode < GetGeometry().PointsNumber(); ++iNode )
    {
        normal += rVariables.N_Slave[iNode] * GetGeometry()[iNode].GetValue(NORMAL); // The opposite direction
    }
    
    normal = normal/norm_2(normal); // It is suppossed to be already unitary (just in case)
    
    // Doing calculations with eta
    GeometryType::CoordinatesArrayType slave_gp_global;
    this->GetGeometry( ).GlobalCoordinates( slave_gp_global, local_point );
    ContactUtilities::ProjectDirection( master_seg, slave_gp_global, projected_gp_global, aux_dist, normal );
    
    GeometryType::CoordinatesArrayType projected_gp_local;
    if( master_seg.IsInside( projected_gp_global.Coordinates( ), projected_gp_local ) )
    {
        // SHAPE FUNCTIONS 
        rVariables.N_Master     = master_seg.ShapeFunctionsValues(         rVariables.N_Master,     projected_gp_local );         
        rVariables.DN_De_Master = master_seg.ShapeFunctionsLocalGradients( rVariables.DN_De_Master, projected_gp_local );         
    }
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateAndAddLHS(
    LocalSystemComponents& rLocalSystem,
    GeneralVariables& rVariables,
    const ContactData& rContactData,
    const unsigned int rPairIndex,
    const double& rIntegrationWeight
    )
{
    if ( rLocalSystem.CalculationFlags.Is( MortarContactCondition::COMPUTE_LHS_MATRIX_WITH_COMPONENTS ) )
    {
        /* COMPONENT-WISE LHS MATRIX */
        const std::vector<Variable<MatrixType> >& rLeftHandSideVariables = rLocalSystem.GetLeftHandSideVariables( );
        bool calculated;

        for ( unsigned int i = 0; i < rLeftHandSideVariables.size( ); i++ )
        {
            calculated = false;

            if ( rLeftHandSideVariables[i] == MORTAR_CONTACT_OPERATOR )
            {
                MatrixType& rLeftHandSideMatrix = rLocalSystem.GetLeftHandSideMatrices( )[i];

                const unsigned int pair_size = GetGeometry().WorkingSpaceDimension() * ( rVariables.GetMasterElement( ).PointsNumber( ) + 2 * GetGeometry( ).PointsNumber( ) ); 
                MatrixType LHS_contact_pair = ZeroMatrix(pair_size, pair_size);
                        
                // Calculate
                this->CalculateLocalLHS( LHS_contact_pair, rVariables, rContactData, rIntegrationWeight );
                
                // Assemble in the correct position
                this->AssembleContactPairLHSToConditionSystem(LHS_contact_pair, rLeftHandSideMatrix, rPairIndex);
                calculated = true;
            }

            if ( calculated == false )
            {
                KRATOS_THROW_ERROR( std::logic_error,  " CONDITION can not supply the required local system variable: ", rLeftHandSideVariables[i] );
            }
        }
    }
    else 
    {
        /* SINGLE LHS MATRIX */
        MatrixType& rLeftHandSideMatrix = rLocalSystem.GetLeftHandSideMatrix( );      
        const unsigned int pair_size = GetGeometry().WorkingSpaceDimension() * ( rVariables.GetMasterElement( ).PointsNumber( ) + 2 * GetGeometry( ).PointsNumber( ) ); 
        MatrixType LHS_contact_pair = ZeroMatrix(pair_size, pair_size);
    
        // Calculate
        this->CalculateLocalLHS( LHS_contact_pair, rVariables, rContactData, rIntegrationWeight );
        
        // Assemble in the correct position
        this->AssembleContactPairLHSToConditionSystem(LHS_contact_pair, rLeftHandSideMatrix, rPairIndex);
    }
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::AssembleContactPairLHSToConditionSystem(
    MatrixType& rPairLHS,
    MatrixType& rConditionLHS,
    const unsigned int rPairIndex
    )
{
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    const unsigned int number_nodes_slave = GetGeometry( ).PointsNumber( );
    const unsigned int num_nodes_master = mThisMasterElements[rPairIndex]->GetGeometry( ).PointsNumber( );
    const unsigned int current_pair_size = dimension * ( num_nodes_master + 2 * number_nodes_slave );
  
    // Find location of the piar's master DOFs in ConditionLHS
    unsigned int index_begin = 0;

    for ( unsigned int i_master_elem = 0; i_master_elem < rPairIndex ; ++i_master_elem ) 
    {
        index_begin += mThisMasterElements[i_master_elem]->GetGeometry( ).PointsNumber( );
        index_begin += 2 * number_nodes_slave;
    }

    index_begin *= dimension;
  
    const unsigned int index_end = index_begin + current_pair_size;
    
    subrange( rConditionLHS, index_begin, index_end, index_begin, index_end) += rPairLHS;
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateLocalLHS(
    MatrixType& rPairLHS,
    GeneralVariables& rVariables,
    const ContactData& rContactData,
    const double& rIntegrationWeight 
    )
{
    /* DEFINITIONS */
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    GeometryType& slave_nodes  = GetGeometry( );
    GeometryType& master_nodes = rVariables.GetMasterElement( );
    const unsigned int number_of_master_nodes = master_nodes.PointsNumber( );
    const unsigned int number_of_slave_nodes  =  slave_nodes.PointsNumber( );
    
    const Vector N1           = rVariables.N_Slave;
    const Vector N2           = rVariables.N_Master;
    const Vector Phi          = rVariables.Phi_LagrangeMultipliers;
    const double J            = rVariables.DetJSlave;
    
    if (dimension == 2)
    {
        if (number_of_master_nodes == 2 &&  number_of_slave_nodes == 2)
        {
            rPairLHS = rIntegrationWeight * ( Contact2D2N2N::ComputeGaussPointLHSInternalContribution( N1, N2, Phi, J, rContactData )
                                           +  Contact2D2N2N::ComputeGaussPointLHSContactContribution( N1, N2, Phi, J, rContactData ) );
        }
        else
        {
            KRATOS_WATCH(number_of_master_nodes);
            KRATOS_THROW_ERROR( std::logic_error,  " COMBINATION OF GEOMETRIES NOT IMPLEMENTED. Number of slave elements: ", number_of_slave_nodes );
        }
    }
    else
    {
//         if (number_of_master_nodes == 3 &&  number_of_slave_nodes == 3)
//         {
//             rPairLHS = rIntegrationWeight * ( Contact3D3N3N::ComputeGaussPointLHSInternalContribution( N1, N2, Phi, J, rContactData )
//                                            +  Contact3D3N3N::ComputeGaussPointLHSContactContribution( N1, N2, Phi, J, rContactData ) );
//         }
//         else if (number_of_master_nodes == 4 &&  number_of_slave_nodes == 4)
//         {
//             rPairLHS = rIntegrationWeight * ( Contact3D4N4N::ComputeGaussPointLHSInternalContribution( N1, N2, Phi, J, rContactData )
//                                            +  Contact3D4N4N::ComputeGaussPointLHSContactContribution( N1, N2, Phi, J, rContactData ) );
//         }
//         else
//         {
            KRATOS_WATCH(number_of_master_nodes);
            KRATOS_THROW_ERROR( std::logic_error,  " COMBINATION OF GEOMETRIES NOT IMPLEMENTED. Number of slave elements: ", number_of_slave_nodes );
//         }
    }
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateAndAddRHS(
    LocalSystemComponents& rLocalSystem,
    GeneralVariables& rVariables,
    const ContactData& rContactData,
    const unsigned int rPairIndex,
    const double& rIntegrationWeight
    )
{
    if ( rLocalSystem.CalculationFlags.Is( MortarContactCondition::COMPUTE_RHS_VECTOR_WITH_COMPONENTS ) )
    {
        /* COMPONENT-WISE RHS VECTOR */
        const std::vector<Variable<VectorType> >& rRightHandSideVariables = rLocalSystem.GetRightHandSideVariables( );
        bool calculated;

        for ( unsigned int i = 0; i < rRightHandSideVariables.size( ); i++ )
        {
            calculated = false;

            if ( rRightHandSideVariables[i] == MORTAR_CONTACT_OPERATOR )
            {
                VectorType& rRightHandSideVector = rLocalSystem.GetRightHandSideVectors()[i];
                const unsigned int pair_size = GetGeometry().WorkingSpaceDimension() * ( rVariables.GetMasterElement( ).PointsNumber( ) + 2 * GetGeometry( ).PointsNumber( ) ); 
                VectorType RHS_contact_pair = ZeroVector(pair_size);
                        
                // Calculate
                this->CalculateLocalRHS( RHS_contact_pair, rVariables, rContactData, rIntegrationWeight);

                // Assemble
                this->AssembleContactPairRHSToConditionSystem( RHS_contact_pair, rRightHandSideVector, rPairIndex );
                
                calculated = true;
            }

            if ( calculated == false )
            {
                KRATOS_THROW_ERROR( std::logic_error,  " CONDITION can not supply the required local system variable: ", rRightHandSideVariables[i] );
            }
        }
    }
    else 
    {
        /* SINGLE RHS VECTOR */
        VectorType& rRightHandSideVector = rLocalSystem.GetRightHandSideVector();
        const unsigned int pair_size = GetGeometry().WorkingSpaceDimension() * ( rVariables.GetMasterElement( ).PointsNumber( ) + 2 * GetGeometry( ).PointsNumber( ) ); 
        VectorType RHS_contact_pair = ZeroVector(pair_size);
    
        // Calculate
        this->CalculateLocalRHS( RHS_contact_pair, rVariables, rContactData, rIntegrationWeight);
        
        // Assemble
        this->AssembleContactPairRHSToConditionSystem( RHS_contact_pair, rRightHandSideVector, rPairIndex );
    }
    
}
  
/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::AssembleContactPairRHSToConditionSystem(
    VectorType& rPairRHS,
    VectorType& rConditionRHS,
    const unsigned int rPairIndex
    )
{
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    const unsigned int number_nodes_slave = GetGeometry( ).PointsNumber( );
    const unsigned int num_nodes_master = mThisMasterElements[rPairIndex]->GetGeometry( ).PointsNumber( );
    const unsigned int current_pair_size = dimension * ( num_nodes_master + 2 * number_nodes_slave );
  
    // Find location of the pair's master DOFs in ConditionRHS
    unsigned int index_begin = 0;

    for ( unsigned int i_master_elem = 0; i_master_elem < rPairIndex; ++i_master_elem )
    {
        index_begin += mThisMasterElements[i_master_elem]->GetGeometry( ).PointsNumber( );
        index_begin += 2 * number_nodes_slave;
    }

    index_begin *= dimension;
    const unsigned int index_end = index_begin + current_pair_size;
    
    // Computing subrange
    subrange( rConditionRHS, index_begin, index_end ) += rPairRHS;
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateLocalRHS(
    VectorType& rPairRHS,
    GeneralVariables& rVariables,
    const ContactData& rContactData,
    const double& rIntegrationWeight 
    )
{
    /* DEFINITIONS */
    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    GeometryType& slave_nodes  = GetGeometry( );
    GeometryType& master_nodes = rVariables.GetMasterElement( );
    const unsigned int number_of_master_nodes = master_nodes.PointsNumber( );
    const unsigned int number_of_slave_nodes  =  slave_nodes.PointsNumber( );
    
    const Vector N1           = rVariables.N_Slave;
    const Vector N2           = rVariables.N_Master;
    const Vector Phi          = rVariables.Phi_LagrangeMultipliers;
    const double J            = rVariables.DetJSlave;
    
//     const double integration_point_gap = inner_prod(rContactData.Gaps, N1);
//     KRATOS_WATCH(integration_point_gap);
    
    if (dimension == 2)
    {
        if (number_of_master_nodes == 2 &&  number_of_slave_nodes == 2)
        {
            rPairRHS = rIntegrationWeight * ( Contact2D2N2N::ComputeGaussPointRHSInternalContribution(N1, N2, Phi, J, rContactData)
                                            + Contact2D2N2N::ComputeGaussPointRHSContactContribution(N1, N2, Phi, J, rContactData) );
        }
        else
        {
            KRATOS_WATCH(number_of_master_nodes);
            KRATOS_THROW_ERROR( std::logic_error,  " COMBINATION OF GEOMETRIES NOT IMPLEMENTED. Number of slave elements: ", number_of_slave_nodes );
        }
    }
    else
    {
//         if (number_of_master_nodes == 3 &&  number_of_slave_nodes == 3)
//         {
//             rPairRHS = rIntegrationWeight * ( Contact3D3N3N::ComputeGaussPointRHSInternalContribution(N1, N2, Phi, J, rContactData)
//                                             + Contact3D3N3N::ComputeGaussPointRHSContactContribution(N1, N2, Phi, J, rContactData) );
//         }
//         if (number_of_master_nodes == 4 &&  number_of_slave_nodes == 4)
//         {
//             rPairRHS = rIntegrationWeight * ( Contact3D4N4N::ComputeGaussPointRHSInternalContribution(N1, N2, Phi, J, rContactData)
//                                             + Contact3D4N4N::ComputeGaussPointRHSContactContribution(N1, N2, Phi, J, rContactData) );
//         }
//         else
//         {
            KRATOS_WATCH(number_of_master_nodes);
            KRATOS_THROW_ERROR( std::logic_error,  " COMBINATION OF GEOMETRIES NOT IMPLEMENTED. Number of slave elements: ", number_of_slave_nodes );
//         }
    }
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::EquationIdVector(
    EquationIdVectorType& rResult,
    ProcessInfo& CurrentProcessInfo )
{
    KRATOS_TRY;
        
    unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    
    rResult.resize( 0, false );

    const std::vector<contact_container> all_conditions = *( this->GetValue( CONTACT_CONTAINERS ) );
        
    /* ORDER - [ MASTER, SLAVE, LAMBDA ] */
    
    const unsigned int number_nodes_slave = GetGeometry().PointsNumber( );
    
    for ( unsigned int i_cond = 0;  i_cond < all_conditions.size( ); ++i_cond )
    {   
        GeometryType& current_master = all_conditions[i_cond].condition->GetGeometry( );
            
        // Master Nodes Displacement Equation IDs
        for ( unsigned int iNode = 0; iNode < current_master.PointsNumber( ); iNode++ )
        {
            NodeType& master_node = current_master[iNode];
            rResult.push_back( master_node.GetDof( DISPLACEMENT_X ).EquationId( ) );
            rResult.push_back( master_node.GetDof( DISPLACEMENT_Y ).EquationId( ) );
            if (dimension == 3)
            {
                rResult.push_back( master_node.GetDof( DISPLACEMENT_Z ).EquationId( ) );
            }
        }
        
        // Slave Nodes Displacement Equation IDs
        for ( unsigned int i_slave = 0; i_slave < number_nodes_slave; ++i_slave )
        {
            NodeType& slave_node = GetGeometry()[ i_slave ];
            rResult.push_back( slave_node.GetDof( DISPLACEMENT_X ).EquationId( ) );
            rResult.push_back( slave_node.GetDof( DISPLACEMENT_Y ).EquationId( ) );
            if (dimension == 3)
            {
                rResult.push_back( slave_node.GetDof( DISPLACEMENT_Z ).EquationId( ) );
            }
        }
        
        // Slave Nodes  Lambda Equation IDs
        for ( unsigned int i_slave = 0; i_slave < number_nodes_slave; ++i_slave )
        {
            NodeType& slave_node = GetGeometry()[ i_slave ];
            rResult.push_back( slave_node.GetDof( VECTOR_LAGRANGE_MULTIPLIER_X ).EquationId( ) );
            rResult.push_back( slave_node.GetDof( VECTOR_LAGRANGE_MULTIPLIER_Y ).EquationId( ) );
            if (dimension == 3)
            {
                rResult.push_back( slave_node.GetDof( VECTOR_LAGRANGE_MULTIPLIER_Z ).EquationId( ) );
            }
        }
    }

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::GetDofList(
    DofsVectorType& rConditionalDofList,
    ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY;
    
    unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    
    rConditionalDofList.resize( 0 );

    const std::vector<contact_container> all_conditions = *( this->GetValue( CONTACT_CONTAINERS ) );
    
    /* ORDER - [ MASTER, SLAVE, LAMBDA ] */
    
    const unsigned int number_nodes_slave = GetGeometry().PointsNumber( );
        
    for ( unsigned int i_cond = 0; i_cond < all_conditions.size( ); ++i_cond )
    {
        GeometryType& current_master = all_conditions[i_cond].condition->GetGeometry( );
        
        // Master Nodes Displacement Equation IDs
        for ( unsigned int iNode = 0; iNode < current_master.PointsNumber( ); iNode++ )
        {
            NodeType& master_node = current_master[iNode];
            rConditionalDofList.push_back( master_node.pGetDof( DISPLACEMENT_X ) );
            rConditionalDofList.push_back( master_node.pGetDof( DISPLACEMENT_Y ) );
            if (dimension == 3)
            {
                rConditionalDofList.push_back( master_node.pGetDof( DISPLACEMENT_Z ) );
            }
        }
        
        // Slave Nodes Displacement Equation IDs
        for ( unsigned int i_slave = 0; i_slave < number_nodes_slave; ++i_slave )
        {
            NodeType& slave_node = GetGeometry()[ i_slave ];
            rConditionalDofList.push_back( slave_node.pGetDof( DISPLACEMENT_X ) );
            rConditionalDofList.push_back( slave_node.pGetDof( DISPLACEMENT_Y ) );
            if (dimension == 3)
            {
                rConditionalDofList.push_back( slave_node.pGetDof( DISPLACEMENT_Z ) );
            }
        }
        
        // Slave Nodes Lambda Equation IDs
        for ( unsigned int i_slave = 0; i_slave < number_nodes_slave; ++i_slave )
        {
            NodeType& slave_node = GetGeometry()[ i_slave ];
            rConditionalDofList.push_back( slave_node.pGetDof( VECTOR_LAGRANGE_MULTIPLIER_X ) );
            rConditionalDofList.push_back( slave_node.pGetDof( VECTOR_LAGRANGE_MULTIPLIER_Y ) );
            if (dimension == 3)
            {
                rConditionalDofList.push_back( slave_node.pGetDof( VECTOR_LAGRANGE_MULTIPLIER_Z ) );
            }
        }
    }

    KRATOS_CATCH( "" );
}

//******************************* GET DOUBLE VALUE *********************************/
/***********************************************************************************/

void MortarContactCondition::GetValueOnIntegrationPoints( 
    const Variable<double>& rVariable,
    std::vector<double>& rValues,
    const ProcessInfo& rCurrentProcessInfo
    )
{
    this->CalculateOnIntegrationPoints( rVariable, rValues, rCurrentProcessInfo );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::CalculateOnIntegrationPoints( 
    const Variable<double>& rVariable,
    std::vector<double>& rOutput,
    const ProcessInfo& rCurrentProcessInfo 
    )
{
    KRATOS_TRY;

    const unsigned int number_of_integration_pts = GetGeometry( ).IntegrationPointsNumber( mThisIntegrationMethod );
    if ( rOutput.size( ) != number_of_integration_pts )
    {
        rOutput.resize( number_of_integration_pts, false );
    }

    // TODO: ADD CONTENT!!!!!

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::GetNodalDeltaMovements( 
    Vector& rValues,
    const int& rNode 
    )
{
    unsigned int dimension = GetGeometry().WorkingSpaceDimension();;

    if( rValues.size() != dimension )
    {
        rValues.resize( dimension );
    }

    rValues = ZeroVector( dimension );

    Vector CurrentValueVector = ZeroVector(3);
    CurrentValueVector = GetCurrentValue( DISPLACEMENT, CurrentValueVector, rNode );

    Vector PreviousValueVector = ZeroVector(3);
    CurrentValueVector = GetPreviousValue( DISPLACEMENT, CurrentValueVector, rNode );

    rValues[0] = CurrentValueVector[0] - PreviousValueVector[0];
    rValues[1] = CurrentValueVector[1] - PreviousValueVector[1];

    if( dimension == 3 ) 
    {
        rValues[2] = CurrentValueVector[2] - PreviousValueVector[2];
    }
}

//************************************************************************************
//************************************************************************************

Vector& MortarContactCondition::GetCurrentValue( 
    const Variable<array_1d<double,3> >& rVariable,
    Vector& rValue,
    const unsigned int& rNode
    )
{
    KRATOS_TRY;

    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();;

    array_1d<double, 3> ArrayValue;
    ArrayValue = GetGeometry( )[rNode].FastGetSolutionStepValue( rVariable );

    if( rValue.size() != dimension )
    {
        rValue.resize( dimension, false );
    }

    for( unsigned int i = 0; i < dimension; i++ )
    {
        rValue[i] = ArrayValue[i];
    }

    return rValue;

    KRATOS_CATCH( "" );
}

//************************************************************************************
//************************************************************************************

Vector& MortarContactCondition::GetPreviousValue( 
    const Variable<array_1d<double,3> >& rVariable,
    Vector& rValue,
    const unsigned int& rNode
    )
{
    KRATOS_TRY;

    const unsigned int dimension = GetGeometry().WorkingSpaceDimension();;

    array_1d<double, 3> ArrayValue;
    ArrayValue = GetGeometry( )[rNode].FastGetSolutionStepValue( rVariable, 1 );

    if( rValue.size() != dimension )
    {
        rValue.resize( dimension, false );
    }

    for( unsigned int i = 0; i < dimension; i++ )
    {
        rValue[i] = ArrayValue[i];
    }

    return rValue;

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

void MortarContactCondition::save( Serializer& rSerializer ) const
{
    KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, Condition );
}

void MortarContactCondition::load( Serializer& rSerializer )
{
    KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, Condition );
}

} // Namespace Kratos
