//---------------------------------------------------------------------------


#pragma hdrstop

#include "GazeUtil.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

	void TGazeUtil::inverseEyeMap(double *yIn, double *zIn) {

		double x = 0.0;
		double y = *yIn - cameraWidth/2;
		double z = cameraHeight/2 - *zIn;


		//project from camera to eye (sphere)
		double qa =  1 +y*y/fc/fc + z*z/fc/fc;
		double qb = -2*bx+2*y*by/fc+2*bz*z/fc;
		double qc = bx*bx+by*by+bz*bz-rEye*rEye;
		double uradical = qb*qb-4*qa*qc;


		if(_isnan(uradical) || uradical < 0 || qa == 0.0) {
			*yIn = NAN;
			*zIn = NAN;
			return;
		}
		x = (-qb - sqrt(uradical))/(2*qa);
		y = (y)*x/fc + by;
		z = (z)*x/fc + bz;
		x = bx - x;


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

		y = x0Eye*ty/tx + y0Eye;
		z = x0Eye*tz/tx + z0Eye;
		x = x0Eye;

		//return in mm, not pixels
		*yIn = y;//1000.0 - 1000.0*y/monitorWidth;
		*zIn = z;//1000.0 - 1000.0*z/monitorHeight;

	}


	void TGazeUtil::sceneMap(double *yIn, double *zIn) {

		double x = 0.0;

		double y = *yIn;//still in mm
		double z = *zIn;//still in mm

		//rotate
		double tx = r00Scene*x + r01Scene*y + r02Scene*z;
		double ty = r10Scene*x + r11Scene*y + r12Scene*z;
		double tz = r20Scene*x + r21Scene*y + r22Scene*z;

		y = (ty-byScene)*fcScene/(tx-bxScene);
		z = (tz-bzScene)*fcScene/(tx-bxScene);
		x = fcScene;

		//radial distortion
		double d2 = (y-ycScene)*(y-ycScene) + (z-zcScene)*(z-zcScene);
		*yIn = y+(y-ycScene)*kScene*d2;
		*zIn = z+(z-zcScene)*kScene*d2;
	 //	printf("yIn: %g, zIn: %g\n", *yIn, *zIn);
	}


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

			aScene = node->Attributes["a"];
			bScene = node->Attributes["b"];
			gScene = node->Attributes["g"];
			bxScene = node->Attributes["bx"];
			byScene = node->Attributes["by"];
			bzScene = node->Attributes["bz"];
			fcScene = node->Attributes["fc"];
			kScene = node->Attributes["k"];
			ycScene = node->Attributes["yc"];
			zcScene = node->Attributes["zc"];

			sa = sin(aScene);
			sb = sin(bScene);
			sg = sin(gScene);
			ca = cos(aScene);
			cb = cos(bScene);
			cg = cos(gScene);

			//rotation matrix
			r00Scene = ca*cb;
			r01Scene =  ca*sb*sg - sa*cg;
			r02Scene =  ca*sb*cg + sa*sg;
			r10Scene =  sa*cb;
			r11Scene = sa*sb*sg + ca*cg;
			r12Scene = sa*sb*cg - ca*sg;
			r20Scene = -sb;
			r21Scene = cb*sg;
			r22Scene = cb*cg;

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
			eyePositionRef = unicodeStringToVector<double>(node->Attributes["eye"]);
			displayRef = unicodeStringToMatrix<double>(node->Attributes["display"]);

			initialized = true;

		}




