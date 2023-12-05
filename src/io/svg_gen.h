//
// Created by Dan Schnurpfeil on 05.12.2023.
//

#ifndef PPR_SRC_IO_SVG_GEN_H_
#define PPR_SRC_IO_SVG_GEN_H_

#include "RecordACC.h"
#include "../genetic_alg/Equation.h"
#include "../preprocessing/utils.h"

void make_svg(std::shared_ptr<Equation> &best_one, float corr,
              std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data, std::string name);

#endif //PPR_SRC_IO_SVG_GEN_H_
