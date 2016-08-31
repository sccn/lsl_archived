/**
Utility class for eye tracking. Handles loading of calibrations, as well as
mapping from one coordinate system to another.

Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD, 2011
*/

#ifndef GazeUtilH
#define GazeUtilH

#include "LinearAlgebra.h"
#include <vcl.h>
#pragma hdrstop

#include "XMLIntf.hpp"
#include "XMLDoc.hpp"
#include "math.h"

#include "math_util.h"


class TGazeUtil {
	public:
		//calibration parameters
		double rEye, x0Eye, y0Eye, z0Eye, a, b, g, bx, by, bz, fc;
		double monitorWidth, monitorHeight, cameraWidth, cameraHeight;
		double r00, r10, r20, r01, r11, r21, r02, r12, r22;
		double distToTarget;

		double sceneCameraWidth, sceneCameraHeight;

		ublas::matrix<double> sceneCameraMatrix;
		ublas::vector<double> sceneDistortionCoeffs;

		ublas::matrix<double> headRef;
		ublas::matrix<double> headCur;
		ublas::vector<int> headMarkers;
		ublas::matrix<double> displayRef;
		ublas::matrix<double> sceneCameraRef;

		bool initialized;

		TGazeUtil() {
			rEye = 0.0; x0Eye = 0.0; y0Eye = 0.0; z0Eye = 0.0; a = 0.0; b = 0.0; g= 0.0; bx = 0.0; by = 0.0; bz = 0.0; fc = 0.0; distToTarget = 0.0;
			monitorWidth = 1.0; monitorHeight = 1.0; cameraWidth = 1.0; cameraHeight = 1.0;
			r00 = 0.0; r10 = 0.0; r20 = 0.0; r01 = 0.0; r11 = 0.0; r21 = 0.0; r02 = 0.0; r12 = 0.0; r22 = 0.0;

			sceneCameraWidth=0.0; sceneCameraHeight=0.0;

			sceneCameraMatrix = ublas::matrix<double>(3,3);
			sceneDistortionCoeffs = ublas::vector<double>(5);

			headRef = ublas::matrix<double>(3,4);
			headCur = ublas::matrix<double>(3,4);
			headMarkers = ublas::vector<int>(4);
			displayRef = ublas::matrix<double>(3,4);
			sceneCameraRef = ublas::matrix<double>(3,4);

			initialized = false;

		};

		~TGazeUtil() {
		};

		/**
		 * Map from eye coordinates to calibration monitor coordinates.
		 */
		void inverseEyeMap(double *yIn, double *zIn);

		void eyeMap(double sceneX, double sceneY, double sceneZ);

		/**
		 * Map from eye coordinates to scene coordinates.
		 */
		void sceneMap(double *yIn, double *zIn);

		/**
		 * Map from eye coordinates to monitor coordinates, when the head is free to move.
		 * Target position is solved elsewhere, and is in the plane of the monitor.
		 */
		void eyeToMonitorHeadFree(ublas::vector<double> eyePos, ublas::vector<double> targetPos,
				ublas::vector<double> monitorBL, ublas::vector<double> monitorBR, ublas::vector<double> monitorUL,
				double *monitorPosX, double *monitorPosY);

		/**
		 * Load calibration parameters from xml file.
		 */
		void LoadGazeCalibration(UnicodeString fileName, TXMLDocument *xdoc_in);
};
//---------------------------------------------------------------------------
#endif

