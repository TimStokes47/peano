#include "tarch/plotter/griddata/VTUTimeSeriesWriter.h"
#include <fstream>

tarch::logging::Log tarch::plotter::griddata::VTUTimeSeriesWriter::_log( "tarch::plotter::griddata::VTUTimeSeriesWriter" );



tarch::plotter::griddata::VTUTimeSeriesWriter::VTUTimeSeriesWriter() {
  _out << "<?xml version=\"1.0\"?>" << std::endl
       << "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl
       << "<Collection>" << std::endl;

}


void tarch::plotter::griddata::VTUTimeSeriesWriter::addSnapshot(const std::string& snapshotFileName, double time) {
  _out << "<DataSet timestep=\"" << time << "\" group=\"\" part=\"0\" file=\"" << snapshotFileName << "\"/>" << std::endl;
}


bool tarch::plotter::griddata::VTUTimeSeriesWriter::writeFile(const std::string& filename) {
  if (filename.rfind(".pvd")==std::string::npos) {
    logWarning( "writeToFile()", "filename should end with .pvd but is " << filename );
  }

  std::ofstream out;
  out.open( filename.c_str(), std::ios::binary );
  if ( (!out.fail()) && out.is_open() ) {
    _log.debug( "close()", "opened data file " + filename );

    out << _out.str();

    out << "</Collection>" << std::endl
	<< "</VTKFile>" << std::endl;

    _log.debug( "close()", "data written to " + filename );

    return true;
  }
  else {
    _log.error( "close()", "unable to write output file " + filename );
    return false;
  }
}
