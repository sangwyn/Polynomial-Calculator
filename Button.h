#ifndef MNOGOCHLEN__BUTTON_H_
#define MNOGOCHLEN__BUTTON_H_

#include <SFML/Graphics.hpp>

class Button {
 private:
  sf::Font font;
  sf::RectangleShape rect;
  sf::Text text_inside;
  bool is_selected = false;
 public:
  Button(sf::Vector2f pos, sf::Font font, sf::String text);
  bool MouseOver(sf::Vector2f mouse_pos);
  void SetSelection(bool select);
  bool Selected();
  void Draw(sf::RenderWindow &window);
};

#endif //MNOGOCHLEN__BUTTON_H_
