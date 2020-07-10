from __future__ import print_function, absolute_import, division
import KratosMultiphysics
import numpy as np

import KratosMultiphysics.KratosUnittest as KratosUnittest
from .MainKratosROM import TestConvectionDiffusionTransientROM
import KratosMultiphysics.kratos_utilities as kratos_utilities

class ROMDynamicConvDiff(KratosUnittest.TestCase):
#########################################################################################

    def test_ConvDiff_Dynamic_ROM_2D(self):

        with KratosUnittest.WorkFolderScope(".", __file__):
            with open("ProjectParameters.json",'r') as parameter_file:
                parameters = KratosMultiphysics.Parameters(parameter_file.read())
            model = KratosMultiphysics.Model()
            Simulation = TestConvectionDiffusionTransientROM(model,parameters)
            Simulation.Run()
            ObtainedOutput = Simulation.EvaluateQuantityOfInterest()
            ExpectedOutput = np.load('ExpectedOutput2.npy')
            NodalArea = Simulation.EvaluateQuantityOfInterest2()
            print(NodalArea)
            for i in range (np.shape(ObtainedOutput)[1]):
                L2 = np.sqrt(      (sum(NodalArea*((1 - ObtainedOutput[:,i]/ExpectedOutput[:,i] )**2)))  /     (sum(NodalArea))      )*100
                print(L2)
                self.assertLess(L2, 1e-12) #percent
            # Cleaning
            kratos_utilities.DeleteDirectoryIfExisting("__pycache__")



##########################################################################################

if __name__ == '__main__':
    KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)
    KratosUnittest.main()
