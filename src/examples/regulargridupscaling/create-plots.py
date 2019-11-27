import matplotlib.pyplot as plt
import os.path
import sys


D     = [2,3]
H     = [ 0.1,       0.05,      0.01,      0.005,     0.001 ]
Flops = [ 1,         100,       10000,     1000000 ]
Color = [ "#ff0000", "#00ff00", "#0000ff", "#a0a000", "#a000a0" ]


def filter_shared_memory():
  for d in [2,3]:
    for h in H:
      for flops in Flops:
        for cores in range(1,64):
          filename = "shared-memory-" + str(cores) + "-cores-" + str(d) + "d-" + str(h) + "-" + str(flops) + "-flops.out"
          if os.path.exists( filename ):
            print( "found " + filename + " write " + filename + ".filter" )
            input_file   = open(filename, "r")
            output_file  = open(filename, "w")
            for line in file:
              if ("start parallel traversals" in line) or ("local unrefined cells" in line) or "terminated successfully" in line:
                output_file.write( line )


def plot_shared_memory():
  for d in [2,3]:
    plt.clf()
    max_cores = 0
    max_serial_time = 0.0
    min_serial_time = 65536.0
    for h in H:
      for flops in Flops:
        max_cells = 0.0
        ydata = []
        xdata = []
        for cores in range(1,64):
          found_file = False
          filename = "shared-memory-" + str(cores) + "-cores-" + str(d) + "d-" + str(h) + "-" + str(flops) + "-flops.out"
          if os.path.exists( filename+".filter" ):
            found_file  = True
            filename   += ".filter"
          elif os.path.exists( filename ):
            found_file = True
          
          if found_file:
            print( "found " + filename )
            file       = open(filename, "r")
            start_time = 0
            for line in file:
              if "start parallel traversals" in line:
                start_time = int( line.split(" ")[0] )
              if "local unrefined cells" in line:
                cells = float( line.split("=")[-1] )
                max_cells = max(cells,max_cells)
              if "terminated successfully" in line:
                time = int( line.split(" ")[0] )
                max_cores = max(max_cores,cores)
                xdata.append(cores)
                new_entry = time - start_time
                ydata.append(new_entry)
                if cores==1:
                  max_serial_time = max(max_serial_time, new_entry/max_cells)
                  min_serial_time = min(min_serial_time, new_entry/max_cells)

        if len(xdata)>0:
          ydata_calibrated = [ y/max_cells for y in ydata ]
          if H.index(h)==0 or Flops.index(flops)==0:
            plt.plot( 
              xdata, ydata_calibrated, 
              label="h=" + str(h) + ",#flops=" + str(flops), 
              #color=Color[Flops.index(flops)],
              #marker=H.index(h)+4 
              color=Color[H.index(h)],
              marker=Flops.index(flops)+4 
            )
          else:
            plt.plot( 
              xdata, ydata_calibrated, 
              color=Color[H.index(h)],
              marker=Flops.index(flops)+4 
            )
            
    xdata = range(1,max_cores)
    plt.plot(
      xdata, [max_serial_time/i for i in xdata],
      "--", color="#000000"
    )
    plt.plot(
      xdata, [min_serial_time/i for i in xdata],
      "--", color="#000000"
    )
            
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
    plt.title( "shared memory scaling " + str(d) + "d" )
    plt.savefig( "shared-memory-" + str(d) + "d.pdf" )


if __name__ == "__main__":
  if len(sys.argv)!=3:
    print( "Usage: python " + sys.argv[0] + " [option] [target]")
    print( "" )
    print( "With option from" )
    print( "  filter            filters out files, i.e. throws away all entries which are not required" )
    print( "  create-plots      generate plots" )
    print( "" )
    print( "With option from" )
    print( "  shared-memory     create shared memory plots" )
  elif sys.argv[1]=="create-plots" and sys.argv[2]=="shared-memory":
    plot_shared_memory()
  else:
    print( "Invalid option. Run without arguments for usage message" )
  
  
