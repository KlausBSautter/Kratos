//  KRATOS  _____________
//         /  _/ ____/   |
//         / // / __/ /| |
//       _/ // /_/ / ___ |
//      /___/\____/_/  |_| Application

// System includes
#include "includes/variables.h"

// External includes

// Project includes
#include "truss_element.h"
#include "iga_application_variables.h"

namespace Kratos {
///@name Degrees of freedom
///@{

void TrussElement::EquationIdVector(
    EquationIdVectorType& rResult,
    const ProcessInfo& rCurrentProcessInfo
    ) const
{
    const auto& r_geometry = GetGeometry();
    const SizeType number_of_control_points = r_geometry.size();

    if (rResult.size() != 3 * number_of_control_points)
        rResult.resize(3 * number_of_control_points, false);

    const IndexType pos = r_geometry[0].GetDofPosition(DISPLACEMENT_X);

    for (IndexType i = 0; i < number_of_control_points; ++i) {
        const IndexType index = i * 3;
        rResult[index]     = r_geometry[i].GetDof(DISPLACEMENT_X, pos).EquationId();
        rResult[index + 1] = r_geometry[i].GetDof(DISPLACEMENT_Y, pos + 1).EquationId();
        rResult[index + 2] = r_geometry[i].GetDof(DISPLACEMENT_Z, pos + 2).EquationId();
    }
};

void TrussElement::GetDofList(
    DofsVectorType& rElementalDofList,
    const ProcessInfo& rCurrentProcessInfo
    ) const
{
    const auto& r_geometry = GetGeometry();
    const SizeType number_of_control_points = r_geometry.size();

    rElementalDofList.resize(3 * number_of_control_points);

    for (IndexType i = 0; i < number_of_control_points; ++i) {
        const IndexType index = i * 3;
        rElementalDofList[index]     = r_geometry[i].pGetDof(DISPLACEMENT_X);
        rElementalDofList[index + 1] = r_geometry[i].pGetDof(DISPLACEMENT_Y);
        rElementalDofList[index + 2] = r_geometry[i].pGetDof(DISPLACEMENT_Z);
    }
};

///@}
///@name Analysis stages
///@{

void TrussElement::Initialize(const ProcessInfo& rCurrentProcessInfo)
{
    mReferenceBaseVector = GetActualBaseVector(0);
}

array_1d<double, 3> TrussElement::GetActualBaseVector(
    IndexType IntegrationPointIndex) const
{
    const auto& r_geometry = GetGeometry();
    const Matrix& DN_De = r_geometry.ShapeFunctionDerivatives(1, IntegrationPointIndex);

    array_1d<double, 3> actual_base_vector = ZeroVector(3);

    for (IndexType i = 0; i < r_geometry.size(); i++)
    {
        actual_base_vector[0] += DN_De(0, i) * r_geometry[i].X();
        actual_base_vector[1] += DN_De(0, i) * r_geometry[i].Y();
        actual_base_vector[2] += DN_De(0, i) * r_geometry[i].Z();
    }

    return actual_base_vector;
}

void TrussElement::CalculateAll(
    MatrixType& rLeftHandSideMatrix,
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo,
    const bool ComputeLeftHandSide,
    const bool ComputeRightHandSide)
{
    KRATOS_TRY;

    const auto& r_geometry = GetGeometry();

    // get integration data
    auto& r_integration_points = r_geometry.IntegrationPoints();
    const double& integration_weight = r_integration_points[0].Weight();
    const Matrix& shape_derivatives = r_geometry.ShapeFunctionDerivatives(1, 0);

    // get properties
    const auto& properties = GetProperties();

    const double E = properties[YOUNG_MODULUS];
    const double A = properties[CROSS_AREA];
    const double prestress = (GetProperties().Has(PRESTRESS_CAUCHY))
        ? properties[PRESTRESS_CAUCHY]
        : 0.0;

    // compute base vectors
    const array_1d<double, 3> actual_base_vector = GetActualBaseVector(0);

    const double reference_a = norm_2(mReferenceBaseVector);
    const double actual_a = norm_2(actual_base_vector);

    const double actual_aa = actual_a * actual_a;
    const double reference_aa = reference_a * reference_a;

    // green-lagrange strain
    const double e11_membrane = 0.5 * (actual_aa - reference_aa);

    // normal force
    const double s11_membrane = prestress * A + e11_membrane * A * E /
        reference_aa;

    for (IndexType r = 0; r < 3; r++) {
        const IndexType dof_type_r = r % 3;
        const IndexType shape_index_r = r / 3;

        const double epsilon_var_r = actual_base_vector[dof_type_r] *
            shape_derivatives(0, shape_index_r) / reference_aa;

        if (ComputeLeftHandSide) {
            for (IndexType s = 0; s < 3; s++) {
                const IndexType dof_type_s = s % 3;
                const IndexType shape_index_s = s / 3;

                const double epsilon_var_s =
                    actual_base_vector[dof_type_s] *
                    shape_derivatives(0, shape_index_s) / reference_aa;

                rLeftHandSideMatrix(r, s) = E * A * epsilon_var_r *
                    epsilon_var_s;

                if (dof_type_r == dof_type_s) {
                    const double epsilon_var_rs =
                        shape_derivatives(0, shape_index_r) *
                        shape_derivatives(0, shape_index_s) / reference_aa;

                    rLeftHandSideMatrix(r, s) += s11_membrane * epsilon_var_rs;
                }
            }
        }

        if (ComputeRightHandSide) {
            rRightHandSideVector[r] = -s11_membrane * epsilon_var_r;
        }
    }

    if (ComputeLeftHandSide) {
        rLeftHandSideMatrix *= reference_a * integration_weight;
    }

    if (ComputeRightHandSide) {
        rRightHandSideVector *= reference_a * integration_weight;
    }
    KRATOS_WATCH("here something? CalculateAll end")

    KRATOS_CATCH("")
}

///@}
///@name Internal functions
///@{

/// Computes tengent E
Vector TrussElement::ComputeTangentModulus(
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    auto& r_integration_points = r_geometry.IntegrationPoints();
    SizeType num_integration_points = r_integration_points.size();
    Vector tangent_moduli = ZeroVector(num_integration_points);
    Vector green_lagrange_strains = ZeroVector(num_integration_points);
    CalculateGreenLagrangeStrain(green_lagrange_strains);
    for (IndexType i = 0; i < num_integration_points; ++i) {
        //Vector strain_vector = ZeroVector(mpConstitutiveLaw->GetStrainSize());
        //strain_vector[0] = green_lagrange_strains[i];

        //ConstitutiveLaw::Parameters Values(GetGeometry(), GetProperties(), rCurrentProcessInfo);
        //Values.SetStrainVector(strain_vector);

        //mpConstitutiveLaw->CalculateValue(Values, TANGENT_MODULUS, tangent_moduli[i]);
    }
    return tangent_moduli;
}

void TrussElement::CalculateGreenLagrangeStrain(Vector& rGreenLagrangeVector) const
{
    const auto& r_geometry = GetGeometry();
    auto& r_integration_points = r_geometry.IntegrationPoints();
    SizeType num_integration_points = r_integration_points.size();
    if (rGreenLagrangeVector.size() != num_integration_points) {
        rGreenLagrangeVector.resize(num_integration_points);
    }
    Vector determinants_of_jacobian;
    r_geometry.DeterminantOfJacobian(determinants_of_jacobian);
    for (IndexType i = 0; i < num_integration_points; ++i) {
        const double l = r_integration_points[i].Weight() * determinants_of_jacobian[i];
        const double L = r_integration_points[i].Weight();
        rGreenLagrangeVector[i] = ((l * l - L * L) / (2.00 * L * L));
    }
}

///@}
} // namespace Kratos