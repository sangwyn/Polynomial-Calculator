#include "Term.h"

Term::Term() {
    sign = 1;
    coeff = 0;
    for (char c = 'a'; c <= 'z'; ++c)
        power[c] = 0;
}