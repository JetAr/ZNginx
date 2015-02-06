#include "../../src/MongoServer.h"

#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char * argv[])
{
    std::string path = argc >=2 ? argv[1] : "";
    Mongo::Server server;
    server
    .setOption("listening_ports","8080")
    .setStart([&](Mongo::Request request, Mongo::Response response) -> bool
    {
        static int i = 1;
        std::string img = request.get("img");
        if( ! img.empty() )
        {
            img = path + img;
            std::ifstream fs(img.c_str(),std::ios::binary);
            if( fs )
            {
                response.contentType("image/jpeg");
                std::vector<char> buf(4096);
                while( fs.read(&buf[0],buf.size()) )
                {
                    response.write(&buf[0],fs.gcount());
                }
            }
            else
            {
                response
                .status(404)
                .printf("image not found");
            }
        }
        else if( request.hasGet("alt") )
        {
            response.printf("U SUCK %d TIMES",i++);
        }
        else {
            response
            .status(200)
            .contentType("text/plain")
            .printf("U SUCK %d",i++)
            ;
        }
        return true;
    })
    .start();
    std::cout << "type something and press enter to exit";
    int x;
    std::cin >> x;
}
