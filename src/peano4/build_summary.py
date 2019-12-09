import matplotlib.pyplot as plt
import os.path


Dimensions_Marker = [ "2d", "3d" ]
MPI_Marker = [ "mpi", "" ]
Multithreading_Marker = [ "omp", "tbb", "C++ threading" ]


def read_build_report( filename, dimensions, mpi, multithreading ):
  """ 
    Return a tuple. The first entry means whether the build is there, the second means that the unit tests have passed
  """
  file = open(filename, "r")
  for line in file:
    pass
  return False




def write_summary():  
  out = open( "build_summary.html", "w" )
  out.write( "<table>")
  out.write( "<tr><td />")
  for dimensions in Dimensions_Marker:
    out.write( "<td colspan=\"" + str(len(MPI_Marker)) + "\">" )   
    out.write( dimensions )   
    out.write( "</td>" )   
  out.write( "</tr>")
  out.write( "<tr><td />")
  for dimensions in Dimensions_Marker:
    for mpi in MPI_Marker:
      out.write( "<td>" )
      out.write( mpi )
      out.write( "</td>" )
  out.write( "</tr>")
  for multithreading in Multithreading_Marker:
    out.write( "<tr>")
    out.write( "<td>")
    out.write( multithreading )
    out.write( "</td>")
    for dimensions in Dimensions_Marker:
      for mpi in MPI_Marker:
        out.write( "<td>" )
        out.write( mpi )
        out.write( "</td>" )
    out.write( "</tr>")
  out.write( "</table>")



if __name__ == "__main__":
  write_summary()

  