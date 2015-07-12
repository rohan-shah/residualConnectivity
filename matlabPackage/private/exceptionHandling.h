#ifndef EXCEPTION_HANDLING_HEADER_GUARD
#define EXCEPTION_HANDLING_HEADER_GUARD
#define BEGIN_MEX_WRAPPER try{
#define END_MEX_WRAPPER }catch(std::runtime_error& err){mexErrMsgTxt(err.what()); return;}
#endif