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
#include <HrlNeuralAnalysis.h>

using namespace hrlAnalysis;
using namespace std;

HrlNeuralAnalysis::HrlNeuralAnalysis():
									spikeActivity_(new vector< pair<int,int> >),
									cellActivity_(new vector< vector<int> >),
									paramsIn_(new NeuronParams()), emptyTrainSynchVal(0.0) {}

HrlNeuralAnalysis::HrlNeuralAnalysis(int startTimeIn, int endTimeIn, int startIdxIn, int endIdxIn, std::vector<std::string> fileNames):
									spikeActivity_(new vector< pair<int,int> >),
									cellActivity_(new vector< vector<int> >),
									paramsIn_(new NeuronParams(startTimeIn, endTimeIn, startIdxIn, endIdxIn, fileNames)), emptyTrainSynchVal(0.0) {}

HrlNeuralAnalysis::~HrlNeuralAnalysis() {}

void HrlNeuralAnalysis::save(std::string filename) {
    std::ofstream ofs(filename.c_str(),std::ofstream::out | std::ofstream::binary);
    boost::archive::binary_oarchive ar(ofs);
    ar & *this;
}

void HrlNeuralAnalysis::load(std::string filename) {
	std::ifstream ifs(filename.c_str(),std::ifstream::in | std::ifstream::binary);
	boost::archive::binary_iarchive ar(ifs);
	ar & *this;
}

void HrlNeuralAnalysis::dumpSpikeActivity() {
    for (vector<std::pair<int,int> >::iterator spikeIt = spikeActivity_->begin(); spikeIt!=spikeActivity_->end(); ++spikeIt) {
        cout << "\t\t" << (*spikeIt).first << "\t" << (*spikeIt).second << endl;
    }
}

void HrlNeuralAnalysis::dumpCellActivity() {
    int count = 0;
    BOOST_FOREACH( vector<int> spikes, *cellActivity_) {
        BOOST_FOREACH( int spike, spikes) {
            cout << spike << endl;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//                  SPIKE Distance Functions
/////////////////////////////////////////////////////////////////////////////////////////

void HrlNeuralAnalysis::setEmptyTrainSynchVal(double val) {
        emptyTrainSynchVal = val;
}

double HrlNeuralAnalysis::calcSynchrony(CellSynchronyInfoPtr cell1Info, CellSynchronyInfoPtr cell2Info) {
    double Sn1 = cell1Info->getSn();
    double Sn2 = cell2Info->getSn();
    double X_isi = ( (double)(cell1Info->x_isi_ + cell2Info->x_isi_) ) / 2.0;
    return ( ( ( Sn1 * cell2Info->x_isi_ ) + (Sn2 * cell1Info->x_isi_) ) / (2*X_isi*X_isi) );
}

double HrlNeuralAnalysis::calcSPIKEDistanceAvg(SynchronyInfoPtr synchPtr) {
    double spikeDistance = 0;
    for(std::vector<double>::iterator itrS = synchPtr->S.begin(); itrS < synchPtr->S.end(); itrS++) {
        spikeDistance += (*itrS);
    }
    return spikeDistance / synchPtr->S.size();
}

double HrlNeuralAnalysis::calcSPIKEDistance(SynchronyInfoPtr synchPtr) {
    double spikeDistance = 0;
    double h = (paramsIn_->endTime - paramsIn_->startTime) / (paramsIn_->sampleFreq * synchPtr->S.size());
    // First element
    spikeDistance = synchPtr->S.front();
    for(std::vector<double>::iterator itrS = synchPtr->S.begin() + 1; itrS < synchPtr->S.end()-1; itrS+=2) {
        spikeDistance += 4 * (*itrS);
    }
    for(std::vector<double>::iterator itrS = synchPtr->S.begin() + 2; itrS < synchPtr->S.end()-2; itrS+=2) {
        spikeDistance += 2 * (*itrS);
    }
    // Last element
    spikeDistance += synchPtr->S.back();
    return (h*spikeDistance)/3;
}

// Get pairwise synchrony measures
void HrlNeuralAnalysis::calcPairSynchronywSpikes(vector<int> *spikeTrain1, vector<int> *spikeTrain2, vector<double> *S) {

    if ( spikeTrain1->size() > 0 && spikeTrain2->size() > 0 ) {
        S->reserve(paramsIn_->endTime - paramsIn_->startTime);
    } else {
        // if both spike trains are empty, return emptyTrainSynchVal, if only one is empty, return 1.0 for dissimilar spike trains
        double synchVal = (spikeTrain1->size() || 1-spikeTrain1->size() ? 1.0 : emptyTrainSynchVal);
        S->resize(paramsIn_->endTime - paramsIn_->startTime, synchVal);
        return;
    }

    CellSynchronyInfoPtr cell1Info(new CellSynchronyInfo(spikeTrain1, paramsIn_->startTime, paramsIn_->endTime)); //paramsIn_.sampleFreq);
    CellSynchronyInfoPtr cell2Info(new CellSynchronyInfo(spikeTrain2, paramsIn_->startTime, paramsIn_->endTime)); //paramsIn_.sampleFreq);

    for(int T = paramsIn_->startTime; T < paramsIn_->endTime; T++) {
        cell1Info->incrementSynchronyTiming();
        cell2Info->incrementSynchronyTiming();
        cell1Info->calcDeltas(cell2Info);
        cell2Info->calcDeltas(cell1Info);
        S->push_back( calcSynchrony(cell1Info, cell2Info) );
    }
}

void HrlNeuralAnalysis::calcPairSynchrony(int idxCell1, int idxCell2, vector<double> *S) {
    vector<int> *spikeTrain1 = &(cellActivity_->at(idxCell1));
    vector<int> *spikeTrain2 = &(cellActivity_->at(idxCell2));
    calcPairSynchronywSpikes(spikeTrain1,spikeTrain2,S);
}

SynchronyInfoPtr HrlNeuralAnalysis::getPairSynchrony(int idxCell1, int idxCell2) {
    SynchronyInfoPtr synchronyInfo(new SynchronyInfo());
    checkDataStructures();
    calcPairSynchrony(idxCell1, idxCell2, &synchronyInfo->S);
    return synchronyInfo;
}

void HrlNeuralAnalysis::calcPopulationSynchrony(vector<double> *S) {

    int N = (paramsIn_->endTime - paramsIn_->startTime);
    S->clear();
    S->resize(N,0);

    for(std::vector< std::vector<int> >::iterator itrN = cellActivity_->begin(); itrN < cellActivity_->end(); itrN++) {
        for(std::vector< std::vector<int> >::iterator itrM = itrN+1; itrM < cellActivity_->end(); itrM++) {
            vector<double> tempS;
            calcPairSynchronywSpikes(&(*itrN),&(*itrM),&tempS);
            for(int i = 0; i < tempS.size(); i++){
                S->at(i) += tempS.at(i);
            }
        }
    }

    for(int i = 0; i < S->size(); i++){
        S->at(i) *= 1.0 / ( (N * (N-1) )/2 );
    }

}

SynchronyInfoPtr HrlNeuralAnalysis::getPopulationSynchrony() {
    SynchronyInfoPtr synchronyInfo(new SynchronyInfo());
    checkDataStructures();
    calcPopulationSynchrony(&synchronyInfo->S);
    return synchronyInfo;
}

/////////////////////////////////////////////////////////////////////////////////////////
//                  Burst Analysis Functions
/////////////////////////////////////////////////////////////////////////////////////////
vector<int>::iterator HrlNeuralAnalysis::getSubBurstPeriodEnd(vector<int> *spikes, vector<int>::iterator searchStartPoint, double spikeRate) {

    vector<int>::iterator retVal = spikes->end();
    bool flag = false;
    vector<int>::iterator spikeSearchIt = searchStartPoint;

    while ( (spikeSearchIt != spikes->end() - 1)  && flag == false) {
        double isi = *(spikeSearchIt+1) - *spikeSearchIt;

        if((double)(1/(isi/paramsIn_->sampleFreq)) < spikeRate) {
            retVal = spikeSearchIt+1;
            flag = true;
        } else {
            spikeSearchIt++;
        }
    }

    return retVal;
}

double HrlNeuralAnalysis::calcSurprise(int t, int nSpikes, double rate) {
    double oldP, newP;
    double rt = (double)(rate*(double)t);
    double sum = exp(-rt);

    oldP = sum;

    for(int i = 1; i < nSpikes; i++) {
        newP = (double) ((rt * oldP) / (double)i);
        sum += newP;
        oldP = newP;
        if (sum >= 1){
            sum = 0.9999999999;
            break;
        }
    }

    double P = 1.0 - sum;

    if(P>=0.9999999999) {
        P = 0.9999999999;
    }

    return -log(P);
}

void HrlNeuralAnalysis::calcBursting(double significance, std::vector<std::vector<BurstInfo> > *burstResults) {

    double prob = -log(significance);

    BOOST_FOREACH( vector<int> spikes, *cellActivity_) {

        std::vector<BurstInfo> cellBurstResults;

        // Make sure there are enough spikes
        if(spikes.size() > MIN_BURST_SPIKE_COUNT) {
            // average spike rate = number of spikes / simulation time
            double spikeRate = (double)spikes.size()/( (paramsIn_->endTime - paramsIn_->startTime) / paramsIn_->sampleFreq);
            // Start the search at the beginning of the spike vector
            for (vector<int>::iterator spikeSearchStartIt = spikes.begin(); spikeSearchStartIt < (spikes.end() - (MIN_BURST_SPIKE_COUNT -1) ); spikeSearchStartIt++) {
                double isi = *(spikeSearchStartIt + (MIN_BURST_SPIKE_COUNT - 1) ) - *spikeSearchStartIt;
                double avgIsiRate = (double)( MIN_BURST_SPIKE_COUNT / (isi / paramsIn_->sampleFreq)  );
                // If the three consecutive spikes have rate that is over the average rate begin the search for a burst.
                if (avgIsiRate >= spikeRate) {
                    int nSpikes = MIN_BURST_SPIKE_COUNT;
                    // quick search forward for an ISI that is greater than the average rate to constrain the search period.
                    vector<int>::iterator spikeEndSearchIt = getSubBurstPeriodEnd(&spikes, spikeSearchStartIt+(MIN_BURST_SPIKE_COUNT -1), spikeRate);
                    // Search through this sub-period for the span with the highest Poisson surprise.
                    BurstInfo newBurst;
                    if(spikeEndSearchIt == spikes.end())
                        spikeEndSearchIt -= 2;
                    // Search this window of spikes for periods of activity that have a higher poisson surprise value then the probability.
                    for (vector<int>::iterator burstSearchStartIt = spikeSearchStartIt; burstSearchStartIt < (spikeEndSearchIt - (MIN_BURST_SPIKE_COUNT-1)); burstSearchStartIt++) {
                        for (vector<int>::iterator burstSearchEndIt = ( burstSearchStartIt + (MIN_BURST_SPIKE_COUNT-1) ); burstSearchEndIt < (spikeEndSearchIt+1); burstSearchEndIt++) {

                            int nSpikes = distance(burstSearchStartIt,burstSearchEndIt) + 1;
                            int timeSpan = *burstSearchEndIt - *burstSearchStartIt;
                            double newSurprise = calcSurprise(timeSpan, nSpikes, spikeRate/paramsIn_->sampleFreq);

                            if( (newSurprise > newBurst.surprise) || ((newSurprise == newBurst.surprise) && (nSpikes >newBurst.nSpikes)) ) {
                                newBurst.start = *burstSearchStartIt;
                                newBurst.stop = *burstSearchEndIt;
                                newBurst.nSpikes = nSpikes;
                                newBurst.surprise = newSurprise;
                            } // if( ( newSurprise...

                        } // for( burstSearchEndIt

                        if(newBurst.surprise > prob) {
                            break;
                        }

                    } // for( burstSearchStartIt

                    // If this new burst meets the minimum surprise add it to the results.
                    if(newBurst.surprise > prob) {
                        cellBurstResults.push_back(newBurst);
                    }

                    if(spikeEndSearchIt >= spikes.end()) {
                        break;
                    } else {
                        spikeSearchStartIt = spikeEndSearchIt-1;    // -1 to compensate for the increment in the for loop
                    }

                } // if(avgIsiRate >= spikeRate)

            } // for(vector<int>::iterator spikeSearchStartIt...)

        } // if(spikes.size() > MIN_BURST_SPIKE_COUNT)
        burstResults->push_back(cellBurstResults);
    } // FOREACH

}

PopulationBurstingInfoPtr HrlNeuralAnalysis::getBursting(double significance) {
    PopulationBurstingInfoPtr burstResult(new std::vector<std::vector<BurstInfo> >);
    checkDataStructures();
    calcBursting(significance, &(*burstResult));
    return burstResult;
}

/////////////////////////////////////////////////////////////////////////////////////////
//                  Cell rate, overall rate and statistic functions
/////////////////////////////////////////////////////////////////////////////////////////
void HrlNeuralAnalysis::calcCellRates(vector<int> *cells, vector<double> *rates) {
    double maxRate;
    calcCellRates(cells,rates,maxRate);
}

void HrlNeuralAnalysis::calcCellRates(vector<int> *cells, vector<double> *rates, double &maxRate) {
    //Calculate the simulation time we're analyzing.
    double simTime = paramsIn_->endTime - paramsIn_->startTime;
    double rate;
    int count = paramsIn_->startIdx - 1;
    maxRate = -1;
    
    cells->reserve(paramsIn_->endIdx - paramsIn_->startIdx + 1);
    rates->reserve(paramsIn_->endIdx - paramsIn_->startIdx + 1);

    BOOST_FOREACH( vector<int> spikes, *cellActivity_ ) {
        rate = (double)spikes.size()/(simTime/paramsIn_->sampleFreq);
        if(maxRate < rate) {
            maxRate = rate;
        }
        cells->push_back(++count);
        rates->push_back(rate);        
    }    
    
}

CellRateInfoPtr HrlNeuralAnalysis::getCellRates() {
    CellRateInfoPtr rateInfo(new CellRateInfo());
    checkDataStructures();
    calcCellRates(&rateInfo->cells, &rateInfo->rates, rateInfo->maxRate);
    return rateInfo;
}

// This assumes that calcCellRates has not been called.
void HrlNeuralAnalysis::calcRateBins(int numBins, vector<double> *freqs, vector<int> *counts) {
    double simTime = paramsIn_->endTime - paramsIn_->startTime;
    double calcRate = 0;
    double maxRate = -1;
    vector<double> rates;
    rates.reserve(paramsIn_->endIdx - paramsIn_->startIdx + 1);
    // Calculate the rates for each cell in the group.
    BOOST_FOREACH( vector<int> spikes, *cellActivity_ ) {
        calcRate = (double)spikes.size()/(simTime/paramsIn_->sampleFreq);
        if(maxRate < calcRate) {
            maxRate = calcRate;
        }
        rates.push_back(calcRate);        
    }
    // Create the frequency bins.
    double stepSize = maxRate/(numBins-1);
    double currEdge = 0;
    freqs->resize(numBins);
    for (vector<double>::iterator freqIt = freqs->begin(); freqIt!=freqs->end(); ++freqIt) {
        (*freqIt) = currEdge;
        currEdge += stepSize;
    }
    // Count the mean rates    
    counts->resize(numBins);
    BOOST_FOREACH( double rate, rates) {
        // This is dangerous but works by truncating the division.
        if(rate/stepSize < numBins)
            ++(counts->at(rate/stepSize));
    }
}

RateBinInfoPtr HrlNeuralAnalysis::getRateBins(int numBins) {
    RateBinInfoPtr rateBinInfo(new RateBinInfo());
    checkDataStructures();
    calcRateBins(numBins, &rateBinInfo->freqs, &rateBinInfo->counts);
    return rateBinInfo;
}

void HrlNeuralAnalysis::calcRatesWithBins(int numBins, \
                                           std::vector<int> *cells, \
                                           std::vector<double> *rates, \
                                           std::vector<double> *freqs, \
                                           std::vector<int> *counts) {
    double maxRate;
    this->calcCellRates(cells,rates,maxRate);
    
    // Create the frequency bins.
    double stepSize = maxRate/(numBins-1);
    double currEdge = 0;
    freqs->resize(numBins);
    for (vector<double>::iterator freqIt = freqs->begin(); freqIt!=freqs->end(); ++freqIt) {
        (*freqIt) = currEdge;
        currEdge += stepSize;
    }

    // Count the mean rates    
    counts->resize(numBins);
    BOOST_FOREACH( double rate, *rates) {
        if(rate/stepSize < numBins)
            ++(counts->at(rate/stepSize));
    }
}

CellRateBinInfoPtr HrlNeuralAnalysis::getRatesWithBins(int numBins) {
    CellRateBinInfoPtr cellRateBinInfo(new CellRateBinInfo());
    checkDataStructures();
    calcRatesWithBins(numBins, \
                      &cellRateBinInfo->cells, &cellRateBinInfo->rates, \
                      &cellRateBinInfo->freqs, &cellRateBinInfo->counts);

    return cellRateBinInfo;
}

double HrlNeuralAnalysis::calcMaxRate() {
    //Calculate the simulation time we're analyzing.
    double simTime = paramsIn_->endTime - paramsIn_->startTime;
    double rate;
    double maxRate;
    int count = paramsIn_->startIdx - 1;
    BOOST_FOREACH( vector<int> spikes, *cellActivity_ ) {
        rate = (double)spikes.size()/(simTime/paramsIn_->sampleFreq);
        if(maxRate < rate) {
            maxRate = rate;
        }
    }
    return maxRate;
}

void HrlNeuralAnalysis::calcWindowRate(double step, double rateWindow, std::vector<double> *rates) {
    int numBins = (paramsIn_->endTime - paramsIn_->startTime)/rateWindow;
    int numCells = paramsIn_->endIdx - paramsIn_->startIdx + 1;
    
    int currPosition = 0;
    int currTime = paramsIn_->startTime;
    int currCount = 0;
    double currRate = 0;

    rates->reserve(numBins+1);

    for(int i = 0; i<numBins; i++) {
        for(int j=0; j<rateWindow; j++) {
            if(spikeActivity_->size() > currPosition) {                
                while(spikeActivity_->at(currPosition).first <= currTime) {
                    ++currPosition;
                    ++currCount;
                    if(spikeActivity_->size() <= currPosition) {
                        break;
                    }
                }
            }
            ++currTime;                        
        }
        rates->push_back( (double)currCount/( numCells*(rateWindow/paramsIn_->sampleFreq) ) );
        currCount = 0;        
    }
}

RateInfoPtr HrlNeuralAnalysis::getWindowRate(double step, double rateWindow) {
    RateInfoPtr rateInfo(new RateInfo());
    checkDataStructures();
    calcWindowRate(step, rateWindow, &rateInfo->rates);
    return rateInfo;
}

void HrlNeuralAnalysis::calcGaussWindowRate(double step, double rateWindow, std::vector<double> *rates) {

    // precompute the spike counts at each time step.
    std::vector<int> spikeCounts;
    spikeCounts.reserve(paramsIn_->endTime - paramsIn_->startTime + 1);
    int currSpikeCount;
    int currCountPosition = 0;
    for(int i = paramsIn_->startTime; i < paramsIn_->endTime; i++ ) {
        currSpikeCount = 0;
        if(spikeActivity_->size() > currCountPosition) {
            while(spikeActivity_->at(currCountPosition).first <= i) {
                ++currCountPosition;
                ++currSpikeCount;
                if(spikeActivity_->size() <= currCountPosition) {
                    break;
                }
            }
        }
        spikeCounts.push_back(currSpikeCount);
    }

    // Loop through and calculate the average spike count using a Gaussian weight function.
    int numBins = (paramsIn_->endTime - paramsIn_->startTime)/rateWindow;
    int numCells = paramsIn_->endIdx - paramsIn_->startIdx + 1;
    rates->reserve(numBins);

    double sigma = rateWindow/paramsIn_->sampleFreq;
    double a = 1/(  sqrt( atan(1)*4*2 )*sigma  );
    double b = 2*sigma*sigma;
    double binStart = 0;
    double binEnd = 0;
    double currRate = 0.0;
    double tau = 0.0;
    int currPosition;

    for(int i = 0; i<numBins; i++) {
        currPosition = (i+1)*rateWindow;
        binStart = currPosition-4*rateWindow;
        if(binStart<0)
            binStart=0;

        binEnd = currPosition+4*rateWindow;
        if(binEnd > (paramsIn_->endTime - paramsIn_->startTime))
            binEnd = (paramsIn_->endTime - paramsIn_->startTime);

        currRate = 0.0;

        for(int j=binStart; j<binEnd; j++) {
            // (Current Center) - (the location we're looking at) converted to seconds.
            tau = ((double)(currPosition - j))/paramsIn_->sampleFreq;
            currRate += (double)spikeCounts.at(j)*a*exp(-(tau*tau)/b);
        }
        rates->push_back( (double)currRate/numCells);
    }
}

RateInfoPtr HrlNeuralAnalysis::getGaussWindowRate(double step, double rateWindow) {
    RateInfoPtr rateInfo(new RateInfo());
    checkDataStructures();
    calcGaussWindowRate(step, rateWindow, &rateInfo->rates);
    return rateInfo;
}

void HrlNeuralAnalysis::calcCOV(vector<int> *cells, vector<double> *COV) {

    vector< boost::shared_ptr< vector<int> > > isi;
    int currCell = paramsIn_->startIdx;
    int prevSpike;

    cells->reserve(paramsIn_->endIdx - paramsIn_->startIdx + 1);
    COV->reserve(paramsIn_->endIdx - paramsIn_->startIdx + 1);
    isi.reserve(paramsIn_->endIdx - paramsIn_->startIdx + 1);

    // Construct the inter-spike interval calculations
    BOOST_FOREACH( vector<int> spikes, *cellActivity_) {
        // This is greater than 10 to create 10 ISI values.
        if(spikes.size() > 10) {

            vector<int>::reverse_iterator spikeIt;
            vector<int>::reverse_iterator spikePrevIt = spikes.rbegin();
            boost::shared_ptr< vector<int> > ptrCellVec(new vector<int>);
            ptrCellVec->reserve(spikes.size() + 1);
            cells->push_back(currCell);
            isi.push_back(ptrCellVec);

            for(spikeIt=spikes.rbegin(); spikeIt < spikes.rend(); ++spikeIt) {
                if(spikeIt != spikePrevIt) {
                    ptrCellVec->push_back(*spikePrevIt - *spikeIt);
                    ++spikePrevIt;
                }
            }
        }
        ++currCell;
    }

    // Loop through and calculate the COV for each of the cells.
    currCell = -1;
    BOOST_FOREACH(boost::shared_ptr< vector<int> > cellIsi, isi) {
        double sum_of_squares = 0;
        double variance = 0;
        double mean = (double)accumulate(cellIsi->begin(), cellIsi->end(), 0)/cellIsi->size();
        BOOST_FOREACH(int isiVal, *cellIsi) {
            sum_of_squares += pow((double)isiVal-mean,2);
        }
        variance = sum_of_squares / (double)cellIsi->size();
        COV->push_back(sqrt(variance) / mean);
    }
}

CovInfoPtr HrlNeuralAnalysis::getCOV() {
    CovInfoPtr covInfo(new CovInfo());
    checkDataStructures();
    calcCOV(&covInfo->cells, &covInfo->cov);
    return covInfo;
}

void HrlNeuralAnalysis::calcSpikeTimes(std::vector<int> *time, std::vector<int> *spikes) {
    time->reserve(spikeActivity_->size());
    spikes->reserve(spikeActivity_->size());
    for (vector<std::pair<int,int> >::iterator spikeIt = spikeActivity_->begin(); spikeIt!=spikeActivity_->end(); ++spikeIt) {
        time->push_back((*spikeIt).first);
        spikes->push_back((*spikeIt).second);
    }
}

RasterInfoPtr HrlNeuralAnalysis::getSpikeTimes() {
    RasterInfoPtr rasterInfo(new RasterInfo());
    checkDataStructures();
    calcSpikeTimes(&rasterInfo->time, &rasterInfo->spikes);
    return rasterInfo;
}

void HrlNeuralAnalysis::buildSpikeActFromCellAct() {
    int currCell = paramsIn_->startIdx;
    spikeActivity_->clear();
    BOOST_FOREACH( vector<int> spikes, *cellActivity_ ) {
        BOOST_FOREACH( int spikeTime, spikes ) {
            spikeActivity_->push_back(make_pair(spikeTime,currCell));
        }
        ++currCell;
    }
    sort(spikeActivity_->begin(), spikeActivity_->end());
}

void HrlNeuralAnalysis::sortSpikeActivity() {
    sort(spikeActivity_->begin(), spikeActivity_->end());
}

void HrlNeuralAnalysis::clearDataStructures() {
    spikeActivity_->clear();
    cellActivity_->clear();
}

SpikeActivityPtr HrlNeuralAnalysis::spikeActivity() {
    return spikeActivity_;
}

CellActivityPtr HrlNeuralAnalysis::cellActivity() {
    return cellActivity_;
}

NeuronParamsPtr HrlNeuralAnalysis::paramsIn() {
    return paramsIn_;
}

void HrlNeuralAnalysis::setParamsIn(NeuronParamsPtr params) {
    paramsIn_ = params;
}

void HrlNeuralAnalysis::addSpike(int time, int index) {
     spikeActivity_->push_back( make_pair(time,index));
}

void HrlNeuralAnalysis::checkDataStructures() {
    if(!paramsIn_->isDataCompiled)
        buildDataStructures();
}

