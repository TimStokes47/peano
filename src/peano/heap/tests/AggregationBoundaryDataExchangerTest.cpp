#include "peano/heap/tests/AggregationBoundaryDataExchangerTest.h"
#include "peano/heap/AggregationBoundaryDataExchanger.h"

#include "peano/heap/CompressedFloatingPointNumbers.h"
#include "peano/heap/SendReceiveTask.h"

#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/tests/TestCaseFactory.h"


registerTest(peano::heap::tests::AggregationBoundaryDataExchangerTest)


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


tarch::logging::Log  peano::heap::tests::AggregationBoundaryDataExchangerTest::_log( "peano::heap::tests::AggregationBoundaryDataExchangerTest" );

 
peano::heap::tests::AggregationBoundaryDataExchangerTest::AggregationBoundaryDataExchangerTest():
  tarch::tests::TestCase( "peano::heap::tests::AggregationBoundaryDataExchangerTest" ) {
}


peano::heap::tests::AggregationBoundaryDataExchangerTest::~AggregationBoundaryDataExchangerTest() {
}


void peano::heap::tests::AggregationBoundaryDataExchangerTest::run() {
  testMethod( testComposeDecompose1 );
}


void peano::heap::tests::AggregationBoundaryDataExchangerTest::testComposeDecompose1() {
  typedef peano::heap::AggregationBoundaryDataExchanger<char, SendReceiveTask<char>, std::vector<char> >  Exchanger;

  constexpr int NumberOfCharsToEncodeTotalMessageCount = Exchanger::NumberOfCharsToEncodeTotalMessageCount;

  Exchanger exchanger;
  exchanger._numberOfSentMessages = 0;

  for (int i=0; i<Exchanger::NumberOfCharsToEncodeTotalMessageCount; i++) {
    exchanger._aggregatedSendData.push_back(0);
  }

  for (int i=0; i<100000; i++) {
    exchanger.setAggregatedMessageHeader();

    SendReceiveTask<char> receivedTask;
    receivedTask.getMetaInformation().setLength( NumberOfCharsToEncodeTotalMessageCount );
    receivedTask.wrapData( exchanger._aggregatedSendData.data() );

    int sentNumberOfMessgaes = exchanger.getNumberOfMessages( receivedTask );

    validateEqualsWithParams6(
      sentNumberOfMessgaes,i,
	  exchanger._numberOfSentMessages,
	  (int)(std::numeric_limits<char>::max()),
	  (int)(exchanger._aggregatedSendData[0]),
	  (int)(exchanger._aggregatedSendData[1]),
	  (int)(exchanger._aggregatedSendData[2]),
	  (int)(exchanger._aggregatedSendData[3])
	);

    receivedTask.freeMemory();

    exchanger._numberOfSentMessages++;
  }
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
