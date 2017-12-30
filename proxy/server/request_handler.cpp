//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root)
{
}

void request_handler::handle_request(const request& req, reply& rep)
{
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/')
  {
    request_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::string full_path = doc_root_ + request_path;
  std::cout << full_path << std::endl;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    rep = reply::stock_reply(reply::not_found);
    return;
  }

  // Fill out the reply to be sent to the client.
  rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    rep.content.append(buf, is.gcount());
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = mime_types::extension_to_type(extension);
}

bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  std::size_t i = 0;
  int num = 0;
  std::string numstr = "";
  while(i < in.size()) {
    if (in[i]=='?' && in[i+1]=='i' && in[i+2]=='d'
         && in[i+3]=='=') {
        i+=4;

        while(i < in.size() && in[i] >= '0' && in[i] <= '9') {
            numstr += in[i];
            ++i;
        }
        num = stoi(numstr);
    }
    ++i;
  }
  //std::cout << num << "\n";
    boost::asio::ip::tcp::iostream stream;
    stream.expires_from_now(boost::posix_time::seconds(60));
    stream.connect("localhost", std::to_string(10001));
    stream << "GET /index.html?id="+numstr+" HTTP/1.0\r\n";
    stream << "Host: localhost:10001\r\n";
    stream << "Accept: */*\r\n";
    stream << "Connection: close\r\n\r\n";
    stream.flush();
    std::ostringstream ss;
    ss << stream.rdbuf();
    std::string str = ss.str();
    str = str.substr(64);
    std::string knowstr = "HTTP/1.0 200 OK\nContent-Length: 28\nContent-Type: text/html\n\n";
    std::string mystr   = "QWERTYUIOPASDFGHJKLZXCVBNMQWERTY";
    for (int i = 0; i < 32; i++) {
        str[i] ^= knowstr[i];
        str[i] ^= mystr[i];
    }
    std::ofstream outstream ("web/myout.html");
    outstream << str;


  out="/myout.html";
  std::cout << in << " " << out << std::endl;
  return true;
}

} // namespace server
} // namespace http
