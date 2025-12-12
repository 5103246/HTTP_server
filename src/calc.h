#ifndef CALC_H
#define CALC_H

#include "parse.h"

char *ExtractQuery(const char *query);
int StringToInt(const char *nums, int n);
int Calculate(const char *expr);
char *HandleCalculate(const RequestLine *request_line);

#endif /* CALC_H */