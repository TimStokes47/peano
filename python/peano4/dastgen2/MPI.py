# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from dastgen2 import *


class MPI(object):
  """ 
  
  Represents Peano's MPI aspect injected into a DaStGen model. 
  It mainly ensures that we include the right headers.
    
  """
  def __init__(self):
    pass


  def set_model(self,data_model):
    self._data_model = data_model

  def get_include(self):
    return """
#include "tarch/mpi/Rank.h"
"""
  
  def get_attributes(self):
    return ""
  
  def get_method_declarations(self,full_qualified_name):
    return """
#ifdef Parallel
    static void sendAndPollDanglingMessages(const """ + full_qualified_name + """& message, int destination, int tag );
    static void receiveAndPollDanglingMessages(""" + full_qualified_name + """& message, int source, int tag );
#endif
    """


  def get_implementation(self,full_qualified_name):
    return """
#ifdef Parallel
void """ + full_qualified_name + """::sendAndPollDanglingMessages(const """ + full_qualified_name + """& message, int destination, int tag ) {
  """ + full_qualified_name + """::send(
    message, destination, tag,
    [&]() {
      int  timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp();
      int  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp();
      bool triggeredTimeoutWarning = false;
      if (
        tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() &&
        (clock()>timeOutWarning) &&
        (!triggeredTimeoutWarning)
      ) {
        tarch::mpi::Rank::getInstance().writeTimeOutWarning( """ + "\"" + full_qualified_name + "\"" + """, "sendAndPollDanglingMessages()",destination, tag );
        triggeredTimeoutWarning = true;
      }
      if (
        tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
        (clock()>timeOutShutdown)
      ) {
        tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( """ + "\"" + full_qualified_name + "\"" + """, "sendAndPollDanglingMessages()", destination, tag );
      }
      tarch::mpi::Rank::getInstance().receiveDanglingMessages();
    },
    tarch::mpi::Rank::getInstance().getCommunicator()
  );
}


void """ + full_qualified_name + """::receiveAndPollDanglingMessages(""" + full_qualified_name + """& message, int source, int tag ) {
  """ + full_qualified_name + """::receive(
    message, source, tag,
    [&]() {
      int  timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp();
      int  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp();
      bool triggeredTimeoutWarning = false;
      if (
        tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() &&
        (clock()>timeOutWarning) &&
        (!triggeredTimeoutWarning)
      ) {
        tarch::mpi::Rank::getInstance().writeTimeOutWarning( """ + "\"" + full_qualified_name + "\"" + """, "receiveAndPollDanglingMessages()", source, tag );
        triggeredTimeoutWarning = true;
      }
      if (
        tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
        (clock()>timeOutShutdown)
      ) {
        tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( """ + "\"" + full_qualified_name + "\"" + """, "receiveAndPollDanglingMessages()", source, tag );
      }
      tarch::mpi::Rank::getInstance().receiveDanglingMessages();
    },
    tarch::mpi::Rank::getInstance().getCommunicator()
  );
}
#endif

    """
    




