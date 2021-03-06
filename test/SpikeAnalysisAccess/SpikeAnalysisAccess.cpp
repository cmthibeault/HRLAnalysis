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
#include <SpikeAnalysisAccess.h>

using namespace hrlAnalysis;
using namespace std;

SpikeAnalysisAccess::SpikeAnalysisAccess(   IndexType startTimeIn, IndexType endTimeIn, int startIdxIn,
                                            int endIdxIn, std::vector<std::string> fileNames,
                                            int numCells):

                                            HrlNeuralAnalysis(startTimeIn,endTimeIn,startIdxIn,endIdxIn,fileNames),
                                            cellActivityAccess_(new vector< vector<IndexType> >(numCells)) {
    cellActivity_ = cellActivityAccess_;
    paramsInAccess_ = paramsIn_;
}

SpikeAnalysisAccess::~SpikeAnalysisAccess() {}

bool SpikeAnalysisAccess::buildDataStructures() {
    return false;
}

void SpikeAnalysisAccess::setStartTime(IndexType startTime) {
    paramsIn_->startTime = startTime;
}

void SpikeAnalysisAccess::setEndTime(IndexType endTime) {
    paramsIn_->endTime = endTime;
}
