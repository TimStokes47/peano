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
    public:
      static MPI_Datatype  Datatype;
    private:
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
    static bool isMessageInQueue(int tag, MPI_Comm communicator);
    #endif
"""


  def get_implementation(self,full_qualified_name):
    result = """
#ifdef Parallel

MPI_Datatype """ + full_qualified_name + """::Datatype;

int """ + full_qualified_name + """::getSenderRank() const {
  return _senderDestinationRank;
}


void """ + full_qualified_name + """::initDatatype() {
  """
    result += full_qualified_name + """  instances[2];
    
  MPI_Datatype subtypes[] = { """


    flattened_mpi_attributes = []
    for i in self._data_model._attributes:
      flattened_mpi_attributes += i.get_native_MPI_type()
      
    wrote_first = False
    for i in flattened_mpi_attributes:
      if wrote_first:
        result += ", "
      else:
        wrote_first = True
      result += i[0]
  
  
    result += """ };
    
  int blocklen[] = { """ 
  
    wrote_first = False
    for i in flattened_mpi_attributes:
      if wrote_first:
        result += ", "
      else:
        wrote_first = True
      result += str(i[1])
    
    result += """ };

  const int NumberOfAttributes = """
    result += str(len(flattened_mpi_attributes))
    result += """;
    
  MPI_Aint  baseFirstInstance;
  MPI_Aint  baseSecondInstance;
  MPI_Get_address( &instances[0], &baseFirstInstance );
  MPI_Get_address( &instances[1], &baseSecondInstance );
  MPI_Aint  disp[ NumberOfAttributes ];
  int       currentAddress = 0;
"""
    for i in self._data_model._attributes:
      for ii in i.get_first_plain_C_attribute():
        result += "  MPI_Get_address( &(instances[0]."
        result += ii[0]
        result += "), &disp[currentAddress] );\n"
        result += "  currentAddress++;\n"
      
    result += """
  MPI_Aint offset = disp[0] - baseFirstInstance;
  MPI_Aint extent = baseSecondInstance - baseFirstInstance - offset;
  for (int i="""
    result += str(len(self._data_model._attributes)) 
    result += """-1; i>=0; i--) {
    disp[i] = disp[i] - disp[0];
  }

  int errorCode = 0; 
  MPI_Datatype tmpType; 
  errorCode += MPI_Type_create_struct( NumberOfAttributes, blocklen, disp, subtypes, &tmpType );
  errorCode += MPI_Type_create_resized( tmpType, offset, extent, &Datatype );
  errorCode += MPI_Type_commit( &Datatype );
  if (errorCode) std::cerr << "error committing MPI datatype in " << __FILE__ << ":" << __LINE__ << std::endl;
}


void """ + full_qualified_name + """::shutdownDatatype() {
  MPI_Type_free( &Datatype );
}


void """ + full_qualified_name + """::send(const """ + full_qualified_name + """& buffer, int destination, int tag, MPI_Comm communicator ) {
  MPI_Send( &buffer, 1, Datatype, destination, tag, communicator);
}


void """ + full_qualified_name + """::receive(""" + full_qualified_name + """& buffer, int source, int tag, MPI_Comm communicator ) {
  MPI_Status status;
  MPI_Recv( &buffer, 1, Datatype, source, tag, communicator, &status);
  buffer._senderDestinationRank = status.MPI_SOURCE;
}


void """ + full_qualified_name + """::send(const """ + full_qualified_name + """& buffer, int destination, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {
  MPI_Request sendRequestHandle; 
  int         flag = 0; 
  MPI_Isend( &buffer, 1, Datatype, destination, tag, communicator, &sendRequestHandle ); 
  MPI_Test( &sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( &sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  }
}


void """ + full_qualified_name + """::receive(""" + full_qualified_name + """& buffer, int source, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {   
  MPI_Status  status;
  MPI_Request receiveRequestHandle; 
  int         flag = 0; 
  MPI_Irecv( &buffer, 1, Datatype, source, tag, communicator, &receiveRequestHandle ); 
  MPI_Test( &receiveRequestHandle, &flag, &status ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( &receiveRequestHandle, &flag, &status ); 
  }
  buffer._senderDestinationRank = status.MPI_SOURCE;
}


bool """ + full_qualified_name + """::isMessageInQueue(int tag, MPI_Comm communicator) {
  int  flag        = 0;
  MPI_Iprobe(
    MPI_ANY_SOURCE, tag,
    communicator, &flag, MPI_STATUS_IGNORE
  );
  return flag;
}

#endif
""" 
    return result;    




