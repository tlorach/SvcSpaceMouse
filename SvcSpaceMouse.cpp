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

/** \file SvcSpaceMouse.cpp
 **	
 **/ /****************************************************************************/
#include "stdafx.h"
#include "SvcSpaceMouse.h"

//#ifdef __cplusplus
extern "C" 
{
const char *PyWin_DLLVersionString = "SvcSpaceMouse";
}

DECLARE_FACTORY_BASICS("Spacemouse manager", SpaceMouseITF_VERSION)

OBJECT_DESCRIPTION_BEGIN(CSpaceMouse, "SpaceMouse","space mouse controller", "device") OBJECT_DESCRIPTION_END()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			REGISTER_MODULE(hModule); // to be able to use the resources
			//START_FACTORY_INIT();
			REGISTER_OBJECT_TO_FACTORY(CSpaceMouse);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
/*----------------------------------------------------------------------------------*/ /**



 **/ //----------------------------------------------------------------------------------
CSpaceMouse::CSpaceMouse()
{ 
   IMPLSMARTREFINIT
   clearObjectTransform();
   clearCameraTransform();
	isConnected = false;
   CSpaceMouse::isValid(true);
};

bool CSpaceMouse::isValid(bool reinit)
{
    //
	// Try to instanciate things
	//
   if((!isConnected) && reinit)
   {
	   HRESULT hr=::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED );
	   if (!SUCCEEDED(hr))
	   {
		  //CString strError;
		  //strError.FormatMessage (_T("Error 0x%x"), hr);
		  //::MessageBox (NULL, strError, _T("CoInitializeEx failed"), MB_ICONERROR|MB_OK);
		  return false;
	   }

		g3DSensor = NULL;
		g3DKeyboard = NULL;
		gKeyStates = 0;
	  {
		 HRESULT hr;
		 CComPtr<IUnknown> _3DxDevice;

		 // Create the device object
		 hr = _3DxDevice.CoCreateInstance(__uuidof(Device));
		 if (SUCCEEDED(hr))
		 {
			hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
			if (SUCCEEDED(hr))
			{
			   // Get the interfaces to the sensor and the keyboard;
			   g3DSensor = _3DxSimpleDevice->Sensor;
			   g3DKeyboard = _3DxSimpleDevice->Keyboard;

			   // Associate a configuration with this device
			   _3DxSimpleDevice->LoadPreferences(_T("Cube3DPolling"));
			   // Connect to the driver
			   _3DxSimpleDevice->Connect();
			   isConnected = _3DxSimpleDevice->IsConnected ? true : false;
			}
		 }
	  }
   }
   return g3DSensor ? true : false;
}
/*----------------------------------------------------------------------------------*/ /**



 **/ //----------------------------------------------------------------------------------
CSpaceMouse::~CSpaceMouse()
{ 
      {
         CComPtr<ISimpleDevice> _3DxDevice;

         // Release the sensor and keyboard interfaces
         if (g3DSensor)
         {
            g3DSensor->get_Device((IDispatch**)&_3DxDevice);
            g3DSensor.Release();
         }

         if (g3DKeyboard)
            g3DKeyboard.Release();



         if (_3DxDevice)
         {
            // Disconnect it from the driver
            _3DxDevice->Disconnect();
            _3DxDevice.Release();
         }
      }
}
/*----------------------------------------------------------------------------------*/ /**



 **/ //----------------------------------------------------------------------------------
const void *CSpaceMouse::NewObjectFunc(size_t lparam1, size_t lparam2)
{
	CSpaceMouse *pObj = new CSpaceMouse();
	return (void*)(static_cast<ISpaceMouse*>(pObj));
}
/*----------------------------------------------------------------------------------*/ /**



 **/ //----------------------------------------------------------------------------------
void CSpaceMouse::updateObjectTransform(float *cameraMat)
{
	//
	// Transform axis and translation according to current camera view
	//
	nv::matrix4f cam;
	if(cameraMat)
		cam.set_value(cameraMat);
	else
		cam = oglMatrixAbsCam;
	nv::vec4f rAxis2;
	rAxis2 = nv::vec4f(rAxis,0.0) * cam;
	nv::vec4f tVec2;
	tVec2 = nv::vec4f(tVec,0.0) * cam;

	//
	// 'Object' transformation
	//
	quat.set_value(nv::vec3f(rAxis2), rAngle * 3.14159f / 180.0f);
	quatAbs = quat * quatAbs;
	//
	// translation
	//
	tVecAbs += nv::vec3f(tVec2);

	quat.get_value(oglMatrix);
	oglMatrix.set_column(3, nv::vec4f(tVec, 1));
	quatAbs.get_value(oglMatrixAbs);
	oglMatrixAbs.set_column(3, nv::vec4f(tVecAbs, 1));
}
void CSpaceMouse::updateCameraTransform(bool IgnoreZAxisForCameraMode)
{
	//
	// Camera style transformation : Euler with z optionally off
	//
	nv::vec3f axis(rAxis);
	axis *= (rAngle * -3.14159f / 180.0f);
	rAngleEuler[0] += axis.x;
	rAngleEuler[1] += axis.y;
	if(!IgnoreZAxisForCameraMode)
	{
		rAngleEuler[2] += axis.z;
	}
	//
	// camera rotation for this instant
	//
	oglMatrixCam.set_RotOx(axis.x);
	oglMatrixCam *= nv::matrix4f().set_RotOy(axis.y);
	if(!IgnoreZAxisForCameraMode)
		oglMatrixCam *= nv::matrix4f().set_RotOz(axis.z);
	//
	// global rotation, taking into accound previous moves
	//
	oglMatrixAbsCam.set_RotOx(rAngleEuler[0]);
	oglMatrixAbsCam *= nv::matrix4f().set_RotOy(rAngleEuler[1]);
	if(!IgnoreZAxisForCameraMode)
		oglMatrixAbsCam *= nv::matrix4f().set_RotOz(rAngleEuler[2]);
	// add translation
	// camera translation
	tVecCam = nv::vec4f(tVec,1.0) * oglMatrixAbsCam;
	tVecAbsCam += nv::vec3f(tVecCam);
	oglMatrixAbsCam *= nv::matrix4f().set_translate(-tVecAbsCam);

}
/*----------------------------------------------------------------------------------*/ /**



 **/ //----------------------------------------------------------------------------------
void CSpaceMouse::pullData(float SensitivityT, float SensitivityR, float *translation, float *rotationAxis, float *Angle)
{
	bool bUpdateTransforms = false;
	if (g3DSensor)
	{
	  try {

		 CComPtr<IAngleAxis> pRotation = g3DSensor->Rotation;
		 CComPtr<IVector3D> pTranslation = g3DSensor->Translation;

		//
		// Translation
		//
		if (pTranslation->Length > 0.)
		{
			pTranslation->Length *= SensitivityT;
			tVec.x = (float)pTranslation->X;
			tVec.y = (float)pTranslation->Y;
			tVec.z = (float)pTranslation->Z;
			bUpdateTransforms = true;
		} else 
		{
			tVec = 0.0f;
		}
		if(translation)
		{
			translation[0] = tVec.x;
			translation[1] = tVec.y;
			translation[2] = tVec.z;
		}
		//
		// Rotation
		//
		if (pRotation->Angle > 0.)
		{
			pRotation->Angle *= SensitivityR;
			rAngle = (float)pRotation->Angle;
			rAxis.x = (float)pRotation->X;
			rAxis.y = (float)pRotation->Y;
			rAxis.z = (float)pRotation->Z;
			//rAxis = normalize(rAxis);
			bUpdateTransforms = true;
		}
		else
		{
			rAxis = 0.0f;
			rAngle = 0.0f;
		}
		if(rotationAxis)
		{
			rotationAxis[0] = rAxis.x;
			rotationAxis[1] = rAxis.y;
			rotationAxis[2] = rAxis.z;
		}
		if(Angle)
			Angle[0] = rAngle;

		pRotation.Release();
		pTranslation.Release();

		}
		catch (...)
		{
		 // Some sort of exception handling
		}
		}
#if 0
	if (g3DKeyboard)
	{
      // Check if any change to the keyboard state
      try {
         long nKeys;
         nKeys = g3DKeyboard->Keys;
         long i;
         for (i=1; i<=nKeys; i++)
         {
            __int64 mask = (__int64)1<<(i-1);
            VARIANT_BOOL isPressed;
            isPressed = g3DKeyboard->IsKeyDown(i);
            if (isPressed == VARIANT_TRUE)
            {
               if (!(gKeyStates & mask))
               {
                  gKeyStates |= mask;
                  //ExecuteKeyFunction (i);
               }
            }
            else
            {
               gKeyStates &= ~mask;
            }
         }
         // Test the special keys
         for (i=30; i<=31; i++)
         {
            __int64 mask = (__int64)1<<(i-1);
            VARIANT_BOOL isPressed;
            isPressed = g3DKeyboard->IsKeyDown(i);
            if (isPressed == VARIANT_TRUE)
            {
               if (!(gKeyStates & mask))
               {
                  gKeyStates |= mask;
                  //ExecuteKeyFunction (i);
               }
            }
            else
            {
               gKeyStates &= ~mask;
            }
         }
      }
      catch (...) {
         // Some sort of exception handling
      }
	}
#endif
}

//
// Object mode
//
float* CSpaceMouse::getObjectMatrix(bool bOGLMode, bool bAbsolute)
{
	if(bAbsolute)
		return oglMatrixAbs._array;
	else
		return oglMatrix._array;
}
float* CSpaceMouse::getObjectQuat(bool bAbsolute)
{
	if(bAbsolute)
		return quatAbs._array;
	else
		return quat._array;
}
float* CSpaceMouse::getObjectTranslation(bool bAbsolute)
{
	if(bAbsolute)
		return tVecAbs._array;
	else
		return tVec._array;
	return NULL;
}
//
// Camera first person mode
//
float* CSpaceMouse::getCameraMatrix(bool bOGLMode, bool bAbsolute)
{
	if(bAbsolute)
		return oglMatrixAbsCam._array;
	else
		return oglMatrixCam._array;
}
//
// Clear data
//
void CSpaceMouse::clearObjectTransform(float *mat, float *pos, float *quatRot)
{
	oglMatrix.make_identity();
	dxMatrix.make_identity();
	oglMatrixAbs.make_identity();
	dxMatrixAbs.make_identity();
	tVec = nv::vec3f(0.0f);
	if(pos)
	{
		tVecAbs = nv::vec3f(pos);
	} else
	{
		tVecAbs = nv::vec3f(0.0f);
	}
	rAxis = nv::vec3f(0.0f);
	rAngle = 0;
	if(quatRot)
	{
		quatAbs = nv::quaternionf(quatRot);
	} else 
	{
		quatAbs = nv::quaternionf(0.0f,0.0f,0.0f,1.0f);
	}
	quat = nv::quaternionf(0.0f,0.0f,0.0f,1.0f);

	updateObjectTransform();
	updateCameraTransform(true);
}
void CSpaceMouse::clearCameraTransform(float *eulerAngles, float *pos)
{
	if(eulerAngles)
	{
		rAngleEuler[0] = eulerAngles[0];
		rAngleEuler[1] = eulerAngles[1];
		rAngleEuler[2] = eulerAngles[2];
	}
	else {
		rAngleEuler[0] = 0.0;
		rAngleEuler[1] = 0.0;
		rAngleEuler[2] = 0.0;
	}
	if(pos)
	{
		tVecAbsCam = nv::vec3f(pos);
	} else
	{
		tVecAbsCam = nv::vec3f(0.0f);
	}
	tVecCam = nv::vec4f(0.0f);
	oglMatrixAbsCam.make_identity();
}

