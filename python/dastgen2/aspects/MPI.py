# This file is part of the DaStGen2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from dastgen2 import *


class MPI(object):
  """ 
  
  Represents the MPI aspect injected into a DaStGen model.
    
  """
  def __init__(self):
    pass


  def set_model(self,data_model):
    self._data_model = data_model

  def get_include(self):
    return """
#ifdef Parallel
  #include <mpi.h>
  #include <functional>
#endif
"""
  
  def get_attributes(self):
    return """
    #ifdef Parallel
    static MPI_Datatype  Datatype;
    int                  _senderDestinationRank;
    #endif
"""

  def get_method_declarations(self,full_qualified_name):
    return """
    #ifdef Parallel
    /**
     * @return The rank of the sender of an object. It only make ssense to call
     *         this routine after you've invoked receive with MPI_ANY_SOURCE.
     */
    int getSenderRank() const;
    
    /**
     * To be called prior to any MPI usage of this class.
     */
    static void initDatatype();
    static void shutdownDatatype();
            
    /**
     * In DaStGen (the first version), I had a non-static version of the send
     * as well as the receive. However, this did not work with newer C++11 
     * versions, as a member function using this as pointer usually doesn't 
     * see the vtable while the init sees the object from outside, i.e. 
     * including a vtable. So this routine now is basically an alias for a
     * blocking MPI_Send. 
     */
    static void send(const """ + full_qualified_name + """& buffer, int destination, int tag, MPI_Comm communicator );
    static void receive(""" + full_qualified_name + """& buffer, int source, int tag, MPI_Comm communicator );

    /**
     * Alternative to the other send() where I trigger a non-blocking send an 
     * then invoke the functor until the corresponding MPI_Test tells me that 
     * the message went through. In systems with heavy MPI usage, this can 
     * help to avoid deadlocks.
     */
    static void send(const """ + full_qualified_name + """& buffer, int destination, int tag, std::function<void()> waitFunctor, MPI_Comm communicator );
    static void receive(""" + full_qualified_name + """& buffer, int source, int tag, std::function<void()> waitFunctor, MPI_Comm communicator );

    /**
     * In older DaStGen version, I tried to find out whether a particular 
     * message type is in the MPI queue. That is, I looked whether a message
     * on this tag does exist, and then I looked whether the memory footprint
     * matches via count. I think this is invalid. MPI really looks only into
     * the number of bytes, so you have to know which type drops in once there
     * is a message on a tag.
     */            
    static bool isMessageInQueue(int tag);
    #endif
"""


  def get_implementation(self,full_qualified_name):
    return """
#ifdef Parallel

MPI_Datatype """ + full_qualified_name + """::Datatype;

int """ + full_qualified_name + """::getSenderRank() const {
  return _senderDestinationRank;
}


void """ + full_qualified_name + """::initDatatype() {
}


void """ + full_qualified_name + """::shutdownDatatype() {
}


void """ + full_qualified_name + """::send(const """ + full_qualified_name + """& buffer, int destination, int tag, MPI_Comm communicator ) {
  MPI_Send( &buffer, 1, Datatype, destination, tag, communicator);
}


void """ + full_qualified_name + """::receive(""" + full_qualified_name + """& buffer, int source, int tag, MPI_Comm communicator ) {
  MPI_Status status;
  MPI_Recv( &buffer, 1, Datatype, destination, tag, communicator, &status);
  _senderDestinationRank = status.MPI_Sender;
}


void """ + full_qualified_name + """::send(const """ + full_qualified_name + """& buffer, int destination, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {
  MPI_Request* sendRequestHandle = new MPI_Request(); 
  int          flag = 0; 
  MPI_Isend( &buffer, 1, Datatype, tag, communicator, sendRequestHandle ); 
  MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  }
}


void """ + full_qualified_name + """::receive(int source, int tag, """ + full_qualified_name + """& buffer, std::function<void()> waitFunctor, MPI_Comm communicator ) {  MPI_Request* sendRequestHandle = new MPI_Request(); 
  MPI_Request* receiveRequestHandle = new MPI_Request(); 
  int          flag = 0; 
  MPI_Irecv( &buffer, 1, Datatype, tag, communicator, receiveRequestHandle ); 
  MPI_Test( receiveRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( receiveRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  }
}


bool """ + full_qualified_name + """::isMessageInQueue(int tag, MPI_Comm communicator, MPI_Comm communicator) {
  int  flag        = 0;
  MPI_Iprobe(
    MPI_ANY_SOURCE, tag,
    communicator, &flag, MPI_STATUS_IGNORE
  );
  return flag;
}

#endif
"""    




