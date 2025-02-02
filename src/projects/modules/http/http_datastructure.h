//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <base/ovcrypto/ovcrypto.h>
#include <base/ovlibrary/ovlibrary.h>
#include <base/ovsocket/ovsocket.h>

#include <functional>
#include <memory>

// RFC7231 - 4. Request Methods
// +---------+-------------------------------------------------+-------+
// | Method  | Description                                     | Sec.  |
// +---------+-------------------------------------------------+-------+
// | GET     | Transfer a current representation of the target | 4.3.1 |
// |         | resource.                                       |       |
// | HEAD    | Same as GET, but only transfer the status line  | 4.3.2 |
// |         | and header section.                             |       |
// | POST    | Perform resource-specific processing on the     | 4.3.3 |
// |         | request payload.                                |       |
// | PUT     | Replace all current representations of the      | 4.3.4 |
// |         | target resource with the request payload.       |       |
// | DELETE  | Remove all current representations of the       | 4.3.5 |
// |         | target resource.                                |       |
// | CONNECT | Establish a tunnel to the server identified by  | 4.3.6 |
// |         | the target resource.                            |       |
// | OPTIONS | Describe the communication options for the      | 4.3.7 |
// |         | target resource.                                |       |
// | TRACE   | Perform a message loop-back test along the path | 4.3.8 |
// |         | to the target resource.                         |       |
// +---------+-------------------------------------------------+-------+
enum class HttpMethod : uint16_t
{
	Unknown = 0x0000,

	Get = 0x0001,
	Head = 0x0002,
	Post = 0x0004,
	Put = 0x0008,
	Delete = 0x0010,
	Connect = 0x0020,
	Options = 0x0040,
	Trace = 0x0080,

	All = Get | Head | Post | Put | Delete | Connect | Options | Trace
};

enum class HttpConnectionPolicy : char
{
	// Send "Connection: Closed" header
	Closed,
	// Send "Connection: Keep-Alive" header
	KeepAlive
};

enum class HttpInterceptorResult : char
{
	Keep,
	Disconnect,
};

enum class HttpNextHandler : char
{
	// Call the next handler
	Call,
	// Do not call the next handler
	DoNotCall
};

inline HttpMethod operator|(HttpMethod lhs, HttpMethod rhs)
{
	return static_cast<HttpMethod>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}

// RFC7231 - 6. Response Status Codes
// +------+-------------------------------+--------------------------+
// | Code | Reason-Phrase                 | Defined in...            |
// +------+-------------------------------+--------------------------+
// | 100  | Continue                      | Section 6.2.1            |
// | 101  | Switching Protocols           | Section 6.2.2            |
// | 200  | OK                            | Section 6.3.1            |
// | 201  | Created                       | Section 6.3.2            |
// | 202  | Accepted                      | Section 6.3.3            |
// | 203  | Non-Authoritative Information | Section 6.3.4            |
// | 204  | No Content                    | Section 6.3.5            |
// | 205  | Reset Content                 | Section 6.3.6            |
// | 206  | Partial Content               | Section 4.1 of [RFC7233] |
// | 300  | Multiple Choices              | Section 6.4.1            |
// | 301  | Moved Permanently             | Section 6.4.2            |
// | 302  | Found                         | Section 6.4.3            |
// | 303  | See Other                     | Section 6.4.4            |
// | 304  | Not Modified                  | Section 4.1 of [RFC7232] |
// | 305  | Use Proxy                     | Section 6.4.5            |
// | 307  | Temporary Redirect            | Section 6.4.7            |
// | 400  | Bad Request                   | Section 6.5.1            |
// | 401  | Unauthorized                  | Section 3.1 of [RFC7235] |
// | 402  | Payment Required              | Section 6.5.2            |
// | 403  | Forbidden                     | Section 6.5.3            |
// | 404  | Not Found                     | Section 6.5.4            |
// | 405  | Method Not Allowed            | Section 6.5.5            |
// | 406  | Not Acceptable                | Section 6.5.6            |
// | 407  | Proxy Authentication Required | Section 3.2 of [RFC7235] |
// | 408  | Request Timeout               | Section 6.5.7            |
// | 409  | Conflict                      | Section 6.5.8            |
// | 410  | Gone                          | Section 6.5.9            |
// | 411  | Length Required               | Section 6.5.10           |
// | 412  | Precondition Failed           | Section 4.2 of [RFC7232] |
// | 413  | Payload Too Large             | Section 6.5.11           |
// | 414  | URI Too Long                  | Section 6.5.12           |
// | 415  | Unsupported Media Type        | Section 6.5.13           |
// | 416  | Range Not Satisfiable         | Section 4.4 of [RFC7233] |
// | 417  | Expectation Failed            | Section 6.5.14           |
// | 426  | Upgrade Required              | Section 6.5.15           |
// | 500  | Internal Server Error         | Section 6.6.1            |
// | 501  | Not Implemented               | Section 6.6.2            |
// | 502  | Bad Gateway                   | Section 6.6.3            |
// | 503  | Service Unavailable           | Section 6.6.4            |
// | 504  | Gateway Timeout               | Section 6.6.5            |
// | 505  | HTTP Version Not Supported    | Section 6.6.6            |
// +------+-------------------------------+--------------------------+
enum class HttpStatusCode : uint16_t
{
	Continue = 100,
	SwitchingProtocols = 101,
	OK = 200,
	Created = 201,
	Accepted = 202,
	NonAuthoritativeInformation = 203,
	NoContent = 204,
	ResetContent = 205,
	PartialContent = 206,
	// RFC 4918 (https://tools.ietf.org/html/rfc4918#section-11.1)
	MultiStatus = 207,
	MultipleChoices = 300,
	MovedPermanently = 301,
	Found = 302,
	SeeOther = 303,
	NotModified = 304,
	UseProxy = 305,
	TemporaryRedirect = 307,
	BadRequest = 400,
	Unauthorized = 401,
	PaymentRequired = 402,
	Forbidden = 403,
	NotFound = 404,
	MethodNotAllowed = 405,
	NotAcceptable = 406,
	ProxyAuthenticationRequired = 407,
	RequestTimeout = 408,
	Conflict = 409,
	Gone = 410,
	LengthRequired = 411,
	PreconditionFailed = 412,
	PayloadTooLarge = 413,
	URITooLong = 414,
	UnsupportedMediaType = 415,
	RangeNotSatisfiable = 416,
	ExpectationFailed = 417,
	UpgradeRequired = 426,
	InternalServerError = 500,
	NotImplemented = 501,
	BadGateway = 502,
	ServiceUnavailable = 503,
	GatewayTimeout = 504,
	HTTPVersionNotSupported = 505
};

#define HTTP_STATUS_RETURN(condition, value) \
	case condition:                          \
		return value

inline constexpr bool IsValidHttpStatusCode(HttpStatusCode status_code)
{
	switch (status_code)
	{
		HTTP_STATUS_RETURN(HttpStatusCode::Continue, true);
		HTTP_STATUS_RETURN(HttpStatusCode::SwitchingProtocols, true);
		HTTP_STATUS_RETURN(HttpStatusCode::OK, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Created, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Accepted, true);
		HTTP_STATUS_RETURN(HttpStatusCode::NonAuthoritativeInformation, true);
		HTTP_STATUS_RETURN(HttpStatusCode::NoContent, true);
		HTTP_STATUS_RETURN(HttpStatusCode::ResetContent, true);
		HTTP_STATUS_RETURN(HttpStatusCode::PartialContent, true);
		HTTP_STATUS_RETURN(HttpStatusCode::MultiStatus, true);
		HTTP_STATUS_RETURN(HttpStatusCode::MultipleChoices, true);
		HTTP_STATUS_RETURN(HttpStatusCode::MovedPermanently, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Found, true);
		HTTP_STATUS_RETURN(HttpStatusCode::SeeOther, true);
		HTTP_STATUS_RETURN(HttpStatusCode::NotModified, true);
		HTTP_STATUS_RETURN(HttpStatusCode::UseProxy, true);
		HTTP_STATUS_RETURN(HttpStatusCode::TemporaryRedirect, true);
		HTTP_STATUS_RETURN(HttpStatusCode::BadRequest, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Unauthorized, true);
		HTTP_STATUS_RETURN(HttpStatusCode::PaymentRequired, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Forbidden, true);
		HTTP_STATUS_RETURN(HttpStatusCode::NotFound, true);
		HTTP_STATUS_RETURN(HttpStatusCode::MethodNotAllowed, true);
		HTTP_STATUS_RETURN(HttpStatusCode::NotAcceptable, true);
		HTTP_STATUS_RETURN(HttpStatusCode::ProxyAuthenticationRequired, true);
		HTTP_STATUS_RETURN(HttpStatusCode::RequestTimeout, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Conflict, true);
		HTTP_STATUS_RETURN(HttpStatusCode::Gone, true);
		HTTP_STATUS_RETURN(HttpStatusCode::LengthRequired, true);
		HTTP_STATUS_RETURN(HttpStatusCode::PreconditionFailed, true);
		HTTP_STATUS_RETURN(HttpStatusCode::PayloadTooLarge, true);
		HTTP_STATUS_RETURN(HttpStatusCode::URITooLong, true);
		HTTP_STATUS_RETURN(HttpStatusCode::UnsupportedMediaType, true);
		HTTP_STATUS_RETURN(HttpStatusCode::RangeNotSatisfiable, true);
		HTTP_STATUS_RETURN(HttpStatusCode::ExpectationFailed, true);
		HTTP_STATUS_RETURN(HttpStatusCode::UpgradeRequired, true);
		HTTP_STATUS_RETURN(HttpStatusCode::InternalServerError, true);
		HTTP_STATUS_RETURN(HttpStatusCode::NotImplemented, true);
		HTTP_STATUS_RETURN(HttpStatusCode::BadGateway, true);
		HTTP_STATUS_RETURN(HttpStatusCode::ServiceUnavailable, true);
		HTTP_STATUS_RETURN(HttpStatusCode::GatewayTimeout, true);
		HTTP_STATUS_RETURN(HttpStatusCode::HTTPVersionNotSupported, true);
	}

	return false;
}

inline constexpr const char *StringFromHttpStatusCode(HttpStatusCode status_code)
{
	switch (status_code)
	{
		HTTP_STATUS_RETURN(HttpStatusCode::Continue, "Continue");
		HTTP_STATUS_RETURN(HttpStatusCode::SwitchingProtocols, "Switching Protocols");
		HTTP_STATUS_RETURN(HttpStatusCode::OK, "OK");
		HTTP_STATUS_RETURN(HttpStatusCode::Created, "Created");
		HTTP_STATUS_RETURN(HttpStatusCode::Accepted, "Accepted");
		HTTP_STATUS_RETURN(HttpStatusCode::NonAuthoritativeInformation, "Non-Authoritative Information");
		HTTP_STATUS_RETURN(HttpStatusCode::NoContent, "No Content");
		HTTP_STATUS_RETURN(HttpStatusCode::ResetContent, "Reset Content");
		HTTP_STATUS_RETURN(HttpStatusCode::PartialContent, "Partial Content");
		HTTP_STATUS_RETURN(HttpStatusCode::MultiStatus, "Multi Status");
		HTTP_STATUS_RETURN(HttpStatusCode::MultipleChoices, "Multiple Choices");
		HTTP_STATUS_RETURN(HttpStatusCode::MovedPermanently, "Moved Permanently");
		HTTP_STATUS_RETURN(HttpStatusCode::Found, "Found");
		HTTP_STATUS_RETURN(HttpStatusCode::SeeOther, "See Other");
		HTTP_STATUS_RETURN(HttpStatusCode::NotModified, "Not Modified");
		HTTP_STATUS_RETURN(HttpStatusCode::UseProxy, "Use Proxy");
		HTTP_STATUS_RETURN(HttpStatusCode::TemporaryRedirect, "Temporary Redirect");
		HTTP_STATUS_RETURN(HttpStatusCode::BadRequest, "Bad Request");
		HTTP_STATUS_RETURN(HttpStatusCode::Unauthorized, "Unauthorized");
		HTTP_STATUS_RETURN(HttpStatusCode::PaymentRequired, "Payment Required");
		HTTP_STATUS_RETURN(HttpStatusCode::Forbidden, "Forbidden");
		HTTP_STATUS_RETURN(HttpStatusCode::NotFound, "Not Found");
		HTTP_STATUS_RETURN(HttpStatusCode::MethodNotAllowed, "Method Not Allowed");
		HTTP_STATUS_RETURN(HttpStatusCode::NotAcceptable, "Not Acceptable");
		HTTP_STATUS_RETURN(HttpStatusCode::ProxyAuthenticationRequired, "Proxy Authentication Required");
		HTTP_STATUS_RETURN(HttpStatusCode::RequestTimeout, "Request Timeout");
		HTTP_STATUS_RETURN(HttpStatusCode::Conflict, "Conflict");
		HTTP_STATUS_RETURN(HttpStatusCode::Gone, "Gone");
		HTTP_STATUS_RETURN(HttpStatusCode::LengthRequired, "Length Required");
		HTTP_STATUS_RETURN(HttpStatusCode::PreconditionFailed, "Precondition Failed");
		HTTP_STATUS_RETURN(HttpStatusCode::PayloadTooLarge, "Payload Too Large");
		HTTP_STATUS_RETURN(HttpStatusCode::URITooLong, "URI Too Long");
		HTTP_STATUS_RETURN(HttpStatusCode::UnsupportedMediaType, "Unsupported Media Type");
		HTTP_STATUS_RETURN(HttpStatusCode::RangeNotSatisfiable, "Range Not Satisfiable");
		HTTP_STATUS_RETURN(HttpStatusCode::ExpectationFailed, "Expectation Failed");
		HTTP_STATUS_RETURN(HttpStatusCode::UpgradeRequired, "Upgrade Required");
		HTTP_STATUS_RETURN(HttpStatusCode::InternalServerError, "Internal Server Error");
		HTTP_STATUS_RETURN(HttpStatusCode::NotImplemented, "Not Implemented");
		HTTP_STATUS_RETURN(HttpStatusCode::BadGateway, "Bad Gateway");
		HTTP_STATUS_RETURN(HttpStatusCode::ServiceUnavailable, "Service Unavailable");
		HTTP_STATUS_RETURN(HttpStatusCode::GatewayTimeout, "Gateway Timeout");
		HTTP_STATUS_RETURN(HttpStatusCode::HTTPVersionNotSupported, "HTTP Version Not Supported");
	}

	return "Unknown";
}

class HttpServer;
class HttpConnection;

using HttpRequestHandler = std::function<HttpNextHandler(const std::shared_ptr<HttpConnection> &client)>;
using HttpRequestErrorHandler = std::function<void(const std::shared_ptr<HttpConnection> &client)>;

using HttpResponseWriteHandler = std::function<bool(const std::shared_ptr<ov::Data> &data)>;