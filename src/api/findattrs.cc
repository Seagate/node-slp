#include <string>
#include <set>
#include "findattrs.h"

using namespace v8;

struct FindAttrsBaton : Baton {
  // arguments
  Nan::Utf8String urlOrServiceType;
  Nan::Utf8String scopeList;
  Nan::Utf8String interfaceList;
  Nan::Utf8String attrIds;
  // result
  std::vector< std::string > result;

  FindAttrsBaton(const Nan::FunctionCallbackInfo<v8::Value>& info) : Baton(), urlOrServiceType(info[0]), interfaceList(info[1]), scopeList(info[2]), attrIds(info[3]) {
    callback = new Nan::Callback(Nan::To<Function>(info[4]).ToLocalChecked());
  }

  static void work(uv_work_t* work_req) {
    FindAttrsBaton* baton = static_cast<FindAttrsBaton*>(work_req->data);
    baton->slp_error = SLPAssociateIFList(baton->slp_handle, *baton->interfaceList);
    if (baton->slp_error != SLP_OK)
    	return;
    baton->slp_error = SLPFindAttrs(baton->slp_handle, *baton->urlOrServiceType, *baton->scopeList, *baton->attrIds,
      &FindAttrsBaton::callbackSLP, baton);
  }

  static SLPBoolean SLPCALLBACK callbackSLP(SLPHandle hSLP, const char* pcAttrList, SLPError errCode, void *pvCookie) {
    FindAttrsBaton* baton = static_cast<FindAttrsBaton*>(pvCookie);
    baton->slp_error = errCode;
    if (errCode == SLP_OK || errCode == SLP_LAST_CALL) {
        if (pcAttrList != NULL) {
            baton->result.push_back(std::string(pcAttrList));
        }
        return SLP_TRUE;
    } else {
        baton->slp_error = errCode;
        return SLP_FALSE;
    }
  }

  // remove SLP-specific escape codes from the source string
  static std::string unescape(const std::string& src) {
    char hex[3];
    std::string result;
    for (auto p = src.begin(); p < src.end(); p++) {
      if (*p == '\\' && (src.end() - p) >= 3) {
        hex[0] = *++p;
        hex[1] = *++p;
        hex[2] = 0;
        result.push_back((char)strtoul(hex, NULL, 16));
      }
      else
        result.push_back(*p);
    }
    return result;
  }

  // parse an SLP attribute string into a key value pair
  static int extract_one_attr(const std::string& src, std::string& attr, std::string& value) {
    std::string::size_type a = 0, b = 0, c = 0, src_len = src.size();

    // set a to point to first character of attribute name
    while (a < src_len && src[a] == ',')
      ++a;

    // empty string => done parsing
    if (a >= src_len)
      return 0;

    // if no parens, entire string is an attribute
    if (src[a] != '(') {
      attr = src.substr(a, (src_len-a));
      return src_len;
    }

    // skip open paren
    while (a < src_len && src[a] == '(')
      ++a;

    // make sure there's a string to parse
    if (a >= src_len)
      return 0;

    // find next delimiter, "=" or close paren
    b = src.find_first_of("=)");
    if (b == std::string::npos)
      return 0;

    // extract the attribute name and if there's
    // no "=" after it, return it with empty value
    attr = src.substr(a, b-a);
    if (src[b] == ')') {
      value = "";
      return b+1;
    }

    // otherwise, look for end of attribute value
    c = src.find(')', b);
    if (c == std::string::npos)
      return 0;

    // and return it
    value = src.substr(b+1, c-(b+1));

    // if value surrounded by double-quotes, remove them
    if (value[0] == '"' && value[value.size()-1] == '"')
        value = value.substr(1, value.size()-2);

    if (value.find('\\') != std::string::npos)
      value = unescape(value);

    // return the number of characters we processed
    return c+1;
  }

  static void parse_attr_list(const std::string& attrlist, Local<Object>& obj) {
    std::string attr, value;
    int cursor = 0, advance = 0;

    do {
      advance = extract_one_attr(attrlist.substr(cursor), attr, value);
      if (advance <= 0)
        break;

      // Add attr-value pair to object
      Nan::Set(obj, Nan::New<String>(attr.c_str()).ToLocalChecked(), Nan::New<String>(value.c_str()).ToLocalChecked());

      cursor += advance;
    } while (advance > 0);

    return;
  }

  static void afterWork(uv_work_t* work_req, int status) {
    Nan::HandleScope scope;
    FindAttrsBaton* baton = static_cast<FindAttrsBaton*>(work_req->data);
    if (baton->slp_error < SLP_OK) {
      Local<Value> argv[1] = { Exception::Error(Nan::New<String>(slp_error_message(baton->slp_error)).ToLocalChecked()) };
      baton->callback->Call(1, argv);
    } else {
      Local<Array> ary = Nan::New<Array>();
      std::set<std::string> unique_responses;
      for (unsigned int i = 0; i < baton->result.size(); i++) {
        unique_responses.emplace(baton->result[i].c_str());
      }

      unsigned int i = 0;
      for (auto it=unique_responses.begin(); it!=unique_responses.end(); ++it) {
        Local<Object> obj = Nan::New<Object>();
        parse_attr_list(*it, obj);
        Nan::Set(ary, i++, obj);
      }

      Local<Value> argv[2] = { Nan::Null(), ary };
      baton->callback->Call(2, argv);
    }
    delete baton->callback;
    delete baton;
  }
};

void FindAttrs(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  homerun(new FindAttrsBaton(info));
}
