#include "tarch/multicore/multicore.h"

#include <algorithm>
#include <limits>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "exagrype/ccz4/Kernels.h"
#include "tarch/Assertions.h"


void exagrype::ccz4::addKreissOligerDissipation(
  const double* __restrict__          QWithHalo,
  double* __restrict__                rightHandSide,
  const tarch::la::Vector<3, double>& patchSize,
  int                                 numberOfGridCellsPerPatchPerAxis,
  int                                 haloSize,
  int                                 unknowns,
  int                                 auxiliaryVariables,
  double                              sigma,
  const int*                          quantities,
  int                                 numberOfQuantities
) {
  assertion1(haloSize >= 3, haloSize);

  const int    n                     = numberOfGridCellsPerPatchPerAxis;
  const int    nWithHalo             = n + 2 * haloSize;
  const int    entriesPerGridCell    = unknowns + auxiliaryVariables;
  const double weights[7]            = {1.0, -6.0, 15.0, -20.0, 15.0, -6.0, 1.0};
  const int    strides[3]            = {entriesPerGridCell, nWithHalo * entriesPerGridCell, nWithHalo * nWithHalo * entriesPerGridCell};

  for (int z = 0; z < n; z++)
  for (int y = 0; y < n; y++)
  for (int x = 0; x < n; x++) {
    const int input  = (((z + haloSize) * nWithHalo + (y + haloSize)) * nWithHalo + (x + haloSize)) * entriesPerGridCell;
    const int output = ((z * n + y) * n + x) * unknowns;
    for (int i = 0; i < numberOfQuantities; i++) {
      const int quantity = quantities[i];
      for (int d = 0; d < 3; d++) {
        double dissipation = 0.0;
        for (int shift = -3; shift <= 3; shift++) {
          dissipation += weights[shift + 3] * QWithHalo[input + shift * strides[d] + quantity];
        }
        rightHandSide[output + quantity] += sigma / 64.0 * n / patchSize(d) * dissipation;
      }
    }
  }
}

#if defined(GPUOffloadingOMP)
#pragma omp declare target
#endif
void exagrype::ccz4::enforceCCZ4constraints(double * luh)
{
    double g_cov[3][3] = { {luh[0], luh[1], luh[2]}, {luh[1], luh[3], luh[4]}, {luh[2], luh[4], luh[5]} };
    const double det = luh[0]*luh[3]*luh[5] - luh[0]*luh[4]*luh[4] - luh[1]*luh[1]*luh[5] + 2*luh[1]*luh[2]*luh[4] -luh[2]*luh[2]*luh[3];

    const double phisq = 1./std::cbrt(det);
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) g_cov[i][j] *= phisq;

    const double det2 = g_cov[0][0]*g_cov[1][1]*g_cov[2][2] -
        g_cov[0][0]*g_cov[1][2]*g_cov[2][1] - g_cov[1][0]*g_cov[0][1]*g_cov[2][2] +
        g_cov[1][0]*g_cov[0][2]*g_cov[2][1] + g_cov[2][0]*g_cov[0][1]*g_cov[1][2] -
        g_cov[2][0]*g_cov[0][2]*g_cov[1][1];


    const double invdet = 1./det2;
    const double g_contr[3][3] = {
        { ( g_cov[1][1]*g_cov[2][2]-g_cov[1][2]*g_cov[2][1])*invdet, -(g_cov[0][1]*g_cov[2][2]-g_cov[0][2]*g_cov[2][1])*invdet, -(-g_cov[0][1]*g_cov[1][2]+g_cov[0][2]*g_cov[1][1])*invdet},
        {-( g_cov[1][0]*g_cov[2][2]-g_cov[1][2]*g_cov[2][0])*invdet,  (g_cov[0][0]*g_cov[2][2]-g_cov[0][2]*g_cov[2][0])*invdet, -( g_cov[0][0]*g_cov[1][2]-g_cov[0][2]*g_cov[1][0])*invdet},
        {-(-g_cov[1][0]*g_cov[2][1]+g_cov[1][1]*g_cov[2][0])*invdet, -(g_cov[0][0]*g_cov[2][1]-g_cov[0][1]*g_cov[2][0])*invdet,  ( g_cov[0][0]*g_cov[1][1]-g_cov[0][1]*g_cov[1][0])*invdet}
    };
    double Aex[3][3] = { {luh[6], luh[7], luh[8]}, {luh[7], luh[9], luh[10]}, {luh[8], luh[10], luh[11]} };

    double traceA = 0;
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) traceA += g_contr[i][j]*Aex[i][j];

    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Aex[i][j] -= 1./3. * traceA * g_cov[i][j];

    luh[ 0] = g_cov[0][0];
    luh[ 1] = g_cov[0][1];
    luh[ 2] = g_cov[0][2];
    luh[ 3] = g_cov[1][1];
    luh[ 4] = g_cov[1][2];
    luh[ 5] = g_cov[2][2];

    luh[ 6] =   Aex[0][0];
    luh[ 7] =   Aex[0][1];
    luh[ 8] =   Aex[0][2];
    luh[ 9] =   Aex[1][1];
    luh[10] =   Aex[1][2];
    luh[11] =   Aex[2][2];

    //new constraints: a minimum for phi(chi) and alpha
    luh[16]=std::fmax(1e-16,luh[16]);
    luh[54]=std::fmax(1e-16,luh[54]);

    //
    // As suggested by our PRD referee, we also enforce the algebraic constraint that results from the first spatial derivative of the constraint
    // det \tilde{\gamma}_ij = 0, which leads to
    //
    // \tilde{\gamma}^{ij} D_kij = 0
    //
    // and is thus a condition of trace-freeness on all submatrices D_kij for k=1,2,3.
    //

    double DD[3][3][3] = {
        {{luh[35], luh[36], luh[37]}, {luh[36], luh[38], luh[39]}, {luh[37], luh[39], luh[40]}},
        {{luh[41], luh[42], luh[43]}, {luh[42], luh[44], luh[45]}, {luh[43], luh[45], luh[46]}},
        {{luh[47], luh[48], luh[49]}, {luh[48], luh[50], luh[51]}, {luh[49], luh[51], luh[52]}}
    };

    for (int l=0;l<3;l++)
    {
        double traceDk = 0;
        for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) traceDk += g_contr[i][j]*DD[l][i][j];

        for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) DD[l][i][j] -= 1./3 * g_cov[i][j]*traceDk;
    }

    luh[35] = DD[0][0][0];
    luh[36] = DD[0][0][1];
    luh[37] = DD[0][0][2];
    luh[38] = DD[0][1][1];
    luh[39] = DD[0][1][2];
    luh[40] = DD[0][2][2];

    luh[41] = DD[1][0][0];
    luh[42] = DD[1][0][1];
    luh[43] = DD[1][0][2];
    luh[44] = DD[1][1][1];
    luh[45] = DD[1][1][2];
    luh[46] = DD[1][2][2];

    luh[47] = DD[2][0][0];
    luh[48] = DD[2][0][1];
    luh[49] = DD[2][0][2];
    luh[50] = DD[2][1][1];
    luh[51] = DD[2][1][2];
    luh[52] = DD[2][2][2];
}
#if defined(GPUOffloadingOMP)
#pragma omp end declare target
#endif

#if defined(GPUOffloadingOMP)
#pragma omp declare target
#endif
void exagrype::ccz4::admconstraints(double* constraints, const double* const Q, const double* const gradQSerialised)
{
    constexpr int nVar(59);
    double gradQin[59][3] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    // De-serialise input data and fill static array
    for (int normal=0; normal<3; normal++)
    for (int i=0; i<nVar; i++) gradQin[i][normal] = gradQSerialised[i+normal*nVar];

    // Note g_cov is symmetric
    const double g_cov[3][3] = { {Q[0], Q[1], Q[2]}, {Q[1], Q[3], Q[4]}, {Q[2], Q[4], Q[5]} };
    const double invdet = 1./( Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] -Q[2]*Q[2]*Q[3]);

    const double g_contr[3][3] = {
        { ( Q[3]*Q[5]-Q[4]*Q[4])*invdet, -( Q[1]*Q[5]-Q[2]*Q[4])*invdet, -(-Q[1]*Q[4]+Q[2]*Q[3])*invdet},
        {-( Q[1]*Q[5]-Q[4]*Q[2])*invdet,  ( Q[0]*Q[5]-Q[2]*Q[2])*invdet, -( Q[0]*Q[4]-Q[2]*Q[1])*invdet},
        {-(-Q[1]*Q[4]+Q[3]*Q[2])*invdet, -( Q[0]*Q[4]-Q[1]*Q[2])*invdet,  ( Q[0]*Q[3]-Q[1]*Q[1])*invdet}
    };

    // NOTE Aex is symmetric
    double Aex[3][3] = { {Q[6], Q[7], Q[8]}, {Q[7], Q[9], Q[10]}, {Q[8], Q[10], Q[11]} };

    double traceA = 0;
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) traceA+=g_contr[i][j]*Aex[i][j];
    traceA *= 1./3;

    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Aex[i][j] -= traceA * g_cov[i][j];

    const double dAex[3][3][3] = {
        {{gradQin[6][0],gradQin[7][0],gradQin[8][0]}, {gradQin[7][0], gradQin[9][0], gradQin[10][0]},  {gradQin[8][0], gradQin[10][0], gradQin[11][0]}},
        {{gradQin[6][1],gradQin[7][1],gradQin[8][1]}, {gradQin[7][1], gradQin[9][1], gradQin[10][1]},  {gradQin[8][1], gradQin[10][1], gradQin[11][1]}},
        {{gradQin[6][2],gradQin[7][2],gradQin[8][2]}, {gradQin[7][2], gradQin[9][2], gradQin[10][2]},  {gradQin[8][2], gradQin[10][2], gradQin[11][2]}}
    };

    const double traceK = Q[53];
    const double dtraceK[3] = {gradQin[53][0], gradQin[53][1], gradQin[53][2]};

    const double phi = std::fmax(1e-2,Q[54]); 
    const double phi2 = phi*phi;
    const double PP[3] = {Q[55], Q[56], Q[57]};

    const double dPP[3][3] = {
        {gradQin[55][0],gradQin[56][0],gradQin[57][0]},
        {gradQin[55][1],gradQin[56][1],gradQin[57][1]},
        {gradQin[55][2],gradQin[56][2],gradQin[57][2]}
    };

    const double DD[3][3][3] = {
        {{Q[35], Q[36], Q[37]}, {Q[36], Q[38], Q[39]}, {Q[37], Q[39], Q[40]}},
        {{Q[41], Q[42], Q[43]}, {Q[42], Q[44], Q[45]}, {Q[43], Q[45], Q[46]}},
        {{Q[47], Q[48], Q[49]}, {Q[48], Q[50], Q[51]}, {Q[49], Q[51], Q[52]}}
    };
    const double dDD[3][3][3][3] = {
        {
                {
                        {gradQin[35][0],gradQin[36][0],gradQin[37][0]}, {gradQin[36][0],gradQin[38][0],gradQin[39][0]}, {gradQin[37][0],gradQin[39][0],gradQin[40][0]},
                },
                {
                        {gradQin[41][0],gradQin[42][0],gradQin[43][0]}, {gradQin[42][0],gradQin[44][0],gradQin[45][0]}, {gradQin[43][0],gradQin[45][0],gradQin[46][0]},
                },
                {
                        {gradQin[47][0],gradQin[48][0],gradQin[49][0]}, {gradQin[48][0],gradQin[50][0],gradQin[51][0]}, {gradQin[49][0],gradQin[51][0],gradQin[52][0]}
                }
        },
        {
                {
                        {gradQin[35][1],gradQin[36][1],gradQin[37][1]}, {gradQin[36][1],gradQin[38][1],gradQin[39][1]}, {gradQin[37][1],gradQin[39][1],gradQin[40][1]},
                },
                {
                        {gradQin[41][1],gradQin[42][1],gradQin[43][1]}, {gradQin[42][1],gradQin[44][1],gradQin[45][1]}, {gradQin[43][1],gradQin[45][1],gradQin[46][1]},
                },
                {
                        {gradQin[47][1],gradQin[48][1],gradQin[49][1]}, {gradQin[48][1],gradQin[50][1],gradQin[51][1]}, {gradQin[49][1],gradQin[51][1],gradQin[52][1]}
                }
        },
        {
                {
                        {gradQin[35][2],gradQin[36][2],gradQin[37][2]}, {gradQin[36][2],gradQin[38][2],gradQin[39][2]}, {gradQin[37][2],gradQin[39][2],gradQin[40][2]},
                },
                {
                        {gradQin[41][2],gradQin[42][2],gradQin[43][2]}, {gradQin[42][2],gradQin[44][2],gradQin[45][2]}, {gradQin[43][2],gradQin[45][2],gradQin[46][2]},
                },
                {
                        {gradQin[47][2],gradQin[48][2],gradQin[49][2]}, {gradQin[48][2],gradQin[50][2],gradQin[51][2]}, {gradQin[49][2],gradQin[51][2],gradQin[52][2]}
                }
        }
    };

    double dgup[3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int k = 0; k < 3; k++)
    for (int m = 0; m < 3; m++)
    for (int l = 0; l < 3; l++)
    for (int n = 0; n < 3; n++)
    for (int j = 0; j < 3; j++) dgup[k][m][l] -= g_contr[m][n]*g_contr[j][l]*2*DD[k][n][j];

    double Kex[3][3]={ 0 };
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Kex[i][j]=Aex[i][j]/phi2 + (1./3)*traceK*g_cov[i][j]/phi2;
    double Kmix[3][3]={0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Kmix[i][j] += phi2*g_contr[i][u] * Kex[u][j];
    double Kup[3][3]={0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Kup[i][j] += phi2*g_contr[i][u] * Kmix[j][u]; // Note the transposition is in the indices

    double Christoffel[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double Christoffel_tilde[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++) {
        Christoffel_tilde[i][j][k] += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] );
        Christoffel[i][j][k]  += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] ) - g_contr[k][l]/phi * ( g_cov[j][l] * PP[i] + g_cov[i][l] * PP[j] - g_cov[i][j] * PP[l] );
    }

    double dChristoffel[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double dChristoffel_tilde[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++)
    {
        dChristoffel[k][i][j][m] += 0.5*g_contr[m][l] * ( //npc part
            dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]
            - g_cov[j][l]*(dPP[k][i] + dPP[i][k])/phi - g_cov[i][l]*(dPP[k][j]+dPP[j][k])/phi +  g_cov[i][j]*(dPP[k][l]+dPP[l][k])/phi )
            +dgup[k][m][l]*(DD[i][j][l]+DD[j][i][l]-DD[l][i][j])  //src part
            -dgup[k][m][l]*(g_cov[j][l]*PP[i]+g_cov[i][l]*PP[j]-g_cov[i][j]*PP[l])/phi
            -2*g_contr[m][l]*(DD[k][j][l]*PP[i]+DD[k][i][l]*PP[j]-DD[k][i][j]*PP[l])/phi
            +g_contr[m][l]*(g_cov[j][l]*PP[i]*PP[k]+g_cov[i][l]*PP[j]*PP[k]-g_cov[i][j]*PP[k]*PP[l])/phi2;

        dChristoffel_tilde[k][i][j][m] += dgup[k][m][l]*(DD[i][j][l]+DD[j][i][l]-DD[l][i][j])
            +0.5*g_contr[m][l]*(dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]);
    }

    double Riemann[3][3][3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++){
        Riemann[i][k][j][m] = dChristoffel[k][i][j][m] - dChristoffel[j][i][k][m];
        for (int l = 0; l < 3; l++){
            Riemann[i][k][j][m] += Christoffel[i][j][l]*Christoffel[l][k][m]-Christoffel[i][k][l]*Christoffel[l][j][m];
        }
    }

    double Ricci[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int m = 0; m < 3; m++)
    for (int n = 0; n < 3; n++)
    for (int l = 0; l < 3; l++) Ricci[m][n] += Riemann[m][l][n][l];

    double R=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) R += phi2*g_contr[i][j]*Ricci[i][j];
    double Kupdown=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) Kupdown += Kex[i][j]*Kup[i][j];

// following is added to calculate nabla_i Z^i
    double Gtilde[3] = {0,0,0};
    for (int l = 0; l < 3; l++)
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) Gtilde[i] += g_contr[j][l] * Christoffel_tilde[j][l][i];

    const double Ghat[3] = {Q[13], Q[14], Q[15]};

    const double dGhat[3][3] = {
        {gradQin[13][0],gradQin[14][0],gradQin[15][0]},
        {gradQin[13][1],gradQin[14][1],gradQin[15][1]},
        {gradQin[13][2],gradQin[14][2],gradQin[15][2]}
    };

    double dGtilde[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int j = 0; j < 3; j++)
    for (int l = 0; l < 3; l++) dGtilde[k][i] += dgup[k][j][l]*Christoffel_tilde[j][l][i]+g_contr[j][l]*dChristoffel_tilde[k][j][l][i];

    double Z[3] = {0,0,0}; // Matrix vector multiplications
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Z[i] += 0.5* g_cov[i][j]* (Ghat[j] - Gtilde[j]);//needed here

    double dZ[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++) dZ[k][i] += DD[k][i][j]*(Ghat[j]-Gtilde[j])+0.5*g_cov[i][j]*(dGhat[k][j]-dGtilde[k][j]);

    double nablaZ[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    {
      nablaZ[i][j] = dZ[i][j];
      for (int k = 0; k < 3; k++) nablaZ[i][j] -= Christoffel[i][j][k]*Z[k];
    }

    double nablaZcontracted=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) nablaZcontracted += g_contr[i][j]*nablaZ[i][j]; // TODO fuse these steps
    nablaZcontracted*=phi2;

//end here

    double Ham=0;
    Ham = R-Kupdown+traceK*traceK;

    double dKex[3][3][3]={0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int k = 0; k < 3; k++) dKex[k][i][j]  = (1.0/phi2)*(dAex[k][i][j]+(1./3)*g_cov[i][j]*dtraceK[k]+(2./3)*traceK*DD[k][i][j])-2*Kex[i][j]*PP[k]/phi;

    double Mom[3]={0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int l = 0; l < 3; l++){
        Mom[i] += phi2*g_contr[j][l]*(dKex[l][i][j]-dKex[i][j][l]);
        for (int m = 0; m < 3; m++){
            Mom[i] += phi2*g_contr[j][l]*(-Christoffel[j][l][m]*Kex[m][i]+Christoffel[j][i][m]*Kex[m][l]);
        }
    }

    tarch::memset(constraints, 0, 4*sizeof(double));

    constraints[0]  =   Ham;        //59
    constraints[1]  =   Mom[0];     //60
    constraints[2]  =   Mom[1];     //61
    constraints[3]  =   Mom[2];     //62
    //constraints[4]  =   R;          //63
    //constraints[5]  =   Kupdown;    //64
    //constraints[6]  =   nablaZcontracted;   //65
    //constraints[4]  =   1.0/invdet-1.0;
    //constraints[5]  =   traceA;
}
#if defined(GPUOffloadingOMP)
#pragma omp end declare target
#endif

#if defined(GPUOffloadingOMP)
#pragma omp declare target
#endif
void exagrype::ccz4::Psi4Calc(double* Psi4, const double* const Q, const double* const gradQSerialised, double* coor)
{
    constexpr int nVar(59);
    double gradQin[59][3] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    // De-serialise input data and fill static array
    for (int normal=0; normal<3; normal++)
    for (int i=0; i<nVar; i++) gradQin[i][normal] = gradQSerialised[i+normal*nVar];

    // Note g_cov is symmetric
    const double g_cov[3][3] = { {Q[0], Q[1], Q[2]}, {Q[1], Q[3], Q[4]}, {Q[2], Q[4], Q[5]} };
    const double invdet = 1./( Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] -Q[2]*Q[2]*Q[3]);

    const double g_contr[3][3] = {
        { ( Q[3]*Q[5]-Q[4]*Q[4])*invdet, -( Q[1]*Q[5]-Q[2]*Q[4])*invdet, -(-Q[1]*Q[4]+Q[2]*Q[3])*invdet},
        {-( Q[1]*Q[5]-Q[4]*Q[2])*invdet,  ( Q[0]*Q[5]-Q[2]*Q[2])*invdet, -( Q[0]*Q[4]-Q[2]*Q[1])*invdet},
        {-(-Q[1]*Q[4]+Q[3]*Q[2])*invdet, -( Q[0]*Q[4]-Q[1]*Q[2])*invdet,  ( Q[0]*Q[3]-Q[1]*Q[1])*invdet}
    };

    // NOTE Aex is symmetric
    double Aex[3][3] = { {Q[6], Q[7], Q[8]}, {Q[7], Q[9], Q[10]}, {Q[8], Q[10], Q[11]} };

    double traceA = 0;
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) traceA+=g_contr[i][j]*Aex[i][j];
    traceA *= 1./3;

    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Aex[i][j] -= traceA * g_cov[i][j];

    const double dAex[3][3][3] = {
        {{gradQin[6][0],gradQin[7][0],gradQin[8][0]}, {gradQin[7][0], gradQin[9][0], gradQin[10][0]},  {gradQin[8][0], gradQin[10][0], gradQin[11][0]}},
        {{gradQin[6][1],gradQin[7][1],gradQin[8][1]}, {gradQin[7][1], gradQin[9][1], gradQin[10][1]},  {gradQin[8][1], gradQin[10][1], gradQin[11][1]}},
        {{gradQin[6][2],gradQin[7][2],gradQin[8][2]}, {gradQin[7][2], gradQin[9][2], gradQin[10][2]},  {gradQin[8][2], gradQin[10][2], gradQin[11][2]}}
    };

    const double traceK = Q[53];
    const double dtraceK[3] = {gradQin[53][0], gradQin[53][1], gradQin[53][2]};

    const double phi = std::fmax(1e-2,Q[54]); ;
    const double phi2 = phi*phi;
    const double PP[3] = {Q[55], Q[56], Q[57]};

    const double dPP[3][3] = {
        {gradQin[55][0],gradQin[56][0],gradQin[57][0]},
        {gradQin[55][1],gradQin[56][1],gradQin[57][1]},
        {gradQin[55][2],gradQin[56][2],gradQin[57][2]}
    };

    const double DD[3][3][3] = {
        {{Q[35], Q[36], Q[37]}, {Q[36], Q[38], Q[39]}, {Q[37], Q[39], Q[40]}},
        {{Q[41], Q[42], Q[43]}, {Q[42], Q[44], Q[45]}, {Q[43], Q[45], Q[46]}},
        {{Q[47], Q[48], Q[49]}, {Q[48], Q[50], Q[51]}, {Q[49], Q[51], Q[52]}}
    };
    const double dDD[3][3][3][3] = {
        {
                {
                        {gradQin[35][0],gradQin[36][0],gradQin[37][0]}, {gradQin[36][0],gradQin[38][0],gradQin[39][0]}, {gradQin[37][0],gradQin[39][0],gradQin[40][0]},
                },
                {
                        {gradQin[41][0],gradQin[42][0],gradQin[43][0]}, {gradQin[42][0],gradQin[44][0],gradQin[45][0]}, {gradQin[43][0],gradQin[45][0],gradQin[46][0]},
                },
                {
                        {gradQin[47][0],gradQin[48][0],gradQin[49][0]}, {gradQin[48][0],gradQin[50][0],gradQin[51][0]}, {gradQin[49][0],gradQin[51][0],gradQin[52][0]}
                }
        },
        {
                {
                        {gradQin[35][1],gradQin[36][1],gradQin[37][1]}, {gradQin[36][1],gradQin[38][1],gradQin[39][1]}, {gradQin[37][1],gradQin[39][1],gradQin[40][1]},
                },
                {
                        {gradQin[41][1],gradQin[42][1],gradQin[43][1]}, {gradQin[42][1],gradQin[44][1],gradQin[45][1]}, {gradQin[43][1],gradQin[45][1],gradQin[46][1]},
                },
                {
                        {gradQin[47][1],gradQin[48][1],gradQin[49][1]}, {gradQin[48][1],gradQin[50][1],gradQin[51][1]}, {gradQin[49][1],gradQin[51][1],gradQin[52][1]}
                }
        },
        {
                {
                        {gradQin[35][2],gradQin[36][2],gradQin[37][2]}, {gradQin[36][2],gradQin[38][2],gradQin[39][2]}, {gradQin[37][2],gradQin[39][2],gradQin[40][2]},
                },
                {
                        {gradQin[41][2],gradQin[42][2],gradQin[43][2]}, {gradQin[42][2],gradQin[44][2],gradQin[45][2]}, {gradQin[43][2],gradQin[45][2],gradQin[46][2]},
                },
                {
                        {gradQin[47][2],gradQin[48][2],gradQin[49][2]}, {gradQin[48][2],gradQin[50][2],gradQin[51][2]}, {gradQin[49][2],gradQin[51][2],gradQin[52][2]}
                }
        }
    };

    double dgup[3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int k = 0; k < 3; k++)
    for (int m = 0; m < 3; m++)
    for (int l = 0; l < 3; l++)
    for (int n = 0; n < 3; n++)
    for (int j = 0; j < 3; j++) dgup[k][m][l] -= g_contr[m][n]*g_contr[j][l]*2*DD[k][n][j];

    double Kex[3][3]={ 0 };
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Kex[i][j]=Aex[i][j]/phi2 + (1./3)*traceK*g_cov[i][j]/phi2;

    double Christoffel[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double Christoffel_tilde[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++) {
        Christoffel_tilde[i][j][k] += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] );
        Christoffel[i][j][k]  += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] ) - g_contr[k][l]/phi * ( g_cov[j][l] * PP[i] + g_cov[i][l] * PP[j] - g_cov[i][j] * PP[l] );
    }

    double dChristoffel[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double dChristoffel_tilde[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++)
    {
        dChristoffel[k][i][j][m] += 0.5*g_contr[m][l] * ( //npc part
            dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]
            - g_cov[j][l]*(dPP[k][i] + dPP[i][k])/phi - g_cov[i][l]*(dPP[k][j]+dPP[j][k])/phi +  g_cov[i][j]*(dPP[k][l]+dPP[l][k])/phi )
            +dgup[k][m][l]*(DD[i][j][l]+DD[j][i][l]-DD[l][i][j])  //src part
            -dgup[k][m][l]*(g_cov[j][l]*PP[i]+g_cov[i][l]*PP[j]-g_cov[i][j]*PP[l])/phi
            -2*g_contr[m][l]*(DD[k][j][l]*PP[i]+DD[k][i][l]*PP[j]-DD[k][i][j]*PP[l])/phi
            +g_contr[m][l]*(g_cov[j][l]*PP[i]*PP[k]+g_cov[i][l]*PP[j]*PP[k]-g_cov[i][j]*PP[k]*PP[l])/phi2;

        dChristoffel_tilde[k][i][j][m] += dgup[k][m][l]*(DD[i][j][l]+DD[j][i][l]-DD[l][i][j])
            +0.5*g_contr[m][l]*(dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]);
    }

    double Gtilde[3] = {0,0,0};
    for (int l = 0; l < 3; l++)
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) Gtilde[i] += g_contr[j][l] * Christoffel_tilde[j][l][i];

    const double Theta = Q[12]; // needed later
    const double Ghat[3] = {Q[13], Q[14], Q[15]};

    double Z[3] = {0,0,0}; // Matrix vector multiplications
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Z[i] += 0.5* g_cov[i][j]* (Ghat[j] - Gtilde[j]);

    const double dGhat[3][3] = {
        {gradQin[13][0],gradQin[14][0],gradQin[15][0]},
        {gradQin[13][1],gradQin[14][1],gradQin[15][1]},
        {gradQin[13][2],gradQin[14][2],gradQin[15][2]}
    };

    double dGtilde[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int j = 0; j < 3; j++)
    for (int l = 0; l < 3; l++) dGtilde[k][i] += dgup[k][j][l]*Christoffel_tilde[j][l][i]+g_contr[j][l]*dChristoffel_tilde[k][j][l][i];

    double dZ[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++) dZ[k][i] += DD[k][i][j]*(Ghat[j]-Gtilde[j])+0.5*g_cov[i][j]*(dGhat[k][j]-dGtilde[k][j]);

    double nablaZ[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    {
      nablaZ[i][j] = dZ[i][j];
      for (int k = 0; k < 3; k++) nablaZ[i][j] -= Christoffel[i][j][k]*Z[k];
    }


    double Riemann[3][3][3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++){
        Riemann[i][k][j][m] = dChristoffel[k][i][j][m] - dChristoffel[j][i][k][m];
        for (int l = 0; l < 3; l++){
            Riemann[i][k][j][m] += Christoffel[i][j][l]*Christoffel[l][k][m]-Christoffel[i][k][l]*Christoffel[l][j][m];
        }
    }

    double Ricci[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int m = 0; m < 3; m++)
    for (int n = 0; n < 3; n++)
    for (int l = 0; l < 3; l++) Ricci[m][n] += Riemann[m][l][n][l];
    
    double dKex[3][3][3]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //derivative  \partial_k K_ij
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++) dKex[k][i][j]  = (1.0/phi2)*(dAex[k][i][j]+(1./3)*g_cov[i][j]*dtraceK[k]+(2./3)*traceK*DD[k][i][j])-2*Kex[i][j]*PP[k]/phi;
    
    double Cov_dKex[3][3][3]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //covariant derivative \nabla_i K_jk 
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int m = 0; m < 3; m++) Cov_dKex[i][j][k] += dKex[i][j][k]-Christoffel[i][k][m]*Kex[j][m]-Christoffel[i][j][m]*Kex[m][k];
    
	const double detgd=(1.0/(phi2*phi2*phi2))*( Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] -Q[2]*Q[2]*Q[3]); //determinant of \gamma_ij
	
	double eps_lc_u[3][3][3]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Levi-Civita tensor
    double eps_lc_symbol[3][3][3]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Levi-Civita symbol
	eps_lc_u[0][1][2] = 1/pow(detgd,0.5); eps_lc_u[1][2][0] = 1/pow(detgd,0.5); eps_lc_u[2][0][1] = 1/pow(detgd,0.5);
	eps_lc_u[2][1][0] =-1/pow(detgd,0.5); eps_lc_u[0][2][1] =-1/pow(detgd,0.5); eps_lc_u[1][0][2] =-1/pow(detgd,0.5);
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++) {
        eps_lc_symbol[i][j][k] = eps_lc_u[i][j][k] * pow(detgd,0.5);
        if (detgd<0){ eps_lc_u[i][j][k]= - eps_lc_u[i][j][k]; }
    }

	
	//calculate the orthonormal basis
    //mainly follow the paper: https://arxiv.org/pdf/gr-qc/0610128v1.pdf
	if ((coor[0]*coor[0]+coor[1]*coor[1])<1e-10) {coor[0]+=1e-10;}
	double u_vec[3]={coor[0], coor[1], coor[2]};//r-direction
	double v_vec[3]={-coor[2], coor[1], 0.0}; //phi-direction
	//double w_vec[3]={coor[0]*coor[2],coor[1]*coor[2],-coor[0]*coor[0]-coor[1]*coor[1]};//theta-direction //??
    double w_vec[3]={0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++) w_vec[i] += phi2*g_contr[i][j]*eps_lc_symbol[j][k][l]*v_vec[k]*u_vec[l];

	
	double dotp1=0;
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) dotp1 += g_cov[i][j]*v_vec[i]*v_vec[j]/phi2;
	for (int a = 0; a < 3; a++) v_vec[a] = v_vec[a] / pow(dotp1,0.5);
	
	dotp1=0;
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) dotp1 += g_cov[i][j]*v_vec[i]*u_vec[j]/phi2;
	for (int a = 0; a < 3; a++) u_vec[a] = u_vec[a] - dotp1* v_vec[a];	
	
	dotp1=0;
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) dotp1 += g_cov[i][j]*u_vec[i]*u_vec[j]/phi2;
	for (int a = 0; a < 3; a++) u_vec[a] = u_vec[a] / pow(dotp1,0.5);
	
	dotp1=0;
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) dotp1 += g_cov[i][j]*v_vec[i]*w_vec[j]/phi2;	
	double dotp2=0;
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) dotp2 += g_cov[i][j]*u_vec[i]*w_vec[j]/phi2;
    
	for (int a = 0; a < 3; a++) w_vec[a] = u_vec[a] - dotp1 * v_vec[a] - dotp2 * u_vec[a];		
		
	dotp1=0;
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) dotp1 += g_cov[i][j]*w_vec[i]*w_vec[j]/phi2;
	for (int a = 0; a < 3; a++) w_vec[a] = w_vec[a] / pow(dotp1,0.5);
	
	//EB part of Weyl
    //reference: https://academic.oup.com/book/9640/chapter/156750056
    //8.3.15 and 8.3.16
	double elec[3][3] = {0,0,0,0,0,0,0,0,0};
	double mag[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++){
    	elec[i][j]=Ricci[i][j];
		for (int m = 0; m < 3; m++)
		for (int k = 0; k < 3; k++){
			elec[i][j] += phi2*g_contr[m][k]*(Kex[i][j]*Kex[m][k]-Kex[i][m]*Kex[k][j])-Kex[i][j]*Theta + 0.5*(nablaZ[i][j]+nablaZ[j][i]);
			for (int l = 0; l < 3; l++){
				mag[i][j] += g_cov[i][l] * eps_lc_u[l][k][m] * Cov_dKex[k][m][j] + g_cov[j][l] * eps_lc_u[l][k][m] * Cov_dKex[k][m][i];
			}
		}
		mag[i][j]=mag[i][j]/(2*phi2);
	}
	
	double electr=0, magtr=0;
	for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++){
    	electr += phi2*g_contr[i][j]*elec[i][j];
		magtr  += phi2*g_contr[i][j]*mag[i][j];
	}
	for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++){
		elec[i][j] = elec[i][j] - g_cov[i][j]*electr / (3*phi2);
		mag[i][j]  = mag[i][j]  - g_cov[i][j]*magtr  / (3*phi2);
	}

    //use 8.6.17 and 8.6.18
    //notice we use \bar{m}^i={1 \over \sqrt{2} }(w^i- i v^i)
    Psi4[0]=0.0; Psi4[1]=0.0;
	for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++){
    	//Real part for Psi4
		Psi4[0] += 0.5*elec[i][j]*(w_vec[i]*w_vec[j]-v_vec[i]*v_vec[j])-0.5*mag[i][j]*(v_vec[i]*w_vec[j]+v_vec[j]*w_vec[i]);
		//imaginary part for Psi4
		Psi4[1] += - 0.5*elec[i][j]*(v_vec[i]*w_vec[j]+v_vec[j]*w_vec[i])-0.5*mag[i][j]*(w_vec[i]*w_vec[j]-v_vec[i]*v_vec[j]);
	}
	
}
#if defined(GPUOffloadingOMP)
#pragma omp end declare target
#endif

#if defined(GPUOffloadingOMP)
#pragma omp declare target
#endif
void exagrype::ccz4::TestingOutput(double* terms, const double* const Q, const double* const gradQSerialised)
{
    constexpr int nVar(59);
    double gradQin[59][3] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    // De-serialise input data and fill static array
    for (int normal=0; normal<3; normal++)
    for (int i=0; i<nVar; i++) gradQin[i][normal] = gradQSerialised[i+normal*nVar];

    // Note g_cov is symmetric
    const double g_cov[3][3] = { {Q[0], Q[1], Q[2]}, {Q[1], Q[3], Q[4]}, {Q[2], Q[4], Q[5]} };
    const double invdet = 1./( Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] -Q[2]*Q[2]*Q[3]);

    const double g_contr[3][3] = {
        { ( Q[3]*Q[5]-Q[4]*Q[4])*invdet, -( Q[1]*Q[5]-Q[2]*Q[4])*invdet, -(-Q[1]*Q[4]+Q[2]*Q[3])*invdet},
        {-( Q[1]*Q[5]-Q[4]*Q[2])*invdet,  ( Q[0]*Q[5]-Q[2]*Q[2])*invdet, -( Q[0]*Q[4]-Q[2]*Q[1])*invdet},
        {-(-Q[1]*Q[4]+Q[3]*Q[2])*invdet, -( Q[0]*Q[4]-Q[1]*Q[2])*invdet,  ( Q[0]*Q[3]-Q[1]*Q[1])*invdet}
    };

    // NOTE Aex is symmetric
    double Aex[3][3] = { {Q[6], Q[7], Q[8]}, {Q[7], Q[9], Q[10]}, {Q[8], Q[10], Q[11]} };

    double traceA = 0;
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) traceA+=g_contr[i][j]*Aex[i][j];
    traceA *= 1./3;

    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Aex[i][j] -= traceA * g_cov[i][j];

    const double dAex[3][3][3] = {
        {{gradQin[6][0],gradQin[7][0],gradQin[8][0]}, {gradQin[7][0], gradQin[9][0], gradQin[10][0]},  {gradQin[8][0], gradQin[10][0], gradQin[11][0]}},
        {{gradQin[6][1],gradQin[7][1],gradQin[8][1]}, {gradQin[7][1], gradQin[9][1], gradQin[10][1]},  {gradQin[8][1], gradQin[10][1], gradQin[11][1]}},
        {{gradQin[6][2],gradQin[7][2],gradQin[8][2]}, {gradQin[7][2], gradQin[9][2], gradQin[10][2]},  {gradQin[8][2], gradQin[10][2], gradQin[11][2]}}
    };

    const double traceK = Q[53];
    const double dtraceK[3] = {gradQin[53][0], gradQin[53][1], gradQin[53][2]};

    const double phi = std::fmax(1e-2,Q[54]); 
    const double phi2 = phi*phi;
    const double PP[3] = {Q[55], Q[56], Q[57]};

    const double dPP[3][3] = {
        {gradQin[55][0],gradQin[56][0],gradQin[57][0]},
        {gradQin[55][1],gradQin[56][1],gradQin[57][1]},
        {gradQin[55][2],gradQin[56][2],gradQin[57][2]}
    };

    const double DD[3][3][3] = {
        {{Q[35], Q[36], Q[37]}, {Q[36], Q[38], Q[39]}, {Q[37], Q[39], Q[40]}},
        {{Q[41], Q[42], Q[43]}, {Q[42], Q[44], Q[45]}, {Q[43], Q[45], Q[46]}},
        {{Q[47], Q[48], Q[49]}, {Q[48], Q[50], Q[51]}, {Q[49], Q[51], Q[52]}}
    };
    const double dDD[3][3][3][3] = {
        {
                {
                        {gradQin[35][0],gradQin[36][0],gradQin[37][0]}, {gradQin[36][0],gradQin[38][0],gradQin[39][0]}, {gradQin[37][0],gradQin[39][0],gradQin[40][0]},
                },
                {
                        {gradQin[41][0],gradQin[42][0],gradQin[43][0]}, {gradQin[42][0],gradQin[44][0],gradQin[45][0]}, {gradQin[43][0],gradQin[45][0],gradQin[46][0]},
                },
                {
                        {gradQin[47][0],gradQin[48][0],gradQin[49][0]}, {gradQin[48][0],gradQin[50][0],gradQin[51][0]}, {gradQin[49][0],gradQin[51][0],gradQin[52][0]}
                }
        },
        {
                {
                        {gradQin[35][1],gradQin[36][1],gradQin[37][1]}, {gradQin[36][1],gradQin[38][1],gradQin[39][1]}, {gradQin[37][1],gradQin[39][1],gradQin[40][1]},
                },
                {
                        {gradQin[41][1],gradQin[42][1],gradQin[43][1]}, {gradQin[42][1],gradQin[44][1],gradQin[45][1]}, {gradQin[43][1],gradQin[45][1],gradQin[46][1]},
                },
                {
                        {gradQin[47][1],gradQin[48][1],gradQin[49][1]}, {gradQin[48][1],gradQin[50][1],gradQin[51][1]}, {gradQin[49][1],gradQin[51][1],gradQin[52][1]}
                }
        },
        {
                {
                        {gradQin[35][2],gradQin[36][2],gradQin[37][2]}, {gradQin[36][2],gradQin[38][2],gradQin[39][2]}, {gradQin[37][2],gradQin[39][2],gradQin[40][2]},
                },
                {
                        {gradQin[41][2],gradQin[42][2],gradQin[43][2]}, {gradQin[42][2],gradQin[44][2],gradQin[45][2]}, {gradQin[43][2],gradQin[45][2],gradQin[46][2]},
                },
                {
                        {gradQin[47][2],gradQin[48][2],gradQin[49][2]}, {gradQin[48][2],gradQin[50][2],gradQin[51][2]}, {gradQin[49][2],gradQin[51][2],gradQin[52][2]}
                }
        }
    };

    double dgup[3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int k = 0; k < 3; k++)
    for (int m = 0; m < 3; m++)
    for (int l = 0; l < 3; l++)
    for (int n = 0; n < 3; n++)
    for (int j = 0; j < 3; j++) dgup[k][m][l] -= g_contr[m][n]*g_contr[j][l]*2*DD[k][n][j];

    double Kex[3][3]={ 0 };
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Kex[i][j]=Aex[i][j]/phi2 + (1./3)*traceK*g_cov[i][j]/phi2;
    double Kmix[3][3]={0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Kmix[i][j] += phi2*g_contr[i][u] * Kex[u][j];
    double Kup[3][3]={0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Kup[i][j] += phi2*g_contr[i][u] * Kmix[j][u]; // Note the transposition is in the indices

    double Christoffel[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double Christoffel_tilde[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++) {
        Christoffel_tilde[i][j][k] += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] );
        Christoffel[i][j][k]  += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] ) - g_contr[k][l]/phi * ( g_cov[j][l] * PP[i] + g_cov[i][l] * PP[j] - g_cov[i][j] * PP[l] );
    }

    double dChristoffel[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double dChristoffel_tilde[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++)
    {
        dChristoffel[k][i][j][m] += 0.5*g_contr[m][l] * ( //npc part
            dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]
            - g_cov[j][l]*(dPP[k][i] + dPP[i][k])/phi - g_cov[i][l]*(dPP[k][j]+dPP[j][k])/phi +  g_cov[i][j]*(dPP[k][l]+dPP[l][k])/phi )
            +dgup[k][m][l]*(DD[i][j][l]+DD[j][i][l]-DD[l][i][j])  //src part
            -dgup[k][m][l]*(g_cov[j][l]*PP[i]+g_cov[i][l]*PP[j]-g_cov[i][j]*PP[l])/phi
            -2*g_contr[m][l]*(DD[k][j][l]*PP[i]+DD[k][i][l]*PP[j]-DD[k][i][j]*PP[l])/phi
            +g_contr[m][l]*(g_cov[j][l]*PP[i]*PP[k]+g_cov[i][l]*PP[j]*PP[k]-g_cov[i][j]*PP[k]*PP[l])/phi2;

        dChristoffel_tilde[k][i][j][m] += dgup[k][m][l]*(DD[i][j][l]+DD[j][i][l]-DD[l][i][j])
            +0.5*g_contr[m][l]*(dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]);
    }

    double Riemann[3][3][3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++){
        Riemann[i][k][j][m] = dChristoffel[k][i][j][m] - dChristoffel[j][i][k][m];
        for (int l = 0; l < 3; l++){
            Riemann[i][k][j][m] += Christoffel[i][j][l]*Christoffel[l][k][m]-Christoffel[i][k][l]*Christoffel[l][j][m];
        }
    }

    double Ricci[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int m = 0; m < 3; m++)
    for (int n = 0; n < 3; n++)
    for (int l = 0; l < 3; l++) Ricci[m][n] += Riemann[m][l][n][l];

    double R=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) R += phi2*g_contr[i][j]*Ricci[i][j];
    double Kupdown=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) Kupdown += Kex[i][j]*Kup[i][j];

// following is added to calculate nabla_i Z^i
    double Gtilde[3] = {0,0,0};
    for (int l = 0; l < 3; l++)
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) Gtilde[i] += g_contr[j][l] * Christoffel_tilde[j][l][i];

    const double Ghat[3] = {Q[13], Q[14], Q[15]};

    const double dGhat[3][3] = {
        {gradQin[13][0],gradQin[14][0],gradQin[15][0]},
        {gradQin[13][1],gradQin[14][1],gradQin[15][1]},
        {gradQin[13][2],gradQin[14][2],gradQin[15][2]}
    };

    double dGtilde[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int j = 0; j < 3; j++)
    for (int l = 0; l < 3; l++) dGtilde[k][i] += dgup[k][j][l]*Christoffel_tilde[j][l][i]+g_contr[j][l]*dChristoffel_tilde[k][j][l][i];

    double Z[3] = {0,0,0}; // Matrix vector multiplications
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Z[i] += 0.5* g_cov[i][j]* (Ghat[j] - Gtilde[j]);//needed here

    double dZ[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++) dZ[k][i] += DD[k][i][j]*(Ghat[j]-Gtilde[j])+0.5*g_cov[i][j]*(dGhat[k][j]-dGtilde[k][j]);

    double nablaZ[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    {
      nablaZ[i][j] = dZ[i][j];
      for (int k = 0; k < 3; k++) nablaZ[i][j] -= Christoffel[i][j][k]*Z[k];
    }

    double nablaZcontracted=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) nablaZcontracted += g_contr[i][j]*nablaZ[i][j]; // TODO fuse these steps
    nablaZcontracted*=phi2;

//end here

    double Ham=0;
    Ham = R-Kupdown+traceK*traceK;

    double dKex[3][3][3]={0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int k = 0; k < 3; k++) dKex[k][i][j]  = (1.0/phi2)*(dAex[k][i][j]+(1./3)*g_cov[i][j]*dtraceK[k]+(2./3)*traceK*DD[k][i][j])-2*Kex[i][j]*PP[k]/phi;

    double Mom[3]={0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int l = 0; l < 3; l++){
        Mom[i] += phi2*g_contr[j][l]*(dKex[l][i][j]-dKex[i][j][l]);
        for (int m = 0; m < 3; m++){
            Mom[i] += phi2*g_contr[j][l]*(-Christoffel[j][l][m]*Kex[m][i]+Christoffel[j][i][m]*Kex[m][l]);
        }
    }

    tarch::memset(terms, 0, 15*sizeof(double));
    terms[0]  =   Ham;        //59
    terms[1]  =   Mom[0];     //60
    terms[2]  =   Mom[1];     //61
    terms[3]  =   Mom[2];     //62
    terms[4]  =   R;          //63
    terms[5]  =   Kupdown;    //64
    terms[6]  =   nablaZcontracted;   //65
    //calculate the terms for Px
    const double alpha = std::fmax(1e-4,Q[16]);
    const double dBB[3][3][3] = {
        {
            { gradQin[26][0], gradQin[27][0], gradQin[28][0]}, { gradQin[29][0], gradQin[30][0], gradQin[31][0]}, { gradQin[32][0], gradQin[33][0], gradQin[34][0]}
        },
        {
            { gradQin[26][1], gradQin[27][1], gradQin[28][1]}, { gradQin[29][1], gradQin[30][1], gradQin[31][1]}, { gradQin[32][1], gradQin[33][1], gradQin[34][1]}
        },
        {
            { gradQin[26][2], gradQin[27][2], gradQin[28][2]}, { gradQin[29][2], gradQin[30][2], gradQin[31][2]}, { gradQin[32][2], gradQin[33][2], gradQin[34][2]}
        }
    };
    const double BB[3][3] = {
        {Q[26], Q[27], Q[28]}, {Q[29], Q[30], Q[31]}, {Q[32], Q[33], Q[34]}
    };
    double traceB = BB[0][0] + BB[1][1] + BB[2][2];
    const double AA[3] = {Q[23], Q[24], Q[25]};

    for (int l = 0; l < 3; l++) terms[7]+=BB[0][l]*PP[l]; //66
    terms[8] = (1.0/3.0)*alpha*traceK*PP[0]; //67
    terms[9] = -(1.0/3.0)*traceB*PP[0]; //68
    terms[10]= (1.0/3.0)*phi*alpha*dtraceK[0]; //69
    terms[11]= (1.0/3.0)*phi*traceK*AA[0]; //70
    for (int l = 0; l < 3; l++) terms[12] -= 1./6.*phi*(dBB[0][l][l] + dBB[l][0][l]);  //71
    double temp=0;
    for (int m = 0; m < 3; m++)
    for (int n = 0; n < 3; n++) temp += g_contr[m][n]*dAex[0][m][n];
    terms[13] += 1./3.*alpha*temp; //72
    temp=0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) temp += dgup[0][i][j]*Aex[i][j];
    terms[14] += 1./3.*alpha*temp;  //73

    //constraints[4]  =   1.0/invdet-1.0;
    //constraints[5]  =   traceA;
}
#if defined(GPUOffloadingOMP)
#pragma omp end declare target
#endif

#if defined(GPUOffloadingOMP)
#pragma omp declare target
#endif
void exagrype::ccz4::ThetaOutputNCP(double* NCPterm, const double* const Q, const double* const gradQSerialised, int normal)
{
    const double alpha = std::exp(std::fmax(-20., std::fmin(20.,Q[16])));
    //printf("alpha %f\n",alpha);
    const double alpha2 = alpha*alpha;
    double fa  = 1.0;
    double faa = 0.0;
    fa  =  2./alpha;
    faa = -fa/alpha;
    const double traceK = Q[53];

    constexpr int nVar(59);

    double gradQin[59][3] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for (int i=0; i<nVar; i++) {
      gradQin[i][normal] = gradQSerialised[i];
      //if (gradQSerialised[i]>0.0) std::cout<< gradQSerialised[i] << std::endl; 
    }

    // Note g_cov is symmetric
    const double g_cov[3][3] = { {Q[0], Q[1], Q[2]}, {Q[1], Q[3], Q[4]}, {Q[2], Q[4], Q[5]} };
    const double invdet = 1./( Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] -Q[2]*Q[2]*Q[3]);

    const double g_contr[3][3] = {
        { ( Q[3]*Q[5]-Q[4]*Q[4])*invdet, -( Q[1]*Q[5]-Q[2]*Q[4])*invdet, -(-Q[1]*Q[4]+Q[2]*Q[3])*invdet},
        {-( Q[1]*Q[5]-Q[4]*Q[2])*invdet,  ( Q[0]*Q[5]-Q[2]*Q[2])*invdet, -( Q[0]*Q[4]-Q[2]*Q[1])*invdet},
        {-(-Q[1]*Q[4]+Q[3]*Q[2])*invdet, -( Q[0]*Q[4]-Q[1]*Q[2])*invdet,  ( Q[0]*Q[3]-Q[1]*Q[1])*invdet}
    };
   const double dPP[3][3] = {
        {gradQin[55][0],gradQin[56][0],gradQin[57][0]},
        {gradQin[55][1],gradQin[56][1],gradQin[57][1]},
        {gradQin[55][2],gradQin[56][2],gradQin[57][2]}
    };

    // NOTE Aex is symmetric
    double Aex[3][3] = { {Q[6], Q[7], Q[8]}, {Q[7], Q[9], Q[10]}, {Q[8], Q[10], Q[11]} };

    double traceA = 0;
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) traceA+=g_contr[i][j]*Aex[i][j];
    //traceA *= 1./3;

    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Aex[i][j] -= 1./3. * traceA * g_cov[i][j];

    // Matrix multiplications Amix = matmul(g_contr, Aex) Aup  = matmul(g_contr, mytranspose(Amix))
    double Amix[3][3]={0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Amix[i][j] += g_contr[i][u] * Aex[u][j];
    double Aup[3][3]={0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Aup[i][j] += g_contr[i][u] * Amix[j][u]; // Note the transposition is in the indices

    const double DD[3][3][3] = {
        {{Q[35], Q[36], Q[37]}, {Q[36], Q[38], Q[39]}, {Q[37], Q[39], Q[40]}},
        {{Q[41], Q[42], Q[43]}, {Q[42], Q[44], Q[45]}, {Q[43], Q[45], Q[46]}},
        {{Q[47], Q[48], Q[49]}, {Q[48], Q[50], Q[51]}, {Q[49], Q[51], Q[52]}}
    };
    double dgup[3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int k = 0; k < 3; k++)
    for (int m = 0; m < 3; m++)
    for (int l = 0; l < 3; l++)
    for (int n = 0; n < 3; n++)
    for (int j = 0; j < 3; j++) dgup[k][m][l] -= g_contr[m][n]*g_contr[j][l]*2*DD[k][n][j];

    const double PP[3] = {Q[55], Q[56], Q[57]};

    double Christoffel[3][3][3]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double Christoffel_tilde[3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //double Christoffel_kind1[3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    {
        //Christoffel_kind1[i][j][k] = DD[k][i][j] + DD[j][i][k] - DD[i][j][k];
        for (int l = 0; l < 3; l++)
        {
            Christoffel_tilde[i][j][k] += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] );
            Christoffel[i][j][k]       += g_contr[k][l] * ( DD[i][j][l] + DD[j][i][l] - DD[l][i][j] ) - g_contr[k][l] * ( g_cov[j][l] * PP[i] + g_cov[i][l] * PP[j] - g_cov[i][j] * PP[l] );
        }
    }

    double Gtilde[3] = {0,0,0};
    for (int l = 0; l < 3; l++)
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++) Gtilde[i] += g_contr[j][l] * Christoffel_tilde[j][l][i];

    const double Ghat[3] = {Q[13], Q[14], Q[15]};

    const double phi = std::exp(std::fmax(-20., std::fmin(20.,Q[54])));
    const double phi2 = phi*phi;

    double Z[3] = {0,0,0};
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Z[i] += 0.5*( g_cov[i][j]* (Ghat[j] - Gtilde[j]));
    double Zup[3] = {0,0,0};
    for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) Zup[i] += phi2 * g_contr[i][j] * Z[j];


    const double dDD[3][3][3][3] = {
        {
            {
                {gradQin[35][0],gradQin[36][0],gradQin[37][0]}, {gradQin[36][0],gradQin[38][0],gradQin[39][0]}, {gradQin[37][0],gradQin[39][0],gradQin[40][0]},
            },
            {
                {gradQin[41][0],gradQin[42][0],gradQin[43][0]}, {gradQin[42][0],gradQin[44][0],gradQin[45][0]}, {gradQin[43][0],gradQin[45][0],gradQin[46][0]},
            },
            {
                {gradQin[47][0],gradQin[48][0],gradQin[49][0]}, {gradQin[48][0],gradQin[50][0],gradQin[51][0]}, {gradQin[49][0],gradQin[51][0],gradQin[52][0]}
            }
        },
        {
            {
                {gradQin[35][1],gradQin[36][1],gradQin[37][1]}, {gradQin[36][1],gradQin[38][1],gradQin[39][1]}, {gradQin[37][1],gradQin[39][1],gradQin[40][1]},
            },
            {
                {gradQin[41][1],gradQin[42][1],gradQin[43][1]}, {gradQin[42][1],gradQin[44][1],gradQin[45][1]}, {gradQin[43][1],gradQin[45][1],gradQin[46][1]},
            },
            {
                {gradQin[47][1],gradQin[48][1],gradQin[49][1]}, {gradQin[48][1],gradQin[50][1],gradQin[51][1]}, {gradQin[49][1],gradQin[51][1],gradQin[52][1]}
            }
        },
        {
            {
                {gradQin[35][2],gradQin[36][2],gradQin[37][2]}, {gradQin[36][2],gradQin[38][2],gradQin[39][2]}, {gradQin[37][2],gradQin[39][2],gradQin[40][2]},
            },
            {
                {gradQin[41][2],gradQin[42][2],gradQin[43][2]}, {gradQin[42][2],gradQin[44][2],gradQin[45][2]}, {gradQin[43][2],gradQin[45][2],gradQin[46][2]},
            },
            {
                {gradQin[47][2],gradQin[48][2],gradQin[49][2]}, {gradQin[48][2],gradQin[50][2],gradQin[51][2]}, {gradQin[49][2],gradQin[51][2],gradQin[52][2]}
            }
        }
    };


    double dChristoffelNCP[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    double dChristoffel_tildeNCP[3][3][3][3] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++)
    {
        dChristoffelNCP      [k][i][j][m] = 0;
        dChristoffel_tildeNCP[k][i][j][m] = 0;
        for (int l = 0; l < 3; l++)
        {
            dChristoffelNCP[k][i][j][m] +=  0.5*g_contr[m][l] * (
                    dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]
                    - g_cov[j][l]*(dPP[k][i] + dPP[i][k]) - g_cov[i][l]*(dPP[k][j]+dPP[j][k]) +  g_cov[i][j]*(dPP[k][l]+dPP[l][k]) );

            dChristoffel_tildeNCP[k][i][j][m] += 0.5*g_contr[m][l]*(dDD[k][i][j][l] + dDD[i][k][j][l] + dDD[k][j][i][l] + dDD[j][k][i][l] - dDD[k][l][i][j] - dDD[l][k][i][j]);

        }
    }

    double RiemannNCP[3][3][3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int m = 0; m < 3; m++)
    for (int k = 0; k < 3; k++) RiemannNCP[i][k][j][m] = dChristoffelNCP[k][i][j][m] - dChristoffelNCP[j][i][k][m];

    double RicciNCP[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int m = 0; m < 3; m++)
    for (int n = 0; n < 3; n++)
    for (int l = 0; l < 3; l++) RicciNCP[m][n] += RiemannNCP[m][l][n][l];

    double dGtildeNCP[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
    for (int j = 0; j < 3; j++)
    for (int l = 0; l < 3; l++) dGtildeNCP[k][i] += g_contr[j][l]*dChristoffel_tildeNCP[k][j][l][i];


    const double dGhat[3][3] = {
        {gradQin[13][0],gradQin[14][0],gradQin[15][0]},
        {gradQin[13][1],gradQin[14][1],gradQin[15][1]},
        {gradQin[13][2],gradQin[14][2],gradQin[15][2]}
    };

    double dZNCP[3][3] = {0,0,0,0,0,0,0,0,0};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++) dZNCP[k][i] += 0.5*g_cov[i][j]*(dGhat[k][j]-dGtildeNCP[k][j]);

    double RicciPlusNablaZNCP[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) RicciPlusNablaZNCP[i][j] = RicciNCP[i][j] + dZNCP[i][j] + dZNCP[j][i];

    double RPlusTwoNablaZNCP = 0;
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) RPlusTwoNablaZNCP += g_contr[i][j]*RicciPlusNablaZNCP[i][j];
    RPlusTwoNablaZNCP*=phi2;

    NCPterm[0]  =   RPlusTwoNablaZNCP;
    //if (NCPterm>0.0) std::cout<< NCPterm << std::endl; 
}
#if defined(GPUOffloadingOMP)
#pragma omp end declare target
#endif


#if defined(GPUOffloadingOMP)
#pragma omp declare target
#endif
void exagrype::ccz4::maxEigenvalue(
        const double* const Q,
        int                 normal,
        const double        CCZ4e,
        const double        CCZ4ds,
        const double        CCZ4GLMc,
        const double        CCZ4GLMd,
        double*             maxEigenvalue
) {
  const double np = 1.0;
  const double qmin = std::min({Q[0],Q[3],Q[5]});
  const double alpha = std::max({1.0, Q[16]}) * std::max({1.0, pow(Q[54],1.0/np)})/std::sqrt(qmin);

  constexpr double sqrtwo = 1.4142135623730951;
  const double tempA = alpha * std::max({sqrtwo, CCZ4e, CCZ4ds, CCZ4GLMc/alpha, CCZ4GLMd/alpha});
  const double tempB = Q[17+normal];//DOT_PRODUCT(Q(18:20),nv(:))
  maxEigenvalue[0] = std::max({1.0, std::abs(-tempA-tempB), std::abs(tempA-tempB)});
}

//this version use alpha and phi
void exagrype::ccz4::source(double* S, const double* const Q,
      const int CCZ4LapseType,
      const double CCZ4ds,
      const double CCZ4c,
      const double CCZ4e,
      const double CCZ4f,
      const double CCZ4bs,
      const double CCZ4sk,
      const double CCZ4xi,
      const double CCZ4itau,
      const double CCZ4eta,
      const double CCZ4k1,
      const double CCZ4k2,
      const double CCZ4k3,
      const double CCZ4SO
      )
{
    // Vacuum CCZ4: 59 entries, with symmetric tensor contractions.
    const double CCZ4helper = (CCZ4SO == 1) ? 0.0 : 1.0;

    // Preserve the upstream lapse floor and alpha (not log(alpha)) variables.
    const double alpha = std::fmax(1e-4,Q[16]);

    // Match CCZ4PlusScalarKernels: 0 = harmonic, 1 = 1+log,
    // all other values disable the slicing driver (advection is retained).
    double fa  = 0.0;
    double faa = 0.0;
    if (CCZ4LapseType == 0)
    {
      fa = 1.0;
    }
    else if (CCZ4LapseType == 1)
    {
      fa  =  2./alpha;
      faa = -fa/alpha;
    }

    const double np = 1.0;
    const int CCZ4Sa = 0;
    const int CCZ4Sp = 0;

    const double g_cov[3][3] = { {Q[0], Q[1], Q[2]},
                                 {Q[1], Q[3], Q[4]},
                                 {Q[2], Q[4], Q[5]} };
    const double det = Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] - Q[2]*Q[2]*Q[3];
    const double invdet = 1./det;
    const double g_contr_12 = -( Q[1]*Q[5]-Q[2]*Q[4])*invdet;
    const double g_contr_13 = -(-Q[1]*Q[4]+Q[2]*Q[3])*invdet;
    const double g_contr_23 = -( Q[0]*Q[4]-Q[2]*Q[1])*invdet;
    const double g_contr[3][3] = {{(Q[3]*Q[5]-Q[4]*Q[4])*invdet, g_contr_12, g_contr_13},
                                  {g_contr_12, (Q[0]*Q[5]-Q[2]*Q[2])*invdet, g_contr_23},
                                  {g_contr_13, g_contr_23, (Q[0]*Q[3]-Q[1]*Q[1])*invdet}};

    double Aex[3][3] = { {Q[6], Q[7], Q[8]},
                         {Q[7], Q[9], Q[10]},
                         {Q[8], Q[10], Q[11]} };

    const double trAex = g_contr[0][0] * Aex[0][0]
                       + g_contr[1][1] * Aex[1][1]
                       + g_contr[2][2] * Aex[2][2]
                       + 2.0 * ( g_contr[0][1] * Aex[0][1]
                               + g_contr[0][2] * Aex[0][2]
                               + g_contr[1][2] * Aex[1][2] );

    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        Aex[i][j] -= 1./3. * trAex * g_cov[i][j];
        Aex[j][i] = Aex[i][j];
    }

    double Amix[3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    for (int u = 0; u < 3; u++) Amix[i][j] += g_contr[i][u] * Aex[u][j];

    double Aup[3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        for (int u = 0; u < 3; u++) Aup[i][j] += g_contr[i][u] * Amix[j][u];
        Aup[j][i] = Aup[i][j];
    }

    const double Theta = Q[12];

    const double DD[3][3][3] = {{ {Q[35], Q[36], Q[37]},
                                  {Q[36], Q[38], Q[39]},
                                  {Q[37], Q[39], Q[40]} },
                                { {Q[41], Q[42], Q[43]},
                                  {Q[42], Q[44], Q[45]},
                                  {Q[43], Q[45], Q[46]} },
                                { {Q[47], Q[48], Q[49]},
                                  {Q[48], Q[50], Q[51]},
                                  {Q[49], Q[51], Q[52]} }};

    double DDup[3][3][3] = {0};
    for (int k = 0; k < 3; k++)
    for (int m = 0; m < 3; m++)
    for (int l = m; l < 3; l++)
    {
        for (int n = 0; n < 3; n++)
        for (int j = 0; j < 3; j++) DDup[k][m][l] += g_contr[m][n] * g_contr[l][j] * DD[k][n][j];

    }

    const double phi = std::fmax(1e-2,pow(Q[54],(1.0/np)));
    const double phi2 = phi*phi;
    const double chi = pow(phi,np);

    const double PP[3] = {Q[55], Q[56], Q[57]};

    double Christoffel_tilde_down[3][3][3] = {0};
    double Christoffel_tilde[3][3][3] = {0};
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        for (int k = 0; k < 3; k++)
        {
            Christoffel_tilde_down[k][i][j] = DD[i][j][k] + DD[j][i][k] - DD[k][i][j];
            Christoffel_tilde_down[k][j][i] = Christoffel_tilde_down[k][i][j];
        }
        for (int k = 0; k < 3; k++)
        {
            for (int l = 0; l < 3; l++) Christoffel_tilde[i][j][k] += g_contr[k][l] * Christoffel_tilde_down[l][i][j];
            Christoffel_tilde[j][i][k] = Christoffel_tilde[i][j][k];
        }
    }

    double Gtilde[3] = {0};
    for (int i = 0; i < 3; i++)
    {
        Gtilde[i] = g_contr[0][0] * Christoffel_tilde[0][0][i]
                  + g_contr[1][1] * Christoffel_tilde[1][1][i]
                  + g_contr[2][2] * Christoffel_tilde[2][2][i]
                  + 2.0 * ( g_contr[0][1] * Christoffel_tilde[0][1][i]
                          + g_contr[0][2] * Christoffel_tilde[0][2][i]
                          + g_contr[1][2] * Christoffel_tilde[1][2][i] );
    }

    const double Ghat[3] = {Q[13], Q[14], Q[15]};

    // Keep DeltaGamma unscaled. Apply ds only to terms inherited from Z_i,
    // including its contributions to Riccihat, Theta and Gammahat.
    double DeltaGamma[3];
    for (int i = 0; i < 3; i++) DeltaGamma[i] = Ghat[i] - Gtilde[i];

    double Zup[3] = {0};
    for (int i = 0; i < 3; i++) Zup[i] = 0.5 * CCZ4ds * phi2 * DeltaGamma[i];

    const double PPdotPP = g_contr[0][0] * PP[0] * PP[0]
                         + g_contr[1][1] * PP[1] * PP[1]
                         + g_contr[2][2] * PP[2] * PP[2]
                         + 2.0 * ( g_contr[0][1] * PP[0] * PP[1]
                                 + g_contr[0][2] * PP[0] * PP[2]
                                 + g_contr[1][2] * PP[1] * PP[2] );

    double PPdotGtilde = 0;
    for (int k = 0; k < 3; k++) PPdotGtilde += PP[k] * Gtilde[k];

    const double AA[3] = {Q[23], Q[24], Q[25]};

    double AAdotPP = 0;
    for (int k = 0; k < 3; k++)
    for (int l = 0; l < 3; l++) AAdotPP += g_contr[k][l] * AA[k] * PP[l];

    const double Va = (CCZ4Sa == 0) ? 1.0 : alpha;
    const double Vp = (CCZ4Sp == 0) ? 1.0/np/chi : 1.0;

    double Riccihat[3][3];
    const double Vp2 = Vp * Vp;
    const double Riccihatfactor1 = CCZ4Sp - (np - 1.0) * Vp2;
    const double Riccihatfactor2 = CCZ4Sp - (np + 1.0) * Vp2;

    for (int i = 0; i < 3; i++)

    for (int j = i; j < 3; j++)
    {
        double temp1=0, temp2=0, temp3=0, temp4=0;
        for (int k = 0; k < 3; k++)
        {
            temp1 += Ghat[k] * DD[k][i][j];
            temp2 += PP[k] * Christoffel_tilde[i][j][k];
            temp3 += DeltaGamma[k] * ( g_cov[i][k] * PP[j]
                                     + g_cov[j][k] * PP[i]
                                     - g_cov[i][j] * PP[k] );
            for (int l = 0; l < 3; l++)
            for (int m = 0; m < 3; m++)
            {
                temp4 += g_contr[l][m] * ( Christoffel_tilde[l][i][k] * Christoffel_tilde_down[j][k][m]
                                         + Christoffel_tilde[l][j][k] * Christoffel_tilde_down[i][k][m]
                                         + Christoffel_tilde[i][m][k] * Christoffel_tilde_down[k][l][j] );
            }
        }
        Riccihat[i][j] = Riccihatfactor1 * PP[i] * PP[j]
                       + temp1 - Vp * temp2 + CCZ4ds * Vp * temp3 + temp4
                       + (Riccihatfactor2 * PPdotPP - Vp * PPdotGtilde) * g_cov[i][j];
        Riccihat[j][i] = Riccihat[i][j];
    }

    double trRiccihat = g_contr[0][0] * Riccihat[0][0]
                      + g_contr[1][1] * Riccihat[1][1]
                      + g_contr[2][2] * Riccihat[2][2]
                      + 2.0 * ( g_contr[0][1] * Riccihat[0][1]
                              + g_contr[0][2] * Riccihat[0][2]
                              + g_contr[1][2] * Riccihat[1][2] );
    double TrRiccihat = phi2 * trRiccihat;

    double DDalpha[3][3] = {0};

    for (int i = 0; i < 3; i++)

    for (int j = i; j < 3; j++)
    {

        double temp = 0;
        for (int k = 0; k < 3; k++) temp += Christoffel_tilde[i][j][k] * AA[k];
        DDalpha[i][j] = Va * Vp * (PP[i] * AA[j] + PP[j] * AA[i] - AAdotPP * g_cov[i][j])
                      + CCZ4Sa * alpha * AA[i] * AA[j]
                      - Va * temp;
        DDalpha[j][i] = DDalpha[i][j];

    }

    double trDDalpha = g_contr[0][0] * DDalpha[0][0]
                     + g_contr[1][1] * DDalpha[1][1]
                     + g_contr[2][2] * DDalpha[2][2]
                     + 2.0 * ( g_contr[0][1] * DDalpha[0][1]
                             + g_contr[0][2] * DDalpha[0][2]
                             + g_contr[1][2] * DDalpha[1][2] );
    double TrDDalpha = phi2 * trDDalpha;

    double SecondOrderTerms[3][3];

    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        SecondOrderTerms[i][j] = alpha * Riccihat[i][j] - DDalpha[i][j];
        SecondOrderTerms[j][i] = SecondOrderTerms[i][j];
    }

    double trSecondOrderTerms = alpha * trRiccihat - trDDalpha;

    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        SecondOrderTerms[i][j] -= 1./3 * trSecondOrderTerms * g_cov[i][j];
        SecondOrderTerms[j][i] = SecondOrderTerms[i][j];
    }

    const double BB[3][3] = { {Q[26], Q[27], Q[28]},
                              {Q[29], Q[30], Q[31]},
                              {Q[32], Q[33], Q[34]} };

    double traceBB = BB[0][0] + BB[1][1] + BB[2][2];
    const double beta[3] = {Q[17], Q[18], Q[19]};

    double dtgamma[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        dtgamma[i][j] = -2.0 * alpha * Aex[i][j]
                      - CCZ4itau * (det - 1.0) * g_cov[i][j]
                      - 2.0 / 3.0 * g_cov[i][j] * traceBB;
        double temp = 0;
        for (int k = 0; k < 3; k++)
        {
            dtgamma[i][j] += g_cov[k][i] * BB[j][k] + g_cov[k][j] * BB[i][k];
            temp += beta[k] * DD[k][i][j];
        }
        dtgamma[i][j] += (1 - CCZ4SO) * 2.0 * temp;
    }

    const double Kex = Q[53];

    double dtAex[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        dtAex[i][j] = phi2 * SecondOrderTerms[i][j]
                    + alpha * Aex[i][j] * (Kex - 2. * Theta * CCZ4c)
                    - 2./3. * Aex[i][j] * traceBB;
        for (int k = 0; k < 3; k++)
        {
            dtAex[i][j] +=  -2. * alpha * Aex[i][k] * Amix[k][j]
                         + Aex[k][i] * BB[j][k]
                         + Aex[k][j] * BB[i][k];
        }
    }

    const double K0 = Q[58];

    // k1 is already lapse-rescaled: retain k1, not alpha*k1, in damping.
    double dtKex = -TrDDalpha
                 + alpha * (TrRiccihat + Kex * Kex - 2.0 * CCZ4c * Theta * Kex)
                 - 3.0 * CCZ4k1 * (1.0 + CCZ4k2) * Theta;

    double dtchi = (1 - CCZ4SO) * ((CCZ4Sp == 0) ? 1.0 : phi) * (beta[0]*PP[0]+beta[1]*PP[1]+beta[2]*PP[2])
                 + np / 3. * (alpha * Kex - traceBB) * chi;

    double dtalpha = - alpha * alpha * fa * (Kex - K0 - 2. * CCZ4c * Theta)
                   + (1 - CCZ4SO) * Va * (beta[0]*AA[0]+beta[1]*AA[1]+beta[2]*AA[2]);

    const double AupAdown = Aex[0][0] * Aup[0][0]
                          + Aex[1][1] * Aup[1][1]
                          + Aex[2][2] * Aup[2][2]
                          + 2.0 * ( Aex[0][1] * Aup[0][1]
                                  + Aex[0][2] * Aup[0][2]
                                  + Aex[1][2] * Aup[1][2] );

    double dtTheta = 0.5 * alpha * CCZ4e * CCZ4e * ( TrRiccihat - AupAdown + 2./3. * Kex * Kex )
                   - alpha * CCZ4c * Theta * Kex
                   - CCZ4k1 * (2. + CCZ4k2) * Theta
                   - Va * (Zup[0]*AA[0]+Zup[1]*AA[1]+Zup[2]*AA[2]);

    // II_k = 2 alpha D_k^{ij} A_ij; only upper-triangular DDup is read.
    double II[3];
    for (int i = 0; i < 3; i++)
    {
        II[i] = 2.0 * alpha * ( DDup[i][0][0] * Aex[0][0]
                              + DDup[i][1][1] * Aex[1][1]
                              + DDup[i][2][2] * Aex[2][2]
                              + 2.0 * ( DDup[i][0][1] * Aex[0][1]
                                      + DDup[i][0][2] * Aex[0][2]
                                     + DDup[i][1][2] * Aex[1][2] ) );
    }

    double dtGhat[3];
    for (int i = 0; i < 3; i++)
    {
        double temp1=0, temp2=0, temp3=0, temp4=0, temp5=0, temp7=0;
        for (int j = 0; j < 3; j++)
        {
            temp1 += Gtilde[j] * BB[j][i];
            temp2 += g_contr[i][j] * AA[j];
            temp3 += DeltaGamma[j] * BB[j][i];
            temp4 += Aup[i][j] * AA[j];
            temp5 += Aup[i][j] * PP[j];
            temp7 += g_contr[i][j] * II[j];
        }
        const double temp6 = Christoffel_tilde[0][0][i] * Aup[0][0]
                           + Christoffel_tilde[1][1][i] * Aup[1][1]
                           + Christoffel_tilde[2][2][i] * Aup[2][2]
                           + 2.0 * ( Christoffel_tilde[0][1][i] * Aup[0][1]
                                   + Christoffel_tilde[0][2][i] * Aup[0][2]
                                   + Christoffel_tilde[1][2][i] * Aup[1][2] );

        dtGhat[i] = 2./3. * traceBB * ( Gtilde[i] + CCZ4k3 * CCZ4ds * DeltaGamma[i] )
                  - ( 2./3. * alpha * Kex + CCZ4k1 ) * CCZ4ds * DeltaGamma[i]
                  - temp1
                  - 2. * Va * Theta * temp2
                  - CCZ4k3 * CCZ4ds * temp3
                  - 2. * Va * temp4
                  - 6. * alpha * Vp * temp5
                  + 2. * alpha * temp6
                  - 2. * CCZ4helper * CCZ4sk * temp7;
    }

    const double myb[3] = {Q[20], Q[21], Q[22]};

    double dtmyb[3];
    for (int i = 0; i < 3; i++) dtmyb[i] = CCZ4sk * ( CCZ4xi * dtGhat[i] - CCZ4eta * myb[i] );

    // bs controls shift advection; sk controls the shift driver.
    // SO primary-variable advection is handled outside this source term.
    double dtbeta[3];

    for (int i = 0; i < 3; i++)
    {
        dtbeta[i] = CCZ4f * myb[i]
                   + CCZ4bs * (1 - CCZ4SO) * (beta[0]*BB[0][i]+beta[1]*BB[1][i]+beta[2]*BB[2][i]);

        dtbeta[i] *= CCZ4sk;
    }

    double dtAA[3];
    double iVa = (CCZ4Sa == 0) ? alpha : 1.0;

    for (int i = 0; i < 3; i++)
    {

      dtAA[i] = - alpha * AA[i] * ( (2. - CCZ4Sa) * fa + alpha * faa ) * ( Kex - K0 - 2. * CCZ4c * Theta )
             + CCZ4sk * iVa * fa * II[i]
             + BB[i][0]*AA[0]+BB[i][1]*AA[1]+BB[i][2]*AA[2];
    }

    double dtBB[3][3];

    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    {
        double temp = 0.0;
        for (int k = 0; k < 3; k++)
            temp += BB[j][k] * BB[k][i];
        dtBB[j][i] = CCZ4sk * temp;
    }

    double dtDD[3][3][3];

    for (int k = 0; k < 3; k++)
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        double temp = 0;
        for (int l = 0; l < 3; l++)
        {
            temp += DD[l][i][j] * BB[k][l]
                  + DD[k][l][i] * BB[j][l]
                  + DD[k][l][j] * BB[i][l];
        }
        dtDD[k][i][j] = - Va * AA[k] * Aex[i][j]
                      - 2./3. * traceBB * DD[k][i][j]
                      - 1./3. * g_cov[i][j] * II[k]
                      + temp;

    }

    double dtPP[3];
    for (int i = 0; i < 3; i++)
    {
        double temp = 0;
        for (int j = 0; j < 3; j++) temp += BB[i][j] * PP[j];
        dtPP[i] = 1./3. / Vp * ( Va * Kex * AA[i] - CCZ4helper * CCZ4sk * II[i] )
                + (1 - CCZ4Sp) * np/3. * (alpha * Kex - traceBB) * PP[i]
                + temp;
    }

    S[0]  = dtgamma[0][0];
    S[1]  = dtgamma[0][1];
    S[2]  = dtgamma[0][2];
    S[3]  = dtgamma[1][1];
    S[4]  = dtgamma[1][2];
    S[5]  = dtgamma[2][2];
    S[6]  = dtAex[0][0];
    S[7]  = dtAex[0][1];
    S[8]  = dtAex[0][2];
    S[9]  = dtAex[1][1];
    S[10] = dtAex[1][2];
    S[11] = dtAex[2][2];

    S[12] = dtTheta;
    for (int i = 0; i < 3; i++) S[13+i] = dtGhat[i];
    S[16] = dtalpha;
    for (int i = 0; i < 3; i++) S[17+i] = dtbeta[i];
    for (int i = 0; i < 3; i++) S[20+i] = dtmyb[i];
    for (int i = 0; i < 3; i++) S[23+i] = dtAA[i];
    S[26] = dtBB[0][0];
    S[27] = dtBB[0][1];
    S[28] = dtBB[0][2];
    S[29] = dtBB[1][0];
    S[30] = dtBB[1][1];
    S[31] = dtBB[1][2];
    S[32] = dtBB[2][0];
    S[33] = dtBB[2][1];
    S[34] = dtBB[2][2];
    S[35] = dtDD[0][0][0];
    S[36] = dtDD[0][0][1];
    S[37] = dtDD[0][0][2];
    S[38] = dtDD[0][1][1];
    S[39] = dtDD[0][1][2];
    S[40] = dtDD[0][2][2];
    S[41] = dtDD[1][0][0];
    S[42] = dtDD[1][0][1];
    S[43] = dtDD[1][0][2];
    S[44] = dtDD[1][1][1];
    S[45] = dtDD[1][1][2];
    S[46] = dtDD[1][2][2];
    S[47] = dtDD[2][0][0];
    S[48] = dtDD[2][0][1];
    S[49] = dtDD[2][0][2];
    S[50] = dtDD[2][1][1];
    S[51] = dtDD[2][1][2];
    S[52] = dtDD[2][2][2];
    S[53] = dtKex;
    S[54] = dtchi;
    for (int i = 0; i < 3; i++) S[55+i] = dtPP[i];
    S[58] = 0;

    // Reconstructed auxiliary variables are not evolved in SO mode.
    if (CCZ4SO==1){
        for (int i = 23; i < 53; i++) S[i] = 0.0;
        for (int i = 0; i < 3; i++) S[55+i] = 0.0;
    }

}

void exagrype::ccz4::ncp(double* BgradQ, const double* const Q, const double* const gradQSerialised, const int normal,
      const int CCZ4LapseType,
      const double CCZ4ds,
      const double CCZ4c,
      const double CCZ4e,
      const double CCZ4f,
      const double CCZ4bs,
      const double CCZ4sk,
      const double CCZ4xi,
      const double CCZ4mu,
      const double CCZ4SO
      )
{
    // Kept in the public interface; upstream has no ds-dependent NCP term.
    (void)CCZ4ds;
    const double CCZ4helper = (CCZ4SO==1) ? 0.0 : 1.0;

    // Advection is supplied by the solver separately; avoid double counting.
    const double advec = 0.0;
    const double advec_aux = 0.0;

    const double alpha = std::fmax(1e-4,Q[16]);

    const double alpha2 = alpha * alpha;
    // Same slicing-driver mapping as source() and CCZ4PlusScalarKernels.
    double fa  = 0.0;
    if (CCZ4LapseType == 0)
    {
      fa = 1.0;
    }
    else if (CCZ4LapseType == 1)
    {
      fa  =  2./alpha;
    }

    const double np = 1.0;
    const int CCZ4Sa = 0;
    const int CCZ4Sp = 0;

    constexpr int nVar(59);

    double gradQin[59][3] = {0};

    // Direction-major input: [normal * 59 + variable].
    for (int i = 0; i < nVar; i++) gradQin[i][normal] = gradQSerialised[i+normal*nVar];

    const double g_cov[3][3] = { {Q[0], Q[1], Q[2]},
                                 {Q[1], Q[3], Q[4]},
                                 {Q[2], Q[4], Q[5]} };
    const double invdet = 1./(Q[0]*Q[3]*Q[5] - Q[0]*Q[4]*Q[4] - Q[1]*Q[1]*Q[5] + 2*Q[1]*Q[2]*Q[4] - Q[2]*Q[2]*Q[3]);

    const double g_contr_12 = -( Q[1]*Q[5]-Q[2]*Q[4])*invdet;
    const double g_contr_13 = -(-Q[1]*Q[4]+Q[2]*Q[3])*invdet;
    const double g_contr_23 = -( Q[0]*Q[4]-Q[2]*Q[1])*invdet;
    const double g_contr[3][3] = {{(Q[3]*Q[5]-Q[4]*Q[4])*invdet, g_contr_12, g_contr_13},
                                  {g_contr_12, (Q[0]*Q[5]-Q[2]*Q[2])*invdet, g_contr_23},
                                  {g_contr_13, g_contr_23, (Q[0]*Q[3]-Q[1]*Q[1])*invdet}};

    const double phi = std::fmax(1e-2,pow(Q[54],1.0/np));
    const double chi  = pow(phi,np);
    const double phi2 = phi*phi;

    double dPP[3][3] = { {gradQin[55][0],gradQin[56][0],gradQin[57][0]},
                         {gradQin[55][1],gradQin[56][1],gradQin[57][1]},
                         {gradQin[55][2],gradQin[56][2],gradQin[57][2]} };

    double dAA[3][3] = { {gradQin[23][0],gradQin[24][0],gradQin[25][0]},
                         {gradQin[23][1],gradQin[24][1],gradQin[25][1]},
                         {gradQin[23][2],gradQin[24][2],gradQin[25][2]} };

    const double dDD[3][3][3][3] = {
        {
            {
                {gradQin[35][0],gradQin[36][0],gradQin[37][0]}, {gradQin[36][0],gradQin[38][0],gradQin[39][0]}, {gradQin[37][0],gradQin[39][0],gradQin[40][0]},
            },
            {
                {gradQin[41][0],gradQin[42][0],gradQin[43][0]}, {gradQin[42][0],gradQin[44][0],gradQin[45][0]}, {gradQin[43][0],gradQin[45][0],gradQin[46][0]},
            },
            {
                {gradQin[47][0],gradQin[48][0],gradQin[49][0]}, {gradQin[48][0],gradQin[50][0],gradQin[51][0]}, {gradQin[49][0],gradQin[51][0],gradQin[52][0]}
            }
        },
        {
            {
                {gradQin[35][1],gradQin[36][1],gradQin[37][1]}, {gradQin[36][1],gradQin[38][1],gradQin[39][1]}, {gradQin[37][1],gradQin[39][1],gradQin[40][1]},
            },
            {
                {gradQin[41][1],gradQin[42][1],gradQin[43][1]}, {gradQin[42][1],gradQin[44][1],gradQin[45][1]}, {gradQin[43][1],gradQin[45][1],gradQin[46][1]},
            },
            {
                {gradQin[47][1],gradQin[48][1],gradQin[49][1]}, {gradQin[48][1],gradQin[50][1],gradQin[51][1]}, {gradQin[49][1],gradQin[51][1],gradQin[52][1]}
            }
        },
        {
            {
                {gradQin[35][2],gradQin[36][2],gradQin[37][2]}, {gradQin[36][2],gradQin[38][2],gradQin[39][2]}, {gradQin[37][2],gradQin[39][2],gradQin[40][2]},
            },
            {
                {gradQin[41][2],gradQin[42][2],gradQin[43][2]}, {gradQin[42][2],gradQin[44][2],gradQin[45][2]}, {gradQin[43][2],gradQin[45][2],gradQin[46][2]},
            },
            {
                {gradQin[47][2],gradQin[48][2],gradQin[49][2]}, {gradQin[48][2],gradQin[50][2],gradQin[51][2]}, {gradQin[49][2],gradQin[51][2],gradQin[52][2]}
            }
        }
    };

    const double dGhat[3][3] = {
        {gradQin[13][0],gradQin[14][0],gradQin[15][0]},
        {gradQin[13][1],gradQin[14][1],gradQin[15][1]},
        {gradQin[13][2],gradQin[14][2],gradQin[15][2]}
    };

    double Vp = (CCZ4Sp == 0) ? 1.0/np/chi : 1.0;

    const double trdPP = g_contr[0][0] * dPP[0][0]
                       + g_contr[1][1] * dPP[1][1]
                       + g_contr[2][2] * dPP[2][2]
                       + g_contr[0][1] * (dPP[0][1] + dPP[1][0])
                       + g_contr[0][2] * (dPP[0][2] + dPP[2][0])
                       + g_contr[1][2] * (dPP[1][2] + dPP[2][1]);

    double Riccihat[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        double temp1 = g_cov[0][i] * dGhat[j][0] + g_cov[0][j] * dGhat[i][0]
                     + g_cov[1][i] * dGhat[j][1] + g_cov[1][j] * dGhat[i][1]
                     + g_cov[2][i] * dGhat[j][2] + g_cov[2][j] * dGhat[i][2];
        double temp2 = g_contr[0][0] * dDD[0][0][i][j]
                     + g_contr[1][1] * dDD[1][1][i][j]
                     + g_contr[2][2] * dDD[2][2][i][j]
                     + g_contr[0][1] * (dDD[0][1][i][j] + dDD[1][0][i][j])
                     + g_contr[0][2] * (dDD[0][2][i][j] + dDD[2][0][i][j])
                     + g_contr[1][2] * (dDD[1][2][i][j] + dDD[2][1][i][j]);
        Riccihat[i][j] = 0.5 * Vp * (dPP[i][j] + dPP[j][i])
                       + 0.5 * temp1 - temp2 + Vp * g_cov[i][j] * trdPP;
        Riccihat[j][i] = Riccihat[i][j];
    }

    double trRiccihat = g_contr[0][0] * Riccihat[0][0]
                      + g_contr[1][1] * Riccihat[1][1]
                      + g_contr[2][2] * Riccihat[2][2]
                      + 2.0 * ( g_contr[0][1] * Riccihat[0][1]
                              + g_contr[0][2] * Riccihat[0][2]
                              + g_contr[1][2] * Riccihat[1][2] );
    double TrRiccihat = phi2 * trRiccihat;

    double Va = (CCZ4Sa == 0) ? 1.0 : alpha;

    double DDalpha[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        DDalpha[i][j] = 0.5 * Va * ( dAA[i][j] + dAA[j][i] );
        DDalpha[j][i] = DDalpha[i][j];
    }

    double trDDalpha = g_contr[0][0] * DDalpha[0][0]
                     + g_contr[1][1] * DDalpha[1][1]
                     + g_contr[2][2] * DDalpha[2][2]
                     + 2.0 * ( g_contr[0][1] * DDalpha[0][1]
                             + g_contr[0][2] * DDalpha[0][2]
                             + g_contr[1][2] * DDalpha[1][2] );
    double TrDDalpha = phi2 * trDDalpha;

    double SecondOrderTerms[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        SecondOrderTerms[i][j] = - DDalpha[i][j] + alpha * Riccihat[i][j];
        SecondOrderTerms[j][i] = SecondOrderTerms[i][j];
    }

    double trSecondOrderTerms = alpha * trRiccihat - trDDalpha;

    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        SecondOrderTerms[i][j] -= 1./3 * trSecondOrderTerms * g_cov[i][j];
        SecondOrderTerms[j][i] = SecondOrderTerms[i][j];
    }

    const double beta[3] = {Q[17], Q[18], Q[19]};

    const double dAex[3][3][3] = {
        {{gradQin[6][0],gradQin[7][0],gradQin[8][0]}, {gradQin[7][0], gradQin[9][0], gradQin[10][0]},  {gradQin[8][0], gradQin[10][0], gradQin[11][0]}},
        {{gradQin[6][1],gradQin[7][1],gradQin[8][1]}, {gradQin[7][1], gradQin[9][1], gradQin[10][1]},  {gradQin[8][1], gradQin[10][1], gradQin[11][1]}},
        {{gradQin[6][2],gradQin[7][2],gradQin[8][2]}, {gradQin[7][2], gradQin[9][2], gradQin[10][2]},  {gradQin[8][2], gradQin[10][2], gradQin[11][2]}}
    };

    double dtAex[3][3];
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)  dtAex[i][j] = phi2 * SecondOrderTerms[i][j]
                                              + advec * (beta[0]*dAex[0][i][j]+beta[1]*dAex[1][i][j]+beta[2]*dAex[2][i][j]);

    const double dKex[3] = {gradQin[53][0], gradQin[53][1], gradQin[53][2]};

    double dtKex = - TrDDalpha + alpha * TrRiccihat + advec * (beta[0]*dKex[0]+beta[1]*dKex[1]+beta[2]*dKex[2]);

    const double dTheta[3] = {gradQin[12][0],gradQin[12][1],gradQin[12][2]};
    double dtTheta = 0.5 * alpha * CCZ4e * CCZ4e * TrRiccihat + advec * (beta[0]*dTheta[0]+beta[1]*dTheta[1]+beta[2]*dTheta[2]);

    const double dBB[3][3][3] = {
        {
            {CCZ4sk*gradQin[26][0],CCZ4sk*gradQin[27][0],CCZ4sk*gradQin[28][0]}, {CCZ4sk*gradQin[29][0],CCZ4sk*gradQin[30][0],CCZ4sk*gradQin[31][0]}, {CCZ4sk*gradQin[32][0],CCZ4sk*gradQin[33][0],CCZ4sk*gradQin[34][0]}
        },
        {
            {CCZ4sk*gradQin[26][1],CCZ4sk*gradQin[27][1],CCZ4sk*gradQin[28][1]}, {CCZ4sk*gradQin[29][1],CCZ4sk*gradQin[30][1],CCZ4sk*gradQin[31][1]}, {CCZ4sk*gradQin[32][1],CCZ4sk*gradQin[33][1],CCZ4sk*gradQin[34][1]}
        },
        {
            {CCZ4sk*gradQin[26][2],CCZ4sk*gradQin[27][2],CCZ4sk*gradQin[28][2]}, {CCZ4sk*gradQin[29][2],CCZ4sk*gradQin[30][2],CCZ4sk*gradQin[31][2]}, {CCZ4sk*gradQin[32][2],CCZ4sk*gradQin[33][2],CCZ4sk*gradQin[34][2]}
        }
    };

    double JJ[3];
    for (int i = 0; i < 3; i++)
    {
        JJ[i] = alpha * ( g_contr[0][0] * dAex[i][0][0]
                        + g_contr[1][1] * dAex[i][1][1]
                        + g_contr[2][2] * dAex[i][2][2]
                        + 2.0 * ( g_contr[0][1] * dAex[i][0][1]
                                + g_contr[0][2] * dAex[i][0][2]
                                + g_contr[1][2] * dAex[i][1][2] ) );
    }

    double dtGhat[3] = {0};
    for (int i = 0; i < 3; i++)
    {
        double temp1=0, temp2=0, temp3=0;
        for (int j = 0; j < 3; j++)
        {
            temp1 += g_contr[i][j] * ( alpha * (dTheta[j] - 2./3. * dKex[j]) + CCZ4helper * CCZ4sk * JJ[j] );
            for (int k = 0; k < 3; k++)
            {
                temp2 += g_contr[j][k] * dBB[j][k][i];
                temp3 += g_contr[i][j] * ( dBB[j][k][k] + dBB[k][j][k] );
            }
        }
        dtGhat[i] = advec * (beta[0]*dGhat[0][i]+beta[1]*dGhat[1][i]+beta[2]*dGhat[2][i])
                  + 2. * temp1 + temp2 + 1./6. * temp3;

    }

    double dtmyb[3];
    for (int i = 0; i < 3; i++)
    {
        dtmyb[i] = CCZ4xi * dtGhat[i]
                 + CCZ4bs * advec *(beta[0]*gradQin[20+i][0]+beta[1]*gradQin[20+i][1]+beta[2]*gradQin[20+i][2] - beta[0]*gradQin[13+i][0]-beta[1]*gradQin[13+i][1]-beta[2]*gradQin[13+i][2]);

        dtmyb[i] *= CCZ4sk;
    }

    double dtAA[3] = {0};
    double iVa = (CCZ4Sa == 0) ? alpha : 1.0;

    for (int i = 0; i < 3; i++)
    {
        dtAA[i] = -alpha * iVa * fa * ( dKex[i] - 2. * CCZ4c * dTheta[i] )
                - CCZ4sk * iVa * fa * JJ[i] * CCZ4helper
                + advec_aux * (beta[0]*dAA[0][i] + beta[1]*dAA[1][i] + beta[2]*dAA[2][i]);
    }

    double dtBB[3][3] = {{CCZ4f*gradQin[20][0],CCZ4f*gradQin[21][0],CCZ4f*gradQin[22][0]},
                         {CCZ4f*gradQin[20][1],CCZ4f*gradQin[21][1],CCZ4f*gradQin[22][1]},
                         {CCZ4f*gradQin[20][2],CCZ4f*gradQin[21][2],CCZ4f*gradQin[22][2]}};
    for (int j = 0; j < 3; j++)
    for (int i = 0; i < 3; i++)
    {
        double temp1=0, temp2=0;
        for (int k = 0; k < 3; k++)
        {
            temp1 += g_contr[i][k] * (dPP[j][k] - dPP[k][j]);
            for (int l = 0; l < 3; l++)
            for (int m = 0; m < 3; m++) temp2 += g_contr[i][k] * g_contr[l][m] * (dDD[j][m][k][l] - dDD[m][j][k][l]);
        }
        dtBB[j][i] += CCZ4bs * advec_aux * (beta[0]*dBB[0][j][i]+beta[1]*dBB[1][j][i]+beta[2]*dBB[2][j][i])
                    + CCZ4helper * CCZ4mu * alpha2 * (Vp * temp1 - temp2);
        dtBB[j][i] *= CCZ4sk;
    }

    double dtDD[3][3][3];
    for (int k = 0; k < 3; k++)
    for (int i = 0; i < 3; i++)
    for (int j = i; j < 3; j++)
    {
        double temp1=0, temp2=0;
        for (int l = 0; l < 3; l++)
        {
            temp1 += g_cov[i][j]* (dBB[k][l][l] + dBB[l][k][l]);
            temp2 += g_cov[l][i]* (dBB[k][j][l] + dBB[j][k][l]) + g_cov[l][j] * (dBB[k][i][l]+dBB[i][k][l]);
        }
        dtDD[k][i][j] = -alpha * dAex[k][i][j] + 1./3. * CCZ4helper * g_cov[i][j] * JJ[k]
                      + advec_aux * (beta[0]*dDD[0][k][i][j]+beta[1]*dDD[1][k][i][j]+beta[2]*dDD[2][k][i][j])
                      - 1./6. * temp1 + 1./4. * temp2;
    }

    double dtPP[3];
    for (int i = 0; i < 3; i++)
    {
        double temp=0;
        for (int j = 0; j < 3; j++) temp += dBB[i][j][j] + dBB[j][i][j];
        dtPP[i] = advec_aux * (beta[0]*dPP[0][i]+beta[1]*dPP[1][i]+beta[2]*dPP[2][i])
                + 1./3./Vp * (alpha * dKex[i] + CCZ4helper * CCZ4sk * JJ[i])
                - 1./6./Vp * temp;
    }

    // Return B(Q) grad(Q), the negative of the derivative RHS computed above.
    BgradQ[0]  = 0.0;
    BgradQ[1]  = 0.0;
    BgradQ[2]  = 0.0;
    BgradQ[3]  = 0.0;
    BgradQ[4]  = 0.0;
    BgradQ[5]  = 0.0;
    BgradQ[6]  = -dtAex[0][0];
    BgradQ[7]  = -dtAex[0][1];
    BgradQ[8]  = -dtAex[0][2];
    BgradQ[9]  = -dtAex[1][1];
    BgradQ[10] = -dtAex[1][2];
    BgradQ[11] = -dtAex[2][2];

    BgradQ[12] = -dtTheta;
    for (int i = 0; i < 3; i++) BgradQ[13+i] = -dtGhat[i];
    BgradQ[16] = 0.0;
    for (int i = 0; i < 3; i++) BgradQ[17+i] = 0.0;
    for (int i = 0; i < 3; i++) BgradQ[20+i] = -dtmyb[i];
    for (int i = 0; i < 3; i++) BgradQ[23+i] = -dtAA[i];
    BgradQ[26] = -dtBB[0][0];
    BgradQ[27] = -dtBB[0][1];
    BgradQ[28] = -dtBB[0][2];
    BgradQ[29] = -dtBB[1][0];
    BgradQ[30] = -dtBB[1][1];
    BgradQ[31] = -dtBB[1][2];
    BgradQ[32] = -dtBB[2][0];
    BgradQ[33] = -dtBB[2][1];
    BgradQ[34] = -dtBB[2][2];

    BgradQ[35] = -dtDD[0][0][0];
    BgradQ[36] = -dtDD[0][0][1];
    BgradQ[37] = -dtDD[0][0][2];
    BgradQ[38] = -dtDD[0][1][1];
    BgradQ[39] = -dtDD[0][1][2];
    BgradQ[40] = -dtDD[0][2][2];

    BgradQ[41] = -dtDD[1][0][0];
    BgradQ[42] = -dtDD[1][0][1];
    BgradQ[43] = -dtDD[1][0][2];
    BgradQ[44] = -dtDD[1][1][1];
    BgradQ[45] = -dtDD[1][1][2];
    BgradQ[46] = -dtDD[1][2][2];

    BgradQ[47] = -dtDD[2][0][0];
    BgradQ[48] = -dtDD[2][0][1];
    BgradQ[49] = -dtDD[2][0][2];
    BgradQ[50] = -dtDD[2][1][1];
    BgradQ[51] = -dtDD[2][1][2];
    BgradQ[52] = -dtDD[2][2][2];
    BgradQ[53] = -dtKex;
    BgradQ[54] = 0.0;
    for (int i = 0; i < 3; i++) BgradQ[55+i] = -dtPP[i];
    BgradQ[58] = 0;

    // Reconstructed auxiliary variables are not evolved in SO mode.
    if (CCZ4SO==1) {
        for (int i = 23; i < 53; i++) BgradQ[i] = 0.0;
        for (int i = 0; i < 3; i++) BgradQ[55+i] = 0.0;
    }
}
#if defined(GPUOffloadingOMP)
#pragma omp end declare target
#endif
