# Node.js module C++ method wrapper

When writing Node.js modules in C++, it's nice to let C++ class instances
correspond to JS objects directly, and being able to call C++ class methods from
JavaScript. Boxing and unboxing V8's "arguments" object, along with type
checking, etc. is pretty tedious, however, and leads to a lot of boilerplate
code.

This library is an attempt to do all the checking and boxing/unboxing
automatically. The samples/simple module shows some more supported types, but
here's an excerpt to wet your appetite. This is literally all you need to export
the C++ method StringTest() as a "stringTest" method on each "Simple" object:

	#include "node-method-wrap.hpp"
	
	class Simple : public ObjectWrap
	{
	  // ...
	  std::string name;
	  // ...
	  std::string StringTest(const char* a, const std::string& b, v8::Local<v8::String> c)
	  {
	    std::string concat = name;
	    concat += a;
	    concat += b;
	    v8::String::Utf8Value c_utf8(c);
	    concat += *c_utf8;
	    return concat;
	  }
	  // ...
	  // In init code:
	  NODE_SET_WRAPPED_PROTOTYPE_METHOD(s_ct /* JS constructor function */, "stringTest", Simple::StringTest);
	  // ...
	};

## Limitations/To Do

In order to do the type conversions and checking, some fairly heavy metaprogramming
is required in the library itself. To keep this to a minimum, the Boost type
traits library is used (headers only). Moreover, for the
`NODE_SET_WRAPPED_PROTOTYPE_METHOD` macro to work, the `typeof` operator is
required (so GCC and compatible only), though it's possible to work around this,
if necessary. I'm unaware of anyone using anything other than GCC to build
node.js modules, though.

At the moment, only member functions are supported, and only those with 0, 1, 2
or 3 arguments. Adding support for more arguments is trivial, though, and support
for static functions should be easy, too. Only arithmetic types, C++ strings and
C strings are currently auto-boxed/unboxed, and `void` return types don't work yet.
Overloading, optional arguments, varargs and `null` don't work yet, either, but
seem doable if needed.

I'm also planning to implement an auto-async-mechanism for wrapping functions
in threads and invoking a callback on completion.

## License

MIT License.

## Acknowledgments

[Cloudkick's node.js module writing guide](https://www.cloudkick.com/blog/2010/aug/23/writing-nodejs-native-extensions/) helped me get up to speed with node.js's C++ bindings.
