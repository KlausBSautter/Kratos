//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Riccardo Rossi
//

// System includes

// External includes


// Project includes
#include "includes/define.h"
#include "custom_python/add_custom_processes_to_python.h"
#include "custom_processes/kutta_condition_process.h"
#include "custom_processes/metrics_potential_hessian_process.h"
#include "custom_processes/compute_custom_nodal_gradient_process.h"

namespace Kratos {
namespace Python {

typedef VariableComponent< VectorComponentAdaptor<array_1d<double, 3> > > ComponentType;

void  AddCustomProcessesToPython(pybind11::module& m)
{
	using namespace pybind11;

        class_<KuttaConditionProcess, KuttaConditionProcess::Pointer, Process >
        (m, "KuttaConditionProcess")
        .def(init<ModelPart&>())
        .def("Execute",&KuttaConditionProcess::Execute)
        ;

        // HESSIAN PROCESS
        py::class_<ComputePotentialHessianSolMetricProcess, ComputePotentialHessianSolMetricProcess::Pointer, Process>
        (m, "ComputePotentialHessianSolMetricProcess")
        .def(py::init<ModelPart&, Parameters>())
        ;
        m.attr("ComputePotentialHessianSolMetricProcess2D") = m.attr("ComputePotentialHessianSolMetricProcess");

        /* Historical */
        py::class_<ComputeCustomNodalGradient< ComputeCustomNodalGradientSettings::SaveAsHistoricalVariable>, ComputeCustomNodalGradient<ComputeCustomNodalGradientSettings::SaveAsHistoricalVariable>::Pointer, Process>(m,"ComputeCustomNodalGradientProcess")
        .def(py::init<ModelPart&, Variable<array_1d<double,3> >& , Variable<double>& >())
        ;

        /* Non-Historical */
        py::class_<ComputeCustomNodalGradient<ComputeCustomNodalGradientSettings::SaveAsNonHistoricalVariable>, ComputeCustomNodalGradient<ComputeCustomNodalGradientSettings::SaveAsNonHistoricalVariable>::Pointer, Process>(m,"ComputeNonHistoricalCustomNodalGradientProcess")
        .def(py::init<ModelPart&, Variable<array_1d<double,3> >& , Variable<double>& >())
        ;

}

}  // namespace Python.

} // Namespace Kratos
