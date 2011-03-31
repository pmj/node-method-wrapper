/*
 *  v8fnwrap.hpp
 *
 *  Created by Phillip Jordan on 31/03/2011.
 *  Copyright 2011 Phillip Jordan. All rights reserved.
 *
 */
 
#ifndef V8FNWRAP_HPP
#define V8FNWRAP_HPP

#include <v8.h>
#include <node.h>
#include <boost/type_traits.hpp>
#include <string>

template <typename T, bool arithmetic, bool string, bool v8_type> struct v8_conversion_selector;

// C++ <-> JS numeric type conversion
template <typename T> struct v8_conversion_selector<T, true, false, false>
{
	typedef v8::Number v8_type;
	typedef T temp_type;
	
	static v8::Local<v8::Number> convert_to_v8(T val)
	{
		return v8::Number::New(val);
	}
		
	static T convert_to(v8::Local<v8::Value> val, bool& thrown, v8::Handle<v8::Value>& exc)
	{
		if (!val->IsNumber())
		{
			exc = v8::ThrowException(v8::Exception::TypeError(v8::String::New("Number expected")));
			thrown = true;
			return 0;
		}
		thrown = false;
		return v8::Local<v8::Number>::Cast(val)->Value();
	}
	static T from_temp(temp_type tmp) { return tmp; }
};

// C++ <-> JS string conversion
template <> struct v8_conversion_selector<char*, false, true, false>
{
	typedef v8::String v8_type;
	typedef v8::String::Utf8Value temp_type;

	static v8::Local<v8::String> convert_to_v8(char* val)
	{
		return v8::String::New(val);
	}
		
	static v8::Handle<v8::String> convert_to(v8::Local<v8::Value> val, bool& thrown, v8::Handle<v8::Value>& exc)
	{
		if (!val->IsString())
		{
			exc = v8::ThrowException(v8::Exception::TypeError(v8::String::New("String expected")));
			thrown = true;
			return v8::Handle<v8::String>();
		}
		thrown = false;
		return (v8::Local<v8::String>::Cast(val));
	}
	
	static char* from_temp(temp_type& utf8) { return *utf8; }
};
template <> struct v8_conversion_selector<std::string, false, true, false>
{
	typedef v8::String v8_type;
	typedef v8::String::Utf8Value temp_type;

	static v8::Local<v8::String> convert_to_v8(const std::string& val)
	{
		return v8::String::New(val.c_str());
	}
		
	static v8::Handle<v8::String> convert_to(v8::Local<v8::Value> val, bool& thrown, v8::Handle<v8::Value>& exc)
	{
		if (!val->IsString())
		{
			exc = v8::ThrowException(v8::Exception::TypeError(v8::String::New("String expected")));
			thrown = true;
			return v8::Handle<v8::String>();
		}
		thrown = false;
		return (v8::Local<v8::String>::Cast(val));
	}
	
	static std::string from_temp(const temp_type& utf8) { return std::string(*utf8, utf8.length()); }
};

// Pass through V8 types, type checking if necessary
template <typename T> struct v8_type_check
{
	static bool check(v8::Local<v8::Value> obj) { return true; }
};

template <> struct v8_type_check<v8::Boolean>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsBoolean(); }
};
template <> struct v8_type_check<v8::Function>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsFunction(); }
};
template <> struct v8_type_check<v8::Object>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsObject(); }
};
template <> struct v8_type_check<v8::Number>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsNumber(); }
};
template <> struct v8_type_check<v8::String>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsString(); }
};
template <> struct v8_type_check<v8::Array>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsArray(); }
};
template <> struct v8_type_check<v8::Date>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsDate(); }
};
template <> struct v8_type_check<v8::RegExp>
{
	static bool check(v8::Local<v8::Value> obj) { return obj->IsRegExp(); }
};

template <typename T> struct v8_conversion_selector<v8::Local<T>, false, false, true>
{
	typedef T v8_type;
	typedef v8::Local<T> temp_type;
	
	static temp_type convert_to_v8(temp_type val)
	{
		return val;
	}
		
	static temp_type convert_to(v8::Local<v8::Value> val, bool& thrown, v8::Handle<v8::Value>& exc)
	{
		if (!v8_type_check<T>::check(val))
		{
			exc = v8::ThrowException(v8::Exception::TypeError(v8::String::New("Incorrect argument type")));
			thrown = true;
			return temp_type();
		}
		temp_type tmp = v8::Local<T>::Cast(val);
		thrown = false;
		return tmp;
	}
	static temp_type from_temp(temp_type tmp) { return tmp; }
};
template <typename T> struct v8_conversion_selector<v8::Handle<T>, false, false, true>
{
	typedef T v8_type;
	typedef v8::Local<T> temp_type;
	
	static v8::Handle<T> convert_to_v8(v8::Handle<T> val)
	{
		return val;
	}
		
	static v8::Handle<T> convert_to(v8::Local<v8::Value> val, bool& thrown, v8::Handle<v8::Value>& exc)
	{
		if (!v8_type_check<T>::check(val))
		{
			exc = v8::ThrowException(v8::Exception::TypeError(v8::String::New("Incorrect argument type")));
			thrown = true;
			return temp_type();
		}
		temp_type tmp = v8::Local<T>::Cast(val);
		thrown = false;
		return tmp;
	}
	static temp_type from_temp(temp_type tmp) { return tmp; }
};


template <typename T> struct strip_cstring_cv
{
	typedef T type;
};
template <> struct strip_cstring_cv<const char*>
{
	typedef char* type;
};
template <> struct strip_cstring_cv<const volatile char*>
{
	typedef char* type;
};
template <> struct strip_cstring_cv<volatile char*>
{
	typedef char* type;
};

template <typename> struct is_string_type : public boost::false_type {};
template <> struct is_string_type<char*> : public boost::true_type {};
template <> struct is_string_type<const char*> : public boost::true_type {};
template <> struct is_string_type<volatile char*> : public boost::true_type {};
template <> struct is_string_type<volatile const char*> : public boost::true_type {};
// UTF-16 string (currently unimplemented)
template <> struct is_string_type<uint16_t*> : public boost::true_type {};
template <> struct is_string_type<std::string> : public boost::true_type {};


template <typename> struct is_v8_handle : public boost::false_type {};
template <typename T> struct is_v8_handle<v8::Handle<T> > : boost::true_type {};
template <typename T> struct is_v8_handle<v8::Local<T> > : boost::true_type {};

template <typename T> struct v8_type_conversion :
	v8_conversion_selector<
		typename strip_cstring_cv<typename boost::remove_cv<typename boost::remove_reference<T>::type>::type>::type,
		boost::is_arithmetic<typename boost::remove_reference<T>::type>::value,
		is_string_type<typename boost::remove_cv<typename boost::remove_reference<T>::type>::type>::value,
		is_v8_handle<typename boost::remove_cv<T>::type>::value>
{};

template <class Class, typename fn_t, fn_t Class::*fn, size_t arity> struct v8_fn_wrapper_base;

// 0-ary functions: f()
template <class Class, typename fn_t, fn_t Class::*fn> struct v8_fn_wrapper_base<Class, fn_t, fn, 0>
{
	typedef boost::function_traits<fn_t> fn_traits;
	static bool call(Class* me, const v8::Arguments& args, typename fn_traits::result_type& out, v8::Handle<v8::Value>&)
	{
		out = (me->*fn)();
		return true;
	}
};

// unary functions: f(a)
template <class Class, typename fn_t, fn_t Class::*fn> struct v8_fn_wrapper_base<Class, fn_t, fn, 1>
{
	typedef boost::function_traits<fn_t> fn_traits;
	static bool call(Class* me, const v8::Arguments& args, typename fn_traits::result_type& out, v8::Handle<v8::Value>& exc)
	{
		bool exc_thrown;
		typedef v8_type_conversion<typename fn_traits::arg1_type> arg1_conv;
		typename arg1_conv::temp_type arg1(arg1_conv::convert_to(args[0], exc_thrown, exc));
		if (exc_thrown) return false;
		
		out = (me->*fn)(arg1_conv::from_temp(arg1));
		return true;
	}
};

// binary functions: f(a, b)
template <class Class, typename fn_t, fn_t Class::*fn> struct v8_fn_wrapper_base<Class, fn_t, fn, 2>
{
	typedef boost::function_traits<fn_t> fn_traits;
	static bool call(Class* me, const v8::Arguments& args, typename fn_traits::result_type& out, v8::Handle<v8::Value>& exc)
	{
		bool exc_thrown;
		typedef v8_type_conversion<typename fn_traits::arg1_type> arg1_conv;
		typename arg1_conv::temp_type arg1(arg1_conv::convert_to(args[0], exc_thrown, exc));
		if (exc_thrown) return false;

		typedef v8_type_conversion<typename fn_traits::arg2_type> arg2_conv;
		typename arg2_conv::temp_type arg2(arg2_conv::convert_to(args[1], exc_thrown, exc));
		if (exc_thrown) return false;
		
		out = (me->*fn)(arg1_conv::from_temp(arg1), arg2_conv::from_temp(arg2));
		return true;
	}
};

// ternary functions: f(a, b, c)
template <class Class, typename fn_t, fn_t Class::*fn> struct v8_fn_wrapper_base<Class, fn_t, fn, 3>
{
	typedef boost::function_traits<fn_t> fn_traits;
	static bool call(Class* me, const v8::Arguments& args, typename fn_traits::result_type& out, v8::Handle<v8::Value>& exc)
	{
		bool exc_thrown;
		typedef v8_type_conversion<typename fn_traits::arg1_type> arg1_conv;
		typename arg1_conv::temp_type arg1(arg1_conv::convert_to(args[0], exc_thrown, exc));
		if (exc_thrown) return false;

		typedef v8_type_conversion<typename fn_traits::arg2_type> arg2_conv;
		typename arg2_conv::temp_type arg2(arg2_conv::convert_to(args[1], exc_thrown, exc));
		if (exc_thrown) return false;
		
		typedef v8_type_conversion<typename fn_traits::arg3_type> arg3_conv;
		typename arg3_conv::temp_type arg3(arg3_conv::convert_to(args[2], exc_thrown, exc));
		if (exc_thrown) return false;
		
		out = (me->*fn)(arg1_conv::from_temp(arg1), arg2_conv::from_temp(arg2), arg3_conv::from_temp(arg3));
		return true;
	}
};



template <typename fn_t, fn_t fn> struct v8_fn_wrapper;
template <typename Class, typename fn_t, fn_t Class::*fn>
struct v8_fn_wrapper <fn_t Class::*, fn> : v8_fn_wrapper_base<Class, fn_t, fn, boost::function_traits<fn_t>::arity>
{
	typedef boost::function_traits<fn_t> fn_traits;
	/// The V8 type conversion corresponding to the function's return value
	typedef v8_type_conversion<typename fn_traits::result_type> result_conversion;
		
	static v8::Handle<v8::Value> wrapped(const v8::Arguments& args)
	{
		v8::HandleScope scope;
		node::ObjectWrap* this_obj = node::ObjectWrap::Unwrap<node::ObjectWrap>(args.This());
		Class* me = dynamic_cast<Class*>(this_obj);
		if (!me)
		{
			return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Bad type for 'this' object\n")));
		}
		
		if (args.Length() < (int)fn_traits::arity)
		{
			std::ostringstream exc;
			exc << "Too few arguments, expected " << fn_traits::arity << ", received " << args.Length();
			return v8::ThrowException(v8::Exception::TypeError(v8::String::New(exc.str().c_str())));
		}
		
		v8::Local<v8::Value> exc;
		typename fn_traits::result_type retval;
		bool ok = call(me, args, retval, exc);
		if (!ok) return scope.Close(exc);
		v8::Handle<typename result_conversion::v8_type> result = result_conversion::convert_to_v8(retval);
		return scope.Close(result);
	}
};

#define NODE_FN_WRAP(MEMBERFN) (v8_fn_wrapper<typeof(&MEMBERFN), &MEMBERFN>::wrapped);
#define NODE_SET_WRAPPED_PROTOTYPE_METHOD(templ, name, memberfn) NODE_SET_PROTOTYPE_METHOD(templ, name, (v8_fn_wrapper<typeof(&memberfn), &memberfn>::wrapped))

#endif
