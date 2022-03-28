#ifndef MNOGOCHLEN__WINDOWMANAGER_H_
#define MNOGOCHLEN__WINDOWMANAGER_H_

#include <SFML/Graphics.hpp>
#include "Node.h"
#include "Term.h"

class WindowManager {
 private:
  sf::Font font;
  sf::RectangleShape screen;
  sf::RectangleShape input_field;
  sf::Text polynom_text;
  sf::Text error_text;
  sf::Text result_text;
 public:
  WindowManager(sf::Font font);

  void AddToPolynom(Node<Node<Term *> *> *&list, Term *&term);
  void InputPolynom(Node<Node<Term *> *> *&list, sf::String s);
  void ExpInputPolynom(Node<Node<Term *> *> *&list, sf::String s);

  Node<Term *>* MultiplyPolynoms(Node<Term *> *p1, Node<Term *> *p2);
  Node<Term *>* SumPolynoms(Node<Term *> *p1, Node<Term *> *p2);
  Node<Node<Term *> *> *DividePolynoms(Node<Term *> *p, Node<Term *> *q, char var);
  float GetValue(Node<Term *> *p1, std::vector<float> values);
  bool CheckRoot(Node<Term *> *p, int n, char var);
  std::vector<float> GetRoots(Node<Term *> *p);
  sf::String GetVariables(Node<Term *> *p);
  Node<Term *> *GetDerivative(Node<Term *> *p, char var);

  sf::String PolynomToString(Node<Term *> *p);
  void PrintPolynom(Node<Term *> *&p);
  void PrintBase(Node<Node<Term *> *> *&list);

  void DrawUI(sf::RenderWindow &window, Node<Node<Term *> *> *&list, sf::Text input_text);
  void Error(sf::String text);
  void Result(sf::String text);
  bool IsResultPolynom();
  void SaveToFile(Node<Node<Term *> *> *&list, sf::String file_name);
  void LoadFromFile(Node<Node<Term *> *> *&list, Node<Node<Term *> *> *&right, sf::String file_name);
};

#endif //MNOGOCHLEN__WINDOWMANAGER_H_
