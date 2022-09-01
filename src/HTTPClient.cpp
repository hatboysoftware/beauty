#ifndef BEAUTY_INCLUDE_HTTP_CLIENT_H_
#define BEAUTY_INCLUDE_HTTP_CLIENT_H_

#include <boost/json.hpp>
#include <boost/beast/http.hpp>

#include <string>

#include <string>
#include <list>
#include <tuple>
#include <set>
#include <initializer_list>
#include <stdexcept>

class RequestBuilder;

// TODO move elsewhere

class EmptyResponse : Response
{
public:
    virtual ~EmptyResponse() {}
};

class JSONResponse
:   public Response
{
public:
    friend class RequestBuilder;
    JSONResponse(const boost::beast::http::response<boost::beast::http::string_body> *pResp,
                 bool hasResponseHandler,
                 std::function<void(std::shared_ptr<Response> pResp)> responseHandler,
                 bool hasJSONBodyResponseHandler,
                 std::function<void(std::shared_ptr<JSONResponse> pResp)> JSONResponseHandler);

public:
    virtual ~JSONResponse() {}
    const boost::json::value &getJSON() const;

    virtual void Respond();

private:
    boost::json::value m_jsonBody;

    bool m_hasJSONBodyResponseHandler;
    std::function<void(std::shared_ptr<JSONResponse> pResp)> m_JSONResponseHandler;
};

// This error is for when a connection cannot be established for some reason;
// If the connection was established then the response callback will be called
// and the Status code will contain 200 or whatever.
class Error
{
public:
    // Textual description of this error; generally only used for debugging
    std::string Verbose();

    // The Request that generated this Error
    Request &getRequest();
};

typedef std::function<void(Error &err)> ErrorHandler;

class BaseAPIHandler;

class RequestBuilder
{
    friend class BaseAPIHandler;
    RequestBuilder(BaseAPIHandler &handler, const std::string &url);

public:
    // Add args to the URL
    RequestBuilder &withArgs(std::list<std::initializer_list<std::string>> args);

    // Send a JSON body request
    RequestBuilder &withJSONBody(boost::json::value body);

    // Specify additional request headers
    RequestBuilder &withHeader(std::string key, std::string value);

    // Register response handlers
    RequestBuilder &onError(ErrorHandler err);

    // onResponse handler is always called first if it's registered
    RequestBuilder &onResponse(std::function<void(std::shared_ptr<Response> pResp)> handler);

    // These specialized responses are called without a guaranteed order.
    // For redundant types (e.g. JSON/string) then probably you should only
    // register one handler
    // REVIEW: why not `const JSONResponse& response`?
    RequestBuilder &onEmptyResponse(std::function<void(std::shared_ptr<EmptyResponse> pResp)> handler);
    RequestBuilder &onJSONResponse(std::function<void(std::shared_ptr<JSONResponse> pResp)> handler);

    // Call one of these to set the verb.  The request is not sent until
    // one of these methods are called, and an error will be thrown if
    // more than one of these methods are called.
    void get();
    void post();
    void patch();

private:
    void go();

    boost::beast::http::verb m_verb;

    BaseAPIHandler &m_APIHandler;
    std::string m_url;

    std::list<std::tuple<std::string, std::string>> m_requestArgs;

    bool m_hasJSONBodyRequest;
    std::string m_requestBody;

    ErrorHandler m_errorHandler;

    bool m_hasResponseHandler;
    std::function<void(std::shared_ptr<Response> pResp)> m_responseHandler;

    bool m_hasJSONBodyResponseHandler;
    std::function<void(std::shared_ptr<JSONResponse> pResp)> m_JSONResponseHandler;

    std::list<std::tuple<std::string, std::string>> m_headers;
};

namespace json = boost::json;

typedef std::function<void(Request &req)> RequestFilter;

// typedef std::function<void(Response& req)> ResponseHandler;

// TODO *pResp should be an auto_ptr
typedef std::function<void(std::shared_ptr<Response>)> ResponseHandler;
typedef std::function<void(Error &err)> ErrorHandler;

class Options
{
public:
    std::string host;
    std::string baseURL;

    int timeout;
    std::list<std::tuple<std::string, std::string>> headers;
    std::string port;
    std::list<RequestFilter> requestFilter;

    bool hasResponseHandler;
    std::function<void(std::shared_ptr<Response>)> responseHandler;
};

typedef std::function<void(Options &options)> Option;

class BaseAPIHandler
{
public:
    RequestBuilder &request(const std::string &url);

    const std::string &getHost() const;
    const std::string &getPort() const;
    const std::string &getBaseURL() const;
    const std::list<std::tuple<std::string, std::string>> &getHeaders() const;

    // Global response set with options during BaseAPIHandler construction
    bool hasResponseHandler() const;
    std::function<void(std::shared_ptr<Response>)> getResponseHandler() const;

    //*************************
    // Constructors
    // explicit
    BaseAPIHandler(std::initializer_list<Option> args);

    explicit BaseAPIHandler();

    //*************************
    // Options
    static Option withHost(std::string host);
    static Option withBaseURL(std::string baseURL);
    static Option withHeader(std::string key, std::string value);
    static Option withPort(std::string port);
    static Option withRequestFilter(RequestFilter filter);
    static Option withResponseHandler(ResponseHandler handler);

private:
    Options m_options;

    // outstanding RequestBuilder
    std::set<RequestBuilder *> m_builders;
};

// TODO move elsewhere

class NotImplemented : public std::logic_error
{
private:
    std::string m_text;

public:
    NotImplemented(const char *message, const char *function)
            : std::logic_error("Not Implemented")
    {
        m_text = message;
        m_text += " : ";
        m_text += function;
    };

    virtual const char *what() const throw()
    {
        return m_text.c_str();
    }
};

#define NOT_IMPLEMENTED() \
  throw NotImplemented("Not Implemented", __FUNCTION__)

#define NOT_IMPLEMENTED_MSG(msg) \
  throw NotImplemented(msg, __FUNCTION__)

#define BEAST_ERR(ec, msg) \
  throw std::logic_error(std::string("beast error: ") + ec.message() + std::string(" - ") + std::string(msg) + __FUNCTION__)

#endif // BEAUTY_INCLUDE_HTTP_CLIENT_H_
