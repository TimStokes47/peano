#include "PingPongTest.h"
#include "../StartTraversalMessage.h"


#include "tarch/la/Vector.h"
#include "tarch/mpi/Rank.h"

defineTest(peano4::parallel::tests::PingPongTest)


tarch::logging::Log peano4::parallel::tests::PingPongTest::_log("peano4::parallel::tests::PingPongTest");


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


void peano4::parallel::tests::PingPongTest::testBuiltInType() {
  #ifdef Parallel
  int out = 23;
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
    MPI_Send(&out,1,MPI_INT,1,0,MPI_COMM_WORLD);
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    int in = 25;
    MPI_Recv(&in,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    validateEquals( in, out );
  }
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
}


void peano4::parallel::tests::PingPongTest::testMultithreadedPingPong() {
  #ifdef Parallel
  int out = 23;
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
	for (int i=0; i<tarch::multicore::Core::getInstance().getNumberOfThreads(); i++) {
      MPI_Send(&out,1,MPI_INT,1,i,MPI_COMM_WORLD);
	}
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    int in = 25;
	for (int i=0; i<tarch::multicore::Core::getInstance().getNumberOfThreads(); i++) {
      MPI_Recv(&in,1,MPI_INT,0,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      validateEquals( in, out );
	}
  }
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
}


void peano4::parallel::tests::PingPongTest::testDaStGenType() {
  #ifdef Parallel
  StartTraversalMessage out;
  out.setStepIdentifier(23);
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
    out.send(1,0,false,StartTraversalMessage::ExchangeMode::Blocking);
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    StartTraversalMessage in;
    in.receive(0,0,false,StartTraversalMessage::ExchangeMode::Blocking);
    validateEqualsWithParams2( in.getStepIdentifier(), out.getStepIdentifier(), in.toString(), out.toString() );
  }
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
}


void peano4::parallel::tests::PingPongTest::testDaStGenArray() {
  #ifdef Parallel
  StartTraversalMessage out[10];
  out[0].setStepIdentifier(23);
  out[1].setStepIdentifier(24);
  out[2].setStepIdentifier(25);
  out[3].setStepIdentifier(26);
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==0) {
    MPI_Send(out,4,StartTraversalMessage::FullDatatype,1,0,MPI_COMM_WORLD);
  }
  if ( tarch::mpi::Rank::getInstance().getNumberOfRanks()>=2 and tarch::mpi::Rank::getInstance().getRank()==1) {
    StartTraversalMessage in[10];
    MPI_Recv(in,4,StartTraversalMessage::FullDatatype,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    validateEqualsWithParams8(
      in[0].getStepIdentifier(), out[0].getStepIdentifier(),
	  in[0].toString(), out[0].toString(),
	  in[1].toString(), out[1].toString(),
	  in[2].toString(), out[2].toString(),
	  in[3].toString(), out[3].toString()
	);
    validateEqualsWithParams8(
      in[1].getStepIdentifier(), out[1].getStepIdentifier(),
	  in[0].toString(), out[0].toString(),
	  in[1].toString(), out[1].toString(),
	  in[2].toString(), out[2].toString(),
	  in[3].toString(), out[3].toString()
	);
    validateEqualsWithParams8(
      in[2].getStepIdentifier(), out[2].getStepIdentifier(),
	  in[0].toString(), out[0].toString(),
	  in[1].toString(), out[1].toString(),
	  in[2].toString(), out[2].toString(),
	  in[3].toString(), out[3].toString()
	);
    validateEqualsWithParams8(
      in[3].getStepIdentifier(), out[3].getStepIdentifier(),
	  in[0].toString(), out[0].toString(),
	  in[1].toString(), out[1].toString(),
	  in[2].toString(), out[2].toString(),
	  in[3].toString(), out[3].toString()
	);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  #endif
}


void peano4::parallel::tests::PingPongTest::run() {
  testMethod(  testBuiltInType );
  testMethod(  testDaStGenType );
  testMethod(  testDaStGenArray );
  testMethod(  testMultithreadedPingPong );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
