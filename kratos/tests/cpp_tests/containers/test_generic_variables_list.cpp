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

#include "testing/testing.h"
#include "containers/generic_variables_list.h"
#include "containers/model.h"
#include "includes/model_part.h"
#include "includes/mpi_serializer.h"

namespace Kratos
{
namespace Testing
{

    class MyTestVisitor : public boost::static_visitor<>
    {
    public:
        //here i pass in the constructor a ModelPart reference...but it is NOt needed
        MyTestVisitor(ModelPart &rModelPart) : mrModelPart(rModelPart)
        {
        }


        //this method is good for both Variable<double> and compoents
        template <class T>
        void operator()(const T& rVar)
        {
            for (auto &node : mrModelPart.Nodes())
            {
                KRATOS_WATCH(rVar)
                auto& value = node.FastGetSolutionStepValue(rVar);
                value = 1.0;
            }
        }

        //treat specially the case of array_1d variable
        void operator()(const Variable < array_1d<double, 3> >& rVar)
        {
            for (auto &node : mrModelPart.Nodes())
            {
                auto &v = node.FastGetSolutionStepValue(rVar);
                for (unsigned int i = 0; i < 3; ++i)
                    v[i] = -1.0;
            }
        }

        //unfortunately we need to also define cases where we want to do nothing
        void operator()(const Variable < Vector >& rVar)
        {}

        void operator()(const Variable < Matrix >& rVar)
        {}

    private:
        ModelPart &mrModelPart;
    };


KRATOS_TEST_CASE_IN_SUITE(GenericVariablesList, KratosCoreFastSuite)
{
    Model current_model;
    ModelPart &mp = current_model.CreateModelPart("test");
    mp.AddNodalSolutionStepVariable(TEMPERATURE);  //not to have an empty var list
    mp.AddNodalSolutionStepVariable(DISPLACEMENT); //not to have an empty var list
    mp.AddNodalSolutionStepVariable(VELOCITY);     //not to have an empty var list

    mp.CreateNewNode(1, 1.0, 2.0, 3.0);
    mp.CreateNewNode(2, 1.0, 2.0, 3.0);
    mp.CreateNewNode(3, 1.0, 2.0, 3.0);




    //now create a generic list of variables
    GenericVariablesList vars;
    vars.AddVariable(TEMPERATURE);
    vars.AddVariable(VELOCITY);
    vars.AddVariable(DISPLACEMENT_X);
    vars.AddVariable(std::string("DISPLACEMENT_Z")); //note that we can add by the name

    //define the functor to be applied onto all the vars types
    //NOTE: this is the part to be user-defined!!
    MyTestVisitor aux(mp);
    vars.ApplyVisitor( aux );



    for (auto& rNode : mp.Nodes())
    {
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(TEMPERATURE), 1.0);
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(DISPLACEMENT_X), 1.0);
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(DISPLACEMENT_Y), 0.0); //NOT SET!!
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(DISPLACEMENT_Z), 1.0);
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(VELOCITY_X), -1.0);
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(VELOCITY_Y), -1.0); 
        KRATOS_CHECK_EQUAL(rNode.FastGetSolutionStepValue(VELOCITY_Z), -1.0);

    }
}

} // namespace Testing
} // namespace Kratos
