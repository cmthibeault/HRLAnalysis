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
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module_init.hpp>
#include <boost/python/call_method.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>
#include <sys/types.h>
#include <string>
#include <vector>
#include <HrlNeuralAnalysis.h>
#include <HrlNetworkAnalysis.h>
#include <HrlNeuralAnalysisHRLSim.h>
#include <HrlNeuralAnalysisVoltage.h>
#include <NeuronParams.h>

using namespace boost::python;
using namespace hrlAnalysis;

struct HrlNeuralAnalysisCallback : HrlNeuralAnalysis, wrapper<HrlNeuralAnalysis> {
        HrlNeuralAnalysisCallback(PyObject *p, int startTimeIn, int endTimeIn, int startIdxIn, int endIdxIn, std::vector<std::string> fileNames) :
                                     HrlNeuralAnalysis(startTimeIn,endTimeIn,startIdxIn,endIdxIn,fileNames),self(p) {};
        bool buildDataStructures() { return call_method<bool>(self, "buildDataStructures"); };
    private:
        PyObject *self;
        std::vector<std::string> fileNames_;
};

BOOST_PYTHON_MODULE(libHrlAnalysis) {

    // Add the HrlNeuralAnalysis Class
    class_<HrlNeuralAnalysis, boost::noncopyable, boost::shared_ptr< HrlNeuralAnalysisCallback > >("HrlNeuralAnalysis", init<int,int,int,int,std::vector<std::string> >())
        .def("dumpSpikeActivity", &HrlNeuralAnalysis::dumpSpikeActivity)
        .def("dumpCellActivity", &HrlNeuralAnalysis::dumpCellActivity)
        .def("save", &HrlNeuralAnalysis::save)
        .def("load", &HrlNeuralAnalysis::load)
        .def("getSpikeTimes", &HrlNeuralAnalysis::getSpikeTimes)
        .def("getCOV", &HrlNeuralAnalysis::getCOV)
        .def("getGaussWindowRate", &HrlNeuralAnalysis::getGaussWindowRate)
        .def("getWindowRate", &HrlNeuralAnalysis::getWindowRate)
        .def("getCellRates", &HrlNeuralAnalysis::getCellRates)
        .def("getRateBins", &HrlNeuralAnalysis::getRateBins)
        .def("getRatesWithBins", &HrlNeuralAnalysis::getRatesWithBins)
        .def("getBursting", &HrlNeuralAnalysis::getBursting)
        .def("setEmptyTrainSynchVal", &HrlNeuralAnalysis::setEmptyTrainSynchVal)
        .def("getPairSynchrony", &HrlNeuralAnalysis::getPairSynchrony)
        .def("getPopulationSynchrony", &HrlNeuralAnalysis::getPopulationSynchrony)
        .def("calcSPIKEDistance", &HrlNeuralAnalysis::calcSPIKEDistance)
        .def("calcSPIKEDistanceAvg", &HrlNeuralAnalysis::calcSPIKEDistanceAvg)
        .def("buildSpikeActFromCellAct", &HrlNeuralAnalysis::buildSpikeActFromCellAct)
        .def("sortSpikeActivity", &HrlNeuralAnalysis::sortSpikeActivity)
        .def("spikeActivity", &HrlNeuralAnalysis::spikeActivity)
        .def("cellActivity", &HrlNeuralAnalysis::cellActivity)
        .def("paramsIn", &HrlNeuralAnalysis::paramsIn)
        .def("setParamsIn", &HrlNeuralAnalysis::setParamsIn)
        .def("addSpike", &HrlNeuralAnalysis::addSpike)
    ;

    class_<HrlNeuralAnalysisHRLSim, bases<HrlNeuralAnalysis> >("HrlNeuralAnalysisHRLSim", init<int,int,int,int,std::vector<std::string> >())
        .def("buildDataStructures", &HrlNeuralAnalysisHRLSim::buildDataStructures)
    ;

    class_<HrlNeuralAnalysisVoltage, bases<HrlNeuralAnalysis> >("HrlNeuralAnalysisVoltage", init<int,int,int,int,std::vector<std::string>,int,bool,float>())
        .def("buildDataStructures", &HrlNeuralAnalysisVoltage::buildDataStructures)
        .def("voltages", &HrlNeuralAnalysisVoltage::voltages)
    ;

    class_<HrlNetworkAnalysis>("HrlNetworkAnalysis", init<>())
        .def("buildNetwork", &HrlNetworkAnalysis::buildNetwork)
        .def("outputGraph", &HrlNetworkAnalysis::outputGraph)
        .def("outputWeights", &HrlNetworkAnalysis::outputWeights)
        .def("addPopulation", &HrlNetworkAnalysis::addPopulation)
        .def("addDummyPopulation", &HrlNetworkAnalysis::addDummyPopulation)
        .def("getWeights", &HrlNetworkAnalysis::getWeights)
        .def("setLogToStd", &HrlNetworkAnalysis::setLogToStd)
        .def("setLogToFile", &HrlNetworkAnalysis::setLogToFile)
    ;

    class_<std::vector< std::vector<int> >, CellActivityPtr > ("CellActivity")
        .def(vector_indexing_suite< std::vector< std::vector<int> > >())
    ;

    class_<std::vector< std::pair<int,int> >, SpikeActivityPtr > ("SpikeActivity")
        .def(vector_indexing_suite< std::vector< std::pair<int,int> > >())
    ;

    class_<std::vector<float> > ("CellVoltageInfo")
        .def(vector_indexing_suite<std::vector<float> >())
    ;

    class_<std::vector<std::vector<float> > > ("PopulationVoltageInfo")
        .def(vector_indexing_suite<std::vector<std::vector<float> > >())
    ;

    class_<VoltageInfo, boost::shared_ptr< VoltageInfo > > ("VoltageInfo")
        .def_readwrite("voltage", &VoltageInfo::voltage)
    ;

    class_<NeuronParams, boost::shared_ptr< NeuronParams > > ("NeuronParams", init<int,int,int,int,std::vector<std::string> >())
        .def_readwrite("startTime", &NeuronParams::startTime)
        .def_readwrite("endTime", &NeuronParams::endTime)
        .def_readwrite("startIdx", &NeuronParams::startIdx)
        .def_readwrite("endIdx", &NeuronParams::endIdx)
        .def_readwrite("sampleFreq", &NeuronParams::sampleFreq)
        .def_readwrite("isDataCompiled", &NeuronParams::isDataCompiled)
    ;

    class_<BurstInfo, boost::shared_ptr< BurstInfo > >("BurstInfo", init<>())
        .def_readwrite("start", &BurstInfo::start)
        .def_readwrite("stop", &BurstInfo::stop)
        .def_readwrite("surprise", &BurstInfo::surprise)
    ;

    class_<std::vector<BurstInfo>, boost::shared_ptr< std::vector<BurstInfo> > > ("CellBurstingInfo")
        .def(vector_indexing_suite<std::vector<BurstInfo> >())
    ;

    class_<std::vector<std::vector<BurstInfo> >, boost::shared_ptr< std::vector<std::vector<BurstInfo> > > > ("PopulationBurstingInfo")
        .def(vector_indexing_suite<std::vector<std::vector<BurstInfo> > >())
    ;

    class_<RasterInfo, boost::shared_ptr<RasterInfo> >("RasterInfo", init<>())
        .def_readwrite("time", &RasterInfo::time)
        .def_readwrite("spikes", &RasterInfo::spikes)
    ;

    class_<CovInfo, boost::shared_ptr<CovInfo> >("CovInfo", init<>())
        .def_readwrite("cells", &CovInfo::cells)
        .def_readwrite("cov", &CovInfo::cov)
    ;

    class_<RateInfo, boost::shared_ptr<RateInfo> >("RateInfo", init<>())
        .def_readwrite("rates", &RateInfo::rates)
    ;

    class_<SynchronyInfo, boost::shared_ptr<SynchronyInfo> >("SynchronyInfo", init<>())
        .def_readwrite("S", &SynchronyInfo::S)
    ;

    class_<RateBinInfo, boost::shared_ptr<RateBinInfo> >("RateBinInfo", init<>())
        .def_readwrite("freqs", &RateBinInfo::freqs)
        .def_readwrite("counts", &RateBinInfo::counts)
    ;

    class_<CellRateBinInfo, boost::shared_ptr<CellRateBinInfo> >("CellRateBinInfo", init<>())
        .def_readwrite("freqs", &CellRateBinInfo::freqs)
        .def_readwrite("counts", &CellRateBinInfo::counts)
        .def_readwrite("cells", &CellRateBinInfo::cells)
        .def_readwrite("rates", &CellRateBinInfo::rates)
    ;

    class_<std::vector<double> >("vector_double")
        .def(vector_indexing_suite<std::vector<double> >())
    ;

    class_<std::vector<int> >("vector_int")
        .def(vector_indexing_suite<std::vector<int> >())
    ;

    class_<std::vector<std::string> >("vector_string")
        .def(vector_indexing_suite<std::vector<std::string> >())
    ;

    class_< std::pair<int,int> >("int_pair", init<int,int>())
        .def_readwrite( "first", &std::pair< int, int >::first)
        .def_readwrite( "second", &std::pair< int, int >::second)
    ;

}
