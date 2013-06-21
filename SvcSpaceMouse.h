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
/** \file rtSpaceMouse.h
 **	
 **/ /****************************************************************************/
#pragma once

#include <atlstr.h>

#pragma warning(disable : 4786)
#include <map>
#include <vector>

#include "nvMath.h"

#include "ISvcSpaceMouse.h"
#include "SvcFactorybase.h"

/**
 **	Implementation of CSpaceMouse
 **
 ** \remark we are using 2 ports: one for the synchrone requests, the other for asynchrone data.
 **/
class CSpaceMouse : public ISpaceMouse, virtual public ISmartRef
{
protected:
	bool isConnected;
	CComPtr<ISensor> g3DSensor;
	CComPtr<IKeyboard> g3DKeyboard;
	CComPtr<ISimpleDevice> _3DxSimpleDevice;
	__int64 gKeyStates;

	nv::matrix4f	oglMatrix;
	nv::matrix4f	oglMatrixCam;
	nv::matrix4f	oglMatrixAbs;
	nv::matrix4f	oglMatrixAbsCam;
	nv::matrix4f	dxMatrix;
	nv::matrix4f	dxMatrixAbs;
	nv::vec3f		tVec;
	nv::vec4f		tVecCam;
	nv::vec3f		tVecAbs;
	nv::vec3f		tVecAbsCam;
	nv::quaternionf	quat;
	nv::quaternionf	quatAbs;
	nv::vec3f		rAxis;
	float			rAngle;
	float			rAngleEuler[3];

	void updateTransforms(bool IgnoreZAxisForCameraMode);
public:
	/// \name Factory stuff
	//@{
    DECL_OBJECTDESC_VARIABLE();
	//DECL_NEWOBJECTFUNC(CSpaceMouse, ISpaceMouse);
	static const void *NewObjectFunc(size_t lparam1, size_t lparam2);
	//@}
	/// \name methods for Smart reference
	//@{
    IMPLSMARTREF()
    BEGINQUERYINTERFACE()
		QIMAPITF(ISpaceMouse)
    ENDQUERYINTERFACE();
	//@}
	CSpaceMouse();
	~CSpaceMouse();
	/// \name interface methods
	//@{
	virtual bool isValid(bool reinit);

	virtual void pullData(float SensitivityT, float SensitivityR, float *translation, float *rotationAxis, float *Angle);
	virtual void updateObjectTransform(float *cameraMat = NULL);
	virtual void updateCameraTransform(bool IgnoreZAxisForCameraMode);

	//
	// Object mode
	//
	virtual float* getObjectMatrix(bool bOGLMode = true, bool bAbsolute = true);
	virtual float* getObjectQuat(bool bAbsolute = true);
	virtual float* getObjectTranslation(bool bAbsolute = true);
	//
	// Camera first person mode
	//
	virtual float* getCameraMatrix(bool bOGLMode = true, bool bAbsolute = true);
	//
	// Trackball view : the camera turns around a target point
	//
	//virtual float* getPolarCameraMatrix(float radius = 1.0f, bool bOGLMode = true, bool bAbsolute = true);
	//
	// Clear data
	//
	virtual void clearObjectTransform(float *mat = NULL, float *pos = NULL, float *quatRot = NULL);
	virtual void clearCameraTransform(float *eulerAngles = NULL, float *pos = NULL);
	//@}
};

