"""

 This Python script uses/invokes DaStGen2 to generate all the datatypes
 required by the core.

"""
import dastgen2
import peano4.dastgen2


integer_message = dastgen2.DataModel( "tarch::mpi::IntegerMessage" )
integer_message.add_attribute( dastgen2.attributes.Integer( "value" ) )

integer_message.add_aspect( peano4.dastgen2.MPI() )
integer_message.add_aspect( dastgen2.aspects.MPI() )

integer_message.write_header_file("../src/tarch/mpi/IntegerMessage.h")
integer_message.write_implementation_file("../src/tarch/mpi/IntegerMessage.cpp")
