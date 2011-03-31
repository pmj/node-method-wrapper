#include "../../src/node-method-wrap.hpp"
#include <cstdio>
#include <string>

using namespace node;
using namespace v8;
using std::string;

class Simple : public ObjectWrap
{
public:
	// The (JS) constructor, which will be exported
  static Persistent<FunctionTemplate> s_ct;
	
  static void Init(Handle<Object> target);
	
	string name;

  Simple(const char* name) : name(name)
  {
		printf("Simple() constructor\n");
  }

  ~Simple()
  {
		printf("~Simple() destructor\n");
  }

  static Handle<Value> New(const Arguments& args);
	
	// The remaining methods are passed through to JavaScript, with implicit boxing/unboxing!
	// We can return a std::string and it's turned into a v8::String...
	string NoArgTest()
	{
		printf("Simple::NoArgTest()\n");
		return name;
	}
	
	// arithmetic types become v8::Number and vice versa
	double NumberTest(float a, long b)
	{
		return a + b;
	}
	
	// In addition to std::string, C-style strings are fine, too. We can also use the V8 types directly, though.
	string StringTest(const char* a, const string& b, Local<String> c)
	{
		string concat = name;
		concat += a;
		concat += b;
		String::Utf8Value c_utf8(c);
		concat += *c_utf8;
		return concat;
	}
	
	// Returning V8 types is fine, too. Note that the arguments are type checked!
	Handle<Object> ObjectTest(Local<String> str, Local<Number> num)
	{
		Local<Object> obj = Object::New();
		obj->Set(str, num);
		return obj;
	}

};

Handle<Value> Simple::New(const Arguments& args)
{
	printf("Simple::New()\n");
	// TODO: this still needs boilerplate code which could easily be wrapped
	HandleScope scope;
	
	if (args.Length() <= 0 || !args[0]->IsString())
	{
		return ThrowException(Exception::TypeError(String::New("Argument to Simple constructor must be a string")));
	}
	String::Utf8Value str(Local<String>::Cast(args[0]));
	
	Simple* me = new Simple(*str);
	me->Wrap(args.This());
	return args.This();
}


void Simple::Init(Handle<Object> target)
{
	printf("Simple::Init()\n");
	HandleScope scope;

	Local<FunctionTemplate> t = FunctionTemplate::New(New);

	s_ct = Persistent<FunctionTemplate>::New(t);
	s_ct->InstanceTemplate()->SetInternalFieldCount(1);
	s_ct->SetClassName(String::NewSymbol("Simple"));

	NODE_SET_WRAPPED_PROTOTYPE_METHOD(s_ct, "noArgTest", Simple::NoArgTest);
	NODE_SET_WRAPPED_PROTOTYPE_METHOD(s_ct, "numberTest", Simple::NumberTest);
	NODE_SET_WRAPPED_PROTOTYPE_METHOD(s_ct, "stringTest", Simple::StringTest);
	NODE_SET_WRAPPED_PROTOTYPE_METHOD(s_ct, "objectTest", Simple::ObjectTest);

	target->Set(String::NewSymbol("Simple"), s_ct->GetFunction());
}


Persistent<FunctionTemplate> Simple::s_ct;

extern "C" {
  static void init (Handle<Object> target)
  {
		printf("init()\n");
    Simple::Init(target);
  }

  NODE_MODULE(simple, init);
}
