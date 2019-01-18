#include "PingPongTest.h"
#include "../StartTraversalMessage.h"


#include "tarch/la/Vector.h"
#include "tarch/mpi/Rank.h"

#include "tarch/tests/TestCaseFactory.h"
registerTest(peano4::parallel::tests::PingPongTest)


tarch::logging::Log peano4::parallel::tests::PingPongTest::_log("peano4::parallel::tests::PingPongTest");


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


void peano4::parallel::tests::PingPongTest::testBuiltInType() {
  int out = 23;
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
	// todo Change
	logWarning( "testBuiltInType()", "out=" << out );
    MPI_Send(&out,1,MPI_INT,1,0,MPI_COMM_WORLD);
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    int in = 25;
    MPI_Recv(&in,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	// todo Change
	logWarning( "testBuiltInType()", "in=" << in );
    validateEquals( in, out );
  }
  MPI_Barrier(MPI_COMM_WORLD);
}


void peano4::parallel::tests::PingPongTest::testDaStGenType() {
  StartTraversalMessage out;
  out.setStepIdentifier(23);
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
	// todo Change
	logWarning( "testDaStGenType()", "out=" << out.toString() );
    out.send(1,0,false,StartTraversalMessage::ExchangeMode::Blocking);
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    StartTraversalMessage in;
    in.receive(0,0,false,StartTraversalMessage::ExchangeMode::Blocking);
	// todo Change
	logWarning( "testDaStGenType()", "in=" << in.toString() );
    validateEqualsWithParams2( in.getStepIdentifier(), out.getStepIdentifier(), in.toString(), out.toString() );
  }
  MPI_Barrier(MPI_COMM_WORLD);
}


void peano4::parallel::tests::PingPongTest::testDaStGenArray() {
  StartTraversalMessage out[10];
  out[0].setStepIdentifier(23);
  out[1].setStepIdentifier(24);
  out[2].setStepIdentifier(25);
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
    MPI_Send(out,3,StartTraversalMessage::FullDatatype,1,0,MPI_COMM_WORLD);
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    StartTraversalMessage in[10];
    MPI_Recv(in,3,StartTraversalMessage::FullDatatype,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    validateEqualsWithParams2( in[0].getStepIdentifier(), out[0].getStepIdentifier(), in[0].toString(), out[0].toString() );
    validateEqualsWithParams2( in[1].getStepIdentifier(), out[1].getStepIdentifier(), in[1].toString(), out[1].toString() );
    validateEqualsWithParams2( in[2].getStepIdentifier(), out[2].getStepIdentifier(), in[2].toString(), out[2].toString() );
  }
  MPI_Barrier(MPI_COMM_WORLD);
}


void peano4::parallel::tests::PingPongTest::run() {
  testMethod(  testBuiltInType );
  testMethod(  testDaStGenType );
  testMethod(  testDaStGenArray );

}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
