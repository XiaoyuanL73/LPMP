#pragma once

#include "MCF-SSP/mcf_ssp.hxx"

namespace LPMP {

    class lifted_disjoint_paths_mcf_factor
    {
        public: 
            double LowerBound() const{return 0;}
            double EvaluatePrimal() const{return 0;}

            template<class ARCHIVE> void serialize_primal(ARCHIVE& ar) { ar(primal_); }
            template<class ARCHIVE> void serialize_dual(ARCHIVE& ar) { ar(); }

            auto export_variables() { return std::tie(); }

        void init_primal() { }

        private:

        size_t primal_;
    };

}
