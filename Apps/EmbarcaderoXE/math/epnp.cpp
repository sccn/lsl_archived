//---------------------------------------------------------------------------


#pragma hdrstop

#include "epnp.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

 #include <iostream>
using namespace std;
//#include "precomp.hpp"
#include "epnp.h"
				   //camera matrix, objectpoints, imagepoints
epnp::epnp(ublas::matrix<double> cameraMatrix, ublas::matrix<double> opoints, ublas::matrix<double> ipoints)
{
  init_camera_parameters(cameraMatrix);

  if(opoints.size1() != 3) {
		throw new IllegalArgumentException("epnp: object ponts must have three dimenstions.\n");
  }

  if(ipoints.size1() != 2) {
		throw new IllegalArgumentException("epnp: image ponts must have two dimensions.\n");
  }

  if(opoints.size2() != ipoints.size2()) {
	  throw new IllegalArgumentException("epnp: object points and image points must have the same number of points.\n");
  }
  //number of points to compare
  number_of_correspondences = opoints.size2(); //

  pws.resize(3 * number_of_correspondences);
  us.resize(2 * number_of_correspondences);


  init_points(opoints, ipoints);

  alphas.resize(4 * number_of_correspondences);
  pcs.resize(3 * number_of_correspondences);

  max_nr = 0;
  A1 = NULL;
  A2 = NULL;
}

epnp::~epnp()
{
	if (A1)
		delete[] A1;
	if (A2)
		delete[] A2;
}

void epnp::choose_control_points(void)
{
  // Take C0 as the reference points centroid:
  cws[0][0] = cws[0][1] = cws[0][2] = 0;
  for(int i = 0; i < number_of_correspondences; i++)
	for(int j = 0; j < 3; j++)
	  cws[0][j] += pws[3 * i + j];

  for(int j = 0; j < 3; j++)
	cws[0][j] /= number_of_correspondences;


  // Take C1, C2, and C3 from PCA on the reference points:
 // CvMat * PW0 = cvCreateMat(number_of_correspondences, 3, CV_64F);
  ublas::matrix<double> PW0(number_of_correspondences, 3);

  double pw0tpw0[3 * 3], dc[3], uct[3 * 3];
  //CvMat PW0tPW0 = cvMat(3, 3, CV_64F, pw0tpw0);
  //CvMat DC      = cvMat(3, 1, CV_64F, dc);
  //CvMat UCt     = cvMat(3, 3, CV_64F, uct);

  ublas::matrix<double> UC(3,3);
  ublas::vector<double> DC(3);
  ublas::matrix<double> VC(3,3);

//  for(int i = 0; i < number_of_correspondences; i++)
//	for(int j = 0; j < 3; j++)
//	  PW0->data.db[3 * i + j] = pws[3 * i + j] - cws[0][j];
 for(unsigned int point=0; point<PW0.size1(); point++) {
	for(unsigned int dim=0; dim<PW0.size2(); dim++) {
		PW0(point,dim) = pws[3*point+dim] - cws[0][dim];
	}

  }


  //puts pwo^transpose * pow in PW0tPW0
  //cvMulTransposed(PW0, &PW0tPW0, 1);
  ublas::matrix<double> PW0tPW0 = ublas::prod(ublas::trans(PW0),PW0);

  //cvSVD(&PW0tPW0, &DC, &UCt, 0, CV_SVD_MODIFY_A | CV_SVD_U_T);
  svdcmp(PW0tPW0, UC, DC, VC);

  UC = trans(UC);

  //cvReleaseMat(&PW0);
  print("UC",UC);
  for(int i = 0; i < 3; i++) {
	//double k = sqrt(dc[i] / number_of_correspondences);
	double k = sqrt(DC(i)/number_of_correspondences);
	for(int j = 0; j < 3; j++)
	  //cws[i][j] = cws[0][j] + k * uct[3 * (i) + j];
	  cws[i][j] = cws[0][j] + k * UC(i,j);
  }
}

void epnp::compute_barycentric_coordinates(void)
{
  //double cc[3 * 3], cc_inv[3 * 3];
  //CvMat CC     = cvMat(3, 3, CV_64F, cc);
  //CvMat CC_inv = cvMat(3, 3, CV_64F, cc_inv);

  ublas::matrix<double> cc(3,3);


  for(int i = 0; i < 3; i++)
	for(int j = 1; j < 4; j++) {
	//cc[3 * i + j - 1] = cws[j][i] - cws[0][i];
	  cc(i,j-1) = cws[j][i] - cws[0][i];
      printf("cws[%d][%d]: %g\n", j,i,cws[j][i]);
	  }

  print("cc",cc);
  return;
  //cvInvert(&CC, &CC_inv, CV_SVD);
  ublas::matrix<double> cc_inv = inverse(cc);

  //double * ci = cc_inv;
  for(int i = 0; i < number_of_correspondences; i++) {
    double * pi = &pws[0] + 3 * i;
    double * a = &alphas[0] + 4 * i;

	for(int j = 0; j < 3; j++)  {
//	  a[1 + j] =
//		  ci[3 * j    ] * (pi[0] - cws[0][0]) +
//		  ci[3 * j + 1] * (pi[1] - cws[0][1]) +
//		  ci[3 * j + 2] * (pi[2] - cws[0][2]);
			  a[1+j] =
			cc_inv(j,0) * (pi[0] - cws[0][0]) +
			cc_inv(j,1) * (pi[1] - cws[0][1]) +
			cc_inv(j,2) * (pi[2] - cws[0][2]);

		}
		a[0] = 1.0f - a[1] - a[2] - a[3];

  }
}


	void epnp::fill_M(ublas::matrix<double> &M,
	  const int row, const double * as, const double u, const double v)
{
  //double * M1 = M->data.db + row * 12;      //start of row
  //double * M2 = M1 + 12;                    //start of row+1

  for(int j = 0; j < 4; j++) {
 //	M1[3 * j    ] = as[j] * fu;
 //	M1[3 * j + 1] = 0.0;
 //	M1[3 * j + 2] = as[j] * (uc - u);

 //	M2[3 * j    ] = 0.0;
 //	M2[3 * j + 1] = as[j] * fv;
 //	M2[3 * j + 2] = as[j] * (vc - v);

	M(row,3 * j) = as[j] * fu;
	M(row,3 * j + 1) = 0.0;
	M(row,3 * j + 2) = as[j] * (uc - u);

	M(row+1,3 * j    ) = 0.0;
	M(row+1,3 * j + 1) = as[j] * fv;
	M(row+1,3 * j + 2) = as[j] * (vc - v);
  }
}


void epnp::compute_ccs(const double * betas, ublas::matrix<double> ut)
{
  for(int i = 0; i < 4; i++)
    ccs[i][0] = ccs[i][1] = ccs[i][2] = 0.0f;

  for(int i = 0; i < 4; i++) {
   // const double * v = ut + 12 * (11 - i);
	for(int j = 0; j < 4; j++)
	  for(int k = 0; k < 3; k++)
//  ccs[j][k] += betas[i] * v[3 * j + k];
 ccs[j][k] += betas[i] * ut(11-i,3 * j + k);

  }
}

void epnp::compute_pcs(void)
{
  for(int i = 0; i < number_of_correspondences; i++) {
    double * a = &alphas[0] + 4 * i;
    double * pc = &pcs[0] + 3 * i;

    for(int j = 0; j < 3; j++)
      pc[j] = a[0] * ccs[0][j] + a[1] * ccs[1][j] + a[2] * ccs[2][j] + a[3] * ccs[3][j];
  }
}

void epnp::compute_pose(ublas::matrix<double> & R, ublas::vector<double>& t)
{

  choose_control_points();

  compute_barycentric_coordinates();
  return;
  //CvMat * M = cvCreateMat(2 * number_of_correspondences, 12, CV_64F);
  ublas::matrix<double> M(2*number_of_correspondences, 12);
  for(int i = 0; i < number_of_correspondences; i++)
	fill_M(M, 2 * i, &alphas[0] + 4 * i, us[2 * i], us[2 * i + 1]);



//  double mtm[12 * 12], d[12], ut[12 * 12];
//  CvMat MtM = cvMat(12, 12, CV_64F, mtm);
//  CvMat D   = cvMat(12,  1, CV_64F, d);
//  CvMat Ut  = cvMat(12, 12, CV_64F, ut);

  ublas::vector<double> D(12);
  ublas::matrix<double> Ut(12,12);
  ublas::matrix<double>VC(12,12);


  //cvMulTransposed(M, &MtM, 1);
  ublas::matrix<double> MtM = ublas::prod(ublas::trans(M),M);

  //cvSVD(&MtM, &D, &Ut, 0, CV_SVD_MODIFY_A | CV_SVD_U_T);
  svdcmp(MtM, Ut, D, VC);
  Ut = trans(Ut);


//  cvReleaseMat(&M);

  //double l_6x10[6 * 10], rho[6];
  //CvMat L_6x10 = cvMat(6, 10, CV_64F, l_6x10);
  //CvMat Rho    = cvMat(6,  1, CV_64F, rho);
  ublas::matrix<double> L_6x10(6,10);
  ublas::vector<double> Rho(6);

  //compute_L_6x10(ut, l_6x10);
  compute_L_6x10(Ut, L_6x10);
  compute_rho(Rho);

  double Betas[4][4], rep_errors[4];
  double Rs[4][3][3], ts[4][3];

  //find_betas_approx_1(&L_6x10, &Rho, Betas[1]);
  find_betas_approx_1(L_6x10, Rho, Betas[1]);
  //gauss_newton(L_6x10, Rho, Betas[1]);
  gauss_newton(L_6x10, Rho, Betas[1]);
  rep_errors[1] = compute_R_and_t(Ut, Betas[1], Rs[1], ts[1]);

  //find_betas_approx_2(&L_6x10, &Rho, Betas[2]);
  find_betas_approx_2(L_6x10, Rho, Betas[2]);
  //gauss_newton(&L_6x10, &Rho, Betas[2]);
  gauss_newton(L_6x10, Rho, Betas[2]);
  rep_errors[2] = compute_R_and_t(Ut, Betas[2], Rs[2], ts[2]);

  //find_betas_approx_3(&L_6x10, &Rho, Betas[3]);
  find_betas_approx_3(L_6x10, Rho, Betas[3]);
  //gauss_newton(&L_6x10, &Rho, Betas[3]);
  gauss_newton(L_6x10, Rho, Betas[3]);
  rep_errors[3] = compute_R_and_t(Ut, Betas[3], Rs[3], ts[3]);

  int N = 1;
  if (rep_errors[2] < rep_errors[1]) N = 2;
  if (rep_errors[3] < rep_errors[N]) N = 3;

  //cv::Mat(3, 1, CV_64F, ts[N]).copyTo(t);
  //cv::Mat(3, 3, CV_64F, Rs[N]).copyTo(R);
  for(int i=0; i<3; i++) {
	t(i) = ts[N][i];
	for(int j=0; j<3; j++) {
    	R(i,j) = Rs[N][i][j];
	}
  }
}

void epnp::copy_R_and_t(const double R_src[3][3], const double t_src[3],
	  double R_dst[3][3], double t_dst[3])
{
  for(int i = 0; i < 3; i++) {
	for(int j = 0; j < 3; j++)
	  R_dst[i][j] = R_src[i][j];
	t_dst[i] = t_src[i];
  }
}

double epnp::dist2(const double * p1, const double * p2)
{
  return
	(p1[0] - p2[0]) * (p1[0] - p2[0]) +
	(p1[1] - p2[1]) * (p1[1] - p2[1]) +
	(p1[2] - p2[2]) * (p1[2] - p2[2]);
}

double epnp::dot(const double * v1, const double * v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void epnp::estimate_R_and_t(double R[3][3], double t[3])
{
  double pc0[3], pw0[3];

  pc0[0] = pc0[1] = pc0[2] = 0.0;
  pw0[0] = pw0[1] = pw0[2] = 0.0;

  for(int i = 0; i < number_of_correspondences; i++) {
    const double * pc = &pcs[3 * i];
    const double * pw = &pws[3 * i];

    for(int j = 0; j < 3; j++) {
      pc0[j] += pc[j];
      pw0[j] += pw[j];
    }
  }
  for(int j = 0; j < 3; j++) {
    pc0[j] /= number_of_correspondences;
    pw0[j] /= number_of_correspondences;
  }

//  double abt[3 * 3], abt_d[3], abt_u[3 * 3], abt_v[3 * 3];
//  CvMat ABt   = cvMat(3, 3, CV_64F, abt);
//  CvMat ABt_D = cvMat(3, 1, CV_64F, abt_d);
//  CvMat ABt_U = cvMat(3, 3, CV_64F, abt_u);
//  CvMat ABt_V = cvMat(3, 3, CV_64F, abt_v);
//
//  cvSetZero(&ABt);
	ublas::matrix<double> abt(3,3);
	abt*=0;
	ublas::vector<double> abt_d(3);
	ublas::matrix<double> abt_u(3,3);
	ublas::matrix<double> abt_v(3,3);

  for(int i = 0; i < number_of_correspondences; i++) {
	double * pc = &pcs[3 * i];
	double * pw = &pws[3 * i];

	for(int j = 0; j < 3; j++) {
//	  abt[3 * j    ] += (pc[j] - pc0[j]) * (pw[0] - pw0[0]);
//	  abt[3 * j + 1] += (pc[j] - pc0[j]) * (pw[1] - pw0[1]);
//	  abt[3 * j + 2] += (pc[j] - pc0[j]) * (pw[2] - pw0[2]);
	  abt(j,0) += (pc[j] - pc0[j]) * (pw[0] - pw0[0]);
	  abt(j,1) += (pc[j] - pc0[j]) * (pw[1] - pw0[1]);
	  abt(j,2) += (pc[j] - pc0[j]) * (pw[2] - pw0[2]);

	}
  }

  //cvSVD(&ABt, &ABt_D, &ABt_U, &ABt_V, CV_SVD_MODIFY_A);
  svdcmp(abt, abt_u, abt_d, abt_v);


  for(int i = 0; i < 3; i++) {
	for(int j = 0; j < 3; j++) {
	  //R[i][j] = dot(abt_u + 3 * i, abt_v + 3 * j);
	  R[i][j] = 0;
	  for(int k=0; k<3; k++) {
      	R[i][j] += abt_u(i,k)*abt_v(j,k);
	  }
	}
  }

  const double det =
	R[0][0] * R[1][1] * R[2][2] + R[0][1] * R[1][2] * R[2][0] + R[0][2] * R[1][0] * R[2][1] -
	R[0][2] * R[1][1] * R[2][0] - R[0][1] * R[1][0] * R[2][2] - R[0][0] * R[1][2] * R[2][1];

  if (det < 0) {
	R[2][0] = -R[2][0];
	R[2][1] = -R[2][1];
	R[2][2] = -R[2][2];
  }

  t[0] = pc0[0] - dot(R[0], pw0);
  t[1] = pc0[1] - dot(R[1], pw0);
  t[2] = pc0[2] - dot(R[2], pw0);
}

void epnp::solve_for_sign(void)
{
  if (pcs[2] < 0.0) {
	for(int i = 0; i < 4; i++)
	  for(int j = 0; j < 3; j++)
  ccs[i][j] = -ccs[i][j];

    for(int i = 0; i < number_of_correspondences; i++) {
      pcs[3 * i    ] = -pcs[3 * i];
      pcs[3 * i + 1] = -pcs[3 * i + 1];
	  pcs[3 * i + 2] = -pcs[3 * i + 2];
    }
  }
}

double epnp::compute_R_and_t(ublas::matrix<double> ut, const double * betas,
		   double R[3][3], double t[3])
{
  compute_ccs(betas, ut);
  compute_pcs();

  solve_for_sign();

  estimate_R_and_t(R, t);

  return reprojection_error(R, t);
}

double epnp::reprojection_error(const double R[3][3], const double t[3])
{
  double sum2 = 0.0;

  for(int i = 0; i < number_of_correspondences; i++) {
    double * pw = &pws[3 * i];
    double Xc = dot(R[0], pw) + t[0];
    double Yc = dot(R[1], pw) + t[1];
	double inv_Zc = 1.0 / (dot(R[2], pw) + t[2]);
    double ue = uc + fu * Xc * inv_Zc;
    double ve = vc + fv * Yc * inv_Zc;
    double u = us[2 * i], v = us[2 * i + 1];

    sum2 += sqrt( (u - ue) * (u - ue) + (v - ve) * (v - ve) );
  }

  return sum2 / number_of_correspondences;
}

// betas10        = [B11 B12 B22 B13 B23 B33 B14 B24 B34 B44]
// betas_approx_1 = [B11 B12     B13         B14]



//void epnp::find_betas_approx_1(const CvMat * L_6x10, const CvMat * Rho,
//			 double * betas)
//{

void epnp::find_betas_approx_1(ublas::matrix<double> L_6x10, ublas::vector<double> Rho,
			 double * betas) {
  //double l_6x4[6 * 4], b4[4];
  //CvMat L_6x4 = cvMat(6, 4, CV_64F, l_6x4);
  //CvMat B4    = cvMat(4, 1, CV_64F, b4);
  ublas::matrix<double> L_6x4(6,4);
  //ublas::vector<double> B4(4);
  for(int i = 0; i < 6; i++) {
//	cvmSet(&L_6x4, i, 0, cvmGet(L_6x10, i, 0));
//	cvmSet(&L_6x4, i, 1, cvmGet(L_6x10, i, 1));
//	cvmSet(&L_6x4, i, 2, cvmGet(L_6x10, i, 3));
//	cvmSet(&L_6x4, i, 3, cvmGet(L_6x10, i, 6));
	L_6x4(i,0) = L_6x10(i,0);
	L_6x4(i,1) = L_6x10(i,1);
	L_6x4(i,2) = L_6x10(i,3);
	L_6x4(i,3) = L_6x10(i,6);

  }
		  //solve L6x4 * B4 = Rho  for B4
   //cvSolve(&L_6x4, Rho, &B4, CV_SVD);
   ublas::vector<double> b4 = linearSolve(L_6x4, Rho);

  if (b4(0) < 0) {
	betas[0] = sqrt(-b4[0]);
	betas[1] = -b4(1) / betas[0];
	betas[2] = -b4(2) / betas[0];
	betas[3] = -b4(3) / betas[0];
  } else {
	betas[0] = sqrt(b4(0));
	betas[1] = b4(1) / betas[0];
	betas[2] = b4(2) / betas[0];
	betas[3] = b4(3) / betas[0];
  }
}

// betas10        = [B11 B12 B22 B13 B23 B33 B14 B24 B34 B44]
// betas_approx_2 = [B11 B12 B22                            ]

//void epnp::find_betas_approx_2(const CvMat * L_6x10, const CvMat * Rho,
//			 double * betas)
 void epnp::find_betas_approx_2(ublas::matrix<double> L_6x10, ublas::vector<double> Rho,
 double * betas)
{
//  double l_6x3[6 * 3], b3[3];
//  CvMat L_6x3  = cvMat(6, 3, CV_64F, l_6x3);
//  CvMat B3     = cvMat(3, 1, CV_64F, b3);
  ublas::matrix<double> L_6x3(6,3);

  for(int i = 0; i < 6; i++) {
  //	cvmSet(&L_6x3, i, 0, cvmGet(L_6x10, i, 0));
  //	cvmSet(&L_6x3, i, 1, cvmGet(L_6x10, i, 1));
  //	cvmSet(&L_6x3, i, 2, cvmGet(L_6x10, i, 2));
	L_6x3(i,0) = L_6x10(i,0);
	L_6x3(i,1) = L_6x10(i,1);
	L_6x3(i,2) = L_6x10(i,2);
  }

  //cvSolve(&L_6x3, Rho, &B3, CV_SVD);
  ublas::vector<double> b3 = linearSolve(L_6x3, Rho);

  if (b3[0] < 0) {
	betas[0] = sqrt(-b3(0));
	betas[1] = (b3(2) < 0) ? sqrt(-b3(2)) : 0.0;
  } else {
	betas[0] = sqrt(b3[0]);
	betas[1] = (b3(2) > 0) ? sqrt(b3(2)) : 0.0;
  }

  if (b3(1) < 0) betas[0] = -betas[0];

  betas[2] = 0.0;
  betas[3] = 0.0;
}

// betas10        = [B11 B12 B22 B13 B23 B33 B14 B24 B34 B44]
// betas_approx_3 = [B11 B12 B22 B13 B23                    ]

//void epnp::find_betas_approx_3(const CvMat * L_6x10, const CvMat * Rho,
//			 double * betas)
 void epnp::find_betas_approx_3(ublas::matrix<double> L_6x10, ublas::vector<double> Rho,
 double * betas)
{
//  double l_6x5[6 * 5], b5[5];
//  CvMat L_6x5 = cvMat(6, 5, CV_64F, l_6x5);
//  CvMat B5    = cvMat(5, 1, CV_64F, b5);
  ublas::matrix<double> L_6x5(6,5);
  for(int i = 0; i < 6; i++) {
//	cvmSet(&L_6x5, i, 0, cvmGet(L_6x10, i, 0));
//	cvmSet(&L_6x5, i, 1, cvmGet(L_6x10, i, 1));
//	cvmSet(&L_6x5, i, 2, cvmGet(L_6x10, i, 2));
//	cvmSet(&L_6x5, i, 3, cvmGet(L_6x10, i, 3));
//	cvmSet(&L_6x5, i, 4, cvmGet(L_6x10, i, 4));
	L_6x5(i,0) = L_6x10(i,0);
	L_6x5(i,1) = L_6x10(i,1);
	L_6x5(i,2) = L_6x10(i,2);
	L_6x5(i,3) = L_6x10(i,3);
	L_6x5(i,4) = L_6x10(i,4);

  }

//  cvSolve(&L_6x5, Rho, &B5, CV_SVD);
  ublas::vector<double> b5 = linearSolve(L_6x5, Rho);

  if (b5(0) < 0) {
	betas[0] = sqrt(-b5[0]);
	betas[1] = (b5(2) < 0) ? sqrt(-b5(2)) : 0.0;
  } else {
	betas[0] = sqrt(b5(0));
	betas[1] = (b5(2) > 0) ? sqrt(b5(2)) : 0.0;
  }
  if (b5(1) < 0) betas[0] = -betas[0];
  betas[2] = b5(3) / betas[0];
  betas[3] = 0.0;
}

void epnp::compute_L_6x10(ublas::matrix<double> ut, ublas::matrix<double> &l_6x10)
{
//  const double * v[4];

//  v[0] = ut + 12 * 11;   //row ll
//  v[1] = ut + 12 * 10;   //row 10
//  v[2] = ut + 12 *  9;   //row 9
//  v[3] = ut + 12 *  8;   //row 8

  double dv[4][6][3];

  for(int i = 0; i < 4; i++) {
	int a = 0, b = 1;
	for(int j = 0; j < 6; j++) {
//	  dv[i][j][0] = v[i][3 * a    ] - v[i][3 * b];
//	  dv[i][j][1] = v[i][3 * a + 1] - v[i][3 * b + 1];
//	  dv[i][j][2] = v[i][3 * a + 2] - v[i][3 * b + 2];
	  dv[i][j][0] = ut(11-i,3 * a) - ut(11-i,3 * b);
	  dv[i][j][1] = ut(11-i,3 * a + 1) - ut(11-i,3 * b + 1);
	  dv[i][j][2] = ut(11-i,3 * a + 2) - ut(11-i,3 * b + 2);


	  b++;
	  if (b > 3) {
  a++;
  b = a + 1;
	  }
	}
  }

  for(int i = 0; i < 6; i++) {
//	double * row = l_6x10 + 10 * i;
//
//	row[0] =        dot(dv[0][i], dv[0][i]);
//	row[1] = 2.0f * dot(dv[0][i], dv[1][i]);
//	row[2] =        dot(dv[1][i], dv[1][i]);
//	row[3] = 2.0f * dot(dv[0][i], dv[2][i]);
//	row[4] = 2.0f * dot(dv[1][i], dv[2][i]);
//	row[5] =        dot(dv[2][i], dv[2][i]);
//	row[6] = 2.0f * dot(dv[0][i], dv[3][i]);
//	row[7] = 2.0f * dot(dv[1][i], dv[3][i]);
//	row[8] = 2.0f * dot(dv[2][i], dv[3][i]);
//	row[9] =        dot(dv[3][i], dv[3][i]);

	l_6x10(i,0) =       dot(dv[0][i], dv[0][i]);
	l_6x10(i,1) = 2.0 * dot(dv[0][i], dv[1][i]);
	l_6x10(i,2) =       dot(dv[1][i], dv[1][i]);
	l_6x10(i,3) = 2.0 * dot(dv[0][i], dv[2][i]);
	l_6x10(i,4) = 2.0 * dot(dv[1][i], dv[2][i]);
	l_6x10(i,5) =       dot(dv[2][i], dv[2][i]);
	l_6x10(i,6) = 2.0 * dot(dv[0][i], dv[3][i]);
	l_6x10(i,7) = 2.0 * dot(dv[1][i], dv[3][i]);
	l_6x10(i,8) = 2.0 * dot(dv[2][i], dv[3][i]);
	l_6x10(i,9) =       dot(dv[3][i], dv[3][i]);

  }
}

void epnp::compute_rho(ublas::vector<double> rho)
{
  rho(0) = dist2(cws[0], cws[1]);
  rho(1) = dist2(cws[0], cws[2]);
  rho(2) = dist2(cws[0], cws[3]);
  rho(3) = dist2(cws[1], cws[2]);
  rho(4) = dist2(cws[1], cws[3]);
  rho(5) = dist2(cws[2], cws[3]);
}

//void epnp::compute_A_and_b_gauss_newton(const double * l_6x10, const double * rho,
//          const double betas[4], CvMat * A, CvMat * b)
void epnp::compute_A_and_b_gauss_newton(ublas::matrix<double> l_6x10, ublas::vector<double> rho,
		  const double betas[4], ublas::matrix<double> &A, ublas::vector<double> &b)

{
  for(int i = 0; i < 6; i++) {
	//const double * rowL = l_6x10 + i * 10; //row i
	//double * rowA = A->data.db + i * 4;   //row i

//	rowA[0] = 2 * rowL[0] * betas[0] +     rowL[1] * betas[1] +     rowL[3] * betas[2] +     rowL[6] * betas[3];
//	rowA[1] =     rowL[1] * betas[0] + 2 * rowL[2] * betas[1] +     rowL[4] * betas[2] +     rowL[7] * betas[3];
//	rowA[2] =     rowL[3] * betas[0] +     rowL[4] * betas[1] + 2 * rowL[5] * betas[2] +     rowL[8] * betas[3];
//	rowA[3] =     rowL[6] * betas[0] +     rowL[7] * betas[1] +     rowL[8] * betas[2] + 2 * rowL[9] * betas[3];
	A(i,0) = 2 * l_6x10(i,0) * betas[0] +     l_6x10(i,1) * betas[1] +     l_6x10(i,3) * betas[2] +     l_6x10(i,6) * betas[3];
	A(i,1) =     l_6x10(i,1) * betas[0] + 2 * l_6x10(i,2) * betas[1] +     l_6x10(i,4) * betas[2] +     l_6x10(i,7) * betas[3];
	A(i,2) =     l_6x10(i,3) * betas[0] +     l_6x10(i,4) * betas[1] + 2 * l_6x10(i,5) * betas[2] +     l_6x10(i,8) * betas[3];
	A(i,3) =     l_6x10(i,6) * betas[0] +     l_6x10(i,7) * betas[1] +     l_6x10(i,8) * betas[2] + 2 * l_6x10(i,9) * betas[3];


//	cvmSet(b, i, 0, rho[i] -
//	 (
//	  rowL[0] * betas[0] * betas[0] +
//	  rowL[1] * betas[0] * betas[1] +
//	  rowL[2] * betas[1] * betas[1] +
//	  rowL[3] * betas[0] * betas[2] +
//	  rowL[4] * betas[1] * betas[2] +
//	  rowL[5] * betas[2] * betas[2] +
//	  rowL[6] * betas[0] * betas[3] +
//	  rowL[7] * betas[1] * betas[3] +
//	  rowL[8] * betas[2] * betas[3] +
//	  rowL[9] * betas[3] * betas[3]
//	  ));
	  b(i) = rho(i) -
	 (
	  l_6x10(i,0) * betas[0] * betas[0] +
	  l_6x10(i,1) * betas[0] * betas[1] +
	  l_6x10(i,2) * betas[1] * betas[1] +
	  l_6x10(i,3) * betas[0] * betas[2] +
	  l_6x10(i,4) * betas[1] * betas[2] +
	  l_6x10(i,5) * betas[2] * betas[2] +
	  l_6x10(i,6) * betas[0] * betas[3] +
	  l_6x10(i,7) * betas[1] * betas[3] +
	  l_6x10(i,8) * betas[2] * betas[3] +
	  l_6x10(i,9) * betas[3] * betas[3]
	  );
  }
}

//void epnp::gauss_newton(const CvMat * L_6x10, const CvMat * Rho, double betas[4])
void epnp::gauss_newton(ublas::matrix<double> L_6x10, ublas::vector<double> Rho, double betas[4])
{
  const int iterations_number = 5;

//  double a[6*4], b[6], x[4];
//  CvMat A = cvMat(6, 4, CV_64F, a);
//  CvMat B = cvMat(6, 1, CV_64F, b);
//  CvMat X = cvMat(4, 1, CV_64F, x);
  ublas::matrix<double> A(6,4);
  ublas::vector<double> B(6);
  ublas::vector<double> X(4);

  for(int k = 0; k < iterations_number; k++)
  {
	//compute_A_and_b_gauss_newton(L_6x10->data.db, Rho->data.db,
	//betas, &A, &B);
	compute_A_and_b_gauss_newton(L_6x10, Rho, betas, A, B);
	//qr_solve(&A, &B, &X);
	ublas::vector<double> X = linearSolve(A,B);
	for(int i = 0; i < 4; i++)
	betas[i] += X(i);
  }
}


//void epnp::qr_solve(CvMat * A, CvMat * b, CvMat * X)
//{
//  const int nr = A->rows;
//  const int nc = A->cols;
//
//  if (max_nr != 0 && max_nr < nr)
//  {
//	delete [] A1;
//	delete [] A2;
//  }
//  if (max_nr < nr)
//  {
//	max_nr = nr;
//	A1 = new double[nr];
//	A2 = new double[nr];
//  }
//
//  double * pA = A->data.db, * ppAkk = pA;
//  for(int k = 0; k < nc; k++)
//  {
//	double * ppAik1 = ppAkk, eta = fabs(*ppAik1);
//	for(int i = k + 1; i < nr; i++)
//	{
//	  double elt = fabs(*ppAik1);
//	  if (eta < elt) eta = elt;
//	  ppAik1 += nc;
//	}
//	if (eta == 0)
//	{
//	  A1[k] = A2[k] = 0.0;
//	  //cerr << "God damnit, A is singular, this shouldn't happen." << endl;
//	  return;
//	}
//    else
//	{
//      double * ppAik2 = ppAkk, sum2 = 0.0, inv_eta = 1. / eta;
//	  for(int i = k; i < nr; i++)
//      {
//        *ppAik2 *= inv_eta;
//		sum2 += *ppAik2 * *ppAik2;
//        ppAik2 += nc;
//      }
//	  double sigma = sqrt(sum2);
//      if (*ppAkk < 0)
//      sigma = -sigma;
//	  *ppAkk += sigma;
//      A1[k] = sigma * *ppAkk;
//      A2[k] = -eta * sigma;
//	  for(int j = k + 1; j < nc; j++)
//      {
//        double * ppAik = ppAkk, sum = 0;
//		for(int i = k; i < nr; i++)
//        {
//          sum += *ppAik * ppAik[j - k];
//		  ppAik += nc;
//        }
//        double tau = sum / A1[k];
//		ppAik = ppAkk;
//        for(int i = k; i < nr; i++)
//        {
//		  ppAik[j - k] -= tau * *ppAik;
//          ppAik += nc;
//        }
//	  }
//    }
//    ppAkk += nc + 1;
//  }
//
//  // b <- Qt b
//  double * ppAjj = pA, * pb = b->data.db;
//  for(int j = 0; j < nc; j++)
//  {
//	double * ppAij = ppAjj, tau = 0;
//    for(int i = j; i < nr; i++)
//    {
//	  tau += *ppAij * pb[i];
//      ppAij += nc;
//    }
//	tau /= A1[j];
//    ppAij = ppAjj;
//    for(int i = j; i < nr; i++)
//	{
//      pb[i] -= tau * *ppAij;
//      ppAij += nc;
//	}
//    ppAjj += nc + 1;
//  }
//
//  // X = R-1 b
//  double * pX = X->data.db;
//  pX[nc - 1] = pb[nc - 1] / A2[nc - 1];
//  for(int i = nc - 2; i >= 0; i--)
//  {
//	double * ppAij = pA + i * nc + (i + 1), sum = 0;
//
//    for(int j = i + 1; j < nc; j++)
//	{
//      sum += *ppAij * pX[j];
//      ppAij++;
//	}
//    pX[i] = (pb[i] - sum) / A2[i];
//  }
//}


