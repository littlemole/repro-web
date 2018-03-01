#ifndef _MOL_DEF_GUARD_DEFINE_MOD_HTTP_ICONV_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_MOD_HTTP_ICONV_DEF_GUARD_

#include <string>
#include <vector>
#include <iconv.h>

namespace reproweb  {


class miconv
{
public:

    miconv( const char* from, const char* to);
    ~miconv();

    std::string convert( const std::string& input );
    
    static std::vector<std::string> encodings();

private:

    iconv_t ict_;
};

} // end namespace mol

#endif


