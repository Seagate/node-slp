#include <utility>
#include <string>
#include <vector>
#include "findsrvs.h"

using namespace v8;

struct FindSrvsBaton : Baton {
  // arguments
  Nan::Utf8String serviceType;
  Nan::Utf8String scopeList;
  Nan::Utf8String searchFilter;
  // result array
  std::vector< std::pair<std::string, unsigned short> > result;

  FindSrvsBaton(const Nan::FunctionCallbackInfo<v8::Value>& info) : Baton(), serviceType(info[0]), scopeList(info[1]), searchFilter(info[2]) {
    callback = new Nan::Callback(Nan::To<Function>(info[3]).ToLocalChecked());
  }

  static void work(uv_work_t* work_req) {
    FindSrvsBaton* baton = static_cast<FindSrvsBaton*>(work_req->data);
    baton->slp_error = SLPFindSrvs(baton->slp_handle, *baton->serviceType, *baton->scopeList, *baton->searchFilter,
      &FindSrvsBaton::callbackSLP, baton);
  }

  static SLPBoolean SLPCALLBACK callbackSLP(SLPHandle hSLP, const char* pcSrvURL, unsigned short sLifetime, SLPError errCode, void *pvCookie) {
    FindSrvsBaton* baton = static_cast<FindSrvsBaton*>(pvCookie);
    if (errCode == SLP_OK || errCode == SLP_LAST_CALL) {
      if (pcSrvURL != NULL)
        baton->result.push_back(std::make_pair(std::string(pcSrvURL), sLifetime));
      return SLP_TRUE;
    } else {
      baton->slp_error = errCode;
      return SLP_FALSE;
    }
  }

  static void afterWork(uv_work_t* work_req, int status) {
    Nan::HandleScope scope;
    FindSrvsBaton* baton = static_cast<FindSrvsBaton*>(work_req->data);
    if (baton->slp_error < SLP_OK) {
      Local<Value> argv[1] = { Exception::Error(Nan::New<String>(slp_error_message(baton->slp_error)).ToLocalChecked()) };
      baton->callback->Call(1, argv);
    } else {
      Local<Array> arr = Nan::New<Array>(); // [ { url: "", lifetime: 0 }, ... ]
      for (unsigned int i = 0; i < baton->result.size(); i++) {
        Local<Object> obj = Nan::New<Object>();
        Nan::Set(obj, Nan::New<String>("url").ToLocalChecked(), Nan::New<String>(baton->result[i].first.c_str()).ToLocalChecked());
        Nan::Set(obj, Nan::New<String>("lifetime").ToLocalChecked(), Nan::New<Integer>(baton->result[i].second));
        Nan::Set(arr, i, obj);
      }
      Local<Value> argv[2] = { Nan::Null(), arr };
      baton->callback->Call(2, argv);
    }

    delete baton->callback;
    delete baton;
  }
};

void FindSrvs(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  homerun(new FindSrvsBaton(info));
}
