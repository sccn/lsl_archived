// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CVWRAPPER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CVWRAPPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CVWRAPPER_EXPORTS
#define CVWRAPPER_API __declspec(dllexport)
#else
#define CVWRAPPER_API __declspec(dllimport)
#endif

// This class is exported from the cvWrapper.dll
class CVWRAPPER_API CcvWrapper {
public:
	CcvWrapper(void);
	// TODO: add your methods here.
};

extern CVWRAPPER_API int ncvWrapper;

CVWRAPPER_API int fncvWrapper(void);


#ifdef __cplusplus
extern "C" {
#endif

extern CVWRAPPER_API int openCV_version();

extern CVWRAPPER_API void openCV_findPose(double **objectPoints, double **imagePoints, int nPoints, double **cameraMatrix, double *distanceCoeffs, double **rMat, double *tvec);

#ifdef __cplusplus
} /* end extern "C" */
#endif