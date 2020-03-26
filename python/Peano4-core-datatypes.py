"""

 This Python script uses/invokes DaStGen2 to generate all the datatypes
 required by the core.

"""
import dastgen2
import peano4.dastgen2


#
# tarch::mpi::IntegerMessage
#
integer_message = dastgen2.DataModel( "tarch::mpi::IntegerMessage" )
integer_message.add_attribute( dastgen2.attributes.Integer( "value" ) )

integer_message.add_aspect( peano4.dastgen2.MPI() )
integer_message.add_aspect( dastgen2.aspects.MPI() )

integer_message.write_header_file("../src/tarch/mpi/IntegerMessage.h")
integer_message.write_implementation_file("../src/tarch/mpi/IntegerMessage.cpp")


#
# tarch::mpi::StringMessage
#
string_message = dastgen2.DataModel( "tarch::mpi::StringMessage" )
string_message.add_attribute( dastgen2.attributes.String( "data" ) )

string_message.add_aspect( peano4.dastgen2.MPI() )
string_message.add_aspect( dastgen2.aspects.MPI() )

string_message.write_header_file("../src/tarch/mpi/StringMessage.h")
string_message.write_implementation_file("../src/tarch/mpi/StringMessage.cpp")




#
# peano4::parallel::TreeManagementMessage
#
tree_management_message = dastgen2.DataModel( "peano4::parallel::TreeManagementMessage" )
tree_management_message.add_attribute( dastgen2.attributes.Integer( "masterSpacetreeId" ) )
tree_management_message.add_attribute( dastgen2.attributes.Integer( "workerSpacetreeId" ) )
tree_management_message.add_attribute( dastgen2.attributes.Enumeration( "action", [ "RequestNewRemoteTree", "CreateNewRemoteTree", "RemoveChildTreeFromBooksAsChildBecameEmpty", "JoinWithWorker", "Acknowledgement" ]))

tree_management_message.add_aspect( peano4.dastgen2.MPI() )
tree_management_message.add_aspect( dastgen2.aspects.MPI() )

tree_management_message.write_header_file("../src/peano4/parallel/TreeManagementMessage.h")
tree_management_message.write_implementation_file("../src/peano4/parallel/TreeManagementMessage.cpp")
