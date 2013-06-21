/*
    Copyright (c) 2013, Tristan Lorach. All rights reserved.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
    OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Please direct any questions to lorachnroll@gmail.com (Tristan Lorach)

*/
/** \file nv_ISvcSpaceMouse.h 
 **
 **/ /***********************************************************************/
#pragma once

#define SpaceMouseITF_VERSION "1.0"

#include "ISvcBase.h"

using namespace nvSvc;

/**
    \brief Interface for Space Mouse device
 **/
class ISpaceMouse
{
public:
	DECLSMARTREF(ISpaceMouse); ///< Macro for GetRef(), AddRef(), Release() , QueryInterface() implementations

	/// \name SpaceMouse interface methods
	//@{
	virtual bool isValid(bool reinit = false) = 0; ///< test if the device is working
	virtual void pullData(float SensitivityT = 0.002f, float SensitivityR = 0.2f, float *translation = NULL, float *rotationAxis = NULL, float *Angle = NULL) = 0;///< pull data from the device
	virtual void updateObjectTransform(float *cameraMat = NULL) = 0;///< update the object transformation. \arg cameraMat to transform in the cameraMat reference
	virtual void updateCameraTransform(bool IgnoreZAxisForCameraMode = true) = 0;///<update the camera. \arg IgnoreZAxisForCameraMode to keep the camera vertical
	//
	// \name Object information
	//@{
	virtual float* getObjectMatrix(bool bOGLMode = true, bool bAbsolute = true) = 0;
	virtual float* getObjectQuat(bool bAbsolute = true) = 0;
	virtual float* getObjectTranslation(bool bAbsolute = true) = 0;
	//@}
	//
	/// Camera first person mode
	//
	virtual float* getCameraMatrix(bool bOGLMode = true, bool bAbsolute = true) = 0;
	//virtual float* getPolarCameraMatrix(float radius = 1.0f, bool bOGLMode = true, bool bAbsolute = true) = 0;
	virtual void clearObjectTransform(float *mat = NULL, float *pos = NULL, float *quatRot = NULL) = 0;///< Clears the transformation of the object
	virtual void clearCameraTransform(float *eulerAngles = NULL, float *pos = NULL) = 0;///< Clears the Camera transformation
};

/**
    \class ISpaceMouse
    Example of how to instanciate the service :
    \code
    #ifdef _DEBUG
        FACTORY_LOAD(pSpaceMouseFactory, "nv_SvcSpaceMouseD.dll");
    #else
        FACTORY_LOAD(pSpaceMouseFactory, "nv_SvcSpaceMouse.dll");
    #endif
    #endif
        if(pSpaceMouseFactory)
        {
            pISpaceMouse = (ISpaceMouse*)pSpaceMouseFactory->CreateInstance("SpaceMouse");
            if(pISpaceMouse)
            {
                pISpaceMouse->clearCameraTransform(NULL, NULL);
                pISpaceMouse->updateCameraTransform();
            }
        }
    \endcode

    Here is an example on how to use it in a rendering loop:
    \code
    if(pISpaceMouse && pISpaceMouse->isValid())
    {
        pISpaceMouse->pullData(0.001, 0.04, NULL, NULL, NULL);
        if(0)//bCameraMode)
            pISpaceMouse->updateCameraTransform();
        else
            pISpaceMouse->updateObjectTransform((float*)g_camera_manip.get_mat().mat_array);
    }
    if(pISpaceMouse)
    {
        float *m = pISpaceMouse->getCameraMatrix(true, true);
        glMultMatrixf(m);
        ...
        float *m = pISpaceMouse->getObjectMatrix(true, true);
        glMultMatrixf(m);
    }
    \endcode

 **/

