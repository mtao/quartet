#include "read_sdf.h"

#include <fstream>


bool
read_objfile(std::unique_ptr<SDF>& sdfptr,
             const char *filename_format,...) {

    va_list ap;
    va_start(ap, filename_format);
#ifdef WIN32
#define FILENAMELENGTH 256
   char filename[FILENAMELENGTH];
   _vsnprintf(filename, FILENAMELENGTH, filename_format, ap);
   std::ifstream input(filename);
#else
   char *filename;
   if (vasprintf(&filename, filename_format, ap) == -1) {
        return false;
   }
   std::ifstream input(filename);
   std::free(filename);
#endif
    va_end(ap);
    int ni,nj,nk;
    float dx;
    Vec3f origin;

    input >> ni  >> nj >> nk;
    input >> dx;
    input >> origin;
    int size = ni * nj * nk;
    sdfptr = std::make_unique<SDF>(origin,dx,ni,nj,nk);
    for(auto&& v: sdfptr->phi.a) {
        input >> v;
    }
    return true;
}
bool write_sdffile(const SDF& sdf,
              const char *filename_format, ...)
{
    va_list ap;
    va_start(ap, filename_format);
#ifdef WIN32
#define FILENAMELENGTH 256
   char filename[FILENAMELENGTH];
   _vsnprintf(filename, FILENAMELENGTH, filename_format, ap);
   std::ofstream output(filename);
#else
   char *filename;
   if (vasprintf(&filename, filename_format, ap) == -1) {
       return false;
   }
   std::ofstream output(filename);
   std::free(filename);
#endif
    va_end(ap);
    if(!output.good()) return false;
    output << sdf.phi.ni 
        << sdf.phi.nj << " "
        << sdf.phi.nk << " "
        << std::endl;
    output << sdf.dx << std::endl;
    output << sdf.origin << std::endl;
    for(auto&& v: sdf.phi.a) {
        output << v << " ";
    }
    output << std::endl;
    return true;
}
