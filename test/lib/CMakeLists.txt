
AUX_SOURCE_DIRECTORY("../../HrlNeuralAnalysis" HrlNeuralAnalysis)
AUX_SOURCE_DIRECTORY("../AnalysisData" AnalysisData)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/HrlAnalysis/lib)

# Look for cppunit
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(CPPUNIT REQUIRED cppunit )

MESSAGE( "CPPUNIT_INCLUDE_DIRS : ${CPPUNIT_INCLUDE_DIRS}" )
MESSAGE( "CPPUNIT_LIBRARY_DIRS : ${CPPUNIT_LIBRARY_DIRS}" )
MESSAGE( "CPPUNIT_LIBRARIES : ${CPPUNIT_LIBRARIES}" )

#----------------------------------------------------------------------------------------------------------
# Make sure the Boost Libraries are installed
#----------------------------------------------------------------------------------------------------------
FIND_PACKAGE(Boost COMPONENTS python REQUIRED)
SET(Boost_PYTHON_INCLUDE ${Boost_INCLUDE_DIRS})
SET(Boost_PYTHON_LIBRARIES ${Boost_LIBRARIES})

IF(Boost_FOUND)
    MESSAGE (STATUS "Found Boost Python ${Boost_VERSION} -- ${Boost_LIBRARIES} -- ${Boost_INCLUDE_DIRS}")
ENDIF(Boost_FOUND) 
#----------------------------------------------------------------------------------------------------------
FIND_PACKAGE(Boost COMPONENTS serialization REQUIRED)

IF(Boost_FOUND)
    MESSAGE (STATUS "Found Boost Serialization ${Boost_VERSION} -- ${Boost_LIBRARIES} -- ${Boost_INCLUDE_DIRS}")
ENDIF(Boost_FOUND) 

#----------------------------------------------------------------------------------------------------------
FIND_PACKAGE(PythonInterp 2.4)
FIND_PACKAGE(PythonLibs 2.4)

IF(PYTHONINTERP_FOUND)
    MESSAGE (STATUS "Found Python Interpreter ${PYTHON_EXECUTABLE}")
ELSE(PYTHONINTERP_FOUND)
    MESSAGE (STATUS "Could not find Python Interpreter!")
ENDIF(PYTHONINTERP_FOUND)

IF(PYTHONLIBS_FOUND)
    MESSAGE (STATUS "Found Python Libs -- ${PYTHON_LIBRARIES} -- ${PYTHON_INCLUDE_PATH} -- ${PYTHON_DEBUG_LIBRARIES}")
ELSE(PYTHONLIBS_FOUND)
    MESSAGE (STATUS "Could not find Python Libs!")
ENDIF(PYTHONLIBS_FOUND)
#----------------------------------------------------------------------------------------------------------

IF( CPPUNIT_FOUND  )
    INCLUDE_DIRECTORIES("../../HrlNeuralAnalysis"
                        "../AnalysisData"
                        ${CMAKE_SOURCE_DIR}
                        ${Boost_INCLUDE_DIRS}
                        ${Boost_PYTHON_INCLUDE}
                        ${PYTHON_INCLUDE_PATH})

    ADD_LIBRARY(HrlAnalysisData SHARED "TestLibrary.cpp"
                                       ${AnalysisData}
                                       ${HrlNeuralAnalysis})

    #SET(CSTANDARD "-std=c++0x")
    #SET(CFLAGS " ${CSTANDARD} ")
    #SET(CMAKE_CXX_FLAGS "-std=c++0x")

    SET(CFLAGS " -fPIC ")

    # Set the libraries that need to be linked
    TARGET_LINK_LIBRARIES(HrlAnalysisData stdc++ ${PYTHON_LIBRARIES} ${Boost_LIBRARIES} ${Boost_PYTHON_LIBRARIES})
    SET_TARGET_PROPERTIES(HrlAnalysisData PROPERTIES LINKER_LANGUAGE CXX)

ENDIF(CPPUNIT_FOUND)
