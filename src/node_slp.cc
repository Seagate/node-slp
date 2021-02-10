#include <stdlib.h> // atexit
#include "node_slp.h"
#include "api/baton.h"
#include "api/dereg.h"
#include "api/delattrs.h"
#include "api/findattrs.h"
#include "api/findsrvs.h"
#include "api/findsrvtypes.h"
#include "api/reg.h"

void Version(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(Nan::New("1.2.1").ToLocalChecked());
}

void GetRefreshInterval(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(Nan::New(SLPGetRefreshInterval()));
}

void GetMaxLifetime(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(Nan::New(SLP_LIFETIME_MAXIMUM));
}

/*
Local<Value> ParseSrvURL(const Arguments& args) {
  HandleScope scope;
  String::Utf8Value srvURL(args[0]);
  SLPSrvURL* pSrvURL;
  SLPError ret = SLPParseSrvURL(*srvURL, &pSrvURL);
  if (ret != SLP_OK) {
    SLPFree(pSrvURL);
    return ThrowException(Exception::Error(String::New(slp_error_message(ret))));
  }
  // build and return an object
  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("type"), String::New(pSrvURL->s_pcSrvType));
  obj->Set(String::NewSymbol("host"), String::New(pSrvURL->s_pcHost));
  obj->Set(String::NewSymbol("port"), Integer::New(pSrvURL->s_iPort));
  obj->Set(String::NewSymbol("netFamily"), String::New(pSrvURL->s_pcNetFamily));
  obj->Set(String::NewSymbol("part"), String::New(pSrvURL->s_pcSrvPart));
  SLPFree(pSrvURL);
  return scope.Close(obj);
}

Local<Value> Escape(const Arguments& args) {
  HandleScope scope;
  String::Utf8Value inbuf(args[0]);
  char* outBuf;
  SLPBoolean isTag = (args.Length() > 1 && args[1]->BooleanValue()) ? SLP_TRUE : SLP_FALSE;
  SLPError ret = SLPEscape(*inbuf, &outBuf, isTag);
  Local<String> result = String::New(outBuf);
  SLPFree(outBuf);
  if (ret == SLP_OK)
    return scope.Close(result);
  else
    return ThrowException(Exception::Error(String::New(slp_error_message(ret))));
}

Local<Value> Unescape(const Arguments& args) {
  HandleScope scope;
  String::Utf8Value inbuf(args[0]);
  char* outBuf;
  SLPError ret = SLPUnescape(*inbuf, &outBuf, args[1]->BooleanValue() ? SLP_TRUE : SLP_FALSE);
  Local<String> result = String::New(outBuf);
  SLPFree(outBuf);
  if (ret == SLP_OK)
    return scope.Close(result);
  else
    return ThrowException(Exception::Error(String::New(slp_error_message(ret))));
}

Local<Value> GetSetProperty(const Arguments& args) {
  HandleScope scope;
  String::Utf8Value name(args[0]);
  if (args.Length() > 1) {
    String::Utf8Value value(args[1]);
    SLPSetProperty(*name, *value);
    return Undefined();
  } else {
    const char* result = SLPGetProperty(*name);
    return (result == NULL) ? Null() : scope.Close(String::New(result));
  }
}

Local<Value> FindScopes(const Arguments& args) {
  HandleScope scope;
  char* scopeList;
  SLPHandle handle = acquire_handle();
  SLPFindScopes(handle, &scopeList);
  release_handle(handle);
  Local<String> result = String::New(scopeList);
  SLPFree(scopeList);
  return scope.Close(result);
}
*/

/*
extern "C" void init(Handle<Object> target) {
  // properties
  target->SetAccessor(String::NewSymbol("version"), Version);
  target->SetAccessor(String::NewSymbol("refreshInterval"), GetRefreshInterval);
  target->SetAccessor(String::NewSymbol("MAX_LIFETIME"), GetMaxLifetime);

  // methods
  NODE_SET_METHOD(target, "parseSrvUrl", ParseSrvURL);
  NODE_SET_METHOD(target, "escape", Escape);
  NODE_SET_METHOD(target, "unescape", Unescape);
  NODE_SET_METHOD(target, "property", GetSetProperty);
  NODE_SET_METHOD(target, "findScopes", FindScopes);
  NODE_SET_METHOD(target, "findSrvs", FindSrvs);
  NODE_SET_METHOD(target, "findSrvTypes", FindSrvTypes);
  NODE_SET_METHOD(target, "findAttrs", FindAttrs);
  NODE_SET_METHOD(target, "reg", Reg);
  NODE_SET_METHOD(target, "dereg", Dereg);
  NODE_SET_METHOD(target, "delAttrs", DelAttrs);

  // cleanup
  atexit(clear_handles);
}
*/

void Init(v8::Local<v8::Object> exports) {
  v8::Local<v8::Context> context = exports->CreationContext();
  exports->Set(context,
               Nan::New("version").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Version)
                   ->GetFunction(context)
                   .ToLocalChecked());
  exports->Set(context,
               Nan::New("refreshInterval").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(GetRefreshInterval)
                   ->GetFunction(context)
                   .ToLocalChecked());
  exports->Set(context,
               Nan::New("MAX_LIFETIME").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(GetMaxLifetime)
                   ->GetFunction(context)
                   .ToLocalChecked());
  exports->Set(context,
               Nan::New("findSrvs").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(FindSrvs)
                   ->GetFunction(context)
                   .ToLocalChecked());
  exports->Set(context,
               Nan::New("findAttrs").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(FindAttrs)
                   ->GetFunction(context)
                   .ToLocalChecked());
}

NODE_MODULE(slp, Init)
