import matplotlib.pyplot as plt
import os.path
import sys


Dimensions_Marker = [ "2d", "3d" ]
MPI_Marker = [ "no mpi", "with mpi" ]
Multithreading_Marker = [ "no threading", "omp", "tbb", "C++ threading" ]


def read_build_report( filename, dimensions, mpi, multithreading ):
  """ 
    Return a tuple. The first entry means whether the build is there, the second means that the unit tests have passed
  """
  try:
    file = open(filename, "r")
    firstEntry  = False
    secondEntry = False
    for line in file:
      if "build: " in line:
        if dimensions in line and mpi in line and multithreading in line:
          firstEntry = True
        if "running global test case collection" in line and "ok" in line and firstEntry:
          return (True,True)
        elif "running global test case collection" in line and firstEntry:
          return (True,False)
    return (firstEntry,secondEntry)
  except:
    return (False,False)




def write_summary():  
  out = open( "build_summary.html", "w" )
  out.write( "<table>")
  out.write( "<tr><td />")
  for dimensions in Dimensions_Marker:
    out.write( "<td bgcolor=\"white\" align=\"center\" colspan=\"" + str(len(MPI_Marker)) + "\">" )   
    out.write( dimensions )   
    out.write( "</td>" )   
  out.write( "</tr>")
  out.write( "<tr><td />")
  for dimensions in Dimensions_Marker:
    for mpi in MPI_Marker:
      out.write( "<td bgcolor=\"white\" >" )
      out.write( mpi )
      out.write( "</td>" )
  out.write( "</tr>")
  for multithreading in Multithreading_Marker:
    out.write( "<tr>")
    out.write( "<td bgcolor=\"white\" >")
    out.write( multithreading )
    out.write( "</td>")
    for dimensions in Dimensions_Marker:
      for mpi in MPI_Marker:
        result = read_build_report(sys.argv[1],dimensions,mpi,multithreading)
        if result[1]:
          out.write( "<td bgcolor=\"green\">unit test passed</td>" )
        elif result[0]:
          out.write( "<td bgcolor=\"yellow\">unit test failed</td>" )
        else:
          out.write( "<td bgcolor=\"red\">build failed</td>" )
    out.write( "</tr>")
  out.write( "</table>")



if __name__ == "__main__":
  if len(sys.argv)!=2:
    print( "Usage: python3 " + sys.argv[0] + " input-file")
  else:
    write_summary()

  