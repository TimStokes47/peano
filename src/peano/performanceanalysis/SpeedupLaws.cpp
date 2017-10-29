#include "peano/performanceanalysis/SpeedupLaws.h"
#include "tarch/la/Matrix.h"


tarch::logging::Log peano::performanceanalysis::SpeedupLaws::_log( "peano::performanceanalysis::SpeedupLaws" );

peano::performanceanalysis::SpeedupLaws::SpeedupLaws():
  _f(0.5),
  _t1(1.0),
  _samples(0) {
}


void peano::performanceanalysis::SpeedupLaws::addMeasurement( int p, double t ) {
  assertion( p>0 );

  #if defined(Asserts) || defined(PerformanceAnalysis)
  logInfo( "addMeasurement()", "p=" << p << ", t=" << t );
  #endif

  for (int i=Entries-1; i>=1; i--) {
    _p(i) = _p(i-1);
    _t(i) = _t(i-1);
  }

  _p(0) = p;
  _t(0) = t;

  if (_samples<Entries) {
    _t1 = t;
  }
  _samples++;
}


void peano::performanceanalysis::SpeedupLaws::relaxAmdahlsLaw() {
  if (_samples>Entries) {
    tarch::la::Vector<2,double>          rhs(0.0);
    tarch::la::Matrix<2,Entries,double>  gradJ(0.0);
    for (int n=0; n<static_cast<int>(_p.size()); n++) {
      double weight     = std::pow(0.5,n-1.0);
      double amdahlTerm = _f * _t1 + (1.0-_f) * _t1/_p(n) - _t(n);
      double dtdf       =      _t1 -            _t1/_p(n);
      double dtdt1      = _f       - (1.0-_f)      /_p(n);

      rhs(0) += weight * amdahlTerm * dtdf;
      rhs(1) += weight * amdahlTerm + dtdt1;

      gradJ(0,n) = dtdf;
      gradJ(1,n) = dtdt1;
    }

    tarch::la::Vector<Entries,double> gradJTrhs;
    gradJTrhs = tarch::la::transpose(gradJ) * rhs;

    tarch::la::Matrix<Entries,Entries,double> gradJTgradJ;
    gradJTgradJ = tarch::la::transpose(gradJ) * gradJ;

    tarch::la::Vector<Entries,double> diagInv;
    diagInv = tarch::la::invertEntries( tarch::la::diag(gradJTgradJ) );

    tarch::la::Vector<2,double>       currentFandT1;
    tarch::la::Vector<Entries,double> x;

    currentFandT1 = _f, _t1;
    // initial guess
    x = tarch::la::transpose(gradJ) * currentFandT1;

    const int    RelaxationSteps = Entries*Entries;
    const double omega = 0.7;
    for (int i=0; i<RelaxationSteps; i++) {
      tarch::la::Vector<Entries,double> res;

      res  = gradJTrhs - gradJTgradJ * x;
      x    = x + omega * tarch::la::multiplyComponents(diagInv, res);

      // Constraints
      tarch::la::Vector<2,double> newUnknowns;
      newUnknowns = gradJ * x;
/*
      if ( newUnknowns(0)<=0.0 ) { // f

      }
      if ( newUnknowns(0)>=1.0 ) { // f

      }
      if ( newUnknowns(1)<=1.0 ) { // t

      }
*/
    }

    tarch::la::Vector<2,double> newUnknowns;
    newUnknowns = gradJ * x;

    const double weight = 0.5;
    _f  = weight  * _f  + (1.0-weight ) * newUnknowns(0);
    _t1 = weight  * _t1 + (1.0-weight ) * newUnknowns(1);
    // Mir loesen nur approximativ, also duerfen wir auch nur mit sliding average updaten

    assertion9( _f>0.0,  _f, _t1, rhs, gradJ, gradJTgradJ, x, gradJTrhs, _p, _t );
    assertion9( _f<1.0,  _f, _t1, rhs, gradJ, gradJTgradJ, x, gradJTrhs, _p, _t );
    assertion9( _t1>0.0, _f, _t1, rhs, gradJ, gradJTgradJ, x, gradJTrhs, _p, _t );
  }

  #if defined(Asserts) || defined(PerformanceAnalysis)
  logInfo( "relaxAmdahlsLaw()", "f=" << _f << ", t1=" << _t1 );
  #endif
}
/*
void peano::performanceanalysis::SpeedupLaws::relaxAmdahlsLaw() {
  assertion( _f>0.0  );
  // evtl. zu harsch
  assertion( _f<1.0 );
  assertion( _t1>0.0 );

//  if (!_p.empty()) {
  if (_p.size()>=2) {
    double  rhsf  = 0.0;
    double  rhst1 = 0.0;
    double* J     = new double[_p.size()*2];

    for (int n=0; n<static_cast<int>(_p.size()); n++) {
      double weight     = std::pow(0.5,n-1.0);
      double amdahlTerm = _f * _t1 + (1.0-_f) * _t1/_p[n] - _t[n];
      double dtdf       =      _t1 -            _t1/_p[n];
      double dtdt1      = _f       - (1.0-_f)      /_p[n];


      rhsf   += weight * amdahlTerm * dtdf;
      rhst1  += weight * amdahlTerm + dtdt1;

      J[n]           = dtdf;
      J[n+_p.size()] = dtdt1;
    }

    double* rhs     = new double[_p.size()*2];
    double* x       = new double[_p.size()*2];
    for (int i=0; i<static_cast<int>(_p.size()); i++) {
      x[i]   = 0.0;
      rhs[i] = J[i] * rhsf + J[i+_p.size()] * rhst1;
    }

    const int    RelaxationSteps = 10;
    const double omega = 0.7;
    for (int i=0; i<RelaxationSteps; i++) {
      // GS update of one line
      for (int row=0; row<static_cast<int>(_p.size()); row++) {
        double res = rhs[i];

        for (int col=0; col<static_cast<int>(_p.size()); col++) {
          double JTJentry = J[row*_p.size()+0] * J[col*_p.size()]
                          + J[row*_p.size()+1] * J[col*_p.size()+_p.size()];
          res -= JTJentry * x[i];
        }

        double diag = J[row*_p.size()+0] * J[row*_p.size()]
                    + J[row*_p.size()+1] * J[row*_p.size()+_p.size()];

        assertion(diag>0.0);

        x[row] += x[row] + omega * res / diag;
      }
    }

    _f  = 0.0;
    _t1 = 0.0;

    for (int i=0; i<static_cast<int>(_p.size()); i++) {
      _f  += J[i]           * x[i];
      _t1 += J[i+_p.size()] * x[i];
    }

    delete[] J;
    delete[] x;
    delete[] rhs;

    #ifdef Asserts
    bool valid = (_f>0.0) && (_f<1.0) && (_t1>0.0);
    if (!valid) {
      std::cerr << std::endl;
      std::cerr << "(grad J)^T=" << std::endl;
      for (int i=0; i<static_cast<int>(_p.size()); i++) {
         std::cerr << J[i] << " " << J[i+_p.size()] << std::endl;
      }

      std::cerr << "grad J=" << std::endl;
      for (int i=0; i<static_cast<int>(_p.size()); i++) {
         std::cerr << J[i] << " ";
      }
      std::cerr << std::endl;
      for (int i=0; i<static_cast<int>(_p.size()); i++) {
         std::cerr << J[i+_p.size()] << " ";
      }
      std::cerr << std::endl;

      std::cerr << "x=" << std::endl;
      for (int i=0; i<static_cast<int>(_p.size()); i++) {
         std::cerr << x[i] std::endl;
      }

      std::cerr << "rhs=" << std::endl;
      for (int i=0; i<static_cast<int>(_p.size()); i++) {
         std::cerr << rhs[i] std::endl;
      }

      assertion(false);
    }
    #endif
  }
}
*/


double peano::performanceanalysis::SpeedupLaws::getSerialTime() const {
  return _t1;
}


double peano::performanceanalysis::SpeedupLaws::getSerialCodeFraction() const {
  return _f;
}
