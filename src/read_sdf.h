#pragma once

#include "sdf.h"
#include <memory>

bool
read_objfile(std::unique_ptr<SDF>& sdfptr,
             const char *filename_format,...);
bool
write_objfile(SDF& sdfptr,
             const char *filename_format,...);
