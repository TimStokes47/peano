"""

 This Python script uses/invokes DaStGen2 to generate all the datatypes
 required by the core.

"""
import dastgen2


integer_message = dastgen2.DataModel( "tarch::mpi::IntegerMessage" )
integer_message.add_attribute( dastgen2.attributes.Integer( "value" ) )

integer_message.write_header_file("IntegerMessage.h")
integer_message.write_implementation_file("IntegerMessage.cpp")
