#pragma once

#include "sdf.h"
#include <memory>

bool
read_sdffile(std::unique_ptr<SDF>& sdfptr,
             const char *filename_format,...);
bool
write_sdffile(const SDF& sdf,
             const char *filename_format,...);
