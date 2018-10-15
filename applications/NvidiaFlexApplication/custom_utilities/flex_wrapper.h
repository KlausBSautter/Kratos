#ifndef FLEX_WRAPPER_H
#define FLEX_WRAPPER_H

#include <string.h>
#include "includes/define.h"
#include "includes/model_part.h"
#include "includes/variables.h"
#include "../../DEM_application/custom_utilities/create_and_destroy.h"

#include "NvFlex.h"
#include "NvFlexExt.h"
#include "NvFlexDevice.h"
#define CUDA_CALLABLE
#include "vec3.h"
#include "vec4.h"
#include "quat.h"

namespace Kratos {

    class KRATOS_API(NVIDIAFLEX_APPLICATION) FlexWrapper {

        public:

            KRATOS_CLASS_POINTER_DEFINITION(FlexWrapper);

            FlexWrapper(ModelPart& rSpheresModelPart, ModelPart& rFemModelPart, ParticleCreatorDestructor& rParticleCreatorDestructor);

            virtual ~FlexWrapper();

            void TransferDataFromKratosToFlex(const bool transfer_spheres, const bool transfer_walls);
            void UpdateFlex(const bool transfer_spheres, const bool transfer_walls);
            void SolveTimeSteps(double dt, int number_of_substeps);
            void SetNvFlexSolverDescParams(NvFlexSolverDesc& g_solverDesc);
            void SetNvFlexParams(NvFlexParams& g_params);
            void SetNvFlexCopyDescParams(NvFlexCopyDesc& copyDesc);
            void TransferDataFromFlexToKratos();
            bool CheckIfItsTimeToChangeGravity(const double velocity_threshold_for_gravity_change,
                                                    const double min_time_between_changes,
                                                    const double max_time_between_changes);
            void SetGravity();
            void Finalize();

            virtual std::string Info() const;
            virtual void PrintInfo(std::ostream& rOStream) const;
            virtual void PrintData(std::ostream& rOStream) const;

        protected:

            NvFlexLibrary* mFlexLibrary;
            NvFlexSolverDesc mSolverDescriptor;
            NvFlexSolver* mFlexSolver;
            NvFlexParams mFlexParameters;
            NvFlexCopyDesc mFlexCopyDescriptor;
            NvFlexVector<Vec4>* mFlexPositions;
            NvFlexVector<Vec3>* mFlexVelocities;
            NvFlexVector<int>* mFlexPhases;
            NvFlexVector<int>* mActiveIndices;
            NvFlexTriangleMeshId mFlexTriangleMesh;
            NvFlexVector<Vec4>* mFlexFemPositions;
            NvFlexVector<int>* mFlexFemConnectivities;
            unsigned int mNumberOfParticles = 1;
            unsigned int mPhaseType = 1;
            //double mDeltaTime = 0.0001;
            ModelPart& mrSpheresModelPart;
            ModelPart& mrFemModelPart;
            ParticleCreatorDestructor& mrParticleCreatorDestructor;
            NvFlexInitDesc mInitDesc;
            int mMaxparticles;
            bool mTimeToChangeGravityValue; //TODO: delete
            double mPreviousTime;

            NvFlexVector<NvFlexCollisionGeometry>* mShapeGeometry;
            NvFlexVector<Vec4>* mShapePositions;
            NvFlexVector<Quat>* mShapeRotations;
            NvFlexVector<Vec4>* mShapePrevPositions;
            NvFlexVector<Quat>* mShapePrevRotations;
            NvFlexVector<int>* mShapeFlags;

        private:

            FlexWrapper& operator=(FlexWrapper const& rOther);
    }; // Class FlexWrapper
} // namespace Kratos

#endif // FLEX_WRAPPER_H
