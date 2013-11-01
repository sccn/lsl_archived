
%module liblsl
%{
#include "../liblsl/include/lsl_cpp.h"
using namespace lsl;
%}

%include "std_vector.i"
%include "std_string.i"
%include "exception.i"
%include "../liblsl/include/lsl_cpp.h"
namespace std {
   %template(vectorf) vector<float>;
   %template(vectord) vector<double>;
   %template(vectorl) vector<long>;
   %template(vectori) vector<int>;
   %template(vectors) vector<short>;
   %template(vectorstr) vector<string>;
   %template(vectorinfo) vector<lsl::stream_info>;   
   %template(chunkf) vector<vector<float> >;
   %template(chunkd) vector<vector<double> >;
   %template(chunkl) vector<vector<long> >;
   %template(chunki) vector<vector<int> >;
   %template(chunks) vector<vector<short> >;
   %template(chunkstr) vector<vector<string> >;   
};

%template(push_chunk) lsl::stream_outlet::push_chunk<std::vectorf>;
%template(push_chunk) lsl::stream_outlet::push_chunk<std::vectord>;
%template(push_chunk) lsl::stream_outlet::push_chunk<std::vectorl>;
%template(push_chunk) lsl::stream_outlet::push_chunk<std::vectori>;
%template(push_chunk) lsl::stream_outlet::push_chunk<std::vectors>;
%template(push_chunk) lsl::stream_outlet::push_chunk<std::vectorstr>;

%template(pull_chunk) lsl::stream_inlet::pull_chunk<float>;
%template(pull_chunk) lsl::stream_inlet::pull_chunk<double>;
%template(pull_chunk) lsl::stream_inlet::pull_chunk<long>;
%template(pull_chunk) lsl::stream_inlet::pull_chunk<int>;
%template(pull_chunk) lsl::stream_inlet::pull_chunk<short>;
%template(pull_chunk) lsl::stream_inlet::pull_chunk<string>;


