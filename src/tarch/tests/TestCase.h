// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_TESTS_TESTCASE_H_
#define _TARCH_TESTS_TESTCASE_H_

#include "tarch/tests/TestMacros.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include <string>


namespace tarch {
  namespace tests {
    class TestCase;
  }
}


/**
 * Represents one test case. Every test case should be a subclass of this
 * class implementing run(). Furthermore subtypes should use the assertion
 * macros included to check any assumption. All the test cases are managed by the
 * TestCaseCollection.
 *
 * If you implement your own tests, inherit from this class, implement
 * run() and call the registration macro on the test case. See TestMacros.h
 * for details.
 *
 *
 * <h2> Implementation remarks </h2>
 *
 * Whenever you write a test case implementation, we recommend to add the
 * code block
 *
 * \code
#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif
   \endcode
 * at the very beginning of your implementation and
 * \code
#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
   \endcode
 *
 * at the end of the implementation file.
 *
 *
 * <h2> Unit tests in libraries </h2>
 *
 * If your unit test is part of a library, then simply registering the test is
 * not enough, as the linker will remove the test again - assuming it is not
 * invoked. So you have to do a couple of things:
 *
 * First, you have to say that you wanna export the whole archive. With the
 * autotools, you have to add
 * <pre>
libTarch_la_LDFLAGS = $(PEANO_LDFLAGS)  -Wl,--whole-archive
libTarch_trace_la_LDFLAGS = $(PEANO_LDFLAGS)  -Wl,--whole-archive
libTarch_debug_la_LDFLAGS = $(PEANO_LDFLAGS)  -Wl,--whole-archive
   </pre>
 *
 * Second, you have to include add your test to the respective UnitTest.h
 * file or, otherwise, you have to ensure that your main routine includes
 * the header of the unit test.
 *
 * Finally, you have to use the declareTest macro which is basically a fake
 * that hinders the linker to kick your test out.
 * The latter trick is documented at https://stackoverflow.com/questions/5202142/static-variable-initialization-over-a-library.
 *
 *
 * @author Tobias Weinzierl, Wolfgang Eckhardt
 * @version $Revision: 1.31 $
 */
class tarch::tests::TestCase {
  protected:
    /*
     * this flag is used for the run_macro to indicate if an error has occurred
     * during the execution of a test method
     */
    bool _error;

    /**
     * Name of the test case.
     */
    std::string _testCaseName;

    /**
     * Error counter. This counter is increased on errors iff you use the
     * validate macros within your test cases.
     */
    int _errors;

    static std::string _outputDirectory;

    TestCase();

  public:
    /**
     * Constructor.
     *
     * @param testCaseName Name of the test case. If a class is tested, this
     *                     string should equal the class name added the
     *                     namespace.
     */
    TestCase( const std::string& testCaseName );

    /**
     * Destructor.
     */
    virtual ~TestCase();

    /**
     * @return Number of errors.
     */
    int getNumberOfErrors() const;

    /**
     * @return the name of the testcase
     */
    const std::string& getTestCaseName() const;

    void setTestCaseName(const std::string& testCaseName );

    /**
     * This routine is triggered by the TestCaseCollection
     */
    virtual void run() = 0;

    /**
     * Set the output directory. Output files usually only are written by
     * integration tests.
     */
    static void setOutputDirectory(const std::string & outputDirectory);
};

#endif
