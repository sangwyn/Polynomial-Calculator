#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <set>
#include "Node.h"
#include "Term.h"
#include "DefineConstants.h"
#include "WindowManager.h"
#include "Functions.h"

WindowManager::WindowManager(sf::Font font) {
    this->font = font;
    screen.setOutlineColor(kUIColor);
    screen.setOutlineThickness(2);
    screen.setFillColor(sf::Color::Transparent);
    screen.setSize({800.0f, 700.0f});
    screen.setPosition(50.0f, 50.0f);

    polynom_text.setFont(this->font);
    polynom_text.setPosition(50.0f, 50.0f);
    polynom_text.setFillColor(kUIColor);

    error_text.setFont(this->font);
    error_text.setPosition(900.0f, 50.0f);
    error_text.setFillColor(kErrorColor);

    result_text.setFont(this->font);
    result_text.setPosition(900.0f, 800.0f);
    result_text.setFillColor(kResultColor);
    result_text.setString("Result");

    input_field.setFillColor(kUIColor);
    input_field.setSize({800.0f,
                         (float) polynom_text.getCharacterSize() + 10.0f});
    input_field.setPosition(50.0f, 800.0f);
}

void WindowManager::AddToPolynom(Node<Node<Term *> *> *&list, Term *&term) {
    if (list == nullptr || term == nullptr)
        return;

    if (list->data == nullptr)
        list->data = new Node<Term *>(new Term);

    if (list->data->data->coeff == 0) {
        //      list->data = new Node<Term *>();
        list->data->data = term;
        return;
    }
    Node<Term *> *it = list->data;
    Node<Term *> *prev = nullptr;
    while (it != nullptr && it->data != nullptr) {
        bool can_add = true;
        int max_power_term = 0, max_power_it = 0;
        std::string str_it = "", str_term = "";
        for (char c = 'a'; c <= 'z'; ++c) {
//            std::cout << it->data->power[c] << ' ';
//            std::cout << term->power[c] << '\n';
            if (max_power_it < it->data->power[c])
                max_power_it = it->data->power[c];
            if (max_power_term < term->power[c])
                max_power_term = term->power[c];
            if (it->data->power[c])
                str_it += c;
            if (term->power[c])
                str_term += c;
//            if (it->data->power[c])
//                was_var = true;
//            if (!was_var && term->power[c] && !it->data->power[c]) {
//                Node<Term *> *q = new Node<Term *>();
//                q->data = term;
//                q->next = it;
//                if (prev != nullptr)
//                    prev->next = q;
//                else
//                    list->data = q;
//                return;
//            }
            if (it->data->power[c] != term->power[c]) {
                can_add = false;
            }
        }
//        2
//        2x^2+5x-8
//        3x^2+4-2x+y


        if (can_add) {
            it->data->coeff =
                it->data->sign * it->data->coeff + term->sign * term->coeff;
            if (it->data->coeff == 0) {
                if (!(list->data == it && it->next == nullptr)) {
                    if (prev != nullptr)
                        prev->next = it->next;
                    else
                        list->data = it->next;
                    delete it;
                }
            } else {
                it->data->sign = it->data->coeff / fabs(it->data->coeff);
                it->data->coeff = fabs(it->data->coeff);
            }
            return;
        }

//        std::cout << max_power_term << " " << str_term << " " << max_power_it
//                  << " " << str_it << "\n";
        if (str_it.empty() || !str_term.empty()
            && (max_power_it < max_power_term || max_power_term == max_power_it
                && (str_it > str_term
                    || str_term.length() < str_it.length()))) {
//            std::cout << "INS BEFORE" << '\n';
            Node<Term *> *q = new Node<Term *>();
            q->data = term;
            q->next = it;
            if (prev != nullptr)
                prev->next = q;
            else
                list->data = q;
            return;
        }

        prev = it;
        it = it->next;
    }
    prev->next = new Node<Term *>;
    prev->next->data = term;
    return;
}

void WindowManager::InputPolynom(Node<Node<Term *> *> *&list, sf::String s) {
    if (list == nullptr)
        return;
    // list->data = new Node<Term>;

    Term *current_term = new Term();
    char current_var = '0';
    bool got_coeff = false;
    bool getting_power = false;
    int power_to_add = 0;
    int coeff_to_multiply = 1;

    int index = 0;
    if (s[0] == '+' || s[0] == '-') {
        if (s[0] == '-')
            current_term->sign = -1;
        ++index;
    }

    for (; index < s.getSize(); ++index) {
        sf::Uint32 c = s[index];
        if (c == '\n')
            continue;
        if (c == ' ') {
            if (!got_coeff && current_term->coeff)
                throw c;
            getting_power = false;
            continue;
        }
        if (std::isdigit(c)) {
            if (!got_coeff || !getting_power) {
                current_term->coeff = current_term->coeff * 10 + (c - '0');
            } else {
                if (current_term->power[current_var] == -1)
                    current_term->power[current_var] = 0;
                current_term->power[current_var] =
                    current_term->power[current_var] * 10 + (c - '0');
            }
        } else if (c >= 'a' && c <= 'z') {
            if (getting_power && current_term->power[current_var] < 1)
                throw c;
            getting_power = false;
            if (current_var != '0' && current_term->power[current_var] == -1)
                current_term->power[current_var] = 1;
            if (current_var != '0')
                current_term->power[current_var] += power_to_add;
            got_coeff = true;
            if (current_term->coeff == 0)
                current_term->coeff = 1;
            current_var = c;
            power_to_add = current_term->power[c];
            current_term->power[c] = -1;
            coeff_to_multiply *= current_term->coeff;
            current_term->coeff = 0;
        } else if (c == '^') {
            got_coeff = true;
            getting_power = true;
        } else if (c == '+' || c == '-') {
            getting_power = false;
            if (current_var != '0' && current_term->power[current_var] == -1)
                current_term->power[current_var] = 1;
            if (current_var != '0') {
                current_term->power[current_var] += power_to_add;
            }
            if (current_term->coeff == 0)
                current_term->coeff = 1;
            current_term->coeff *= coeff_to_multiply;
            AddToPolynom(list, current_term);
            current_term = new Term();
            if (c == '-')
                current_term->sign = -1;
            got_coeff = false;
            current_var = '0';
            power_to_add = 0;
            coeff_to_multiply = 1;
        } else {
            throw c;
        }
    }
    if (current_var != '0' && current_term->power[current_var] == -1)
        current_term->power[current_var] = 1;
    if (current_var != '0') {
        current_term->power[current_var] += power_to_add;
        current_term->coeff *= coeff_to_multiply;
    }
    if (current_term->coeff == 0) {
        current_term->coeff = 1;
        current_term->coeff *= coeff_to_multiply;
    }
    AddToPolynom(list, current_term);
    return;
}

void WindowManager::ExpInputPolynom(Node<Node<Term *> *> *&list, sf::String s) {
    if (list == nullptr)
        return;

    Term *current_term = new Term();
    int state = 0;
    char last_var = '0';
    int power_to_add = 0;
    int coeff_to_add = 0;

    int i = 0;
    if (s[0] == '+' || s[0] == '-') {
        i = 1;
        if (s[0] == '-')
            current_term->sign = -1;
        if (i >= s.getSize()) {
            throw s[0];
        }
    }
    current_term->coeff = -1;

    for (; i < s.getSize(); ++i) {
        auto c = s[i];
        if (state == 0) {
            if (std::isdigit(c)) {
                if (current_term->coeff == -1)
                    current_term->coeff = 0;
                current_term->coeff = current_term->coeff * 10 + (c - '0');
            } else if (c >= 'a' && c <= 'z') {
                if (current_term->coeff == -1)
                    current_term->coeff = 1;
                if (coeff_to_add)
                    current_term->coeff *= coeff_to_add;
                coeff_to_add = 0;

                state = 1;
                last_var = c;
                power_to_add = current_term->power[last_var];
                current_term->power[last_var] = -1;
            } else if (c == ' ') {
                if (current_term->coeff != -1)
                    state = 3;
            } else if (c == '+' || c == '-') {
                if (current_term->coeff == -1)
                    throw c;
                if (coeff_to_add)
                    current_term->coeff *= coeff_to_add;
                coeff_to_add = 0;
                for (char ch = 'a'; ch <= 'z'; ++ch) {
                    if (current_term->power[ch] == -1)
                        current_term->power[ch] = 1;
                }
                AddToPolynom(list, current_term);
                current_term = new Term;
                if (c == '-') {
                    current_term->sign = -1;
                }
                state = 0;
                last_var = '0';
                current_term->coeff = -1;
            } else {
                throw c;
            }
        } else if (state == 1) {
            if (c == '^') {
                if (last_var == '0')
                    throw c;
                if (current_term->power[last_var] == -1)
                    current_term->power[last_var] = 0;
                power_to_add = std::max(current_term->power[last_var], power_to_add);
                current_term->power[last_var] = -1;

                state = 2;
            } else if (c >= 'a' && c <= 'z') {
                if (current_term->power[last_var] == -1)
                    current_term->power[last_var] = 1;
                current_term->power[last_var] += power_to_add;
                power_to_add = 0;

                last_var = c;
                power_to_add = current_term->power[last_var];
                current_term->power[last_var] = -1;
            } else if (c == '+' || c == '-') {
                if (current_term->power[last_var] == -1)
                    current_term->power[last_var] = 1;
                current_term->power[last_var] += power_to_add;
                power_to_add = 0;

                for (char ch = 'a'; ch <= 'z'; ++ch) {
                    if (current_term->power[ch] == -1)
                        current_term->power[ch] = 1;
                }

                AddToPolynom(list, current_term);
                current_term = new Term;
                if (c == '-') {
                    current_term->sign = -1;
                }
                state = 0;
                last_var = '0';
                current_term->coeff = -1;
            } else if (std::isdigit(c)) {
                coeff_to_add = current_term->coeff;
                current_term->coeff = c - '0';
                state = 0;
                if (current_term->power[last_var] == -1)
                    current_term->power[last_var] = 1;
            } else if (c == ' ') {
//                if (current_term->power[last_var] == -1)
//                    current_term->power[last_var] = 1;
            } else {
                throw c;
            }
        } else if (state == 2) {
            if (std::isdigit(c)) {
                if (current_term->power[last_var] == -1)
                    current_term->power[last_var] = 0;
                current_term->power[last_var] =
                    current_term->power[last_var] * 10 + (c - '0');
            } else if (c >= 'a' && c <= 'z') {
                if (current_term->power[last_var] == -1)
                    throw c;
                current_term->power[last_var] += power_to_add;
                power_to_add = 0;

                state = 1;
                last_var = c;
                power_to_add = current_term->power[last_var];
                current_term->power[last_var] = -1;
            } else if (c == ' ') {
                current_term->power[last_var] += power_to_add;
                power_to_add = 0;

                state = 1;
                last_var = '0';
                current_term->coeff = -1;
            } else if (c == '+' || c == '-') {
                if (current_term->power[last_var] == -1)
                    throw c;
                current_term->power[last_var] += power_to_add;
                power_to_add = 0;

                for (char ch = 'a'; ch <= 'z'; ++ch) {
                    if (current_term->power[ch] == -1)
                        current_term->power[ch] = 1;
                }

                AddToPolynom(list, current_term);
                current_term = new Term;
                current_term->coeff = -1;
                if (c == '-') {
                    current_term->sign = -1;
                }
                state = 0;
                last_var = '0';
            } else {
                throw c;
            }
        } else if (state == 3) {
            if (c >= 'a' && c <= 'z') {
                if (coeff_to_add)
                    current_term->coeff *= coeff_to_add;
                coeff_to_add = 0;

                state = 1;
                last_var = c;
                power_to_add = current_term->power[last_var];
                current_term->power[last_var] = -1;
            } else if (c == '+' || c == '-') {
                if (coeff_to_add)
                    current_term->coeff *= coeff_to_add;
                coeff_to_add = 0;

                state = 0;
                last_var = '0';

                for (char ch = 'a'; ch <= 'z'; ++ch) {
                    if (current_term->power[ch] == -1)
                        current_term->power[ch] = 1;
                }

                AddToPolynom(list, current_term);
                current_term = new Term;
                current_term->coeff = -1;
            } else {
                throw c;
            }
        }
    }
    if (state == 2 && current_term->power[last_var] <= 0)
        throw s[i - 1];
    if (last_var != '0') {
        if (current_term->power[last_var] == -1)
            current_term->power[last_var] = 1;
        current_term->power[last_var] += power_to_add;
    }
    if (state == 0) {
        if (coeff_to_add == 0 && current_term->coeff == -1)
            throw s[i - 1];
        if (coeff_to_add)
            current_term->coeff *= coeff_to_add;
    }
    if (current_term->coeff != 0) {
        for (char ch = 'a'; ch <= 'z'; ++ch) {
            if (current_term->power[ch] == -1)
                current_term->power[ch] = 1;
        }
        AddToPolynom(list, current_term);
    }

    if (list->data == nullptr) {
        list->data = new Node<Term *>(new Term);
    }

    std::cout << "";

    return;
}

Node<Term *> *WindowManager::MultiplyPolynoms(Node<Term *> *p1,
                                              Node<Term *> *p2) {
    Node<Term *> *res = new Node<Term *>(new Term);
    Node<Term *> *res_it = res;
    Node<Term *> *p1_it = p1;
    while (p1_it != nullptr) {
        res_it->data->power = p1_it->data->power;
        res_it->data->coeff = p1_it->data->coeff;
        res_it->data->sign = p1_it->data->sign;
        p1_it = p1_it->next;
        if (p1_it != nullptr)
            res_it->next = new Node<Term *>(new Term);
        res_it = res_it->next;
    }
    Node<Term *> *it = p2;
    bool first = true;
    while (it != nullptr) {
//        PrintPolynom(res);
//        std::cout << '\n';
        Node<Term *> *multiplied = new Node<Term *>(new Term);
        Node<Term *> *multiplied_it = multiplied;
        p1_it = p1;
        while (p1_it != nullptr) {
            multiplied_it->data->power = p1_it->data->power;
            multiplied_it->data->coeff = p1_it->data->coeff;
            multiplied_it->data->sign = p1_it->data->sign;
            p1_it = p1_it->next;
            if (p1_it != nullptr)
                multiplied_it->next = new Node<Term *>(new Term);
            multiplied_it = multiplied_it->next;
        }

        Node<Term *> *j = multiplied;
        while (j != nullptr) {
            j->data->coeff *= it->data->coeff;
            j->data->sign *= it->data->sign;
            for (char c = 'a'; c <= 'z'; ++c) {
                j->data->power[c] += it->data->power[c];
            }
            j = j->next;
        }

        if (first) {
            j = multiplied;
            res_it = res;
            while (j != nullptr) {
                res_it->data->power = j->data->power;
                res_it->data->coeff = j->data->coeff;
                res_it->data->sign = j->data->sign;
                j = j->next;
                res_it = res_it->next;
            }
        } else
            res = SumPolynoms(res, multiplied);

        it = it->next;
        delete multiplied;
        first = false;
    }
    return res;
}

Node<Term *> *WindowManager::SumPolynoms(Node<Term *> *p1, Node<Term *> *p2) {
    Node<Term *> *res = new Node<Term *>(new Term);
    Node<Term *> *p = res;
    Node<Term *> *i = p1, *j = p2;

//    PrintPolynom(p1);
//    std::cout << '\n';
//    PrintPolynom(p2);
//    std::cout << '\n';

    while (i != nullptr && j != nullptr) {
//        if (i->data == nullptr)
//            i->data = new Term;
//        if (j->data == nullptr)
//            j->data = new Term;
        if (i->data->coeff == 0) {
            i = i->next;
            continue;
        }
        if (j->data->coeff == 0) {
            j = j->next;
            continue;
        }
        int max_power_i = 0, max_power_j = 0;
        std::string str_i = "", str_j = "";
        bool can_add = true;
        for (char c = 'a'; c <= 'z'; ++c) {
            if (i->data->power[c]) {
                str_i += c;
                if (i->data->power[c] > max_power_i)
                    max_power_i = i->data->power[c];
            }
            if (j->data->power[c]) {
                str_j += c;
                if (j->data->power[c] > max_power_j)
                    max_power_j = j->data->power[c];
            }
            if (i->data->power[c] != j->data->power[c])
                can_add = false;
        }

//        std::cout << i->data->coeff << ' ' << max_power_i << ' ' << str_i << ' ' << j->data->coeff << ' ' << max_power_j << ' ' << str_j << '\n';

        if (can_add) {
//            std::cout << "CAN\n";
            float new_coef =
                j->data->sign * j->data->coeff + i->data->sign * i->data->coeff;
            if (!new_coef) {
                i = i->next;
                j = j->next;
                continue;
            }
            p->data->power = i->data->power;

            p->data->coeff = new_coef;
            p->data->sign = 1;
            if (p->data->coeff < 0)
                p->data->sign = -1;
            p->data->coeff = fabs(p->data->coeff);

            i = i->next;
            j = j->next;
        } else {
//            std::cout << "CANNOT\n";
            if (max_power_i > max_power_j || max_power_i == max_power_j
                && (str_j > str_i || str_j.length() > str_i.length())) {
                p->data->power = i->data->power;
                p->data->coeff = i->data->coeff;
                p->data->sign = i->data->sign;
                i = i->next;
            } else {
                p->data->power = j->data->power;
                p->data->coeff = j->data->coeff;
                p->data->sign = j->data->sign;
                j = j->next;
            }
        }

        if (i != nullptr || j != nullptr)
            p->next = new Node<Term *>(new Term);
        p = p->next;
    }
    while (i != nullptr) {
        if (i->data->coeff == 0) {
            i = i->next;
            continue;
        }
        p->data->power = i->data->power;
        p->data->coeff = i->data->coeff;
        p->data->sign = i->data->sign;
        i = i->next;
        if (i != nullptr || j != nullptr)
            p->next = new Node<Term *>(new Term);
        p = p->next;
    }
    while (j != nullptr) {
        if (j->data->coeff == 0) {
            j = j->next;
            continue;
        }
        p->data->power = j->data->power;
        p->data->coeff = j->data->coeff;
        p->data->sign = j->data->sign;
        j = j->next;
        if (j != nullptr)
            p->next = new Node<Term *>(new Term);
        p = p->next;
    }

    Node<Term *> *it = res;
    if (it->data->coeff == 0) {
        if (it->next != nullptr) {
            res = it = it->next;
        }
    }
    if (res == nullptr) {
        res = new Node<Term *>(new Term);
    }
    if (res->data == nullptr) {
        res->data = new Term;
    }

    while (it != nullptr && it->next != nullptr) {
        if (it->next->data == nullptr) {
            it->next->data = new Term;
        }
        if (it->next->data->coeff == 0) {
            Node<Term *> *tmp = it->next;
            it->next = tmp->next;
            delete tmp;
        }
        it = it->next;
    }

    if (res == nullptr) {
        res = new Node<Term *>(new Term);
    }

//    PrintPolynom(res);
//    std::cout << "\n\n";
    return res;
}

Node<Node<Term *> *> *WindowManager::DividePolynoms(Node<Term *> *p,
                                                    Node<Term *> *q,
                                                    char var) {
    Node<Node<Term *> *>
        *res = new Node<Node<Term *> *>(new Node<Term *>(new Term));
    res->next = new Node<Node<Term *> *>(new Node<Term *>(new Term));

    Node<Term *> *minus_q = new Node<Term *>(new Term);
    Node<Term *> *q_i = q, *minus_q_i = minus_q;
    while (q_i != nullptr) {
        minus_q_i->data->power = q_i->data->power;
        minus_q_i->data->coeff = q_i->data->coeff;
        minus_q_i->data->sign = -q_i->data->sign;
        q_i = q_i->next;
        if (q_i != nullptr) {
            minus_q_i->next = new Node<Term *>(new Term);
        }
        minus_q_i = minus_q_i->next;
    }
    Node<Term *> *p_copy = new Node<Term *>(new Term), *p_i = p;
    Node<Term *> *p_copy_i = p_copy;
    while (p_i != nullptr) {
        p_copy_i->data->power = p_i->data->power;
        p_copy_i->data->coeff = p_i->data->coeff;
        p_copy_i->data->sign = p_i->data->sign;
        p_i = p_i->next;
        if (p_i != nullptr) {
            p_copy_i->next = new Node<Term *>(new Term);
        }
        p_copy_i = p_copy_i->next;
    }
    Node<Term *> *new_term;
    while (true) {
        new_term = new Node<Term *>(new Term);
        new_term->data->power[var] = p_copy->data->power[var] - q->data->power[var];
        if (new_term->data->power[var] < 0)
            break;
        new_term->data->coeff = p_copy->data->coeff / q->data->coeff;
        new_term->data->sign = q->data->sign * p_copy->data->sign;
        res->data = SumPolynoms(res->data, new_term);
        Node<Term *> *mult_res = MultiplyPolynoms(minus_q, new_term);
        Node<Term *> *sum_res = SumPolynoms(p_copy, mult_res);

//        PrintPolynom(new_term);
//        std::cout << '\n';
//        PrintPolynom(mult_res);
//        std::cout << '\n';
//        PrintPolynom(sum_res);
//        std::cout << "\n\n";

        delete p_copy;
        p_copy = sum_res;
        delete new_term;
    }
    res->next->data = p_copy;
    return res;
}

float WindowManager::GetValue(Node<Term *> *p1, std::vector<float> values) {
    std::map<char, float> vals;
    int i = 0;
    for (char c = 'a'; c <= 'z'; ++c) {
        Node<Term *> *it = p1;
        while (it != nullptr) {
            if (it->data->power[c]) {
                if (i >= values.size())
                    throw i;
                vals[c] = values[i];
                ++i;
                break;
            }
            it = it->next;
        }
    }
    if (i < values.size())
        throw i;
    float res = 0;
    Node<Term *> *it = p1;
    while (it != nullptr) {
        float val = it->data->sign * it->data->coeff;
        for (char c = 'a'; c <= 'z'; ++c) {
            if (it->data->power[c]) {
                for (int k = 0; k < it->data->power[c]; ++k)
                    val *= vals[c];
            }
        }
        res += val;
        it = it->next;
    }
    return res;
}

bool WindowManager::CheckRoot(Node<Term *> *p, int n, char var) {
    Node<Term *> *i = p;
    int res = 0;

    while (i != nullptr) {
        int term_res = i->data->sign * i->data->coeff;
        for (int power = 0; power < i->data->power[var]; ++power)
            term_res *= n;
        res += term_res;
        i = i->next;
    }

    return !res;
}

std::vector<float> WindowManager::GetRoots(Node<Term *> *p) {
    Node<Term *> *i = p;
    Node<Term *> *prev = p;
    char var = '0';
    int free_term = 0;
    while (i != nullptr) {
        if (i->data->coeff != (int) i->data->coeff)
            throw i->data->coeff;
        bool no_vars = true;
        for (char c = 'a'; c <= 'z'; ++c) {
            if (i->data->power[c]) {
                no_vars = false;
                if (var == '0')
                    var = c;
                if (c != var)
                    throw c;
            }
        }
        if (no_vars)
            free_term = i->data->coeff;
        prev = i;
        i = i->next;
    }
    if (var == '0')
        throw var;

    free_term = prev->data->coeff;
    std::vector<float> res;
//    if (free_term) {
        for (int n = 0; n * n <= free_term; ++n) {
            if (n && free_term % n != 0)
                continue;
            if (CheckRoot(p, n, var))
                res.push_back(n);
            if (!n)
                continue;
            if (CheckRoot(p, -n, var))
                res.push_back(-n);
            if (free_term / n == n)
                continue;
            if (CheckRoot(p, free_term / n, var))
                res.push_back(-n);
            if (CheckRoot(p, -free_term / n, var))
                res.push_back(-n);
        }
//    } else {
//        res.push_back(0);

//    }
    return res;
}

sf::String WindowManager::GetVariables(Node<Term *> *p) {
    Node<Term *> *i = p;
    std::set<char> vars;
    while (i != nullptr) {
        for (char c = 'a'; c <= 'z'; ++c) {
            if (i->data->power[c])
                vars.insert(c);
        }
        i = i->next;
    }
    sf::String res = "";
    for (auto c: vars)
        res += c;
    return res;
}

Node<Term *> *WindowManager::GetDerivative(Node<Term *> *p, char var) {
    Node<Term *> *res = new Node<Term *>(new Term);
    Node<Term *> *res_it = res;
    Node<Term *> *p_it = p;
    while (p_it != nullptr) {
        res_it->data->power = p_it->data->power;
        res_it->data->coeff = p_it->data->coeff;
        res_it->data->sign = p_it->data->sign;
        p_it = p_it->next;
        if (p_it != nullptr)
            res_it->next = new Node<Term *>(new Term);
        res_it = res_it->next;
    }
    res_it = res;
    while (res_it != nullptr) {
        res_it->data->coeff *= res_it->data->power[var];
        --res_it->data->power[var];
        res_it = res_it->next;
    }
    Node<Term *> *prev = nullptr;
    res_it = res;
    while (res_it != nullptr) {
        if (!res_it->data->coeff) {
            if (prev != nullptr)
                prev->next = res_it->next;
            else
                res = res_it->next;
            delete res_it;
        }
        prev = res_it;
        res_it = res_it->next;
    }
    return res;
}

sf::String WindowManager::PolynomToString(Node<Term *> *p) {
    Node<Term *> *j = p;
    sf::String res;
    bool first = true;
    while (j != nullptr) {
        if (j->data->coeff == 0) {
            if (!first)
                res += "+";
            res += "0";
            j = j->next;
            first = false;
            continue;
        }
        sf::String term_str;
        if (j->data->sign == -1 || !first) {
            term_str += (j->data->sign == 1 ? "+" : "-");
        }
        if (fabs(j->data->coeff) != 1) {
            term_str += ToStr(j->data->coeff);
        }
        bool no_powers = true;
        for (char c = 'a'; c <= 'z'; ++c) {
            if (j->data->power[c]) {
                no_powers = false;
                term_str += c;
                if (j->data->power[c] != 1)
                    term_str += "^" + std::to_string(j->data->power[c]);
            }
        }
        if (no_powers && j->data->coeff <= 1)
            term_str += ToStr(j->data->coeff);
        res += term_str;
        j = j->next;
        first = false;
    }
    return res;
}

void WindowManager::PrintPolynom(Node<Term *> *&p) {
    Node<Term *> *it = p;
    while (it != nullptr) {
        if (it->data->sign == 1)
            std::cout << '+';
        else
            std::cout << '-';

        std::cout << it->data->coeff;
        for (char c = 'a'; c <= 'z'; ++c) {
            if (it->data->power[c]) {
                std::cout << c << '^' << it->data->power[c];
            }
        }

        it = it->next;
    }
}

void WindowManager::PrintBase(Node<Node<Term *> *> *&list) {
    if (list == nullptr)
        return;
    Node<Node<Term *> *> *it = list;
    int index = 1;
    while (it != nullptr) {
        std::cout << index << ". ";

        PrintPolynom(it->data);

        std::cout << '\n';
        it = it->next;
        ++index;
    }
    return;
}

void WindowManager::DrawUI(sf::RenderWindow &window,
                           Node<Node<Term *> *> *&list,
                           sf::Text input_text) {
    sf::String polynom_str;

    Node<Node<Term *> *> *i = list;
    int index = 1;
    while (i->next != nullptr) {
        sf::String
            line_str = std::to_string(index) + ". " + PolynomToString(i->data);
        int ind = kMaxInput;
        while (ind < line_str.getSize()) {
            line_str.insert(ind, '\n');
            ind += kMaxInput;
        }
        polynom_str += line_str + '\n';
        i = i->next;
        ++index;
    }

    polynom_text.setString(polynom_str);

    window.draw(screen);
    window.draw(input_field);
    window.draw(input_text);
    window.draw(polynom_text);
    window.draw(error_text);
    window.draw(result_text);
}

void WindowManager::Error(sf::String text) {
    error_text.setString(text);
}

void WindowManager::Result(sf::String text) {
    result_text.setString(text);
}

void WindowManager::SaveToFile(Node<Node<Term *> *> *&list,
                               sf::String file_name) {
    if (list == nullptr)
        return;
    std::ofstream fout(file_name + ".txt");
    Node<Node<Term *> *> *it = list;
    int index = 1;
    while (it->next != nullptr) {
        Node<Term *> *p = it->data;
        while (p != nullptr) {
            if (p->data->sign == 1)
                fout << '+';
            else
                fout << '-';

            fout << p->data->coeff;
            for (char c = 'a'; c <= 'z'; ++c) {
                if (p->data->power[c]) {
                    fout << c << '^' << p->data->power[c];
                }
            }

            p = p->next;
        }

        fout << '\n';
        it = it->next;
        ++index;
    }
    fout.close();
    return;
}

void WindowManager::LoadFromFile(Node<Node<Term *> *> *&list,
                                 Node<Node<Term *> *> *&right,
                                 sf::String file_name) {
    if (list != nullptr)
        delete list;
    list = new Node<Node<Term *> *>(new Node<Term *>(new Term()));
    right = list;
    std::ifstream fin(file_name + ".txt");
    while (!fin.eof()) {
        std::string s;
        fin >> s;
        if (s.empty())
            break;
        sf::String str = s;
        InputPolynom(right, str);
        right->next = new Node<Node<Term *> *>(new Node<Term *>(new Term()));
        right = right->next;
    }
    fin.close();
}