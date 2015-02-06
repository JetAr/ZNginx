#include "Template.h"
#include <fstream>
#include <vector>

namespace Mongo
{

Template::Template(Response & response, std::string basePath_):
    response(response),
    basePath(basePath_)
{
    if( ! basePath.empty() && basePath.back() != '/' )
    {
        basePath.push_back('/');
    }
}

std::string Template::loadFileContents(char const * filename)
{
    using namespace std;
    ifstream fs(basePath.empty() ? filename : (basePath+filename).c_str(),ios::binary);
    if( ! fs.seekg(0,ios_base::end) ) return "";

    std::string contents(size_t(fs.tellg()),'\0');
    fs.seekg(0,ios_base::beg);
    size_t p = 0;
    while( fs && p < contents.size() )
    {
        fs.read(&contents[p],contents.size()-p);
        p += fs.gcount();
    }

    return contents;
}

void Template::print(char const * filename)
{
    std::ifstream fs(basePath.empty() ? filename : (basePath+filename).c_str(),std::ios::binary);
    response.write(fs);
}

void Template::printf(char const * filename, ...)
{
    using namespace std;
    ifstream fs(basePath.empty() ? filename : (basePath+filename).c_str(),ios::binary);
    if( ! fs.seekg(0,ios_base::end) ) return;

    vector<char> contents(size_t(fs.tellg())+1);		// +1 for null terminator
    fs.seekg(0,ios_base::beg);
    size_t p = 0;
    while( fs && p < contents.size()-1 )
    {
        fs.read(&contents[p],contents.size()-1-p);
        p += fs.gcount();
    }
    // if( p != contents.size()-1 ) return;

    va_list ap;
    va_start(ap, filename);
    response.vprintf(&contents[0],ap);
    va_end(ap);
}

}

