# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

import atexit
import datetime
import enum
import io
import os


class LoggerPrintOption(enum.Enum):
    """! Controls whether Logger.log() also prints a message to the console. """

    ALWAYS_PRINT = 1
    IF_VERBOSE = 2
    NEVER_PRINT = 3


class Logger:
    """!

    Singleton logger for the Peano 4 Python API

    Python has no dedicated singleton keyword or construct. The idiomatic way
    to get the same effect is to override __new__, so repeated calls to
    Logger() all return the one and only instance instead of creating a new
    object every time.

    The logger holds an in-memory output stream (self._stream), which is
    protected and never accessed from the outside. Use log()/new_section()
    to accumulate details into it over the course of the run; nothing is
    written to disk until the stream is flushed into file_name.

    Once the singleton is created, it registers itself with atexit, so
    close() is invoked automatically when the program terminates - you do
    not have to call it yourself.

    """

    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._verbose = False
            cls._instance._file_name = "README.md"
            cls._instance._stream = io.StringIO()
            atexit.register(cls._instance.close)
        return cls._instance

    @property
    def verbose(self) -> bool:
        return self._verbose

    @verbose.setter
    def verbose(self, value: bool):
        self._verbose = value

    @property
    def file_name(self) -> str:
        return self._file_name

    @file_name.setter
    def file_name(self, value: str):
        self._file_name = value

    def log(self,
            message: str,
            print_option: LoggerPrintOption = LoggerPrintOption.IF_VERBOSE,
            ):
        """! By default, only printed to the console if verbose is switched on. """
        if print_option == LoggerPrintOption.ALWAYS_PRINT or (
            print_option == LoggerPrintOption.IF_VERBOSE and self._verbose
        ):
            print(message)
        self._stream.write( message )
        self._stream.write( "\n" )


    def new_section(self, section_name: str):
        if self._verbose:
            print("------------------------------")
            print(section_name)
            print("------------------------------")
        self._stream.write("## {}\n\n".format(section_name))

    def close(self):
        """!
        Called automatically when the program terminates (see atexit
        registration in __new__). Flushes the accumulated stream into
        file_name, preceded by a section stating when the file was closed.

        If file_name already exists, we do not overwrite it. Instead, we
        attach a timestamp to the file name, i.e. README.md becomes
        something similar to README-20240101-120000.md.
        
        This routine always prints a report to the console about the newly
        generated logging file.
        """
        closed_at = datetime.datetime.now()
        
        self.new_section("File creation")
        self.log( f"Closed at {closed_at.isoformat()}.")
        
        file_name = self._file_name
        if os.path.exists(file_name):
            base, ext = os.path.splitext(file_name)
            file_name = f"{base}-{closed_at.strftime('%Y%m%d-%H%M%S')}{ext}"
        with open(file_name, "w") as file:
            file.write(self._stream.getvalue())
            
        print( "Wrote output log to file {}".format(file_name))
