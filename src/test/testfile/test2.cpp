#include <%%dependencie%%>

%%if __TIMESTAMP__%%
long timestamp = %%__TIMESTAMP__%%;
%%else%%
long timestamp = 0;
%%endif%%


long %%methodName%%(%%two_method_args_int(a,b)%%) {
    return %%__TIMESTAMP__%%;
}
