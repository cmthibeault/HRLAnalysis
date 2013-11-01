/*
    HRLAnalysis(TM) Software License - Version 1.0 - August 27th, 2013

    Permission is hereby granted, free of charge, to any person or 
    organization obtaining a copy of the software and accompanying 
    documentation covered by this license (the "Software") to use, 
    reproduce, display, distribute, execute, and transmit the 
    Software, and to prepare derivative works of the Software, and 
    to permit third-parties to whom the Software is furnished to do 
    so, all subject to the following:

    The copyright notices in the Software and this entire statement, 
    including the above license grant, this restriction and the 
    following disclaimer, must be included in all copies of the 
    Software, in whole or in part, and all derivative works of the 
    Software, unless such copies or derivative works are solely in 
    the form of machine-executable object code generated by a source 
    language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND 
    NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR 
    ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR 
    OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
    OTHER DEALINGS IN THE SOFTWARE, INCLUDING BUT NOT LIMITED TO THE 
    COMPATIBILITY OF THIS LICENSE WITH OTHER SOFTWARE LICENSES.
*/

/**********************************************************************/
//! Logging.
/*!
 *
 * Description: Based on the article:
 *              "Logging In C++
 *              A typesafe, threadsafe, portable logging mechanism"
 *              by Petru Marginean, Dr. Dobb's Sept. 5, 2007.
 *
 *              This code is somewhat confusing.  Everything is included
 *              here to ensure performance.  This is why the code is
 *              template and macro based.  Additionally all of the 
 *              variables are static and access to them is not through 
 *              more traditional get/set methods.  The main logging 
 *              class, HRL_logger is actually a template that
 *              requires a class to handle the File IO.  This is done 
 *              here with the INFOLog_2File Macro.  Since all of the
 *              methods and variables of this class are static each
 *              file requires a separate definition of the INFOLog_2File
 *              class.  All of this creates very fast code but does 
 *              increase the size of the binary.  To add a new output
 *              destination use this macro (here we're going to call the
 *              new class New_Output_2File):
 * 
 *                  LOG2FILE(New_Output_2File);
 * 
 *              Then create a new class that derives from the 
 *              NvPcomp_logger class:
 * 
 *                  class New_Output : public HRL_logger<New_Output_2File>
 * 
 *              Please pay attention to the naming conventions used 
 *              here.  This is extremely important to the subsequent 
 *              Macros that use these classes.  The Log2File 
 *              implementation must have the name of the NvPcomp_logger
 *              class we're creating follow by a "_2File"  If you decide
 *              to change this convention make sure you change the LOG
 *              Macro below.  This brings us to using the newly created 
 *              loggers.  In your code simplly call the macro LOG, with 
 *              the name of the NvPcomp_logger class and the desired
 *              level of output and treat that as a std::ostringstream.
 * 
 *                  LOG(New_Output, LEVEL1) << "Some logging message."
 * 
 *              Since this is a ostringstream you can also pass it the
 *              same object types you would pipe to cout.
 *
 */
/**********************************************************************/
#ifndef __HRL_LOGGER_H__
#define __HRL_LOGGER_H__
/**********************************************************************
 * Includes
 **********************************************************************/
#include <sstream>
#include <string>
#include <stdio.h>
/**********************************************************************
 * Macros
 **********************************************************************/
//! This is the Macro call that will output information if needed.
#define LOG(log_name, level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > log_name::ReportingLevel() || !log_name##_2File::Stream()) ; \
    else log_name().Get(level)

#define SET_OUTPUT(log_name, out) \
    log_name##_2File::Stream() = out;
    
#define SET_LOG_LEVEL(log_name, level) \
    log_name::ReportingLevel() = level;
    
#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logLEVEL8
#endif
/**********************************************************************
 * Enumerations
 **********************************************************************/
//! enum of Log Level
/*! This holds information about the possible enum levels.*/
enum LogLevel {
    logLEVEL0,
    logLEVEL1,
    logLEVEL2,
    logLEVEL3,
    logLEVEL4,
    logLEVEL5,
    logLEVEL6,
    logLEVEL7,
    logLEVEL8
};

/**********************************************************************/
//! Templatized Logging Class
/*!
 * \class HRL_logger
 * 
 * \author CMT
 *
 */
/**********************************************************************/
template <typename T>
class HRL_logger {
public:
    //! Constructor
    HRL_logger();
    //! Virtual Desctructor
    virtual ~HRL_logger();
    //! Get function to return the processing stream
    /*!
     * \param level The log level of this statement
     */ 
    std::ostringstream& Get(LogLevel level = logLEVEL3);
public:
    static LogLevel& ReportingLevel();
    static std::string ToString(LogLevel level);
    static LogLevel FromString(const std::string& level);
protected:
    //! The output string stream
    std::ostringstream os;
private:
    HRL_logger(const HRL_logger&);
    HRL_logger& operator =(const HRL_logger&);
};

/**********************************************************************
 * Logging Class Implementation
 **********************************************************************/
// Public Constructor.
template <typename T>
HRL_logger<T>::HRL_logger(){}

// Get Function
template <typename T>
std::ostringstream& HRL_logger<T>::Get(LogLevel level)
{
    //os << " " << ToString(level) << ": ";
    os << std::string(level > logLEVEL1 ? level - logLEVEL1 : 0, '\t');
    return os;
}

// Destructor
template <typename T>
HRL_logger<T>::~HRL_logger()
{
    //os << std::endl;
    T::Output(os.str());
}

// Reporting Level
template <typename T>
LogLevel& HRL_logger<T>::ReportingLevel()
{
    static LogLevel reportingLevel = logLEVEL3;
    return reportingLevel;
}

template <typename T>
std::string HRL_logger<T>::ToString(LogLevel level)
{
    static const char* const buffer[] = {"LEVEL0", "LEVEL1", "LEVEL2", "LEVEL3", "LEVEL4", "LEVEL5", "LEVEL6", "LEVEL7", "LEVEL8"};
    return buffer[level];
}

template <typename T>
LogLevel HRL_logger<T>::FromString(const std::string& level)
{
    if (level == "LEVEL0")
        return logLEVEL0;
    if (level == "LEVEL1")
        return logLEVEL1;
    if (level == "LEVEL2")
        return logLEVEL2;
    if (level == "LEVEL3")
        return logLEVEL3;       
    if (level == "LEVEL4")
        return logLEVEL4;
    if (level == "LEVEL5")
        return logLEVEL5;
    if (level == "LEVEL6")
        return logLEVEL6;
    if (level == "LEVEL7")
        return logLEVEL7;
    if (level == "LEVEL8")
        return logLEVEL8;
    HRL_logger<T>().Get(logLEVEL3) << "Unknown logging level '" << level << "'. Using LEVEL4 as default.";
    return logLEVEL4;
}
/**********************************************************************/

/**********************************************************************
 * Macro definition of the file logging class.
 **********************************************************************/

/**********************************************************************/
//! Write log to Information File.
/*!
 * \class INFOLog_2File
 *
 * Description:
 *
 * \author CMT, DRJ & BFB
 *
 */
/**********************************************************************/
#define LOG2FILE(className)                                     \
    class className                                             \
    {                                                           \
    public:                                                     \
        static FILE*& Stream();                                \
        static void Output(const std::string& msg);           \
    };                                                          \
    inline FILE*& className::Stream()                          \
    {                                                           \
        static FILE* pStream;                                  \
        return pStream;                                        \
    }                                                           \
    inline void className::Output(const std::string& msg)     \
    {                                                           \
        FILE* pStream = Stream();                               \
        if (!pStream)                                           \
            return;                                            \
        fprintf(pStream, "%s", msg.c_str());                    \
        fflush(pStream);                                        \
    }
/**********************************************************************/

/**********************************************************************/
// This is defining all of the different Log2File converters
// it is done this way for efficiency, as well as to confuse anyone
// who considers looking at it.
LOG2FILE(NETWORKLog_2File);

// Definition of the separate log classes.
class NETWORKLog : public HRL_logger<NETWORKLog_2File> {};
/**********************************************************************/

#endif // __HRL_LOGGER_H__