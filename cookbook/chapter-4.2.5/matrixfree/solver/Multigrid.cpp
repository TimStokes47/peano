#include "matrixfree/solver/JacobiSmoother.h"
#include "matrixfree/solver/Multigrid.h"
#include "matrixfree/stencil/StencilFactory.h"

#include "peano/utils/Loop.h"


tarch::logging::Log matrixfree::solver::Multigrid::_log( "matrixfree::solver::Multigrid" );


matrixfree::solver::Multigrid::Multigrid():
  _dLinearInterpolation( matrixfree::stencil::getDLinearInterpolation() ),
  _numberOfStencilEvaluations(0){
}


matrixfree::solver::Multigrid::~Multigrid() {
}


matrixfree::solver::Multigrid::Multigrid(const Multigrid& workerThread):
  _dLinearInterpolation( workerThread._dLinearInterpolation ),
  _numberOfStencilEvaluations(0) {
}


void matrixfree::solver::Multigrid::mergeWithWorkerThread(const Multigrid& workerThread) {
  _numberOfStencilEvaluations += workerThread._numberOfStencilEvaluations;
}




tarch::la::Vector<TWO_POWER_D,double> matrixfree::solver::Multigrid::injectFineGridValues(
  const tarch::la::Vector<FOUR_POWER_D,double>&  fineGridValues
) {
  tarch::la::Vector<TWO_POWER_D,double> result;
  dfor2(k)
    const tarch::la::Vector<DIMENSIONS,int> fineGridIndex   = k*3;
    result(kScalar) = fineGridValues( peano::utils::dLinearised(fineGridIndex,4) );
  enddforx

  _numberOfStencilEvaluations += TWO_POWER_D;

  return result;
}


tarch::la::Vector<TWO_POWER_D,std::complex<double> > matrixfree::solver::Multigrid::injectFineGridValues(
  const tarch::la::Vector<FOUR_POWER_D,std::complex<double> >&  fineGridValues
) {
  tarch::la::Vector<TWO_POWER_D,std::complex<double> > result;
  dfor2(k)
    const tarch::la::Vector<DIMENSIONS,int> fineGridIndex   = k*3;
    result(kScalar) = fineGridValues( peano::utils::dLinearised(fineGridIndex,4) );
  enddforx

  _numberOfStencilEvaluations += TWO_POWER_D;

  return result;
}


tarch::la::Vector<TWO_POWER_D, double> matrixfree::solver::Multigrid::calculateP(
  const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&  coarsePStencils,
  const tarch::la::Vector<DIMENSIONS,int>&                          fineGridVertexPosition
) const {
  tarch::la::Vector<TWO_POWER_D, double> result;

  dfor2(i)
    tarch::la::Vector<DIMENSIONS,int>  entryOfCoarseGridStencil;
    bool                               coarseGridStencilInfluencesFineGridVertex;
    int                                indexOfCoarseGridStencil;

    matrixfree::solver::Multigrid::getPositionIn5PowDStencilRelativeToKthCoarseVertex(
      i,
      fineGridVertexPosition,
      entryOfCoarseGridStencil,
      indexOfCoarseGridStencil,
      coarseGridStencilInfluencesFineGridVertex
    );

    if(coarseGridStencilInfluencesFineGridVertex){
      result(iScalar) = coarsePStencils(
        iScalar * FIVE_POWER_D + indexOfCoarseGridStencil
      );
    }
    else{
      result(iScalar) = 0.0;
    }
  enddforx

  return result;
}



tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double>  matrixfree::solver::Multigrid::calculateP(
  const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&  coarsePStencils
) const {
  tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double> result;

  dfor4(j)
    const tarch::la::Vector<TWO_POWER_D, double> row = calculateP(coarsePStencils,j);
    for (int i=0; i<TWO_POWER_D; i++) {
      result(jScalar, i) = row(i);
    }
  enddforx

  return result;
}




tarch::la::Vector<TWO_POWER_D, double> matrixfree::solver::Multigrid::calculateP(
  const tarch::la::Vector<FIVE_POWER_D, double>&                    coarsePStencil,
  const tarch::la::Vector<DIMENSIONS,int>&                          fineGridVertexPosition
) const {
  tarch::la::Vector<TWO_POWER_D, double> result;

  dfor2(i)
    tarch::la::Vector<DIMENSIONS,int>  entryOfCoarseGridStencil;
    bool                               coarseGridStencilInfluencesFineGridVertex;
    int                                indexOfCoarseGridStencil;

    matrixfree::solver::Multigrid::getPositionIn5PowDStencilRelativeToKthCoarseVertex(
      i,
      fineGridVertexPosition,
      entryOfCoarseGridStencil,
      indexOfCoarseGridStencil,
      coarseGridStencilInfluencesFineGridVertex
    );

    if(coarseGridStencilInfluencesFineGridVertex){
      result(iScalar) = coarsePStencil( indexOfCoarseGridStencil );
    }
    else{
      result(iScalar) = 0.0;
    }
  enddforx

  return result;
}


tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double>  matrixfree::solver::Multigrid::calculateP(
  const tarch::la::Vector<FIVE_POWER_D, double>&  coarsePStencil
) const {
  tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double> result;

  dfor4(j)
    const tarch::la::Vector<TWO_POWER_D, double> row = calculateP(coarsePStencil,j);
    for (int i=0; i<TWO_POWER_D; i++) {
      result(jScalar, i) = row(i);
    }
  enddforx

  return result;
}


tarch::la::Vector<TWO_POWER_D, double> matrixfree::solver::Multigrid::calculateP(
  const tarch::la::Vector<DIMENSIONS,int>&                          fineGridVertexPosition
) const {
  return calculateP(_dLinearInterpolation,fineGridVertexPosition);
}


tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double>  matrixfree::solver::Multigrid::calculateP() const {
  return calculateP(_dLinearInterpolation);
}


tarch::la::Vector<THREE_POWER_D,double> matrixfree::solver::Multigrid::getGalerkinMultigridOperatorForDLinearInterpolationAndRestriction( int fineGridSubdivisionFactor, const tarch::la::Vector<THREE_POWER_D,double>& fineGridStencil ) {
  assertion(fineGridSubdivisionFactor>=2);

  tarch::la::Vector<THREE_POWER_D,double> result(0.0);

  const int numberOfVertices1d = 4*fineGridSubdivisionFactor+1;
  const int numberOfVertices   = tarch::la::aPowI(DIMENSIONS,numberOfVertices1d);
  double* fineGridP  = new double[numberOfVertices];
  double* fineGridAP = new double[numberOfVertices];

  dfor3(coarseGridVertex)
    // initialise fine grid with zero
    int counter = 0;
    dfor(i,numberOfVertices1d) {
      fineGridP[counter]  = 0.0;
      fineGridAP[counter] = 0.0;
      counter++;
    }

    // prolong
    dfor(fineGridVertex,numberOfVertices1d) {
      const int linearisedFineGridVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex,numberOfVertices1d);
      double weight = 1.0;
      for (int d=0; d<DIMENSIONS; d++) {
        const double weight1d = static_cast<double>((fineGridSubdivisionFactor - std::abs( (coarseGridVertex(d)+1)*fineGridSubdivisionFactor - fineGridVertex(d) ) )) / static_cast<double>(fineGridSubdivisionFactor);
        assertion(weight1d<=1.0);
        weight *= tarch::la::greaterEquals(weight1d,0.0) ? weight1d : 0.0;
      }
      fineGridP[linearisedFineGridVertex] = weight;
    }

    // apply stencil
    dfor(fineGridVertex,2*fineGridSubdivisionFactor-1) {
      const int linearisedDestVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex+fineGridSubdivisionFactor+1,numberOfVertices1d);
      dfor3(stencilEntry)
        const int linearisedSrcVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex+fineGridSubdivisionFactor+stencilEntry,numberOfVertices1d);
        fineGridAP[linearisedDestVertex] += fineGridP[linearisedSrcVertex] * fineGridStencil(stencilEntryScalar);
      enddforx
    }

    // restrict
    dfor(fineGridVertex,numberOfVertices1d) {
      const int linearisedFineGridVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex,numberOfVertices1d);
      double weight = 1.0;
      for (int d=0; d<DIMENSIONS; d++) {
        // always restrict towards centre
        const double weight1d = static_cast<double>((fineGridSubdivisionFactor - std::abs( 2*fineGridSubdivisionFactor - fineGridVertex(d) ) )) / static_cast<double>(fineGridSubdivisionFactor);
        assertion(weight1d<=1.0);
        weight *= tarch::la::greaterEquals(weight1d,0.0) ? weight1d : 0.0;
      }
      result(coarseGridVertexScalar) += fineGridAP[linearisedFineGridVertex] * weight;
    }
    logDebug( "getGalerkinMultigridOperatorForDLinearInterpolationAndRestriction(int,stencil)", "result stencil entry " << coarseGridVertex << ": " << result(coarseGridVertexScalar) );

  enddforx

  delete[] fineGridP;
  delete[] fineGridAP;
  return result;
}


tarch::la::Vector<THREE_POWER_D,double> matrixfree::solver::Multigrid::getGalerkinMultigridOperatorForDLinearInterpolationAndInjection( int fineGridSubdivisionFactor, const tarch::la::Vector<THREE_POWER_D,double>& fineGridStencil ) {
  assertion(fineGridSubdivisionFactor>=2);

  tarch::la::Vector<THREE_POWER_D,double> result(0.0);

  const int numberOfVertices1d = 4*fineGridSubdivisionFactor+1;
  const int numberOfVertices   = tarch::la::aPowI(DIMENSIONS,numberOfVertices1d);
  double* fineGridP  = new double[numberOfVertices];
  double* fineGridAP = new double[numberOfVertices];

  dfor3(coarseGridVertex)
    // initialise fine grid with zero
    int counter = 0;
    dfor(i,numberOfVertices1d) {
      fineGridP[counter]  = 0.0;
      fineGridAP[counter] = 0.0;
      counter++;
    }

    // prolong
    dfor(fineGridVertex,numberOfVertices1d) {
      const int linearisedFineGridVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex,numberOfVertices1d);
      double weight = 1.0;
      for (int d=0; d<DIMENSIONS; d++) {
        const double weight1d = static_cast<double>((fineGridSubdivisionFactor - std::abs( (coarseGridVertex(d)+1)*fineGridSubdivisionFactor - fineGridVertex(d) ) )) / static_cast<double>(fineGridSubdivisionFactor);
        assertion(weight1d<=1.0);
        weight *= tarch::la::greaterEquals(weight1d,0.0) ? weight1d : 0.0;
      }
      fineGridP[linearisedFineGridVertex] = weight;
    }

    // apply stencil (too generic, should be only central element)
    dfor(fineGridVertex,2*fineGridSubdivisionFactor-1) {
      const int linearisedDestVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex+fineGridSubdivisionFactor+1,numberOfVertices1d);
      dfor3(stencilEntry)
        const int linearisedSrcVertex = peano::utils::dLinearisedWithoutLookup(fineGridVertex+fineGridSubdivisionFactor+stencilEntry,numberOfVertices1d);
        fineGridAP[linearisedDestVertex] += fineGridP[linearisedSrcVertex] * fineGridStencil(stencilEntryScalar);
      enddforx
    }

    // restrict
    const int linearisedFineGridVertexCoincidingWithCoarseGridPoint = peano::utils::dLinearisedWithoutLookup(2*fineGridSubdivisionFactor,numberOfVertices1d);
    result(coarseGridVertexScalar) = fineGridAP[linearisedFineGridVertexCoincidingWithCoarseGridPoint];
    logDebug( "getGalerkinMultigridOperatorForDLinearInterpolationAndRestriction(int,stencil)", "result stencil entry " << coarseGridVertex << ": " << result(coarseGridVertexScalar) );

  enddforx

  delete[] fineGridP;
  delete[] fineGridAP;
  return result;
}

/**

tarch::la::Vector<TWO_POWER_D, double> matrixfree::solver::Multigrid::prolongCellValues(
	    const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&	coarseGridVerticesP,
	    const tarch::la::Vector<TWO_POWER_D_TIMES_D,int>&                  fineGridVerticesPositions,
		tarch::la::Vector<TWO_POWER_D, double>								cellValues
){

	tarch::la::Vector<TWO_POWER_D, double> result;

	tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> cellP = calculateCellInterGridTransferOperator(coarseGridVerticesP, fineGridVerticesPositions)*(1.0/TWO_POWER_D);

	result = cellP * cellValues;

	return result;
}

tarch::la::Vector<TWO_POWER_D, double> matrixfree::solver::Multigrid::restrictCellValues(
	    const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&	coarseGridVerticesR,
	    const tarch::la::Vector<TWO_POWER_D_TIMES_D,int>&                  fineGridVerticesPositions,
		tarch::la::Vector<TWO_POWER_D, double> 								cellValues
){
	tarch::la::Vector<TWO_POWER_D, double> result;

	tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> cellR = calculateCellInterGridTransferOperator(coarseGridVerticesR, fineGridVerticesPositions)*(1.0/TWO_POWER_D);

	result = transpose(cellR) * cellValues;

	return result;
}

tarch::la::Vector<TWO_POWER_D, double> matrixfree::solver::Multigrid::computeCellResidual(
		tarch::la::Vector<TWO_POWER_D, double> cellValues,
		tarch::la::Vector<TWO_POWER_D, double> rhs,
		tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> elementWiseAssemblyMatrix
){
	tarch::la::Vector<TWO_POWER_D, double> result;

	tarch::la::Vector<TWO_POWER_D, double> aTimesU = elementWiseAssemblyMatrix*cellValues;

	dfor2(i)

		result(iScalar) = rhs(iScalar) - aTimesU(iScalar);

	enddforx

	return result;
}

*/


tarch::la::Vector<FOUR_POWER_D,double > matrixfree::solver::Multigrid::dLinearInterpolation(
  const tarch::la::Vector<TWO_POWER_D,double >&  coarseGridValues
) {
  tarch::la::Vector<FOUR_POWER_D,double > result;

  dfor4(k)
    result(kScalar) = getDLinearInterpolatedValue(coarseGridValues,k);
  enddforx

  return result;
}


tarch::la::Vector<FOUR_POWER_D,std::complex<double> > matrixfree::solver::Multigrid::dLinearInterpolation(
  const tarch::la::Vector<TWO_POWER_D,std::complex<double> >&  coarseGridValues
) {
  tarch::la::Vector<FOUR_POWER_D,std::complex<double> > result;

  dfor4(k)
    result(kScalar) = getDLinearInterpolatedValue(coarseGridValues,k);
  enddforx

  return result;
}



double matrixfree::solver::Multigrid::getDLinearInterpolatedValue(
  const tarch::la::Vector<TWO_POWER_D,double>&            coarseGridValues,
  const tarch::la::Vector<DIMENSIONS,int>&                fineGridPositionOfVertex
) {
  logTraceInWith2Arguments( "interpolateWithDLinearShapeFunctions(...)", coarseGridValues, fineGridPositionOfVertex );

  double result = 0.0;
  double tmp;

  dfor2(k)
    tmp=
      coarseGridValues(kScalar) *
      computeContributionWeightOfInterGridTransfer(
        k,
        _dLinearInterpolation,
        fineGridPositionOfVertex
      );
    result+=tmp;
  enddforx

  logTraceOutWith1Argument( "interpolateWithDLinearShapeFunctions(...)", result );

  return result;
}


tarch::la::Vector<TWO_POWER_D,double >  matrixfree::solver::Multigrid::getDLinearInterpolationWeights(
  const tarch::la::Vector<DIMENSIONS,int>&                     fineGridPositionOfVertex
) {
  tarch::la::Vector<TWO_POWER_D,double > result;
  dfor2(k)
    const double weight =
        computeContributionWeightOfInterGridTransfer(
          k,
          _dLinearInterpolation,
          fineGridPositionOfVertex
        );
    result(kScalar) = weight;
  enddforx
  return result;
}


std::complex<double> matrixfree::solver::Multigrid::getDLinearInterpolatedValue(
  const tarch::la::Vector<TWO_POWER_D,std::complex<double> >&   coarseGridValues,
  const tarch::la::Vector<DIMENSIONS,int>&                      fineGridPositionOfVertex
) {
  logTraceInWith2Arguments( "interpolateWithDLinearShapeFunctions(...)", coarseGridValues, fineGridPositionOfVertex );

  std::complex<double> result = 0.0;

  const tarch::la::Vector<TWO_POWER_D,double > coarseGridValuesReal = tarch::la::real(coarseGridValues);
  const tarch::la::Vector<TWO_POWER_D,double > coarseGridValuesImag = tarch::la::imag(coarseGridValues);

  dfor2(k)
    const double weight =
        computeContributionWeightOfInterGridTransfer(
          k,
          _dLinearInterpolation,
          fineGridPositionOfVertex
        );

    /**
     * This version does not work with gcc 4.7 as this compiler version's std
     * lib does not offer these setters. Unfortunately, SuperMIC currently
     * runs exactly this gcc version (2015-04-08).
     */
    //result.real( coarseGridValuesReal(kScalar) * weight + result.real() );
    //result.imag( coarseGridValuesImag(kScalar) * weight + result.imag() );

    result += std::complex<double>( coarseGridValuesReal(kScalar) * weight, coarseGridValuesImag(kScalar) * weight );
  enddforx

  logTraceOutWith1Argument( "interpolateWithDLinearShapeFunctions(...)", result );

  return result;
}

/*

void matrixfree::solver::Multigrid::getPositionIn5PowDStencilRelativeToKthCoarseVertex(
      const tarch::la::Vector<DIMENSIONS,int>&       coarseGridVertexPosition,
      const tarch::la::Vector<DIMENSIONS,int>&       fineGridVertexPosition,
      tarch::la::Vector<DIMENSIONS,int>&             entryOfCoarseGridStencil,
      int&                                           indexOfCoarseGridStencil,
      bool&                                          coarseGridStencilInfluencesFineGridVertex
) {
  indexOfCoarseGridStencil                  = 0;
  coarseGridStencilInfluencesFineGridVertex = true;
  int baseOfStencilEntry                    = 1;
  for (int d=0; d<DIMENSIONS; d++) {
    assertion(coarseGridVertexPosition(d)>=0 && coarseGridVertexPosition(d)<=1);
    if (coarseGridVertexPosition(d)==1) {
      entryOfCoarseGridStencil(d) = 2 - (3-fineGridVertexPosition(d));
    }
    else {
      entryOfCoarseGridStencil(d) = 2 + fineGridVertexPosition(d);
    }
    coarseGridStencilInfluencesFineGridVertex &= entryOfCoarseGridStencil(d)>=0 && entryOfCoarseGridStencil(d)<5;
    indexOfCoarseGridStencil                  += entryOfCoarseGridStencil(d) * baseOfStencilEntry;
    baseOfStencilEntry                        *= 5;
  }
}

int matrixfree::solver::Multigrid::getPositionInCellInterGridTransferOperatorVector(
    const int coarseGridVertexNumber,
    const int positionInOperator
){
  return coarseGridVertexNumber*FIVE_POWER_D + positionInOperator;
}

int matrixfree::solver::Multigrid::getPositionInCellStencilVector(
    const int coarseGridVertexNumber,
    const int positionInOperator
){
  return coarseGridVertexNumber*THREE_POWER_D + positionInOperator;
}
*/


double matrixfree::solver::Multigrid::computeContributionWeightOfInterGridTransfer(
  const tarch::la::Vector<DIMENSIONS,int>&       currentCoarseGridVertex,
  const tarch::la::Vector<FIVE_POWER_D,double>&  currentCoarseGridVertexsInterGridTransferOperator,
  const tarch::la::Vector<DIMENSIONS,int>&       fineGridPositionOfVertex
) {
  logTraceInWith3Arguments( "computeContributionWeightOfInterGridTransfer(...)", currentCoarseGridVertex, currentCoarseGridVertexsInterGridTransferOperator, fineGridPositionOfVertex );

  double result;

  tarch::la::Vector<DIMENSIONS,int>  positionRelativeToKthCoarseVertex;
  bool                               isInfluencedByVertexK;

  int stencilEntry;
  getPositionIn5PowDStencilRelativeToKthCoarseVertex(
    currentCoarseGridVertex,
    fineGridPositionOfVertex,
    positionRelativeToKthCoarseVertex,
    stencilEntry,
    isInfluencedByVertexK
  );

  if (isInfluencedByVertexK) {
    int baseOfGridVectorEntry = 1;
    int coarseGridVectorEntry = 0;
    for (int d=0; d<DIMENSIONS; d++) {
      coarseGridVectorEntry += currentCoarseGridVertex(d) * baseOfGridVectorEntry;
      baseOfGridVectorEntry *= 2;
    }
    _numberOfStencilEvaluations++;
    result = currentCoarseGridVertexsInterGridTransferOperator(stencilEntry);
  }
  else {
    result = 0.0;
  }

  logTraceOutWith1Argument( "computeContributionWeightOfInterGridTransfer(...)", result );

  return result;
}


void matrixfree::solver::Multigrid::getPositionIn5PowDStencilRelativeToKthCoarseVertex(
      const tarch::la::Vector<DIMENSIONS,int>&       coarseGridVertexPosition,
      const tarch::la::Vector<DIMENSIONS,int>&       fineGridVertexPosition,
      tarch::la::Vector<DIMENSIONS,int>&             entryOfCoarseGridStencil,
      int&                                           indexOfCoarseGridStencil,
      bool&                                          coarseGridStencilInfluencesFineGridVertex
) {
  indexOfCoarseGridStencil                  = 0;
  coarseGridStencilInfluencesFineGridVertex = true;
  int baseOfStencilEntry                    = 1;
  for (int d=0; d<DIMENSIONS; d++) {
    assertion(coarseGridVertexPosition(d)>=0 && coarseGridVertexPosition(d)<=1);
    if (coarseGridVertexPosition(d)==1) {
      entryOfCoarseGridStencil(d) = 2 - (3-fineGridVertexPosition(d));
    }
    else {
      entryOfCoarseGridStencil(d) = 2 + fineGridVertexPosition(d);
    }
    coarseGridStencilInfluencesFineGridVertex &= entryOfCoarseGridStencil(d)>=0 && entryOfCoarseGridStencil(d)<5;
    indexOfCoarseGridStencil                  += entryOfCoarseGridStencil(d) * baseOfStencilEntry;
    baseOfStencilEntry                        *= 5;
  }
}


/*
tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> matrixfree::solver::Multigrid::calculateCellInterGridTransferOperator(
  const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&		coarseGridVerticesInterGridTransferOperators,
  const tarch::la::Vector<TWO_POWER_D_TIMES_D,int>&                    fineGridVerticesPositions
){

	  logTraceInWith2Arguments( "calculateCellInterGridTransferOperator(...)", coarseGridVerticesInterGridTransferOperators, fineGridVerticesPositions);

	  tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> result;

	  dfor2(l) // run over fine grid vertices
	    tarch::la::Vector<DIMENSIONS,int> fineGridVertexPosition;
	    for(int d = 0; d < DIMENSIONS; d++){
	      fineGridVertexPosition(d) = fineGridVerticesPositions(lScalar*DIMENSIONS + d);
	    }
	    dfor2(k) // run over coarse grid vertices
	      tarch::la::Vector<DIMENSIONS,int>  entryOfCoarseGridStencil;
	      bool                               coarseGridStencilInfluencesFineGridVertex;
	      int                                indexOfCoarseGridStencil;

	      matrixfree::solver::Multigrid::getPositionIn5PowDStencilRelativeToKthCoarseVertex(
	        k,
	        fineGridVertexPosition,
	        entryOfCoarseGridStencil,
	        indexOfCoarseGridStencil,
	        coarseGridStencilInfluencesFineGridVertex
	      );

	      int indexInOperator = matrixfree::solver::Multigrid::getPositionInCellInterGridTransferOperatorVector(kScalar, indexOfCoarseGridStencil);

	      if(coarseGridStencilInfluencesFineGridVertex){
	        result(lScalar, kScalar) = coarseGridVerticesInterGridTransferOperators(indexInOperator);
	      }
	      else{
	    	  result(lScalar, kScalar) = 0.0;
	      }
	    enddforx
	  enddforx

	  logTraceOutWith1Argument( "calculateCellInterGridTransferOperator(...)", result );

	  return result;
}

tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> matrixfree::solver::Multigrid::calculatePetrovGalerkinCoarseGridOperator(
    const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&	coarseGridVerticesP,
    const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&  	coarseGridVerticesR,
    const tarch::la::Vector<TWO_POWER_D_TIMES_D,int>&                  fineGridVerticesPositions,
    tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double>                	elementWiseAssemblyMatrix
){

	tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> cellP = calculateCellInterGridTransferOperator(coarseGridVerticesP, fineGridVerticesPositions);
	tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> cellR = calculateCellInterGridTransferOperator(coarseGridVerticesR, fineGridVerticesPositions);

	tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> result;

	tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double> cellAP;
	cellAP = elementWiseAssemblyMatrix * cellP;
	result = transpose(cellR) * cellAP;

	return result;
}


tarch::la::Vector<TWO_POWER_D_TIMES_THREE_POWER_D,double> matrixfree::solver::Multigrid::computeBoxMGIntergridTransferOperator(tarch::la::Vector<THREE_POWER_D_TIMES_FOUR_POWER_D,double> stencils){

	 tarch::la::Vector<TWO_POWER_D_TIMES_THREE_POWER_D,double> result;

#ifdef Dim2

//	  for(int i = 0; i<FOUR_POWER_D; i++){
//	     printf("\n %f %f %f", stencils(i*THREE_POWER_D + 0), stencils(i*THREE_POWER_D + 1), stencils(i*THREE_POWER_D + 2));
//	     printf("\n %f %f %f", stencils(i*THREE_POWER_D + 3), stencils(i*THREE_POWER_D + 4), stencils(i*THREE_POWER_D + 5));
//	     printf("\n %f %f %f", stencils(i*THREE_POWER_D + 6), stencils(i*THREE_POWER_D + 7), stencils(i*THREE_POWER_D + 8));
//	   }

	 // Initialise and set weights to 1.0 for c points
	 tarch::la::assignList(result) = 1.0, 0.0, 0.0,
	                                 0.0, 0.0, 0.0,
	                                 0.0, 0.0, 0.0,

	                                 0.0, 0.0, 1.0,
	                                 0.0, 0.0, 0.0,
	                                 0.0, 0.0, 0.0,

	                                 0.0, 0.0, 0.0,
	                                 0.0, 0.0, 0.0,
	                                 1.0, 0.0, 0.0,

	                                 0.0, 0.0, 0.0,
	                                 0.0, 0.0, 0.0,
	                                 0.0, 0.0, 1.0;

	 //Compute gamma points (using fine-grid vertex numbering)

	 //bottom edge (coarse grid points 0 and 1) -> sum over columns
	 tarch::la::Vector<3,double> g1;
	 tarch::la::assignList(g1) = stencils(1*9 + 0) + stencils(1*9 + 3) + stencils(1*9 + 6),
	                             stencils(1*9 + 1) + stencils(1*9 + 4) + stencils(1*9 + 7),
	                             stencils(1*9 + 2) + stencils(1*9 + 5) + stencils(1*9 + 8);

	 tarch::la::Vector<3,double> g2;
	 tarch::la::assignList(g2)= stencils(2*9 + 0) + stencils(2*9 + 3) + stencils(2*9 + 6),
	                            stencils(2*9 + 1) + stencils(2*9 + 4) + stencils(2*9 + 7),
	                            stencils(2*9 + 2) + stencils(2*9 + 5) + stencils(2*9 + 8);

	 //left edge (coarse grid points 0 and 2) -> sum over rows
	 tarch::la::Vector<3,double> g4;
	 tarch::la::assignList(g4)= stencils(1*9 + 0) + stencils(1*9 + 1) + stencils(1*9 + 2),
	                            stencils(1*9 + 3) + stencils(1*9 + 4) + stencils(1*9 + 5),
	                            stencils(1*9 + 6) + stencils(1*9 + 7) + stencils(1*9 + 8);

	 tarch::la::Vector<3,double> g8;
	 tarch::la::assignList(g8)= stencils(8*9 + 0) + stencils(8*9 + 1) + stencils(8*9 + 2),
	                            stencils(8*9 + 3) + stencils(8*9 + 4) + stencils(8*9 + 5),
	                            stencils(8*9 + 6) + stencils(8*9 + 7) + stencils(8*9 + 8);

	 //right edge (coarse grid points 1 and 3) -> sum over rows
	 tarch::la::Vector<3,double> g7;
	 tarch::la::assignList(g7)= stencils(7*9 + 0) + stencils(7*9 + 1) + stencils(7*9 + 2),
	                            stencils(7*9 + 3) + stencils(7*9 + 4) + stencils(7*9 + 5),
	                            stencils(7*9 + 6) + stencils(7*9 + 7) + stencils(7*9 + 8);

	 tarch::la::Vector<3,double> g11;
	 tarch::la::assignList(g11)= stencils(11*9 + 0) + stencils(11*9 + 1) + stencils(11*9 + 2),
	                             stencils(11*9 + 3) + stencils(11*9 + 4) + stencils(11*9 + 5),
	                             stencils(11*9 + 6) + stencils(11*9 + 7) + stencils(11*9 + 8);

  //top edge (coarse grid points 2 and 3) -> sum over columns
   tarch::la::Vector<3,double> g13;
   tarch::la::assignList(g13)= stencils(13*9 + 0) + stencils(13*9 + 3) + stencils(13*9 + 6),
                               stencils(13*9 + 1) + stencils(13*9 + 4) + stencils(13*9 + 7),
                               stencils(13*9 + 2) + stencils(13*9 + 5) + stencils(13*9 + 8);

   tarch::la::Vector<3,double> g14;
   tarch::la::assignList(g14)= stencils(14*9 + 0) + stencils(14*9 + 3) + stencils(14*9 + 6),
                               stencils(14*9 + 1) + stencils(14*9 + 4) + stencils(14*9 + 7),
                               stencils(14*9 + 2) + stencils(14*9 + 5) + stencils(14*9 + 8);

   //Solve the four equation systems (2 equations with 2 unknowns for every c point)
   tarch::la::Vector<16, double> gammas = solveGammaSystems(g1, g2, g4, g7, g8, g11, g13, g14);

   //bottom edge (from coarse grid point 0 and 1)
   result(0*9 + 1) = gammas(0);
   result(1*9 + 0) = gammas(2);

   result(0*9 + 2) = gammas(1);
   result(1*9 + 1) = gammas(3);

   //left edge (from coarse grid point 0 and 2)
   result(0*9 + 3) = gammas(4);
   result(2*9 + 0) = gammas(6);

   result(0*9 + 6) = gammas(8);
   result(2*9 + 3) = gammas(10);

   //right edge (from coarse grid point 1 and 3)
   result(1*9 + 5) = gammas(5);
   result(3*9 + 2) = gammas(7);

   result(1*9 + 8) = gammas(9);
   result(3*9 + 5) = gammas(11);

   //top edge (from coarse grid point 2 and 3)
   result(2*9 + 7) = gammas(12);
   result(3*9 + 6) = gammas(14);

   result(2*9 + 8) = gammas(13);
   result(3*9 + 7) = gammas(15);

	 //Compute iota points
   tarch::la::Vector<9,double> i5;
   tarch::la::assignList(i5) = stencils(5*9 + 0), stencils(5*9 + 1), stencils(5*9 + 2),
		   	   	   	   	   	   stencils(5*9 + 3), stencils(5*9 + 4), stencils(5*9 + 5),
		   	   	   	   	   	   stencils(5*9 + 6), stencils(5*9 + 7), stencils(5*9 + 8);

   tarch::la::Vector<9,double> i6;
   tarch::la::assignList(i6) = stencils(6*9 + 0), stencils(6*9 + 1), stencils(6*9 + 2),
		   	   	   	   	   	   stencils(6*9 + 3), stencils(6*9 + 4), stencils(6*9 + 5),
		   	   	   	   	   	   stencils(6*9 + 6), stencils(6*9 + 7), stencils(6*9 + 8);

   tarch::la::Vector<9,double> i9;
   tarch::la::assignList(i9) = stencils(9*9 + 0), stencils(9*9 + 1), stencils(9*9 + 2),
		   	   	   	   	   	   stencils(9*9 + 3), stencils(9*9 + 4), stencils(9*9 + 5),
		   	   	   	   	   	   stencils(9*9 + 6), stencils(9*9 + 7), stencils(9*9 + 8);

   tarch::la::Vector<9,double> i10;
   tarch::la::assignList(i10) = stencils(10*9 + 0), stencils(10*9 + 1), stencils(10*9 + 2),
		   	   	   	   	   	    stencils(10*9 + 3), stencils(10*9 + 4), stencils(10*9 + 5),
		   	   	   	   	   	    stencils(10*9 + 6), stencils(10*9 + 7), stencils(10*9 + 8);

   // Solve equation system (4 equations with 4 unknowns for every c point)
   tarch::la::Vector<16, double> iotas = solveIotaSystem(i5, i6, i9, i10, gammas);

   //from coarse grid point 0
   result(0*9 + 4) = iotas(0);
   result(0*9 + 5) = iotas(1);
   result(0*9 + 7) = iotas(2);
   result(0*9 + 8) = iotas(3);

   //from coarse grid point 1
   result(1*9 + 3) = iotas(4);
   result(1*9 + 4) = iotas(5);
   result(1*9 + 6) = iotas(6);
   result(1*9 + 7) = iotas(7);

   //from coarse grid point 2
   result(2*9 + 1) = iotas(8);
   result(2*9 + 2) = iotas(9);
   result(2*9 + 4) = iotas(10);
   result(2*9 + 5) = iotas(11);

   //from coarse grid point 3
   result(3*9 + 0) = iotas(12);
   result(3*9 + 1) = iotas(13);
   result(3*9 + 3) = iotas(14);
   result(3*9 + 4) = iotas(15);

   // Test for valid values
   for(int i = 0; i<TWO_POWER_D_TIMES_THREE_POWER_D; i++){
     assertion2(result(i) != std::numeric_limits<double>::infinity(), i, result(i));
     assertion2(result(i) == result(i), i, result(i));
   }

#else
   for(int i = 0; i < TWO_POWER_D_TIMES_THREE_POWER_D; i++){
     result(i) = 0.0;
   }

   logDebug("computeBoxMGIntergridTransferOperator(...): ", "To be implemented");
#endif

   return result;
}

tarch::la::Vector<16, double> matrixfree::solver::Multigrid::solveGammaSystems(
    tarch::la::Vector<3, double> stencil1,
    tarch::la::Vector<3, double> stencil2,
    tarch::la::Vector<3, double> stencil3,
    tarch::la::Vector<3, double> stencil4,
    tarch::la::Vector<3, double> stencil5,
    tarch::la::Vector<3, double> stencil6,
    tarch::la::Vector<3, double> stencil7,
    tarch::la::Vector<3, double> stencil8){

  tarch::la::Vector<16, double> result;
  tarch::la::Vector<2, double> rtemp;
  tarch::la::Vector<3,double> g1;
  tarch::la::Vector<3,double> g2;

  //bottom edge from c0
  g1 = stencil1;
  g2 = stencil2;
  g2(2) = 0.0;
  rtemp = solve2x2System(g1, g2);
  result(0) = rtemp(0);
  result(1) = rtemp(1);

  //bottom edge from c1
  g1(0) = 0.0;
  g2 = stencil2;
  rtemp = solve2x2System(g1, g2);
  result(2) = rtemp(0);
  result(3) = rtemp(1);

  //left edge from c0
  g1 = stencil3;
  g2 = stencil5;
  g2(2) = 0.0;
  rtemp = solve2x2System(g1, g2);
  result(4) = rtemp(0);
  result(8) = rtemp(1);

  //left edge from c2
  g1(0) = 0.0;
  g2 = stencil5;
  rtemp = solve2x2System(g1, g2);
  result(6) = rtemp(0);
  result(10) = rtemp(1);

  //right edge from c1
  g1 = stencil4;
  g2 = stencil6;
  g2(2) = 0.0;
  rtemp = solve2x2System(g1, g2);
  result(5) = rtemp(0);
  result(9) = rtemp(1);

  //right edge from c3
  g1(0) = 0.0;
  g2 = stencil6;
  rtemp = solve2x2System(g1, g2);
  result(7) = rtemp(0);
  result(11) = rtemp(1);

  //top edge from c2
  g1 = stencil7;
  g2 = stencil8;
  g2(2) = 0.0;
  rtemp = solve2x2System(g1, g2);
  result(12) = rtemp(0);
  result(13) = rtemp(1);

  //top edge from c3
  g1(0) = 0.0;
  g2 = stencil8;
  rtemp = solve2x2System(g1, g2);
  result(14) = rtemp(0);
  result(15) = rtemp(1);

  return result;

}

tarch::la::Vector<16, double> matrixfree::solver::Multigrid::solveIotaSystem(
    tarch::la::Vector<9, double> stencil1,
    tarch::la::Vector<9, double> stencil2,
    tarch::la::Vector<9, double> stencil3,
    tarch::la::Vector<9, double> stencil4,
    tarch::la::Vector<16, double> gammas){

  tarch::la::Vector<16, double> result;
  tarch::la::Vector<4, double> tempresult;

  tarch::la::Matrix<4, 4, double> a;
  tarch::la::Vector<4, double> rhs;

  //from c0
  a(0, 0) = stencil1(4);
  a(1, 0) = stencil1(5);
  a(2, 0) = stencil1(7);
  a(3, 0) = stencil1(8);
  rhs(0) = -(stencil1(0)*1.0 + stencil1(1)*gammas(0) + stencil1(2)*gammas(1) + stencil1(3)* gammas(4) + stencil1(6)*gammas(8));

  a(0, 1) = stencil2(3);
  a(1, 1) = stencil2(4);
  a(2, 1) = stencil2(6);
  a(3, 1) = stencil2(7);
  rhs(1) = -(stencil2(0)*gammas(2) + stencil2(1)*gammas(3) + stencil2(2)*0.0 + stencil2(5)*0.0 + stencil2(8)*0.0);

  a(0, 2) = stencil3(1);
  a(1, 2) = stencil3(2);
  a(2, 2) = stencil3(4);
  a(3, 2) = stencil3(5);
  rhs(2) = -(stencil3(0)*gammas(6) + stencil3(3)*gammas(10) + stencil3(6)*0.0 + stencil3(7)*0.0 + stencil3(8)*0.0);

  a(0, 3) = stencil4(0);
  a(1, 3) = stencil4(1);
  a(2, 3) = stencil4(3);
  a(3, 3) = stencil4(4);
  rhs(3) = -(stencil4(2)*0.0 + stencil4(5)*0.0 + stencil4(6)*0.0 + stencil4(7)*0.0 + stencil4(8)*0.0);

  tempresult = solve4x4System(a, rhs);
  result(0) = tempresult(0);
  result(1) = tempresult(1);
  result(2) = tempresult(2);
  result(3) = tempresult(3);

  //from c1
//  a(0, 0) = stencil1(4);
//  a(1, 0) = stencil1(5);
//  a(2, 0) = stencil1(7);
//  a(3, 0) = stencil1(8);
  rhs(0) = -(stencil1(0)*0.0 + stencil1(1)*gammas(2) + stencil1(2)*gammas(3) + stencil1(3)*0.0 + stencil1(6)*0.0);

//  a(0, 1) = stencil2(3);
//  a(1, 1) = stencil2(4);
//  a(2, 1) = stencil2(6);
//  a(3, 1) = stencil2(7);
  rhs(1) = -(stencil2(0)*gammas(2) + stencil2(1)*gammas(3) + stencil2(2)*1.0 + stencil2(5)*gammas(5) + stencil2(8)*gammas(9));

//  a(0, 2) = stencil3(1);
//  a(1, 2) = stencil3(2);
//  a(2, 2) = stencil3(4);
//  a(3, 2) = stencil3(5);
  rhs(2) = -(stencil3(0)*0.0 + stencil3(3)*0.0 + stencil3(6)*0.0 + stencil3(7)*0.0 + stencil3(8)*0.0);

//  a(0, 3) = stencil4(0);
//  a(1, 3) = stencil4(1);
//  a(2, 3) = stencil4(3);
//  a(3, 3) = stencil4(4);
  rhs(3) = -(stencil4(2)*gammas(7) + stencil4(5)*gammas(11) + stencil4(6)*0.0 + stencil4(7)*0.0 + stencil4(8)*0.0);

  tempresult = solve4x4System(a, rhs);
  result(4) = tempresult(0);
  result(5) = tempresult(1);
  result(6) = tempresult(2);
  result(7) = tempresult(3);

  //from c2
//  a(0, 0) = stencil1(4);
//  a(1, 0) = stencil1(5);
//  a(2, 0) = stencil1(7);
//  a(3, 0) = stencil1(8);
  rhs(0) = -(stencil1(0)*0.0 + stencil1(1)*0.0 + stencil1(2)*0.0 + stencil1(3)*gammas(6) + stencil1(6)*gammas(10));

//  a(0, 1) = stencil2(3);
//  a(1, 1) = stencil2(4);
//  a(2, 1) = stencil2(6);
//  a(3, 1) = stencil2(7);
  rhs(1) = -(stencil2(0)*0.0 + stencil2(1)*0.0 + stencil2(2)*0.0 + stencil2(5)*0.0 + stencil2(8)*0.0);

//  a(0, 2) = stencil3(1);
//  a(1, 2) = stencil3(2);
//  a(2, 2) = stencil3(4);
//  a(3, 2) = stencil3(5);
  rhs(2) = -(stencil3(0)*gammas(6) + stencil3(3)*gammas(10) + stencil3(6)*1.0 + stencil3(7)*gammas(12) + stencil3(8)*gammas(13));

//  a(0, 3) = stencil4(0);
//  a(1, 3) = stencil4(1);
//  a(2, 3) = stencil4(3);
//  a(3, 3) = stencil4(4);
  rhs(3) = -(stencil4(2)*0.0 + stencil4(5)*0.0 + stencil4(6)*gammas(12) + stencil4(7)*gammas(13) + stencil4(8)*0.0);

  tempresult = solve4x4System(a, rhs);
  result(8) = tempresult(0);
  result(9) = tempresult(1);
  result(10) = tempresult(2);
  result(11) = tempresult(3);

  //from c3
//  a(0, 0) = stencil1(4);
//  a(1, 0) = stencil1(5);
//  a(2, 0) = stencil1(7);
//  a(3, 0) = stencil1(8);
  rhs(0) = -(stencil1(0)*0.0 + stencil1(1)*0.0 + stencil1(2)*0.0 + stencil1(3)*0.0 + stencil1(6)*0.0);

//  a(0, 1) = stencil2(3);
//  a(1, 1) = stencil2(4);
//  a(2, 1) = stencil2(6);
//  a(3, 1) = stencil2(7);
  rhs(1) = -(stencil2(0)*0.0 + stencil2(1)*0.0 + stencil2(2)*0.0 + stencil2(5)*gammas(7) + stencil2(8)*gammas(11));

//  a(0, 2) = stencil3(1);
//  a(1, 2) = stencil3(2);
//  a(2, 2) = stencil3(4);
//  a(3, 2) = stencil3(5);
  rhs(2) = -(stencil3(0)*0.0 + stencil3(3)*0.0 + stencil3(6)*0.0 + stencil3(7)*gammas(14) + stencil3(8)*gammas(15));

//  a(0, 3) = stencil4(0);
//  a(1, 3) = stencil4(1);
//  a(2, 3) = stencil4(3);
//  a(3, 3) = stencil4(4);
  rhs(3) = -(stencil4(2)*gammas(7) + stencil4(5)*gammas(11) + stencil4(6)*gammas(14) + stencil4(7)*gammas(15) + stencil4(8)*1.0);

  tempresult = solve4x4System(a, rhs);
  result(12) = tempresult(0);
  result(13) = tempresult(1);
  result(14) = tempresult(2);
  result(15) = tempresult(3);

  return result;

}

tarch::la::Vector<2, double> matrixfree::solver::Multigrid::solve2x2System(
    tarch::la::Vector<3, double> stencil1,
    tarch::la::Vector<3, double> stencil2){

	tarch::la::Vector<2, double> result;

	double invdet = 1.0/(stencil1(1)*stencil2(1) - stencil1(2)*stencil2(0));

	// Test for nan
	assertion2(invdet != std::numeric_limits<double>::infinity(), stencil1, stencil2);
	assertion2(invdet == invdet, stencil1, stencil2);

	result(0) = invdet*(stencil2(1)*(-stencil1(0)) + (-stencil1(2)*(-stencil2(2))));

	result(1) = invdet*((-stencil2(0))*(-stencil1(0)) + stencil1(1)*(-stencil2(2)));

	return result;

}

tarch::la::Vector<4, double> matrixfree::solver::Multigrid::solve4x4System(
    tarch::la::Matrix<4, 4, double> a,
    tarch::la::Vector<4, double> rhs){

  tarch::la::Vector<4, double> result;

  tarch::la::Matrix<4, 4, double> q;
  tarch::la::Matrix<4, 4, double> r;

  //QR decomposition for solving the system (A = Q*R)
  modifiedGramSchmidt(a, q, r);

  tarch::la::Vector<4, double> tempRhs;

  //tempRhs = Q^{t}*rhs
  tempRhs = transpose(q)*rhs;

  // back substitution (x = R^{-1}*tempRhs)
  for(int k = 3; k >= 0; k--){
    for(int i = k+1; i <= 3; i++){
      tempRhs(k) -= r(k, i)*result(i);
    }
    result(k) = tempRhs(k)/r(k, k);
  }

  return result;
}


int matrixfree::solver::Multigrid::getNumberOfStencilUpdates() const {
  return _numberOfStencilEvaluations;
}


void matrixfree::solver::Multigrid::clearNumberOfStencilUpdates() {
  _numberOfStencilEvaluations = 0;
}


tarch::la::Vector<TWO_POWER_D_TIMES_THREE_POWER_D, double> matrixfree::solver::Multigrid::addUpdateToStencils(
	const tarch::la::Vector<TWO_POWER_D_TIMES_THREE_POWER_D, double>&	 verticesStencils,
	const tarch::la::Matrix<TWO_POWER_D, TWO_POWER_D, double>&         cellWiseStencilUpdate
) const {
	tarch::la::Vector<TWO_POWER_D_TIMES_THREE_POWER_D, double> result = verticesStencils;

	dfor2(c) //Run over coarse vertices of the cell
		tarch::la::Vector<TWO_POWER_D, int> positionsInA = getPositionsInA(cScalar);
		dfor2(f) //Run over fine vertices (i.e., rows of the four times four cell-wise stencil update and at the same time entries of positionsInA)
			int vertexIndex = cScalar*THREE_POWER_D + positionsInA(fScalar); //index in verticesStencils
			result(vertexIndex) = result(vertexIndex) + cellWiseStencilUpdate(cScalar, fScalar);
		enddforx
	enddforx

	return result;
}


tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double> matrixfree::solver::Multigrid::fillInIntergridTransferOperators(
  const tarch::la::Vector<TWO_POWER_D_TIMES_THREE_POWER_D,double>&  operators3x3,
  const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&  operators5x5
) const {
  tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double> result = operators5x5;

  dfor2(c) //Run over coarse vertices of the cell
    tarch::la::Vector<THREE_POWER_D, int> positionsInOperator = getPositionsInIntergridTransferOperator(cScalar);
    for(int i = 0; i<THREE_POWER_D; i++){
      int vertexIndex = cScalar*FIVE_POWER_D + positionsInOperator(i);
      double value = operators3x3(cScalar*THREE_POWER_D + i);
      result(vertexIndex) = value;
    }
  enddforx

  return result;
}


tarch::la::Vector<TWO_POWER_D, int> matrixfree::solver::Multigrid::getPositionsInA(
		const int coarseVertexNumber
){
	tarch::la::Vector<TWO_POWER_D, int> result;

	switch(coarseVertexNumber){

	case 0:
		tarch::la::assignList(result) = 4, 5, 7, 8;
		break;
	case 1:
		tarch::la::assignList(result) = 3, 4, 6, 7;
		break;
	case 2:
		tarch::la::assignList(result) = 1, 2, 4, 5;
		break;
	case 3:
		tarch::la::assignList(result) = 0, 1, 3, 4;
		break;
	}

	return result;
}

tarch::la::Vector<THREE_POWER_D, int> matrixfree::solver::Multigrid::getPositionsInIntergridTransferOperator(
  const int coarseVertexNumber
){
  tarch::la::Vector<THREE_POWER_D, int> result;

  switch(coarseVertexNumber){

  case 0:
    tarch::la::assignList(result) = 12, 13, 14, 17, 18, 19, 22, 23, 24;
    break;
  case 1:
    tarch::la::assignList(result) = 10, 11, 12, 15, 16, 17, 20, 21, 22;
    break;
  case 2:
    tarch::la::assignList(result) = 2, 3, 4, 7, 8, 9, 12, 13, 14;
    break;
  case 3:
    tarch::la::assignList(result) = 0, 1, 2, 5, 6, 7, 10, 11, 12;
    break;
  }

  return result;

}
*/
