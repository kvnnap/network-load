//
// Created by kevin on 9/22/16.
//

#ifndef NETWORK_LOAD_STEPPDF_H
#define NETWORK_LOAD_STEPPDF_H

#include <cstddef>
#include <vector>

#include "UniformSampler.h"

namespace Sampler {
    class StepPDF {
    public:
        StepPDF(const std::vector<uint32_t>& p_pdsf);
        ssize_t getNext();
        const std::vector<uint32_t>& getPDSF() const;
    private:
        UniformSampler uniformSampler;
        std::vector<uint32_t> pdsf;
    };

    std::ostream& operator<< (std::ostream& strm, const StepPDF& stepPDF);
}


#endif //NETWORK_LOAD_STEPPDF_H
