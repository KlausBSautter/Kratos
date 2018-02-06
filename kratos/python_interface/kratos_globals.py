from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7

class KratosGlobals:

    def __init__(self, ThisKernel, ThisCaller, ApplicationsRoot):
        self.__dict__["Kernel"] = ThisKernel
        self.__dict__["RequestedApplications"] = dict()
        self.__dict__["AuthorizedCaller"] = ThisCaller
        self.__dict__["ApplicationsRoot"] = ApplicationsRoot
        self.__dict__["ApplicationsInterfaceIsDeprecated"] = False

    def __setattr__(self, name, value):
        if name in self.__dict__:
            # self.__dict__[name] = value
            print("Ignoring request to set KratosGlobals attribute", name)
        else:
            print("Ignoring request to set unknown KratosGlobals attribute:", name)

    def echo(self):
        print("Kernel:", self.Kernel)
        print("RequestedApplications:", self.RequestedApplications)
        print("Main Python script:", self.AuthorizedCaller)
        print("Kratos Applications base folder:", self.ApplicationsRoot)
        return

    def GetVariable(self,VarName):

        kernel = self.Kernel

        if kernel.HasDoubleVariable(VarName):
            return kernel.GetDoubleVariable(VarName)
        elif kernel.HasArrayVariable(VarName):
            return kernel.GetArrayVariable(VarName)
        elif kernel.HasVariableComponent(VarName):
            return kernel.GetVariableComponent(VarName)
        elif kernel.HasBoolVariable(VarName):
            return kernel.GetBoolVariable(VarName)
        elif kernel.HasIntVariable(VarName):
            return kernel.GetIntVariable(VarName)
        elif kernel.HasUnsignedIntVariable(VarName):
            return kernel.GetUnsignedIntVariable(VarName)
        elif kernel.HasVectorVariable(VarName):
            return kernel.GetVectorVariable(VarName)
        elif kernel.HasMatrixVariable(VarName):
            return kernel.GetMatrixVariable(VarName)
        elif kernel.HasStringVariable(VarName):
            return kernel.GetStringVariable(VarName)
        elif kernel.HasFlagsVariable(VarName):
            return kernel.GetFlagsVariable(VarName)
        elif kernel.HasVariableData(VarName):
            raise ValueError("\nKernel.GetVariable() ERROR: Variable {0} is defined but is of unsupported type\n".format(VarName))
        else:
            raise ValueError("\nKernel.GetVariable() ERROR: Variable {0} is unknown. Maybe you need to import the application where it is defined?\n".format(VarName))
        
    def HasVariable(self,VarName):

        kernel = self.Kernel

        if kernel.HasDoubleVariable(VarName):
            return True
        elif kernel.HasArrayVariable(VarName):
            return True
        elif kernel.HasVariableComponent(VarName):
            return True
        elif kernel.HasBoolVariable(VarName):
            return True
        elif kernel.HasIntVariable(VarName):
            return True
        elif kernel.HasUnsignedIntVariable(VarName):
            return True
        elif kernel.HasVectorVariable(VarName):
            return True
        elif kernel.HasMatrixVariable(VarName):
            return True
        elif kernel.HasStringVariable(VarName):
            return True
        elif kernel.HasFlagsVariable(VarName):
            return True
        elif kernel.HasVariableData(VarName):
            raise True
        else:
            return False

    def GetConstitutiveLaw(self, ConstitutiveLawName):

        kernel = self.Kernel

        if kernel.HasConstitutiveLaw(ConstitutiveLawName):
            return kernel.GetConstitutiveLaw(ConstitutiveLawName)
        else:
            raise ValueError("\nKernel.GetConstitutiveLaw() ERROR: ConstitutiveLaw {0} is unknown. Maybe you need to import the application where it is defined?\n".format(ConstitutiveLawName))

    def HasConstitutiveLaw(self, ConstitutiveLawName):

        kernel = self.Kernel

        if kernel.HasConstitutiveLaw(ConstitutiveLawName):
            return True
        else:
            return False

