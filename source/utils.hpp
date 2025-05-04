namespace ir {

#define RETURN_ON_FAIL(x) \
    ret = x; \
    if (R_FAILED(ret)) \
        return ret; 

} // End of namespace ir