#include "core/tingx_descriptor.hpp"
#include "core/tingx_parser.h"
#include "http/tingx_http.h"
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <map>

using namespace tingx;
using namespace std;

static std::string test_recv_header = 
"GET / HTTP/1.1\r\n"
"Host: localhost:65534\r\n"
"Connection: keep-alive\r\n"
"Cache-Control: max-age=0\r\n"
"sec-ch-ua: \"Google Chrome\";v=\"95\", \"Chromium\";v=\"95\", \";Not A Brand\";v=\"99\"\r\n"
"sec-ch-ua-mobile: ?0\r\n"
"sec-ch-ua-platform: \"Linux\"\r\n"
"Upgrade-Insecure-Requests: 1\r\n"
"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.69 Safari/537.36\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
"Sec-Fetch-Site: none\r\n"
"Sec-Fetch-Mode: navigate\r\n"
"Sec-Fetch-User: ?1\r\n"
"Sec-Fetch-Dest: document\r\n"
"Accept-Encoding: gzip, deflate, br\r\n"
"Accept-Language: en-US,en;q=0.9\r\n"
"\r\n"
"Hello world";

int main() {

    Request *req = RequestParser(test_recv_header);
    cout << req->header << endl;
    cout << "body:" << req->body << endl;
    cout << "host: " << req->header["Host"] << endl;
    delete req;
    return 0;
}