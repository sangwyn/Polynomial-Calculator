#include <SFML/Graphics.hpp>
#include <cmath>
#include "Node.h"
#include "Term.h"
#include "WindowManager.h"
#include "DefineConstants.h"
#include "Functions.h"
#include "Button.h"

#include <iostream>

int main() {
    Node<Node<Term *> *>
        *base = new Node<Node<Term *> *>(new Node<Term *>(new Term()));
    Node<Node<Term *> *> *right = base;
    int polynomials_count = 0;

    sf::RenderWindow
        window(sf::VideoMode(1600, 900),
               "Polynomial Calculator", sf::Style::Close);
    sf::Image icon;
    icon.loadFromFile("Assets/logo.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::Font font;
    font.loadFromFile("Assets/Hack-Regular.ttf");

    WindowManager window_manager(font);

    sf::Text input_text;
    input_text.setFont(font);
    input_text.setPosition(50.0f, 800.0f);
    input_text.setFillColor(kUnselectedColor);
    sf::String last_input;

    Node<Node<Term *> *> *last_res = nullptr;

    std::vector<Button *> buttons;
    buttons.push_back(new Button({900, 400},
                                 font,
                                 "Sum two polynomials (<i> <j>)"));
    buttons.push_back(new Button({900, 450},
                                 font,
                                 "Multiply two polynomials (<i> <j>)"));
    buttons.push_back(new Button({900, 500},
                                 font,
                                 "Get value in point (alphabet order) \n(<i> <val1> ... <valN>)"));
    buttons.push_back(new Button({900, 585},
                                 font,
                                 "Get derivative (<i> <var>)"));
    buttons.push_back(new Button({900, 635},
                                 font,
                                 "Divide two polynomials (<i> <j>)"));
    buttons.push_back(new Button({900, 685}, font, "Get roots (<i>)"));
    buttons.push_back(new Button({900, 250},
                                 font,
                                 "Save base to file (<filename>)"));
    buttons.push_back(new Button({900, 300},
                                 font,
                                 "Load base from file (<filename>)"));
    buttons.push_back(new Button({900, 200},
                                 font,
                                 "Add last polynomial result to base"));
    buttons.push_back(new Button({900, 150},
                                 font,
                                 "Delete polynomial from base (<i>)"));

    float char_size = input_text.getCharacterSize();
    sf::RectangleShape cursor;
    cursor.setSize({2, char_size});
    cursor.setFillColor(kUnselectedColor);
    sf::Vector2f cursor_pos({input_text.getPosition().x,
                             input_text.getPosition().y + 5.0f});
    cursor.setPosition(cursor_pos);
    sf::Clock cursor_blinking;
    float blink_interval = 0.6f;
    int cursor_x = 0;
    char_size = 18;

    float delta_time;
    sf::Clock delta;
    while (window.isOpen()) {
        delta_time = delta.getElapsedTime().asSeconds();
        delta.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered) {
                if (input_text.getString().getSize() <= kMaxInput
                    && event.text.unicode != 8
                    && event.text.unicode != 13 && event.text.unicode != 9) {
                    sf::String str = input_text.getString();
                    str.insert(std::max(cursor_x, 0), event.text.unicode);
                    input_text.setString(str);
                    ++cursor_x;
                    cursor.setFillColor(kUnselectedColor);
                    cursor_blinking.restart();
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (input_text.getString().getSize() > 0 && cursor_x
                    && event.key.code == sf::Keyboard::BackSpace) {
                    sf::String str = input_text.getString();
                    str.erase(cursor_x - 1, 1);
                    input_text.setString(str);
                    cursor_x = std::max(0, cursor_x - 1);
                    cursor.setFillColor(kUnselectedColor);
                    cursor_blinking.restart();
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    window_manager.Error("");
                    if (input_text.getString()[0] != '/') {
                        try {
                            if (polynomials_count + 1 > kMaxPolynoms)
                                throw polynomials_count;
                            window_manager.ExpInputPolynom(right,
                                                           input_text.getString());
                            right->next =
                                new Node<Node<Term *> *>(new Node<Term *>(new Term()));
                            right = right->next;
                            ++polynomials_count;
                            last_input = input_text.getString();
                            input_text.setString("");
                            cursor_x = 0;
                        }
                        catch (sf::Uint32 c) {
                            sf::String error_text =
                                "Unexpected symbol in polynomial: '";
                            error_text += c;
                            error_text += "'.";
                            window_manager.Error(error_text);
                            delete right->data;
                            right->data = new Node<Term *>(new Term());

                        }
                        catch (int cnt) {
                            window_manager.Error(
                                "Polynomials count reached maximum.");
                        }
                    } else {
                        sf::String cmd;
                        int i = 1;
                        for (; i < input_text.getString().getSize(); ++i) {
                            if (input_text.getString()[i] == ' ')
                                break;
                            cmd += input_text.getString()[i];
                        }
                        ++i;
                        try {
                            if (cmd == "sum") {
                                try {
                                    int n1 = -1, n2 = -1;
                                    bool got_first = false;
                                    try {
                                        for (; i
                                                   < input_text.getString().getSize();
                                               ++i) {
                                            if (input_text.getString()[i]
                                                == ' ') {
                                                if (!got_first) {
                                                    got_first = true;
                                                    continue;
                                                }
                                                break;
                                            }
                                            if (std::isdigit(input_text.getString()[i])) {
                                                if (!got_first) {
                                                    if (n1 == -1)
                                                        n1 = 0;
                                                    n1 = 10 * n1
                                                        + (input_text.getString()[i]
                                                            - '0');
                                                } else {
                                                    if (n2 == -1)
                                                        n2 = 0;
                                                    n2 = 10 * n2
                                                        + (input_text.getString()[i]
                                                            - '0');
                                                }
                                            } else {
                                                throw input_text.getString()[i];
                                            }
                                        }
                                    }
                                    catch (sf::Uint32 c) {
                                        char ch = c;
                                        sf::String error_text =
                                            "Unexpected symbol in arguments: '";
                                        error_text += ch;
                                        error_text +=
                                            "'.\nTwo positive integer numbers expected.";
                                        window_manager.Error(error_text);
                                    }
                                    if (n1 == -1 || n1 < 1
                                        || n1 > polynomials_count)
                                        throw n1;
                                    if (n2 == -1 || n2 < 1
                                        || n2 > polynomials_count)
                                        throw n2;
                                    Node<Node<Term *> *> *it = base;
                                    Node<Term *> *p1 = nullptr, *p2 = nullptr;
                                    int index = 1;
                                    while (it != nullptr) {
                                        if (index == n1) {
                                            p1 = it->data;
                                            if (p2 != nullptr)
                                                break;
                                        }
                                        if (index == n2) {
                                            p2 = it->data;
                                            if (p1 != nullptr)
                                                break;
                                        }
                                        it = it->next;
                                        ++index;
                                    }
//                                    delete last_res;
                                    if (last_res == nullptr) {
                                        last_res = new Node<Node<Term *> *>;
                                    }
                                    if (last_res->data != nullptr)
                                        last_res->data = nullptr;
                                    if (last_res->next != nullptr)
                                        last_res->next = nullptr;
                                    last_res->data =
                                        window_manager.SumPolynoms(p1, p2);

                                    window_manager.Result(window_manager.PolynomToString(
                                        last_res->data));
                                    last_input = input_text.getString();
                                    input_text.setString("");
                                    cursor_x = 0;
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Wrong argument: '" + std::to_string(n)
                                            + "'.\nIndex out of range.");
                                }
                            } else if (cmd == "mult") {
                                try {
                                    int n1 = -1, n2 = -1;
                                    bool got_first = false;
                                    try {
                                        for (; i
                                                   < input_text.getString().getSize();
                                               ++i) {
                                            if (input_text.getString()[i]
                                                == ' ') {
                                                if (!got_first) {
                                                    got_first = true;
                                                    continue;
                                                }
                                                break;
                                            }
                                            if (std::isdigit(input_text.getString()[i])) {
                                                if (!got_first) {
                                                    if (n1 == -1)
                                                        n1 = 0;
                                                    n1 = 10 * n1
                                                        + (input_text.getString()[i]
                                                            - '0');
                                                } else {
                                                    if (n2 == -1)
                                                        n2 = 0;
                                                    n2 = 10 * n2
                                                        + (input_text.getString()[i]
                                                            - '0');
                                                }
                                            } else {
                                                throw input_text.getString()[i];
                                            }
                                        }
                                    }
                                    catch (sf::Uint32 c) {
                                        char ch = c;
                                        sf::String error_text =
                                            "Unexpected symbol in arguments: '";
                                        error_text += ch;
                                        error_text +=
                                            "'.\nTwo positive integer numbers expected.";
                                        window_manager.Error(error_text);
                                    }
                                    if (n1 == -1 || n1 < 1
                                        || n1 > polynomials_count)
                                        throw n1;
                                    if (n2 == -1 || n2 < 1
                                        || n2 > polynomials_count)
                                        throw n2;
                                    Node<Node<Term *> *> *it = base;
                                    Node<Term *> *p1 = nullptr, *p2 = nullptr;
                                    int index = 1;
                                    while (it != nullptr) {
                                        if (index == n1) {
                                            p1 = it->data;
                                            if (p2 != nullptr)
                                                break;
                                        }
                                        if (index == n2) {
                                            p2 = it->data;
                                            if (p1 != nullptr)
                                                break;
                                        }
                                        it = it->next;
                                        ++index;
                                    }
//                                    delete last_res;
                                    if (last_res == nullptr) {
                                        last_res = new Node<Node<Term *> *>;
                                    }
                                    if (last_res->data != nullptr)
                                        last_res->data = nullptr;
                                    if (last_res->next != nullptr)
                                        last_res->next = nullptr;
                                    last_res->data =
                                        window_manager.MultiplyPolynoms(p1, p2);

                                    window_manager.Result(window_manager.PolynomToString(
                                        last_res->data));
                                    last_input = input_text.getString();
                                    input_text.setString("");
                                    cursor_x = 0;
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Wrong argument: '" + std::to_string(n)
                                            + "'.\nIndex out of range.");
                                }
                            } else if (cmd == "divide") {
                                try {
                                    int n1 = -1, n2 = -1;
                                    bool got_first = false;
                                    try {
                                        for (; i
                                                   < input_text.getString().getSize();
                                               ++i) {
                                            if (input_text.getString()[i]
                                                == ' ') {
                                                if (!got_first) {
                                                    got_first = true;
                                                    continue;
                                                }
                                                break;
                                            }
                                            if (std::isdigit(input_text.getString()[i])) {
                                                if (!got_first) {
                                                    if (n1 == -1)
                                                        n1 = 0;
                                                    n1 = 10 * n1
                                                        + (input_text.getString()[i]
                                                            - '0');
                                                } else {
                                                    if (n2 == -1)
                                                        n2 = 0;
                                                    n2 = 10 * n2
                                                        + (input_text.getString()[i]
                                                            - '0');
                                                }
                                            } else {
                                                throw input_text.getString()[i];
                                            }
                                        }
                                    }
                                    catch (sf::Uint32 c) {
                                        char ch = c;
                                        sf::String error_text =
                                            "Unexpected symbol in arguments: '";
                                        error_text += ch;
                                        error_text +=
                                            "'.\nTwo positive integer numbers expected.";
                                        window_manager.Error(error_text);
                                    }
                                    if (n1 == -1 || n1 < 1
                                        || n1 > polynomials_count)
                                        throw n1;
                                    if (n2 == -1 || n2 < 1
                                        || n2 > polynomials_count)
                                        throw n2;
                                    Node<Node<Term *> *> *it = base;
                                    Node<Term *> *p1 = nullptr, *p2 = nullptr;
                                    int index = 1;
                                    while (it != nullptr) {
                                        if (index == n1) {
                                            p1 = it->data;
                                            if (p2 != nullptr)
                                                break;
                                        }
                                        if (index == n2) {
                                            p2 = it->data;
                                            if (p1 != nullptr)
                                                break;
                                        }
                                        it = it->next;
                                        ++index;
                                    }
//                                    delete last_res;
                                    if (window_manager.GetVariables(p1)
                                        != window_manager.GetVariables(p2)
                                        || window_manager.GetVariables(p1).getSize()
                                            > 1) {
                                        throw window_manager.GetVariables(p1);
                                    }
                                    last_res =
                                        window_manager.DividePolynoms(p1,
                                                                      p2,
                                                                      window_manager.GetVariables(
                                                                          p1)[0]);

                                    window_manager.Result("Quotient: " +
                                        window_manager.PolynomToString(
                                            last_res->data) + "\nRemainder: "
                                            + window_manager.PolynomToString(
                                                last_res->next->data));
                                    last_input = input_text.getString();
                                    input_text.setString("");
                                    cursor_x = 0;
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Wrong argument: '" + std::to_string(n)
                                            + "'.\nIndex out of range.");
                                }
                                catch (sf::String vars) {
                                    window_manager.Error(
                                        "Only one variable polynomials \nare allowed.");
                                }
                            } else if (cmd == "del") {
                                int n = -1;
                                try {
                                    for (; i
                                               < input_text.getString().getSize();
                                           ++i) {
                                        if (std::isdigit(input_text.getString()[i])) {
                                            if (n == -1)
                                                n = 0;
                                            n = 10 * n
                                                + (input_text.getString()[i]
                                                    - '0');
                                        } else {
                                            throw input_text.getString()[i];
                                        }
                                    }
                                    if (n < 1 || n > polynomials_count)
                                        throw n;
                                    Node<Node<Term *> *> *it = base;
                                    int index = 1;
                                    while (it != nullptr) {
                                        if (index + 1 >= n)
                                            break;
                                        ++index;
                                        it = it->next;
                                    }
                                    --polynomials_count;
                                    Node<Node<Term *> *> *deleting = it->next;
                                    if (deleting == right)
                                        right = it;
                                    it->next = deleting->next;
                                    delete deleting;
                                    last_input = input_text.getString();
                                    input_text.setString("");
                                    cursor_x = 0;
                                }
                                catch (sf::Uint32 c) {
                                    char ch = c;
                                    sf::String error_text =
                                        "Unexpected symbol in arguments: '";
                                    error_text += ch;
                                    error_text +=
                                        "'.\nOne positive integer number expected.";
                                    window_manager.Error(error_text);
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Wrong argument: '" + std::to_string(n)
                                            + "'.\nIndex out of range.");
                                }
                            } else if (cmd == "get") {
                                std::vector<float> values;
                                float n = -1;
                                int sign = 0;
                                float fraction = 10;
                                bool getting_fraction = false;
                                int index = 0;
                                try {
                                    for (; i < input_text.getString().getSize();
                                           ++i) {
                                        if (input_text.getString()[i] == ' ') {
                                            ++i;
                                            break;
                                        }
                                        if (std::isdigit(input_text.getString()[i])) {
                                            index = 10 * index
                                                + (input_text.getString()[i]
                                                    - '0');
                                        } else {
                                            std::cout << "!"
                                                      << input_text.getString()[i]
                                                      << "!\n";
                                            throw input_text.getString()[i];
                                        }
                                    }
                                    if (index < 1
                                        || index > polynomials_count) {
                                        throw index;
                                    }
                                    for (; i < input_text.getString().getSize();
                                           ++i) {
                                        if (input_text.getString()[i] == ' ') {
                                            if (n == -1)
                                                throw input_text.getString()[i];
                                            values.push_back(n * sign);
                                            n = -1;
                                            sign = 0;
                                            getting_fraction = false;
                                            fraction = 10;
                                        } else if (std::isdigit(input_text.getString()[i])) {
                                            if (n == -1)
                                                n = 0;
                                            if (sign == 0)
                                                sign = 1;
                                            if (!getting_fraction) {
                                                n = 10 * n
                                                    + (float) (
                                                        input_text.getString()[i]
                                                            - '0');
                                            } else {
                                                n = (fraction * n + (float) (
                                                    input_text.getString()[i]
                                                        - '0')) / fraction;
                                                fraction *= 10;
                                            }
                                        } else if (input_text.getString()[i]
                                            == '-') {
                                            if (sign != 0)
                                                throw input_text.getString()[i];
                                            sign = -1;
                                        } else if (input_text.getString()[i]
                                            == '+') {
                                            if (sign != 0)
                                                throw input_text.getString()[i];
                                            sign = 1;
                                        } else if (input_text.getString()[i]
                                            == '.') {
                                            if (getting_fraction)
                                                throw input_text.getString()[i];
                                            if (sign == 0)
                                                sign = 1;
                                            getting_fraction = true;
                                        } else {
                                            throw input_text.getString()[i];
                                        }
                                    }
                                    if (n == -1)
                                        throw input_text.getString()[i - 1];
                                    values.push_back(n * sign);

                                    Node<Node<Term *> *> *it = base;
                                    int ind = 1;
                                    while (it != nullptr) {
                                        if (ind == index)
                                            break;
                                        it = it->next;
                                        ++ind;
                                    }
                                    try {
                                        Node<Term *>
                                            *res = new Node<Term *>(new Term);
                                        res->data->coeff =
                                            window_manager.GetValue(it->data,
                                                                    values);
//                                        delete last_res;
                                        if (last_res == nullptr) {
                                            last_res = new Node<Node<Term *> *>;
                                        }
                                        if (last_res->data != nullptr)
                                            last_res->data = nullptr;
                                        if (last_res->next != nullptr)
                                            last_res->next = nullptr;
                                        last_res->data =
                                            new Node<Term *>(new Term);
                                        last_res->data->data->coeff =
                                            res->data->coeff;
                                        if (last_res->data->data->coeff < 0) {
                                            last_res->data->data->sign = -1;
                                            last_res->data->data->coeff =
                                                fabs(last_res->data->data->coeff);
                                        }
                                        window_manager.Result(window_manager.PolynomToString(
                                            last_res->data));
                                        last_input = input_text.getString();
                                        input_text.setString("");
                                        cursor_x = 0;
                                    }
                                    catch (int n) {
                                        window_manager.Error(
                                            "Values count doesn't match \nvariables count.");
                                    }
                                }
                                catch (sf::Uint32 c) {
                                    sf::String
                                        error_str = "Invalid argument: '";
                                    error_str += c;
                                    error_str +=
                                        "'. \nExpected index and array of float \nnumbers.";
                                    window_manager.Error(error_str);
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Invalid index: '" + std::to_string(n)
                                            + "'. \nIndex out of range");
                                }
                            } else if (cmd == "roots") {
                                int n = -1;
                                try {
                                    for (; i
                                               < input_text.getString().getSize();
                                           ++i) {
                                        if (std::isdigit(input_text.getString()[i])) {
                                            if (n == -1)
                                                n = 0;
                                            n = 10 * n
                                                + (input_text.getString()[i]
                                                    - '0');
                                        } else {
                                            throw input_text.getString()[i];
                                        }
                                    }
                                    if (n < 1 || n > polynomials_count)
                                        throw n;
                                    try {
                                        Node<Node<Term *> *> *p = base;
                                        int index = 1;
                                        while (p != nullptr) {
                                            if (index == n)
                                                break;
                                            p = p->next;
                                            ++index;
                                        }
                                        std::vector<float> res =
                                            window_manager.GetRoots(p->data);
                                        sf::String str = "";
                                        if (res.empty()) {
                                            str = "No roots";
                                        } else {
                                            for (auto x: res)
                                                str += (ToStr(x) + ' ');
                                        }
                                        window_manager.Result(str);
                                        last_input = input_text.getString();
                                        input_text.setString("");
                                        cursor_x = 0;
                                    }
                                    catch (float coeff) {
                                        window_manager.Error(
                                            "Fractional coefficient found: '"
                                                + ToStr(coeff) + "'");
                                    }
                                    catch (char var) {
                                        window_manager.Error(
                                            "Only one variable polynomials \nare allowed.");
                                    }
                                }
                                catch (sf::Uint32 c) {
                                    char ch = c;
                                    sf::String error_text =
                                        "Unexpected symbol in arguments: '";
                                    error_text += ch;
                                    error_text +=
                                        "'.\nOne positive integer number expected.";
                                    window_manager.Error(error_text);
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Wrong argument: '" + std::to_string(n)
                                            + "'.\nIndex out of range.");
                                }
                            } else if (cmd == "deriv") {
                                int n = -1;
                                try {
                                    for (; i
                                               < input_text.getString().getSize();
                                           ++i) {
                                        if (std::isdigit(input_text.getString()[i])) {
                                            if (n == -1)
                                                n = 0;
                                            n = 10 * n
                                                + (input_text.getString()[i]
                                                    - '0');
                                        } else if (input_text.getString()[i]
                                            == ' ') {
                                            break;
                                        } else {
                                            throw input_text.getString()[i];
                                        }
                                    }
                                    if (n < 1 || n > polynomials_count)
                                        throw n;
                                    try {
                                        ++i;
                                        char var = '0';
                                        for (; i
                                                   < input_text.getString().getSize();
                                               ++i) {
                                            if (var != '0')
                                                throw input_text.getString()[i];
                                            if (input_text.getString()[i] >= 'a'
                                                && input_text.getString()[i]
                                                    <= 'z') {
                                                var = input_text.getString()[i];
                                            }
                                        }
                                        if (var == '0')
                                            throw input_text.getString()[
                                                input_text.getString().getSize()
                                                    - 1];
                                        Node<Node<Term *> *> *p = base;
                                        int index = 1;
                                        while (p != nullptr) {
                                            if (index == n)
                                                break;
                                            p = p->next;
                                            ++index;
                                        }
                                        sf::String p_vars =
                                            window_manager.GetVariables(p->data);
                                        bool found_var = false;
                                        for (auto c: p_vars) {
                                            if (c == var) {
                                                found_var = true;
                                                break;
                                            }
                                            if (c > var) {
                                                throw var;
                                            }
                                        }
                                        if (!found_var)
                                            throw var;
                                        if (last_res == nullptr) {
                                            last_res = new Node<Node<Term *> *>;
                                        }
                                        if (last_res->data != nullptr)
                                            last_res->data = nullptr;
                                        if (last_res->next != nullptr)
                                            last_res->next = nullptr;
                                        last_res->data =
                                            window_manager.GetDerivative(p->data,
                                                                         var);
                                        sf::String str =
                                            window_manager.PolynomToString(
                                                last_res->data);
                                        window_manager.Result(str);
                                        last_input = input_text.getString();
                                        input_text.setString("");
                                        cursor_x = 0;
                                    }
                                    catch (char var) {
                                        window_manager.Error(
                                            "No such variable in polynomial.");
                                    }
                                }
                                catch (sf::Uint32 c) {
                                    char ch = c;
                                    sf::String error_text =
                                        "Unexpected symbol in arguments: '";
                                    error_text += ch;
                                    error_text +=
                                        "'.\nOne positive integer number and \nchar expected.";
                                    window_manager.Error(error_text);
                                }
                                catch (int n) {
                                    window_manager.Error(
                                        "Wrong argument: '" + std::to_string(n)
                                            + "'.\nIndex out of range.");
                                }
                            } else if (cmd == "save") {
                                sf::String file_name;
                                for (; i < input_text.getString().getSize();
                                       ++i) {
                                    file_name += input_text.getString()[i];
                                }
                                window_manager.SaveToFile(base, file_name);
                                window_manager.Result(
                                    "Saved to " + file_name + ".txt");
                                last_input = input_text.getString();
                                input_text.setString("");
                                cursor_x = 0;
                            } else if (cmd == "load") {
                                try {
                                    sf::String file_name;
                                    for (; i < input_text.getString().getSize();
                                           ++i) {
                                        file_name += input_text.getString()[i];
                                    }
                                    if (!FileExists(file_name + ".txt"))
                                        throw file_name;
                                    window_manager.LoadFromFile(base, right,
                                                                file_name);
                                    window_manager.Result(
                                        "Loaded from " + file_name + ".txt");
                                    last_input = input_text.getString();
                                    input_text.setString("");
                                    cursor_x = 0;
                                    Node<Node<Term *> *> *p = base;
                                    polynomials_count = -1;
                                    while (p != nullptr) {
                                        ++polynomials_count;
                                        p = p->next;
                                    }
                                }
                                catch (sf::String file_name) {
                                    window_manager.Error("File '" + file_name
                                                             + "' doesn't exist.");
                                }
                            } else if (cmd == "add") {
                                try {
                                    if (last_res == nullptr || last_res->data == nullptr)
                                        throw last_res;
                                    if (polynomials_count + 1 > kMaxPolynoms)
                                        throw polynomials_count;
                                    right->data = last_res->data;
                                    right->next =
                                        new Node<Node<Term *> *>(new Node<Term *>(
                                            new Term()));
                                    right = right->next;
                                    ++polynomials_count;
                                    if (last_res->next != nullptr) {
                                        if (polynomials_count >= kMaxPolynoms)
                                            throw polynomials_count;
                                        right->data = last_res->next->data;
                                        right->next =
                                            new Node<Node<Term *> *>(new Node<
                                                Term *>(
                                                new Term()));
                                        right = right->next;
                                        ++polynomials_count;
                                    }
                                    last_input = input_text.getString();
                                    input_text.setString("");
                                    cursor_x = 0;
                                }
                                catch (Node<Term *> *p) {
                                    window_manager.Error(
                                        "No available result to add.");
                                }
                                catch (int cnt) {
                                    window_manager.Error(
                                        "Polynomials count reached maximum.");
                                }
                            } else {
                                throw cmd;
                            }
                        }
                        catch (sf::String cmd) {
                            window_manager.Error(
                                "Unknown command: '" + cmd + "'");
                        }
                    }
//                    window_manager.PrintBase(base);
//                    std::cout << '\n';
                }
                if (event.key.code == sf::Keyboard::Up) {
                    input_text.setString(last_input);
                    cursor_x = input_text.getString().getSize();
                }
                if (event.key.code == sf::Keyboard::Left) {
                    cursor_x = std::max(0, cursor_x - 1);
                    cursor.setFillColor(kUnselectedColor);
                    cursor_blinking.restart();
                }
                if (event.key.code == sf::Keyboard::Right) {
                    cursor_x = std::min((int) input_text.getString().getSize(),
                                        cursor_x + 1);
                    cursor.setFillColor(kUnselectedColor);
                    cursor_blinking.restart();
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                for (int i = 0; i < buttons.size(); ++i) {
                    if (buttons[i]->Selected()) {
                        if (i == 0) {
                            input_text.setString("/sum ");
                        } else if (i == 1) {
                            input_text.setString("/mult ");
                        } else if (i == 2) {
                            input_text.setString("/get ");
                        } else if (i == 3) {
                            input_text.setString("/deriv ");
                        } else if (i == 4) {
                            input_text.setString("/divide ");
                        } else if (i == 5) {
                            input_text.setString("/roots ");
                        } else if (i == 6) {
                            input_text.setString("/save ");
                        } else if (i == 7) {
                            input_text.setString("/load ");
                        } else if (i == 8) {
                            try {
                                if (last_res == nullptr)
                                    throw last_res;
                                if (polynomials_count + 1 > kMaxPolynoms)
                                    throw polynomials_count;
                                right->data = last_res->data;
                                right->next =
                                    new Node<Node<Term *> *>(new Node<Term *>(
                                        new Term()));
                                right = right->next;
                                ++polynomials_count;
                                if (last_res->next != nullptr) {
                                    if (polynomials_count >= kMaxPolynoms)
                                        throw polynomials_count;
                                    right->data = last_res->next->data;
                                    right->next =
                                        new Node<Node<Term *> *>(new Node<Term *>(
                                            new Term()));
                                    right = right->next;
                                    ++polynomials_count;
                                }
                            }
                            catch (Node<Node<Term *> *> *p) {
                                window_manager.Error(
                                    "No available result to add.");
                            }
                            catch (int cnt) {
                                window_manager.Error(
                                    "Polynomials count reached maximum.");
                            }
                        } else if (i == 9) {
                            input_text.setString("/del ");
                        }
                        cursor_x = input_text.getString().getSize();
                        break;
                    }
                }
            }
        }
        cursor.setPosition(cursor_pos.x + cursor_x * char_size, cursor_pos.y);

        window.clear(kBackgroundColor);

        window_manager.DrawUI(window, base, input_text);
        for (auto b: buttons) {
            b->SetSelection(b->MouseOver((sf::Vector2f) sf::Mouse::getPosition(
                window)));
            b->Draw(window);
        }
        if (cursor_blinking.getElapsedTime().asSeconds() > blink_interval) {
            if (cursor.getFillColor() == kUnselectedColor)
                cursor.setFillColor(sf::Color::Transparent);
            else
                cursor.setFillColor(kUnselectedColor);
            cursor_blinking.restart();
        }
        window.draw(cursor);

        window.display();
//        std::cout << polynomials_count << '\n';
    }

    return 0;
}
