#include <ruby.h>
#include <sstream>
#include <string>
#include <vector>

#define PRINT_FUNC() fprintf(stderr, "[%s]\n", __func__)
#define PRINT_CXXFUNC() fprintf(stderr, "[CXX %s]\n", __func__)
#if TRACE
#define TRACE_PRINTF printf
#else
#define TRACE_PRINTF
#endif

//TODO Below variable is originally defined as TLSData<...> and TLSData is defined in opencv2/core/utils/tls.hpp
thread_local std::vector<std::string> conversionErrorsTLS;

void rbRaiseCVOverloadException(const std::string& functionName)
{
    std::string msg(functionName);
    const std::vector<std::string>& conversionErrors = conversionErrorsTLS;
    //const std::size_t conversionErrorsCount = conversionErrors.size();
    for (const auto& convErr : conversionErrors) {
        msg += convErr;
    }
    rb_raise(rb_eTypeError, "%s", msg.c_str());
}

void rbPopulateArgumentConversionErrors(const std::string& msg)
{
    conversionErrorsTLS.push_back(msg);
}

template<typename T>
static bool rbopencv_to(VALUE obj, T& p){
    TRACE_PRINTF("[rbopencv_to primary] should not be used\n");
    return false;
}

template<>
bool rbopencv_to(VALUE obj, int& value){
    TRACE_PRINTF("[rbopencv_to int]\n");
    if (!FIXNUM_P(obj))
        return false;
    value = FIX2INT(obj);
    return true;
}

template<typename T>
static VALUE rbopencv_from(const T& src) {
    TRACE_PRINTF("[rbopencv_from primary] should not be used\n");
    return Qnil;
}

template<>
VALUE rbopencv_from(const int& value){
    TRACE_PRINTF("[rbopencv_from int]\n");
    return INT2NUM(value);
}

struct MethodDef {
    using func_ptr_for_ruby_method = VALUE (*)(int, VALUE*, VALUE);
    const char *name;
    func_ptr_for_ruby_method wrapper_func;
};

struct ConstDef {
    const char *name;
    long long val;
};

#include "autogen/rbopencv_include.hpp"
using namespace cv;
static VALUE mCV2;
#include "autogen/rbopencv_wrapclass.hpp"

#include "autogen/rbopencv_funcs.hpp"
#include "autogen/rbopencv_modules_content.hpp"

// 1st arg (top_module) must be mCV2
static void init_submodule(VALUE top_module, const char* name, MethodDef method_defs[], ConstDef const_defs[]){
    // traverse and create nested submodules
    std::string s = name;
    size_t i = s.find('.');
    VALUE parent_mod = top_module;
    while (i < s.length() && i != std::string::npos)
    {
        size_t j = s.find('.', i);
        if (j == std::string::npos)
            j = s.length();
        std::string short_name = s.substr(i, j-i);
        std::string full_name = s.substr(0, j);
        i = j+1;
        std::string module_short_name{short_name};
        // Ruby module name must begins with uppercase
        module_short_name[0] = toupper(module_short_name[0]);

        if (module_short_name == "")
            parent_mod = top_module;
        else {
            int name_sym = rb_intern(module_short_name.c_str());
            int is_defined = rb_const_defined(parent_mod, name_sym);
            VALUE submod;
            if (is_defined)
                submod = rb_const_get(parent_mod, name_sym);
            else
                submod = rb_define_module_under(parent_mod, module_short_name.c_str());
            parent_mod = submod;
        }
    }

    MethodDef *method_def = method_defs;
    while (method_def->name) {
        rb_define_module_function(parent_mod, method_def->name, method_def->wrapper_func, -1);
        method_def++;
    }
    ConstDef *const_def = const_defs;
    while (const_def->name) {
        // Need to check whether the 1st character is upper case.
        // cv::datasets defines both uppercase and lowercase constants (e.g. "CIRCLE" and "circle")
        if (const_def->name[0] != '_' && isupper(const_def->name[0]))
            rb_define_const(parent_mod, const_def->name, INT2FIX(const_def->val));
        const_def++;
    }
}

static std::vector<std::string> split_string(const std::string& str, char delim){
    std::vector<std::string> substrs;
    std::stringstream sstream{str};
    std::string substr;
    while (getline(sstream, substr, delim))
        if (!substr.empty())
            substrs.push_back(substr);
    return substrs;
}

static VALUE get_parent_module_by_wname(VALUE top_module, const std::string wname){
    // wname: Ns1_Ns11_SubSubC1
    auto modnames = split_string(wname, '_'); // ["Ns1", "Ns11", "SubSubC1"]
    modnames.pop_back(); // remove the last element (class name) => ["Ns1", "Ns11"]
    VALUE parent_mod = top_module;
    VALUE submod;
    for (const auto &modname : modnames) {
        int name_sym = rb_intern(modname.c_str());
        int is_defined = rb_const_defined(parent_mod, name_sym);
        VALUE submod;
        if (is_defined)
            submod = rb_const_get(parent_mod, name_sym);
        else {
            fprintf(stderr, "[ruby cv2.cpp %s] Error: parent_mod is not defined\n", __func__);
            parent_mod = Qnil;
            break;
        }
        parent_mod = submod;
    }
    return parent_mod;
}

extern "C" {
void Init_cv2(){
    PRINT_FUNC();
    mCV2 = rb_define_module("CV2");

    #include "autogen/rbopencv_namespaceregistration.hpp"
    #include "autogen/rbopencv_classregistration.hpp"
}
}
