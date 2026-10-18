# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

import atexit
import datetime
import enum
import io
import os
import sys


class LoggerPrintOption(enum.Enum):
    """! Controls whether Logger.log() also prints a message to the console. """

    ALWAYS_PRINT = 1
    IF_VERBOSE = 2
    NEVER_PRINT = 3


class LoggerVerbosity(enum.Enum):
    """! Controls how much detail Logger.log()/new_section() print to the console.

    OFF: nothing tagged IF_VERBOSE is printed.
    TEXT: plain, textual verbose output is printed.
    VISUALISATION: verbose output is printed, and the run additionally
      produces/updates a visualisation of what is going on.

    TEXT and VISUALISATION both switch on the same "verbose" console output;
    they only differ in whether a visualisation is on top of that.
    """

    OFF = 1
    TEXT = 2
    VISUALISATION = 3


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
            cls._instance._verbosity = LoggerVerbosity.OFF
            cls._instance._file_name = "log.md"
            cls._instance._stream = io.StringIO()
            cls._instance._visualisation_window_shown = False
            atexit.register(cls._instance.close)
        return cls._instance

    @property
    def verbosity(self) -> LoggerVerbosity:
        return self._verbosity

    @verbosity.setter
    def verbosity(self, value: LoggerVerbosity):
        if value == LoggerVerbosity.VISUALISATION and not self._visualisation_window_shown:
            self._show_visualisation_window()
            self._visualisation_window_shown = True
        self._verbosity = value

    def _show_visualisation_window(self):
        """!
        Open a window displaying the Peano logo. Called once, the first time
        verbosity is ever set to LoggerVerbosity.VISUALISATION. If no display
        is available (e.g. a headless run), matplotlib prints a warning and
        this silently is a no-op rather than failing the run.

        The window is opened non-blocking, so the run continues straight
        away. It is not closed automatically though: we register a
        blocking plt.show() to run at program exit, so the window stays on
        screen - the user has to close it manually - rather than vanishing
        the moment the process would otherwise terminate.
        """
        import matplotlib.image as mpimg
        import matplotlib.pyplot as plt

        icon_file = os.path.join(
            os.path.dirname(os.path.abspath(__file__)),
            "..", "..", "..", "documentation", "Peano-icon.png",
        )
        image = mpimg.imread(icon_file)
        plt.figure("Peano")
        plt.imshow(image)
        plt.axis("off")
        plt.show(block=False)
        plt.pause(0.001)

        atexit.register(plt.show)

    @property
    def is_visualisation(self) -> bool:
        """!

        True if and only if verbosity is set to LoggerVerbosity.VISUALISATION.
        Implies is_text, since visualising always dumps text alongside the
        visualisation (@see is_text) - callers that branch on both should
        therefore never treat these two as mutually exclusive alternatives.

        """
        return self._verbosity == LoggerVerbosity.VISUALISATION

    @property
    def is_text(self) -> bool:
        """!

        True if verbosity is set to LoggerVerbosity.TEXT or
        LoggerVerbosity.VISUALISATION - visualising always dumps text
        alongside the visualisation, it is never the only output, so this
        is also true whenever is_visualisation is (@see is_visualisation;
        the converse does not hold: plain TEXT verbosity produces no
        visualisation).

        """
        return self._verbosity in (LoggerVerbosity.TEXT, LoggerVerbosity.VISUALISATION)

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
        """! By default, only printed to the console if verbosity is not OFF. """
        if print_option == LoggerPrintOption.ALWAYS_PRINT or (
            print_option == LoggerPrintOption.IF_VERBOSE and self._verbosity != LoggerVerbosity.OFF
        ):
            print(message)
        self._stream.write( message )
        self._stream.write( "\n" )

    AcceptWarningsEnvironmentVariable = "PEANO_ACCEPT_WARNINGS"

    def warn_and_confirm(self, message: str):
        """!

        Log a warning and let the user decide whether to continue

        The warning is always printed and written into the log. Afterwards,
        the user has to accept the warning or abort. If the user aborts, the
        routine raises an exception, i.e. the Python script terminates.

        If the environment variable PEANO_ACCEPT_WARNINGS is set to 1, all
        warnings are accepted without asking. Use this for batch jobs. If
        the variable is not set and there is no terminal to ask the user
        (stdin is not a terminal), the routine aborts.

        @param message: str
          The warning. It should explain what goes wrong if the user
          accepts it.

        """
        self.log("Warning: " + message, LoggerPrintOption.ALWAYS_PRINT)

        if os.environ.get(Logger.AcceptWarningsEnvironmentVariable) == "1":
            self.log(f"Warning accepted, as {Logger.AcceptWarningsEnvironmentVariable}=1", LoggerPrintOption.ALWAYS_PRINT)
            return

        if not sys.stdin.isatty():
            self.log(
                f"Abort, as there is no terminal to confirm the warning. Set {Logger.AcceptWarningsEnvironmentVariable}=1 to accept all warnings",
                LoggerPrintOption.ALWAYS_PRINT,
            )
            raise Exception("Warning not accepted: " + message)

        while True:
            answer = input("[A]ccept or a[b]ort? ").strip().lower()
            if answer in ("a", "accept"):
                self.log("Warning accepted by user", LoggerPrintOption.NEVER_PRINT)
                return
            if answer in ("b", "abort"):
                self.log("Abort requested by user", LoggerPrintOption.NEVER_PRINT)
                raise Exception("Warning not accepted: " + message)


    def new_section(self, section_name: str):
        if self._verbosity != LoggerVerbosity.OFF:
            print("------------------------------")
            print(section_name)
            print("------------------------------")
        self._stream.write("\n\n## {}\n\n".format(section_name))

    def close(self):
        """!
        Called automatically when the program terminates (see atexit
        registration in __new__). Flushes the accumulated stream into
        file_name, preceded by a section stating when the file was closed.

        If file_name already exists, we do not overwrite it. Instead, we
        attach a timestamp to the file name, i.e. log.md becomes
        something similar to log-20240101-120000.md.
        
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
