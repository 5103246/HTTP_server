#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

char *ExtractQuery(const char *query) {
    if (!query) {
        perror("no query");
        return NULL;
    }
    const char *prefix = "query=";
    char *query_value = strstr(query, prefix);
    if (!query_value) {
        return NULL;
    }

    query_value += strlen(prefix);
    const size_t value_len = strlen(query_value);
    char *buf = malloc(value_len);
    if (!buf) {
        perror("extract query mallc failed");
        return NULL;
    }
    memcpy(buf, query_value, value_len + 1);
    buf[value_len] = '\0';
    return buf;
}

int StringToInt(const char *nums, int n)  {
    int result = 0;
    for (int i = 0; i < n; i++) {
        char ch = nums[i];
        if (ch < '0' || ch > '9') {
            printf("Error: character is not a digit\n");
            return -1;
        }
        int num = ch - '0';
        result = result * 10 + num;
    }
    return result;
}

int Calculate(const char *expr) {
    if (!expr) {
        printf("no expression\n");
        return EXIT_FAILURE;
    }
    char *operator = strpbrk(expr, "+-*/");
    if (!operator) {
        printf("no operator\n");
        return EXIT_FAILURE;
    }

    int left_digit = operator - expr;
    int left_operand = StringToInt(expr, left_digit);
    char *right_num = operator + 1;
    int right_digit = strlen(expr) - left_digit - 1;
    int right_operand = StringToInt(right_num, right_digit);
    int result = 0;

    switch (*operator) {
    case '+':
        result = left_operand + right_operand;
        break;
    case '-':
        result = left_operand - right_operand;
        break;
    case '*':
        result = left_operand * right_operand;
        break;
    case '/':
        result = (right_operand != 0) ? (left_operand / right_operand) : 0;
        break;
    }
    return result;
}

char *HandleCalculate(const RequestLine *request_line) {
    if (!request_line) {
        printf("no request_line\n");
        return NULL;
    }

    char *expression = ExtractQuery(request_line->query);
    if (!expression) {
        printf("failed in ExtractQuery\n");
        return NULL;
    }

    int result = Calculate(expression);
    free(expression);
    if (result == EXIT_FAILURE) {
        printf("failed in Calculate\n");
        return NULL;
    }
    int required_len = snprintf(NULL, 0, "%d", result);
    char *buf = malloc(required_len + 1);
    if (!buf) {
        printf("malloc failed in HandleCalculate\n");
        return NULL;
    }

    if (snprintf(buf, required_len + 1, "%d", result) < 0) {
        free(buf);
        return NULL;
    }
    return buf;
}