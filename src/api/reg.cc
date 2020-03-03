#include <string>
#include "reg.h"

using namespace v8;

struct RegBaton : Baton {
  // arguments
  String::Utf8Value srvURL;
  String::Utf8Value attrs;
  unsigned short usLifetime;
  // result = slp_error

  RegBaton(const Arguments& args) : Baton(), srvURL(args[0]), attrs(args[2]), usLifetime(args[1]->Uint32Value()) {
    HandleScope scope;
    callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));
  }

  static void work(uv_work_t* work_req) {
    RegBaton* baton = static_cast<RegBaton*>(work_req->data);
    baton->slp_error = SLPReg(baton->slp_handle, *baton->srvURL, baton->usLifetime, NULL,
      *baton->attrs, SLP_TRUE, &RegBaton::callbackSLP, baton);
  }

  static void SLPCALLBACK callbackSLP(SLPHandle hSLP, SLPError errCode, void *pvCookie) {
    RegBaton* baton = static_cast<RegBaton*>(pvCookie);
    baton->slp_error = errCode;
  }

  static void afterWork(uv_work_t* work_req) {
    HandleScope scope;
    Local<Value> argv[1];
    RegBaton* baton = static_cast<RegBaton*>(work_req->data);
    if (baton->slp_error < SLP_OK) {
      argv[0] = Exception::Error(String::New(slp_error_message(baton->slp_error)));
    } else {
      argv[0] = Local<Value>::New(Null());
    }
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
    baton->callback.Dispose();
    delete baton;
  }
};

Local<Value> Reg(const Arguments& args) {
  homerun(new RegBaton(args));
  return Undefined();
}
