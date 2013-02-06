//---------------------------------------------------------------------------


#pragma hdrstop

#include "GazeUtil.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

	void TGazeUtil::inverseEyeMap(double distanceFromSceneCamToTarget, double *yIn, double *zIn) {

		double x = 0;
		double y = *yIn - (cameraWidth-1)/2; //from raw pixel coordinates to centered
		double z = (cameraHeight-1)/2 - *zIn; //from raw pixel coordinates to centered inverted

		//printf("x: %g y: %g z: %g fc: %g bx: %g by: %g bz: %g rEye: %g\n", x,y,z,fc,bx,by,bz,rEye);
		//project from camera to eye (sphere)
		double qa =  1 +y*y/fc/fc + z*z/fc/fc;
		double qb = -2*bx+2*y*by/fc+2*bz*z/fc;
		double qc = bx*bx+by*by+bz*bz-rEye*rEye;
		double uradical = qb*qb-4*qa*qc;

		//printf("qa: %g qb: %g qc: %g uradical: %g\n", qa,qb,qc,uradical);

		if(_isnan(uradical) || uradical < 0 || qa == 0.0) {
			*yIn = NAN;
			*zIn = NAN;
			return;
		}
		x = (-qb - sqrt(uradical))/(2*qa);
		y = (y)*x/fc + by;
		z = (z)*x/fc + bz;
		x = bx - x;

		//printf("x: %g y: %g z: %g\n", x,y,z);
		//rotate
		double tx = r00*x + r01*y + r02*z;
		double ty = r10*x + r11*y + r12*z;
		double tz = r20*x + r21*y + r22*z;

		//project to monitor
		if(tx == 0) {
			*yIn = NAN;
			*zIn = NAN;
			return;
		}

		x = distanceFromSceneCamToTarget;  //mm centered at scene camera axis
		y = (distanceFromSceneCamToTarget - x0Eye)*ty/tx + y0Eye;  //mm centered at scene camera axis
		z = (distanceFromSceneCamToTarget - x0Eye)*tz/tx + z0Eye;  //mm centered at scene camera axis


		//return in mm, not pixels
		*yIn = y;//1000.0 - 1000.0*y/monitorWidth;
		*zIn = z;//1000.0 - 1000.0*z/monitorHeight;

	}


	void TGazeUtil::sceneMap(double distanceFromSceneCamToTarget, double *yIn, double *zIn) {
		double fy = 557;
		double fz = 557;
		double k1 = -0.3669624087278113;
		double k2 = -0.07638290902180184;
		double k3 = 0.5764668364450144;
		double y = (*yIn)/distanceFromSceneCamToTarget;
		double z = (*zIn)/distanceFromSceneCamToTarget;
		double r2 = y*y + z*z;
		double radialDistortion =  (1 + k1*r2+k2*r2*r2+k3*r2*r2*r2);
		y = radialDistortion*y;
		z = radialDistortion*z;


		*yIn = -fy*y+(sceneCameraWidth-1)/2;// *radialDistortion;
		*zIn = -fz*z+(sceneCameraHeight-1)/2;// *radialDistortion;

		printf("yIn: %g, zIn: %g\n", *yIn, *zIn);
	}

	//Not currently used.
	void TGazeUtil::eyeToMonitorHeadFree(ublas::vector<double> eyePos, ublas::vector<double> targetPos,
			ublas::vector<double> monitorBL, ublas::vector<double> monitorBR, ublas::vector<double> monitorUL,
			double *monitorPosX, double *monitorPosY) {

		ublas::matrix<double> A(3,3);
		ublas::vector<double> b(3);

		for(int i=0; i<3; i++) {
			A(i,0) = targetPos(i) - eyePos(i);
			A(i,1) = monitorBL(i) - monitorBR(i);
			A(i,2) = monitorBL(i) - monitorUL(i);
			b(i) = monitorBL(i) - eyePos(i);
		}

		ublas::vector<double> scaleFactors = linearSolve(A,b);

		*monitorPosX = scaleFactors(1)*monitorWidth;
		*monitorPosY = scaleFactors(2)*monitorHeight;

	}


	void TGazeUtil::LoadGazeCalibration(UnicodeString fileName, TXMLDocument *xdoc_in) {
		xdoc_in->LoadFromFile(fileName);
					_di_IXMLNode node =
		xdoc_in->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("EyeCalibration");
			rEye = node->Attributes["r"];
			x0Eye = node->Attributes["x0"];
			y0Eye = node->Attributes["y0"];
			z0Eye = node->Attributes["z0"];
			a = node->Attributes["a"];
			b = node->Attributes["b"];
			g = node->Attributes["g"];
			bx = node->Attributes["bx"];
			by = node->Attributes["by"];
			bz = node->Attributes["bz"];
			fc = node->Attributes["fc"];

			double sa = sin(a);
			double sb = sin(b);
			double sg = sin(g);
			double ca = cos(a);
			double cb = cos(b);
			double cg = cos(g);


			//rotation matrix
			r00 = ca*cb;
			r10 =  ca*sb*sg - sa*cg;
			r20 =  ca*sb*cg + sa*sg;
			r01 =  sa*cb;
			r11 = sa*sb*sg + ca*cg;
			r21 = sa*sb*cg - ca*sg;
			r02 = -sb;
			r12 = cb*sg;
			r22 = cb*cg;

			node =
				xdoc_in->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("SceneCalibration");
			sceneCameraMatrix = unicodeStringToMatrix<double>(node->Attributes["cameraMatrix"]);
			sceneDistortionCoeffs = unicodeStringToVector<double>(node->Attributes["distortionCoeffs"]);


			node =
				xdoc_in->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Scaling");
			monitorWidth = node->Attributes["monitorWidth"];
			monitorHeight = node->Attributes["monitorHeight"];
			cameraWidth = node->Attributes["eyeCameraWidth"];
			cameraHeight = node->Attributes["eyeCameraHeight"];
			sceneCameraWidth = node->Attributes["sceneCameraWidth"];
			sceneCameraHeight = node->Attributes["sceneCameraHeight"];

			node =
				xdoc_in->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("ReferencePositions");
			headRef = unicodeStringToMatrix<double>(node->Attributes["head"]);
			headMarkers = unicodeStringToVector<int>(node->Attributes["headMarkers"]);
			displayRef = unicodeStringToMatrix<double>(node->Attributes["display"]);
			sceneCameraRef = unicodeStringToMatrix<double>(node->Attributes["sceneCamera"]);

			initialized = true;

		}





