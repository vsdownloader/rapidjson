#include "rapidjson/document.h"
using namespace rapidjson;

#include <iostream>
#include <string>

struct RapidException
{
    RapidException(const std::string& func, const std::string& key, const std::string& reason) : _func(func), _key(key), _reason(reason) {}
    std::string ToString(){ return _func + " : " + _key + " " + _reason; }
    std::string _func, _key, _reason;
};

#define TRE(reason)             throw RapidException(__FUNCTION__, key, reason)
#define HAS_CHECK               if(op.IsNull() || !op.IsObject() || !op.HasMember(key)) { TRE(" not exist");} auto& val = op[key]
#define TYPE_CHECK              TRE(" type error")
#define TYPE_CHECK_EX(type)     if(!val.Is##type()) TYPE_CHECK

#define CK(type) HAS_CHECK; TYPE_CHECK_EX(type)

Value& GetObject(Value& op, const char* key) { CK(Object); return val; }
Value& GetArray(Value& op, const char* key) { CK(Array); return val; }
int GetInt(Value& op, const char* key) { CK(Int); return val.GetInt(); }

std::string GetString(Value& op, const char* key)
{
    CK(String);

    std::string s8;
    auto p = val.GetString();
    auto nLen = val.GetStringLength();
    if (p && nLen > 0)
    {
        s8.assign(p, nLen);
    }
    return s8;
}

//Utils
#define GOBJ(key) GetObject(op, #key)
#define GARR(key) GetArray(op, #key)
#define GSTR(key) GetString(op, #key)
#define GINT(key) GetInt(op, #key)

int main(int argc, char* argv)
{
    std::string s = R"({"video":{"id":"abc","title":"体验课","duration":503,"images":[{"index":0,"url":"img/0.jpg"},{"index":1},{"index":2,"url":"img/2.jpg"},{"index":3,"url":"img/3.jpg"}]}})";

    std::string errMsg;
    try
    {
        Document op;
        op.Parse(s.c_str());
        if (op.HasParseError())
        {
            errMsg = "Json Parse Error";
        }
        else
        {
            Value& val = GOBJ(video);
            {
                auto& op = val;
                auto id = GSTR(id);
                auto title = GSTR(title);
                auto duration = GINT(duration);

                Value& arr = GARR(images);
                {
                    for (SizeType i = 0; i < arr.Size(); i++)
                    {
                        Value& op = arr[i];
                        auto index = GINT(index);
                        auto url = GSTR(url);
                    }
                }
            }
        }
    }
    catch (RapidException& re) {
        errMsg = re.ToString();
    }
    catch (const std::exception& e) {
        errMsg = e.what();
    }
    catch (...) {
        errMsg = "Unknown error";
    }
    std::cout << errMsg << std::endl;
    //actual errMsg = "GetString : url not exist"
    //english : expect errMsg = "GetString : R.video.images[1].url not exist" , the R symbol just represent the root element，R.video.images[1].url is the path from root to the url node
    //chinese ：期望errMsg = "GetString : R.video.images[1].url not exist" , R代表根，只是一个表示符号，也可以用其它的表示，问题就是如何自动得到某一节点的NRP
    return 0;
}
