import matplotlib.pyplot as plt
import os.path

D = [2,3]
H = [0.5,0.1,0.005,0.001]

def plot_shared_memory():
  for d in [2,3]:
    plt.clf()
    max_cores = 0
    for h in H:
      ydata = []
      xdata = []
      for cores in range(1,64):
        filename = "shared-memory-" + str(cores) + "-cores-" + str(d) + "d-" + str(h) + ".out"
        if os.path.exists( filename ):
          print( "found " + filename )
          file       = open(filename, "r")
          start_time = 0
          for line in file:
            if "start parallel traversals" in line:
              start_time = int( line.split(" ")[0] )
            if "terminated successfully" in line:
              time = int( line.split(" ")[0] )
              max_cores = max(max_cores,cores)
              xdata.append(cores)
              ydata.append(time - start_time)
      if len(xdata)>0:
        plt.plot( xdata, ydata, label="h=" + str(h), marker=H.index(h)+4 )
    plt.legend()
    plt.xlabel( "cores" )
    plt.ylabel( "time/cell" )
    plt.xscale( "log", basex=2 )
    plt.yscale( "log", basey=2 )
    xtics   = [ 1 ]
    xlabels = [ "serial" ]
    while xtics[-1] < max_cores:
      xtics.append( xtics[-1]*2 )
      xlabels.append( str(xtics[-1]) )
    plt.xticks( xtics, xlabels )
    plt.title( "shared memory scaling" )
    plt.savefig( "shared-memory-" + str(d) + "d.pdf" )


if __name__ == "__main__":
  plot_shared_memory()
  
  
