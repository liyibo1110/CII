#include "arena.h"
#include "assert.h"
#include "except.h"
#include <stdlib.h>
#include <string.h>

const Except_T Arena_NewFailed = {"Arena_NewFailed"};
const Except_T Arena_Failed = {"Arena_Failed"};

