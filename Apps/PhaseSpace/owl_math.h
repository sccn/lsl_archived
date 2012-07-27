// owl_math.h
// OWL v1.3

#ifndef OWL_MATH_H
#define OWL_MATH_H 

#ifndef OWLAPI
#ifdef WIN32
#ifdef __DLL
#define OWLAPI __declspec(dllexport)
#else // !__DLL
#define OWLAPI __declspec(dllimport)
#endif // __DLL
#else // ! WIN32
#define OWLAPI
#endif // WIN32
#endif

#ifdef __cplusplus
extern "C" {
#endif

// POSE
// input: pos(x, y, z), angle, rotation about v(vx, vy, vz)
// output: pos(x, y, z), quaterion(sin(angle/2), v*sin(angle/2))
#define POSE(x, y, z, angle, vx, vy, vz) \
{ x, y, z, cos(angle/2.0), vx*sin(angle/2.0), vy*sin(angle/2.0), vz*sin(angle/2.0) }

OWLAPI extern const float owl_y_up[7];
OWLAPI extern const float owl_z_up[7];

OWLAPI void mult_mv3_v3(const float *a, const float *b, float *ab);
OWLAPI void mult_qq(const float *a, const float *b, float *ab);
OWLAPI void mult_qvq(const float *q, const float *v, float *qvq);
OWLAPI void normalize_p(float *p);
OWLAPI void mult_pp(const float *p1, const float *p2, float *p3);
OWLAPI void invert_p(float *p);
OWLAPI void convert_pm(const float *p, float *m);
OWLAPI void convert_pmi(const float *p, float *m);
OWLAPI void convert_mp(const float *m, float *p);

#ifdef __cplusplus
}
#endif

#endif // OWL_MATH_H
