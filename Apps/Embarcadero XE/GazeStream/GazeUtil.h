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

		double aScene, bScene, gScene, bxScene, byScene;
		double bzScene, fcScene, kScene, ycScene, zcScene;
		double sceneCameraWidth, sceneCameraHeight;
		double r00Scene, r10Scene, r20Scene, r01Scene, r11Scene, r21Scene, r02Scene, r12Scene, r22Scene;


		ublas::matrix<double> headRef;
		ublas::matrix<double> headCur;
		ublas::vector<int> headMarkers;
		ublas::vector<double> eyePositionRef;
		ublas::matrix<double> displayRef;

		bool initialized;

		TGazeUtil() {
			rEye = 0.0; x0Eye = 0.0; y0Eye = 0.0; z0Eye = 0.0; a = 0.0; b = 0.0; g= 0.0; bx = 0.0; by = 0.0; bz = 0.0; fc = 0.0;
			monitorWidth = 1.0; monitorHeight = 1.0; cameraWidth = 1.0; cameraHeight = 1.0;
			r00 = 0.0; r10 = 0.0; r20 = 0.0; r01 = 0.0; r11 = 0.0; r21 = 0.0; r02 = 0.0; r12 = 0.0; r22 = 0.0;

			aScene = 0.0; bScene = 0.0; gScene = 0.0; bxScene = 0.0; byScene = 0.0;
			bzScene = 0.0; fcScene = 0.0; kScene = 0.0; ycScene = 0.0; zcScene = 0.0;
			sceneCameraWidth=0.0; sceneCameraHeight=0.0;
			r00Scene = 0.0; r10Scene = 0.0; r20Scene = 0.0; r01Scene = 0.0; r11Scene = 0.0; r21Scene = 0.0; r02Scene = 0.0; r12Scene = 0.0; r22Scene = 0.0;

			initialized = false;

			headRef = ublas::matrix<double>(3,4);
			headCur = ublas::matrix<double>(3,4);
			headMarkers = ublas::vector<int>(4);
			eyePositionRef = ublas::vector<double>(3);
			displayRef = ublas::matrix<double>(3,4);


		};

		~TGazeUtil() {
		};

		/**
		 * Map from eye coordinates to calibration monitor coordinates.
		 */
		void inverseEyeMap(double *yIn, double *zIn);

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
