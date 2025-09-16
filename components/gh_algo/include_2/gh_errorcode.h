#ifndef GHERRORCODE_H
#define GHERRORCODE_H

#define GHCAT(a,b) a##b
#define GHRPC_ERROR_DECLARE(mod) GHRPC_ERROR_##mod##_BASE_ID
#define GHRPC_ERROR_BASE(mod) GHRPC_ERROR_##mod##_BASE = GHRPC_ERROR_##mod##_BASE_ID <<10
#define GHRPC_ERROR_INFO_DECLARE(mod) extern char* g_gh_error_code_##mod[]

enum GHRPC_MODULE_ERROR_BASE
{
    GHRPC_NORMAL,
    GHRPC_ERROR_DECLARE(CHAMMAP),
    GHRPC_ERROR_DECLARE(PACKAGE),
    GHRPC_ERROR_DECLARE(INVOKE)
};

//if support char* detail(must imp char g_cardiff_error_code_<module_name>[])
GHRPC_ERROR_INFO_DECLARE(CHAMMAP);
GHRPC_ERROR_INFO_DECLARE(PACKAGE);
GHRPC_ERROR_INFO_DECLARE(INVOKE);

#define GHERRORINFO(mod,code) GHCAT(GHCAT(g_gh_error_code_,mod),[code-(GHRPC_ERROR_BASE(mod))])

//we recommend all function use error code of this system write this macro in first
//if this function not use error_code, use /*CARIDFF_EXCEPTION_FUNCTION*/
#define CARIDFF_EXCEPTION_FUNCTION int error_code = 0;
//if error_code is not directly return, recommend to use macro after for debug easily
#define CARIDFF_EXCEPTION_HANDLE GH_EXCEPTION:
#define CARIDFF_EXCEPTION_DEAL do{if(error_code)goto GH_EXCEPTION;}while(0)
#define TRY_RUN(exp)  do{error_code = exp;if(error_code){goto GH_EXCEPTION;}}while(0)
#define GH_EXCEPTION_ASSERT(exp,code) do{if(exp==0){error_code = code;goto GH_EXCEPTION;}}while(0)
//only for has some clear work
#define GH_EXCEPTION_RETURN(exp) do{error_code = (exp);goto GH_EXCEPTION;}while(0)


#define T2(type,var) (*(type*)&var)
#define FLEXIBLE_ARRAY  //for c89, not support flexible array, this must be 1

#endif //GHERRORCODE_H
