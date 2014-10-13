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
#ifndef BURSTINFO_H_
#define BURSTINFO_H_

#include <boost/shared_ptr.hpp>
#include <vector>

namespace hrlAnalysis {

class BurstInfo
{
    public:
        BurstInfo() : start(0), stop(0), surprise(0), nSpikes(0) {};
                // operators are required to make boost/shared_ptr happy
        bool operator==(const BurstInfo&) const {return false;}
        bool operator!=(const BurstInfo&) const {return true;}
    public:
        IndexType start;
        IndexType stop;
        int nSpikes;
        float surprise;
};

typedef boost::shared_ptr<BurstInfo> BurstInfoPtr;
typedef boost::shared_ptr< std::vector<BurstInfo> > CellBurstingInfoPtr;
typedef boost::shared_ptr< std::vector<std::vector<BurstInfo> > > PopulationBurstingInfoPtr;

}

#endif /* BURSTINFO_H_ */
