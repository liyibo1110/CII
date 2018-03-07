#ifndef ATOM_H_
#define ATOM_H_

const char *atomNew(const char *str, int len);
const char *atomString(const char *str);
const char *atomInt(long n);
int atomLength(const char *str);

#endif