#ifndef BEAUTY_SESSION_H_INCLUDED_
#define BEAUTY_SESSION_H_INCLUDED_

#include <string>
#include <list>
#include <tuple>
#include <functional>


namespace Beauty {
namespace Client {

class RequestBuilder;
class Request;
class Response;
class Option;

typedef std::function<void(Request &req)> RequestFilter;
typedef std::function<void(Response& req)> ResponseHandler;

class Session {
public:
  RequestBuilder &request(const std::string &url);

  const std::string &getHost() const;
  const std::string &getPort() const;
  const std::string &getBaseURL() const;
  const std::list<std::tuple<std::string, std::string>> &getHeaders() const;


    //*************************
    // Options
    static Option withHost(std::string host);
    static Option withBaseURL(std::string baseURL);
    static Option withHeader(std::string key, std::string value);
    static Option withPort(std::string port);
    static Option withRequestFilter(RequestFilter filter);
    static Option withResponseHandler(ResponseHandler handler);

};

} // namespace Client
} // namespace Beauty

#endif // BEAUTY_SESSION_H_INCLUDED_
