// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:         BSD License
//                   license: structural_mechanics_application/license.txt
//
//  Main authors:    Riccardo Rossi
//


// System includes

// External includes

// Project includes
#include <custom_constitutive/linear_isotropic_damage_plane_strain_2d.h>
#include "custom_python/add_custom_constitutive_laws_to_python.h"

// Elastic laws
#include "custom_constitutive/truss_constitutive_law.h"
#include "custom_constitutive/truss_plasticity_constitutive_law.h"
#include "custom_constitutive/beam_constitutive_law.h"
#include "custom_constitutive/elastic_isotropic_3d.h"
#include "custom_constitutive/axisym_elastic_isotropic.h"
#include "custom_constitutive/linear_plane_stress.h"
#include "custom_constitutive/linear_plane_strain.h"
#include "custom_constitutive/elastic_isotropic_plane_stress_uncoupled_shear.h"
#include "custom_constitutive/hyper_elastic_isotropic_kirchhoff_3d.h"
#include "custom_constitutive/hyper_elastic_isotropic_kirchhoff_plane_stress_2d.h"
#include "custom_constitutive/hyper_elastic_isotropic_kirchhoff_plane_strain_2d.h"
#include "custom_constitutive/hyper_elastic_isotropic_neo_hookean_3d.h"
#include "custom_constitutive/hyper_elastic_isotropic_neo_hookean_plane_strain_2d.h"
#include "custom_constitutive/linear_elastic_orthotropic_2D_law.h"
#include "custom_constitutive/linear_j2_plasticity_3d.h"
#include "custom_constitutive/linear_j2_plasticity_plane_strain_2d.h"
#include "custom_constitutive/linear_isotropic_damage_3D_law.h"

// Plastic, damage laws and viscosities
#include "custom_constitutive/small_strain_isotropic_plasticity_factory.h"
#include "custom_constitutive/finite_strain_isotropic_plasticity_factory.h"
#include "custom_constitutive/small_strain_isotropic_damage_factory.h"
#include "custom_constitutive/viscous_generalized_maxwell.h"
#include "custom_constitutive/viscous_generalized_kelvin.h"
#include "custom_constitutive/generic_small_strain_viscoplasticity_3d.h"
#include "custom_constitutive/generic_small_strain_isotropic_plasticity.h"
#include "custom_constitutive/generic_finite_strain_isotropic_plasticity.h"
#include "custom_constitutive/generic_small_strain_isotropic_damage.h"

// Integrators
#include "custom_constitutive/constitutive_laws_integrators/generic_constitutive_law_integrator_damage.h"
#include "custom_constitutive/constitutive_laws_integrators/generic_constitutive_law_integrator_plasticity.h"
#include "custom_constitutive/constitutive_laws_integrators/generic_finite_strain_constitutive_law_integrator_plasticity.h"

/* Small strain */ // TODO: Move to independent folder and rename
// Yield surfaces
#include "custom_constitutive/yield_surfaces/generic_yield_surface.h"
#include "custom_constitutive/yield_surfaces/von_mises_yield_surface.h"
#include "custom_constitutive/yield_surfaces/modified_mohr_coulomb_yield_surface.h"
#include "custom_constitutive/yield_surfaces/rankine_yield_surface.h"
#include "custom_constitutive/yield_surfaces/simo_ju_yield_surface.h"
#include "custom_constitutive/yield_surfaces/drucker_prager_yield_surface.h"
#include "custom_constitutive/yield_surfaces/tresca_yield_surface.h"

// Plastic potentials
#include "custom_constitutive/plastic_potentials/generic_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/von_mises_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/tresca_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/modified_mohr_coulomb_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/drucker_prager_plastic_potential.h"

/* Finite strain */
// Yield surfaces
#include "custom_constitutive/yield_surfaces/finite_strain/generic_yield_surface.h"
#include "custom_constitutive/yield_surfaces/finite_strain/von_mises_yield_surface.h"
#include "custom_constitutive/yield_surfaces/finite_strain/modified_mohr_coulomb_yield_surface.h"
#include "custom_constitutive/yield_surfaces/finite_strain/rankine_yield_surface.h"
#include "custom_constitutive/yield_surfaces/finite_strain/simo_ju_yield_surface.h"
#include "custom_constitutive/yield_surfaces/finite_strain/drucker_prager_yield_surface.h"
#include "custom_constitutive/yield_surfaces/finite_strain/tresca_yield_surface.h"

// Plastic potentials
#include "custom_constitutive/plastic_potentials/finite_strain/generic_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/finite_strain/von_mises_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/finite_strain/tresca_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/finite_strain/modified_mohr_coulomb_plastic_potential.h"
#include "custom_constitutive/plastic_potentials/finite_strain/drucker_prager_plastic_potential.h"

namespace Kratos
{
namespace Python
{

using namespace pybind11;


void  AddCustomConstitutiveLawsToPython(pybind11::module& m)
{

    class_< TrussConstitutiveLaw, typename TrussConstitutiveLaw::Pointer, ConstitutiveLaw >
    (m, "TrussConstitutiveLaw").def(init<>() )
    ;

    class_< TrussPlasticityConstitutiveLaw, typename TrussPlasticityConstitutiveLaw::Pointer, ConstitutiveLaw >
    (m, "TrussPlasticityConstitutiveLaw").def(init<>() )
    ;

    class_< BeamConstitutiveLaw, typename BeamConstitutiveLaw::Pointer, ConstitutiveLaw >
    (m, "BeamConstitutiveLaw").def(init<>() )
    ;

    class_< LinearPlaneStress, typename LinearPlaneStress::Pointer, ConstitutiveLaw >
    (m, "LinearElasticPlaneStress2DLaw").def(init<>() )
    ;

    class_< LinearPlaneStrain, typename LinearPlaneStrain::Pointer, ConstitutiveLaw >
    (m, "LinearElasticPlaneStrain2DLaw").def(init<>() )
    ;

    class_< ElasticIsotropic3D, typename ElasticIsotropic3D::Pointer, ConstitutiveLaw >
    (m, "LinearElastic3DLaw").def(init<>() )
    ;

    class_< AxisymElasticIsotropic, typename AxisymElasticIsotropic::Pointer, ConstitutiveLaw >
    (m, "LinearElasticAxisym2DLaw").def(init<>() )
    ;

    class_< ElasticIsotropicPlaneStressUncoupledShear, typename ElasticIsotropicPlaneStressUncoupledShear::Pointer, ConstitutiveLaw >
    (m, "ElasticPlaneStressUncoupledShear2DLaw").def(init<>() )
    ;

    class_< HyperElasticIsotropicKirchhoff3D, typename HyperElasticIsotropicKirchhoff3D::Pointer, ConstitutiveLaw >
    (m, "KirchhoffSaintVenant3DLaw").def(init<>() )
    ;

    class_< HyperElasticIsotropicKirchhoffPlaneStress2D, typename HyperElasticIsotropicKirchhoffPlaneStress2D::Pointer, ConstitutiveLaw >
    (m, "KirchhoffSaintVenantPlaneStress2DLaw").def(init<>() )
    ;

    class_< HyperElasticIsotropicKirchhoffPlaneStrain2D, typename HyperElasticIsotropicKirchhoffPlaneStrain2D::Pointer, ConstitutiveLaw >
    (m, "KirchhoffSaintVenantPlaneStrain2DLaw").def(init<>() )
    ;

    class_< HyperElasticIsotropicNeoHookean3D, typename HyperElasticIsotropicNeoHookean3D::Pointer, ConstitutiveLaw >
    (m, "HyperElastic3DLaw").def(init<>() )
    ;

    class_< HyperElasticIsotropicNeoHookeanPlaneStrain2D, typename HyperElasticIsotropicNeoHookeanPlaneStrain2D::Pointer, ConstitutiveLaw >
    (m, "HyperElasticPlaneStrain2DLaw").def(init<>() )
    ;

    class_< LinearElasticOrthotropic2DLaw, typename LinearElasticOrthotropic2DLaw::Pointer, ConstitutiveLaw >
    (m,"LinearElasticOrthotropic2DLaw").def( init<>())
    ;

    class_< LinearJ2PlasticityPlaneStrain2D, typename LinearJ2PlasticityPlaneStrain2D::Pointer,  ConstitutiveLaw >
    (m,"LinearJ2PlasticityPlaneStrain2DLaw").def( init<>())
    ;

    class_< LinearJ2Plasticity3D, typename LinearJ2Plasticity3D::Pointer,  ConstitutiveLaw >
    (m,"LinearJ2Plasticity3DLaw").def( init<>())
    ;

    class_< LinearIsotropicDamagePlaneStrain2D, typename LinearIsotropicDamagePlaneStrain2D::Pointer,  ConstitutiveLaw  >
    (m,"LinearIsotropicDamagePlaneStrain2DLaw").def( init<>())
    ;

    class_< LinearIsotropicDamage3D, typename LinearIsotropicDamage3D::Pointer,  ConstitutiveLaw  >
    (m,"LinearIsotropicDamage3DLaw").def( init<>())
    ;
    

    class_< SmallStrainIsotropicDamageFactory, typename SmallStrainIsotropicDamageFactory::Pointer,  ConstitutiveLaw  >
    (m,"SmallStrainIsotropicDamageFactory").def( init<>())
    ;

    class_< SmallStrainIsotropicPlasticityFactory, typename SmallStrainIsotropicPlasticityFactory::Pointer,  ConstitutiveLaw  >
    (m,"SmallStrainIsotropicPlasticityFactory").def( init<>())
    ;

    class_< FiniteStrainIsotropicPlasticityFactory, typename FiniteStrainIsotropicPlasticityFactory::Pointer,  ConstitutiveLaw  >
    (m,"FiniteStrainIsotropicPlasticityFactory").def( init<>())
    ;

    class_< ViscousGeneralizedKelvin<ElasticIsotropic3D>, typename ViscousGeneralizedKelvin<ElasticIsotropic3D>::Pointer,  ConstitutiveLaw  >
    (m,"ViscousGeneralizedKelvin3D").def( init<>())
    ;

    class_< ViscousGeneralizedMaxwell<ElasticIsotropic3D>, typename ViscousGeneralizedMaxwell<ElasticIsotropic3D>::Pointer,  ConstitutiveLaw  >
    (m,"ViscousGeneralizedMaxwell3D").def( init<>())
    ;

    // Custom Constitutive Laws Registration
    // Plasticity
    /* Small strain */
    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DVonMisesVonMises").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DVonMisesModifiedMohrCoulomb").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DVonMisesDruckerPrager").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<VonMisesYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DVonMisesTresca").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DModifiedMohrCoulombVonMises").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DModifiedMohrCoulombModifiedMohrCoulomb").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DModifiedMohrCoulombDruckerPrager").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<ModifiedMohrCoulombYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DModifiedMohrCoulombTresca").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DTrescaVonMises").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DTrescaModifiedMohrCoulomb").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DTrescaDruckerPrager").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<TrescaYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DTrescaTresca").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DDruckerPragerVonMises").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DDruckerPragerModifiedMohrCoulomb").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DDruckerPragerDruckerPrager").def( init<>());

    class_< GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicPlasticity <GenericConstitutiveLawIntegratorPlasticity<DruckerPragerYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicPlasticity3DDruckerPragerTresca").def( init<>());

    /* Finite strain */
    // Kirchhoff
    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DVonMisesVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DVonMisesModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DVonMisesDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DVonMisesTresca").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DModifiedMohrCoulombVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DModifiedMohrCoulombModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DModifiedMohrCoulombDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DModifiedMohrCoulombTresca").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DTrescaVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DTrescaModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DTrescaDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DTrescaTresca").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DDruckerPragerVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DDruckerPragerModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DDruckerPragerDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicKirchhoff3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicKirchhoffPlasticity3DDruckerPragerTresca").def( init<>());

    // Neo-Hookean
    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DVonMisesVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DVonMisesModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DVonMisesDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainVonMisesYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DVonMisesTresca").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DModifiedMohrCoulombVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DModifiedMohrCoulombModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DModifiedMohrCoulombDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainModifiedMohrCoulombYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DModifiedMohrCoulombTresca").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DTrescaVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DTrescaModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DTrescaDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainTrescaYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DTrescaTresca").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainVonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DDruckerPragerVonMises").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DDruckerPragerModifiedMohrCoulomb").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainDruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DDruckerPragerDruckerPrager").def( init<>());

    class_< GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>,
    typename GenericFiniteStrainIsotropicPlasticity <HyperElasticIsotropicNeoHookean3D, GenericFiniteStrainConstitutiveLawIntegratorPlasticity<FiniteStrainDruckerPragerYieldSurface<FiniteStrainTrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"HyperElasticIsotropicNeoHookeanPlasticity3DDruckerPragerTresca").def( init<>());

    // Damage
    /* Small strain */
    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DVonMisesVonMises").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DVonMisesModifiedMohrCoulomb").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DVonMisesDruckerPrager").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<VonMisesYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DVonMisesTresca").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DModifiedMohrCoulombVonMises").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DModifiedMohrCoulombModifiedMohrCoulomb").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DModifiedMohrCoulombDruckerPrager").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<ModifiedMohrCoulombYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DModifiedMohrCoulombTresca").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DTrescaVonMises").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DTrescaModifiedMohrCoulomb").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DTrescaDruckerPrager").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<TrescaYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DTrescaTresca").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DDruckerPragerVonMises").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DDruckerPragerModifiedMohrCoulomb").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DDruckerPragerDruckerPrager").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<DruckerPragerYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DDruckerPragerTresca").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DRankineVonMises").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DRankineModifiedMohrCoulomb").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DRankineDruckerPrager").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<RankineYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DRankineTresca").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<VonMisesPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<VonMisesPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DSimoJuVonMises").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<ModifiedMohrCoulombPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DSimoJuModifiedMohrCoulomb").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<DruckerPragerPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<DruckerPragerPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DSimoJuDruckerPrager").def( init<>());

    class_<  GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<TrescaPlasticPotential<6>>>>,
    typename GenericSmallStrainIsotropicDamage <GenericConstitutiveLawIntegratorDamage<SimoJuYieldSurface<TrescaPlasticPotential<6>>>>::Pointer,
    ConstitutiveLaw >
    (m,"SmallStrainIsotropicDamage3DSimoJuTresca").def( init<>());

    class_< GenericSmallStrainViscoplasticity3D, typename GenericSmallStrainViscoplasticity3D::Pointer,  ConstitutiveLaw  >
    (m,"GenericSmallStrainViscoplasticity3D").def( init<>())
    ;
}

}  // namespace Python.
}  // namespace Kratos.
