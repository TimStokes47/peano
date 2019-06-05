#if  defined(SharedTBB)  && defined(Parallel)
#include "PerformanceMonitor.h"

//#include "peano/datatraversal/TaskSet.h"

#include "tarch/multicore/Lock.h"
#include "tarch/parallel/Node.h"

tarch::logging::Log tarch::multicore::PerformanceMonitor::_log( "tarch::multicore::perfmon" );

tarch::multicore::PerformanceMonitor::PerformanceMonitor() :
  _isStarted(false),
  _gather_request(MPI_REQUEST_NULL)
{
	_currentNumBackgroundTasks = new int[tarch::parallel::Node::getInstance().getNumberOfNodes()];
	_currentNumBackgroundTasksBuffer = new int[tarch::parallel::Node::getInstance().getNumberOfNodes()];
	//MPI_Comm_dup(MPI_COMM_WORLD, &_mpi_comm);
}

tarch::multicore::PerformanceMonitor::~PerformanceMonitor() {
	delete[] _currentNumBackgroundTasks;
	delete[] _currentNumBackgroundTasksBuffer;
}

tarch::multicore::PerformanceMonitor& tarch::multicore::PerformanceMonitor::getInstance() {
	static PerformanceMonitor perfMon;
	return perfMon;
}

void tarch::multicore::PerformanceMonitor::start() {
	_isStarted=true;
	//logInfo("performance monitor","perfmon has started");
//	PerformanceMonitorJob performanceMonitorJob;
//    peano::datatraversal::TaskSet spawnedSet( performanceMonitorJob, peano::datatraversal::TaskSet::TaskType::Background  );

}

void tarch::multicore::PerformanceMonitor::stop() {
	if(_isStarted) {
		_isStarted=false;
		logInfo("performance monitor","perfmon has stopped");
	}
	while(!isGloballyTerminated())
		progressGather();
}

void tarch::multicore::PerformanceMonitor::setCurrentNumberOfBGTasks(int num) {
	int myRank = tarch::parallel::Node::getInstance().getRank();
	_currentNumBackgroundTasks[myRank]=num;
}

void tarch::multicore::PerformanceMonitor::run() {

	int myRank = tarch::parallel::Node::getInstance().getRank();
    std::string str="perfmon is active ";
    for(int i=0;i<tarch::parallel::Node::getInstance().getNumberOfNodes();i++)  str=str+" , "+std::to_string(_currentNumBackgroundTasks[i]);
	//logInfo("performance monitor",str);
	progressGather();
}

void tarch::multicore::PerformanceMonitor::progressGather() {
	int myRank = tarch::parallel::Node::getInstance().getRank();
	int completed = 0;
    tarch::multicore::Lock lock(_semaphore);

    if(!isGloballyTerminated())   MPI_Test(&_gather_request, &completed, MPI_STATUS_IGNORE);

    if(_gather_request==MPI_REQUEST_NULL && !isGloballyTerminated()) {
    	postGather();
    }

    if(completed) {
        std::string str="received new update ";
        for(int i=0;i<tarch::parallel::Node::getInstance().getNumberOfNodes();i++)  str=str+" , "+std::to_string(_currentNumBackgroundTasksBuffer[i]);
#if defined(PerformanceAnalysisStealing)
        logInfo("performance monitor", str);
#endif
    	for(int i=0;i<tarch::parallel::Node::getInstance().getNumberOfNodes();i++)  _currentNumBackgroundTasks[i]=_currentNumBackgroundTasksBuffer[i];
    }
    lock.free();

}

void tarch::multicore::PerformanceMonitor::postGather() {
	static int terminateSignal=-1;
	int myRank = tarch::parallel::Node::getInstance().getRank();
	int *buffer = _isStarted ? &_currentNumBackgroundTasks[myRank]  : &terminateSignal;
    //std::cout<<"Sending "<<*buffer<<std::endl;

	MPI_Iallgather(buffer, 1, MPI_INTEGER, _currentNumBackgroundTasksBuffer, 1, MPI_INTEGER, MPI_COMM_WORLD, &_gather_request);
}

bool tarch::multicore::PerformanceMonitor::isGloballyTerminated() {
        static bool globalTermination=false;        
        if(globalTermination) return true;
        if(_isStarted) return false;        
                
        bool result=true;
        for(int i=0;i<tarch::parallel::Node::getInstance().getNumberOfNodes();i++)  result= result &&  _currentNumBackgroundTasksBuffer[i]==-1;
        globalTermination=result; 

        return globalTermination;
}

/*tarch::multicore::PerformanceMonitorJob::PerformanceMonitorJob() {

}

tarch::multicore::PerformanceMonitorJob::~PerformanceMonitorJob(){

}

bool tarch::multicore::PerformanceMonitorJob::operator()() {
	logInfo("performance monitor job","perfmon is active");
	std::cout<<"num current "<<tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs()<<std::endl;
    return true;
}*/
#endif
