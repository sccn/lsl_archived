//---------------------------------------------------------------------------

#ifndef epnpH
#define epnpH

#include "LinearAlgebra.h"


//#include "precomp.hpp"

class epnp {
 public:
 epnp(ublas::matrix<double> cameraMatrix, ublas::matrix<double> opoints, ublas::matrix<double> ipoints);
 // epnp(const cv::Mat& cameraMatrix, const cv::Mat& opoints, const cv::Mat& ipoints);
  ~epnp();

  void add_correspondence(const double X, const double Y, const double Z,
              const double u, const double v);

 // void compute_pose(cv::Mat& R, cv::Mat& t);
 void compute_pose(ublas::matrix<double> & R, ublas::vector<double>& t);
 private:

  void init_camera_parameters(ublas::matrix<double> cameraMatrix)
  {
	uc = cameraMatrix(0, 2);
	vc = cameraMatrix(1, 2);
	fu = cameraMatrix(0, 0);
	fv = cameraMatrix(1, 1);
  }


  void init_points(ublas::matrix<double> opoints, ublas::matrix<double> ipoints)
  {
      for(int i = 0; i < number_of_correspondences; i++)
	  {
		  //pws[3 * i    ] = opoints.at<OpointType>(0,i).x;
		  pws[3 * i    ] = opoints(0,i);
		  //pws[3 * i + 1] = opoints.at<OpointType>(0,i).y;
		  pws[3 * i + 1] = opoints(1,i);
		  //pws[3 * i + 2] = opoints.at<OpointType>(0,i).z;
		  pws[3 * i + 2] = opoints(2,i);

		  us[2 * i    ] = ipoints(0,i)*fu + uc;
		  us[2 * i + 1] = ipoints(1,i)*fv + vc;
      }
  }
  double reprojection_error(const double R[3][3], const double t[3]);
  void choose_control_points(void);
  void compute_barycentric_coordinates(void);
 // void fill_M(CvMat * M, const int row, const double * alphas, const double u, const double v);
  void fill_M(ublas::matrix<double> &M,const int row, const double * as, const double u, const double v);
  //void compute_ccs(const double * betas, const double * ut);
  void compute_ccs(const double * betas, ublas::matrix<double> ut);
  void compute_pcs(void);

  void solve_for_sign(void);

//  void find_betas_approx_1(const CvMat * L_6x10, const CvMat * Rho, double * betas);
//  void find_betas_approx_2(const CvMat * L_6x10, const CvMat * Rho, double * betas);
//  void find_betas_approx_3(const CvMat * L_6x10, const CvMat * Rho, double * betas);

  void find_betas_approx_1(ublas::matrix<double> L_6x10, ublas::vector<double> Rho, double * betas);
  void find_betas_approx_2(ublas::matrix<double> L_6x10, ublas::vector<double> Rho, double * betas);
  void find_betas_approx_3(ublas::matrix<double> L_6x10, ublas::vector<double> Rho, double * betas);
//  void qr_solve(CvMat * A, CvMat * b, CvMat * X);

  double dot(const double * v1, const double * v2);
  double dist2(const double * p1, const double * p2);

  void compute_rho(ublas::vector<double> rho);
  //void compute_L_6x10(const double * ut, double * l_6x10);
  void compute_L_6x10(ublas::matrix<double> ut, ublas::matrix<double> &l_6x10);

  //void gauss_newton(const CvMat * L_6x10, const CvMat * Rho, double current_betas[4]);
  void gauss_newton(ublas::matrix<double> L_6x10, ublas::vector<double> Rho, double betas[4]);
//  void compute_A_and_b_gauss_newton(const double * l_6x10, const double * rho,
//					const double cb[4], CvMat * A, CvMat * b);
  void compute_A_and_b_gauss_newton(ublas::matrix<double> l_6x10, ublas::vector<double> rho,
		  const double betas[4], ublas::matrix<double> &A, ublas::vector<double> &b);


  double compute_R_and_t(ublas::matrix<double> ut, const double * betas,
		   double R[3][3], double t[3]);

  void estimate_R_and_t(double R[3][3], double t[3]);

  void copy_R_and_t(const double R_dst[3][3], const double t_dst[3],
			double R_src[3][3], double t_src[3]);


  double uc, vc, fu, fv;

  std::vector<double> pws, us, alphas, pcs;    //objectpoints (vectorized), imagepoints (vectorized)
  int number_of_correspondences;

  double cws[4][3], ccs[4][3];
  double cws_determinant;
  int max_nr;
  double * A1, * A2;
};

#endif

