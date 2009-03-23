#ifndef _Web_httpcli_h_
#define _Web_httpcli_h_

class HttpClient
{
public:
	HttpClient();
	HttpClient(const char *url);

	HttpClient&  TimeoutMsecs(int t)              { timeout_msecs = t; return *this; }
	HttpClient&  MaxHeaderSize(int m)             { max_header_size = m; return *this; }
	HttpClient&  MaxContentSize(int m)            { max_content_size = m; return *this; }

	HttpClient&  Host(String h)                   { host = h; return *this; }
	HttpClient&  Port(int p)                      { port = p; return *this; }
	HttpClient&  Path(String p)                   { path = p; return *this; }
	HttpClient&  User(String u, String p)         { username = u; password = p; return *this; }
	HttpClient&  URL(const char *url);
	HttpClient&  KeepAlive(bool k)                { keepalive = k; return *this; }
	HttpClient&  Proxy(String h, int p)           { proxy_host = h; proxy_port = p; return *this; }
	HttpClient&  Proxy(const char *url);
	HttpClient&  ProxyAuth(String u, String p)    { proxy_username = u; proxy_password = p; return *this; }

	HttpClient&  Headers(String h)                { client_headers = h; return *this; }
	HttpClient&  ClearHeaders()                   { return Headers(Null); }
	HttpClient&  AddHeaders(String h)             { client_headers.Cat(h); return *this; }

	HttpClient&  StdHeaders(bool sh)              { std_headers = sh; return *this; }
	HttpClient&  NoStdHeaders()                   { return StdHeaders(false); }
	HttpClient&  Accept(String a)                 { accept = a; return *this; }
	HttpClient&  Agent(String a)                  { agent = a; return *this; }
	HttpClient&  ContentType(String a)            { contenttype = a; return *this; }

	HttpClient&  Method(int m)                    { method = m; return *this; }
	HttpClient&  Get()                            { return Method(METHOD_GET); }
	HttpClient&  Post()                           { return Method(METHOD_POST); }
	HttpClient&  Head()                           { return Method(METHOD_HEAD); }

	HttpClient&  PostData(String pd)              { postdata = pd; return *this; }
	HttpClient&  PostUData(String pd)             { return PostData(UrlEncode(pd)); }
	HttpClient&  Post(const String& data)         { Post(); return PostData(data); }
	HttpClient&  PostU(const String& data)        { Post(); return PostUData(data); }
	HttpClient&  PostU(const char *key, const String& data);

	String       Execute(Gate2<int, int> progress = false);
	String       ExecuteRedirect(int max_redirect = DEFAULT_MAX_REDIRECT,
		int retries = DEFAULT_RETRIES, Gate2<int, int> progress = false);

	bool         IsError() const                  { return !IsNull(error); }
	String       GetError() const                 { return error; }
	bool         IsAborted() const                { return aborted; }

	int          GetStatusCode() const            { return status_code; }
	String       GetStatusLine() const            { return status_line; }
	String       GetHeaders() const               { return server_headers; }

	bool         IsRedirect() const               { return is_redirect; }
	String       GetRedirectURL() const           { return redirect_url; }

	void         Close()                          { socket.Close(); }

	static void  Trace(bool b = true);
	
public:
	Socket       socket;
	bool         keepalive;
	bool         aborted;
	String       error;

	int          timeout_msecs;
	int          max_header_size;
	int          max_content_size;

	String       host;
	int          port;
	bool         is_post;
	bool         std_headers;
	int          method;
	String       proxy_host;
	int          proxy_port;
	String       proxy_username;
	String       proxy_password;
	String       path;
	String       username;
	String       password;
	String       client_headers;
	String       accept;
	String       agent;
	String       contenttype;
	String       postdata;

	int          status_code;
	String       status_line;
	String       server_headers;

	bool         is_redirect;
	String       redirect_url;

	enum {
		DEFAULT_PORT             = 80,
		DEFAULT_TIMEOUT_MSECS    = 120000,
		DEFAULT_MAX_HEADER_SIZE  = 1000000,
		DEFAULT_MAX_CONTENT_SIZE = 10000000,
		DEFAULT_MAX_REDIRECT     = 5,
		DEFAULT_RETRIES          = 3,
	};

	enum {
		METHOD_GET,
		METHOD_POST,
		METHOD_HEAD,
	};

private:
	void         Init();
	String       ReadUntilProgress(char until, int start_time, int end_time, Gate2<int, int> progress);
};

String HttpClientGet(String url, String *server_headers = NULL, String *error = NULL,
	Gate2<int, int> progress = false, int timeout = HttpClient::DEFAULT_TIMEOUT_MSECS,
	int max_redirect = HttpClient::DEFAULT_MAX_REDIRECT, int retries = HttpClient::DEFAULT_RETRIES);

String HttpClientGet(String url, String username, String password,
	String *server_headers = NULL, String *error = NULL,
	Gate2<int, int> progress = false, int timeout = HttpClient::DEFAULT_TIMEOUT_MSECS,
	int max_redirect = HttpClient::DEFAULT_MAX_REDIRECT, int retries = HttpClient::DEFAULT_RETRIES);

String HttpClientGet(String url, String proxy,
	String *server_headers = NULL, String *error = NULL,
	Gate2<int, int> progress = false, int timeout = HttpClient::DEFAULT_TIMEOUT_MSECS,
	int max_redirect = HttpClient::DEFAULT_MAX_REDIRECT, int retries = HttpClient::DEFAULT_RETRIES);

String HttpClientGet(String url, String proxy, String username, String password,
	String *server_headers = NULL, String *error = NULL,
	Gate2<int, int> progress = false, int timeout = HttpClient::DEFAULT_TIMEOUT_MSECS,
	int max_redirect = HttpClient::DEFAULT_MAX_REDIRECT, int retries = HttpClient::DEFAULT_RETRIES);

#endif
