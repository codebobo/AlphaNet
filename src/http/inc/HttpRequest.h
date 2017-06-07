
#ifndef HTTP_HTTPREQUEST_H
#define HTTP_HTTPREQUEST_H

#include <map>
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include "TrantorTimestamp.h"
#include "StringBuffer.h"
#include "Log.h"
using std::string;

class HttpRequest
{
public:
    friend class HttpContext;
    enum Method {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version {
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
            : method_(kInvalid),
              version_(kUnknown)
    {
    }

    void setVersion(Version v)
    {
        version_ = v;
    }

    Version getVersion() const
    {
        return version_;
    }
    void parsePremeter();
    bool setMethod(const char* start, const char* end)
    {

        assert(method_ == kInvalid);
        std::string m(start, end);
        if (m == "GET") {
            method_ = kGet;
        } else if (m == "POST") {
            method_ = kPost;
        } else if (m == "HEAD") {
            method_ = kHead;
        } else if (m == "PUT") {
            method_ = kPut;
        } else if (m == "DELETE") {
            method_ = kDelete;
        } else {
            method_ = kInvalid;
        }
        if(method_!=kInvalid)
        {
            content_="";
            query_="";
            cookies_.clear();
            premeter_.clear();
            headers_.clear();
        }
        return method_ != kInvalid;
    }

    bool setMethod(const Method method)
    {
        method_ = method;
        content_="";
        query_="";
        cookies_.clear();
        premeter_.clear();
        headers_.clear();
        return true;
    }

    Method method() const
    {
        return method_;
    }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch(method_) {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    void setPath(const char* start, const char* end)
    {
        path_.assign(start, end);
    }
    void setPath(const std::string& path)
    {
        path_ = path;
    }

    std::map<std::string,std::string > getPremeter() const
    {
        return premeter_;
    }
    const std::string& path() const
    {
        return path_;
    }

    void setQuery(const char* start, const char* end)
    {
        query_.assign(start, end);
    }
    void setQuery(const std::string& query)
    {
        query_ = query;
    }
//            const string& content() const
//            {
//                return content_;
//            }
    const std::string& query() const
    {
        if(query_!="")
            return query_;
        if(method_==kPost)
            return content_;
        return query_;
    }

    void setReceiveTime(TrantorTimestamp t)
    {
        receiveTime_ = t;
    }

    TrantorTimestamp receiveTime() const
    {
        return receiveTime_;
    }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon)) {
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && isspace(value[value.size() - 1])) {
            value.resize(value.size() - 1);
        }
        headers_[field] = value;
        transform(field.begin(), field.end(), field.begin(), ::tolower);
        if(field == "cookie") {
            LOG4CPLUS_DEBUG(_logger, "cookies!!!:"<<value);
            std::string::size_type pos;
            while((pos = value.find(";")) != std::string::npos) {
                std::string coo = value.substr(0, pos);
                auto epos = coo.find("=");
                if(epos != std::string::npos) {
                    std::string cookie_name = coo.substr(0, epos);
                    std::string::size_type cpos=0;
                    while(isspace(cookie_name[cpos])&&cpos<cookie_name.length())
                        cpos++;
                    cookie_name=cookie_name.substr(cpos);
                    std::string cookie_value = coo.substr(epos + 1);
                    cookies_[cookie_name] = cookie_value;
                }
                value=value.substr(pos+1);
            }
            if(value.length()>0)
            {
                std::string &coo = value;
                auto epos = coo.find("=");
                if(epos != std::string::npos) {
                    std::string cookie_name = coo.substr(0, epos);
                    std::string::size_type cpos=0;
                    while(isspace(cookie_name[cpos])&&cpos<cookie_name.length())
                        cpos++;
                    cookie_name=cookie_name.substr(cpos);
                    std::string cookie_value = coo.substr(epos + 1);
                    cookies_[cookie_name] = cookie_value;
                }
            }
        }
    }


    std::string getHeader(const std::string& field) const
    {
        std::string result;
        std::map<std::string, std::string>::const_iterator it = headers_.find(field);
        if (it != headers_.end()) {
            result = it->second;
        }
        return result;
    }

    std::string getCookie(const std::string& field) const
    {
        std::string result;
        std::map<std::string, std::string>::const_iterator it = cookies_.find(field);
        if (it != cookies_.end()) {
            result = it->second;
        }
        return result;
    }
    const std::map<std::string, std::string>& headers() const
    {
        return headers_;
    }

    const std::map<std::string, std::string>& cookies() const
    {
        return cookies_;
    }

    const std::string& getContent() const
    {
        return content_;
    }
    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        receiveTime_.swap(that.receiveTime_);
        headers_.swap(that.headers_);
    }

    void setContent(const std::string& content)
    {
        content_ = content;
    }
    void addHeader(const std::string& key, const std::string& value)
    {
        headers_[key] = value;
    }
    void addCookie(const std::string& key, const std::string& value)
    {
        cookies_[key] = value;
    }

    void appendToBuffer(StringBuffer* output) const;
private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;

    TrantorTimestamp receiveTime_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> cookies_;
    std::map<std::string, std::string> premeter_;
protected:
    std::string content_;
    size_t contentLen;

};




#endif  // MUDUO_NET_HTTP_HTTPREQUEST_H
