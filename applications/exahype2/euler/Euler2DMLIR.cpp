#include <cmath>

extern "C" {

    void maxEigenvalue(
        const void * __restrict__ voidQ,
        const double * faceCentre, //  tarch::la::Vector<Dimensions,double>&  faceCentre,
        const double * volumeH, // tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        void* __restrict__                         voidMaxEigenvalue
    ) {
        const double* Q = static_cast<const double*>(voidQ);
        double* maxEigenvalue = static_cast<double*>(voidMaxEigenvalue);
        const double GAMMA = 1.0;

        const double irho = 1.0 / fabs(Q[0]);
        const double p = (GAMMA - 1.0) * (Q[3] - 0.5 * irho * (Q[1] * Q[1] + Q[2] * Q[2]));

        const double speedOfSound = sqrt(GAMMA * fabs(p) * irho);
        const double u_n          = Q[normal + 1] * irho;
        //double result       = fmax(fabs(u_n - speedOfSound), fabs(u_n + speedOfSound));
        double result       = fabs(u_n - speedOfSound);
        const double temp = fabs(u_n + speedOfSound);
        if (temp > result) result = temp;

        maxEigenvalue[0] = result;
    }

    void flux(
        const void* const __restrict__             voidQ,
        const double* x,
        const double* h,
        const double                                 t,
        const double                                 dt,
        const int                                    normal,
        void* const __restrict__                   voidF
    ) {
        const double* Q = static_cast<const double*>(voidQ);
        double* F = static_cast<double*>(voidF);
        const double GAMMA = 1.0;

        const double irho = 1.0 / Q[0];
        const double p = (GAMMA - 1.0) * (Q[3] - 0.5 * irho * (Q[1] * Q[1] + Q[2] * Q[2]));

        F[0]          = Q[normal + 1];
        F[1]            = Q[normal + 1] * Q[1] * irho;
        F[2]            = Q[normal + 1] * Q[2] * irho;
        F[3]            = Q[normal + 1] * (Q[3] + p) * irho;

        F[normal + 1] += p;
    }
}