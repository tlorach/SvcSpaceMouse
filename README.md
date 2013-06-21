SvcSpaceMouse is a dll that you can consider as a service: no linkage required and only the option of Loading the dll at runtime is possible.

This dll is meant to help you to use the 3DConnexion Space-Mouse (http://www.3dconnexion.com) in your application.
The original use of any space-mouse from 3DConnextion makes you depend on Windows COM objects. It is okay but some might like (like me) to stay away from this kind of library and certainly not add any dependencies to the project only because of an optional device.

I essentially tested it with one device and cannot tell if it is working

# How to use it

Include **ISvcSpaceMouse.h**

Declare few things:
````
nvSvc::ISvcFactory* g_pFactMouse = NULL;
ISpaceMouse *       g_pISpaceMouse = NULL;
````
* the factory found in the dll that will get loaded
* the interface with methods to query the space-mouse

Initialize things:
````
    FACTORY_LOAD(g_pFactMouse, "SvcSpaceMouse_1.0.dll");
    if(!g_pFactMouse)
    {
        FACTORY_LOAD(g_pFactMouse, "SvcSpaceMouse64_1.0.dll");
    }
    if(g_pFactMouse)
    {
        g_pISpaceMouse = (ISpaceMouse*)g_pFactMouse->CreateInstance("SpaceMouse");
    }
````

Pull data from the device:
````
    if(g_pISpaceMouse && g_pISpaceMouse->isValid())
    {
        static float v[] = {0,0,0,0,0,0,0,0,0};
        float newv[9];


        //args: float SensitivityT = 0.002f, float SensitivityR = 0.2f, float *translation = NULL, float *rotationAxis = NULL, float *Angle = NULL
        g_pISpaceMouse->pullData(0.001f/(g_bCameraMode ? 1.0f : g_scale), 0.02f, newv, newv+3, newv+6);
        if((v[0]!=newv[0])||(v[1]!=newv[1])||(v[2]!=newv[2])||
           (v[3]!=newv[3])||(v[4]!=newv[4])||(v[5]!=newv[5])||
           (v[6]!=newv[6])||(v[7]!=newv[7])||(v[8]!=newv[8]))
           bContinue = true;
    }
````

after pulling data, ask for update internal matrices for later
````
    g_pISpaceMouse->updateCameraTransform(/*IgnoreZAxisForCameraMode*/false);
````
or
````
    g_pISpaceMouse->updateObjectTransform(cameraMat);
````
cameraMat is a float cameraMat[16] that would contain the view matrix, so that the transformation gets properly done according to the view point.

You might arbitrarily gather either the object matrix or camera matrix:
````
    float* m = g_pISpaceMouse->getObjectMatrix(/*bool bOGLMode = */true, /*bool bAbsolute = */true);
    float* q = g_pISpaceMouse->getObjectQuat(/*bool bAbsolute = */true);
    float* t = g_pISpaceMouse->getObjectTranslation(/*bool bAbsolute = */true);
    float* m = g_pISpaceMouse->getCameraMatrix(/*bool bOGLMode = */true, /*bool bAbsolute = */true);
````

You can reset transformations:
```
    g_pISpaceMouse->clearObjectTransform();
    g_pISpaceMouse->clearCameraTransform();
```
