//
// Created by kevin on 9/22/16.
//

#include <stdexcept>
#include <ostream>

#include "StepPDF.h"

using namespace std;
using namespace Sampler;

StepPDF::StepPDF(const vector<uint32_t> &p_pdsf)
    : pdsf ( p_pdsf )
{
    // Sanity check
    uint32_t check = 0;
    for (uint32_t pdsfVal : pdsf) {
        check += pdsfVal;
    }
    if (check > 100) {
        throw invalid_argument("Probability Distribution Step Function exceeds 100");
    }
}

ssize_t StepPDF::getNext() {
    uint32_t selected = static_cast<uint32_t>(uniformSampler.nextSample((size_t)0, (size_t)99));
    uint32_t val = 0;
    for (size_t i = 0; i < pdsf.size(); ++i) {
        if (selected >= val && selected < (val + pdsf[i])) {
            return i;
        }
        val += pdsf[i];
    }
    return -1;
    //throw runtime_error("Fatal: Could not select next");
}

const std::vector<uint32_t> &StepPDF::getPDSF() const {
    return pdsf;
}


ostream& Sampler::operator<<(ostream &strm, const StepPDF &stepPDF) {
    strm << "{";
    for (size_t i = 0; i < stepPDF.getPDSF().size(); ++i) {
        strm << stepPDF.getPDSF()[i];
        if ((i + 1) != stepPDF.getPDSF().size()) {
            strm << ", ";
        }
    }
    strm << "}";
    return strm;
}
