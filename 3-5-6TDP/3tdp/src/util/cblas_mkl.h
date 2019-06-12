#ifdef USE_MKL
#include <mkl.h>
#define MYBLAS_TRANSPOSE CBLAS_TRANSPOSE
#define MYBLAS_ORDER CBLAS_ORDER
#else
#include <cblas.h>
#define MYBLAS_TRANSPOSE enum CBLAS_TRANSPOSE
#define MYBLAS_ORDER enum CBLAS_ORDER
#endif
