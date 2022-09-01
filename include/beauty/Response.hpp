#ifndef BEAUTY_RESPONSE_H_INCLUDED_
#define BEAUTY_RESPONSE_H_INCLUDED_

#include <memory>

#include <boost/beast/http.hpp>

class Request;

class Response
:   public std::enable_shared_from_this<Response>
{
private:
    Response();

protected:
    friend class RequestBuilder;

public:
    virtual ~Response();

    Response(const boost::beast::http::response<boost::beast::http::string_body> *pResp,
             bool hasResponseHandler,
             std::function<void(std::shared_ptr<Response> pResp)> responseHandler);

public:
    int Status() ;
    std::string StatusText() ;

    const std::string &Body() ;

    // Not implemented, but the intention will be to use this
    // to get the returned headers
    const boost::beast::http::header<false, boost::beast::http::fields>& Headers();

    // The IRequest that generated this response
    Request &getRequest();

    // This function should be called once and only once by the optional
    // response handler specified in the BaseAPIHandler constructor.
    virtual void Respond();

private:
    const boost::beast::http::response<boost::beast::http::string_body> *m_pResponse;

    bool m_hasResponseHandler;
    std::function<void(std::shared_ptr<Response> pResp)> m_responseHandler;
};

#endif // BEAUTY_RESPONSE_H_INCLUDED_
