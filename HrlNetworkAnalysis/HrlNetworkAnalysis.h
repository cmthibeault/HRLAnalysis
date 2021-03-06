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
#ifndef HrlNetworkAnalysis_H
#define HrlNetworkAnalysis_H

#include <boost/foreach.hpp>
#include <sys/types.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <math.h>
#include <utility>
#include <climits>
#include <stdio.h>
#include <assert.h>
#include <HRLLog.h>

typedef unsigned int uint;
typedef unsigned char byte;

namespace hrlAnalysis {

//
// name: to_string
// @param const T& t: Value to convert to a std::string
// @return The converted string
//
template <class T>
inline std::string to_string(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

struct synapticInfo {
    uint connectionCount;
    std::vector<float> avgWeight;                       // Weight at each time step
    std::vector<std::pair<uint,uint> > synapses;
    uint synStartIdx;
    uint synEndIdx;
};

struct populationInfo {
    std::string strName;
    uint size;
    uint startIdx;
    uint endIdx;
    bool inhibitory;
    bool dummy;
    std::vector<synapticInfo> excConnectionCount;
    std::vector<synapticInfo> inhConnectionCount;
};

class HrlNetworkAnalysis
{
    public:
        /**
         * Base constructor
         */
        HrlNetworkAnalysis();
        /**
         * Base destructor
         */
        virtual ~HrlNetworkAnalysis();
        bool buildNetwork(std::string fileName);
        void outputGraph(std::string fileName);
        void outputWeights(std::string populationFileName, std::string weightFileName);
        void addPopulation(std::string popName, uint startIdx, uint endIdx);
        void addDummyPopulation(std::string popName, uint startIdx, uint endIdx);
        void getWeights(std::string fileName);
        void setLogToStd();
        void setLogToFile(std::string fileName);
    private:
        void readNetwork(std::ifstream & in);
        void buildPopulationStats();
        void getSynapseCount(int prePopulationIdx, int postPopulationIdx);
        uint binaryConnectionSearch(int preNeuronIdx, int postNeuronIdx);
    private:
        int numReal_;
        int numTot_;
        uint synMax_;
        uint synTot_;
        int *inputOffset_;
        int *numInputs_;
        int *preIndex_;
        bool *plasticInputs_;
        bool *plasticOutputs_;
        bool *inhibitoryList_;
        std::vector<populationInfo> populations_;
};

}

#endif /* HrlNetworkAnalysis_H */
