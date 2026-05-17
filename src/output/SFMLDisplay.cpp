#include "output/SFMLDisplay.hpp"
#ifdef RAYTRACER_SFML

#include <cmath>

namespace rt {

SFMLDisplay::SFMLDisplay(int width, int height)
    : window_(sf::VideoMode({static_cast<unsigned>(width),
                              static_cast<unsigned>(height)}),
              "Raytracer")
    , texture_(sf::Vector2u{static_cast<unsigned>(width),
                             static_cast<unsigned>(height)})
    , sprite_(texture_)
    , rgba_(static_cast<std::size_t>(width * height * 4))
{
    window_.setFramerateLimit(30);
}

std::uint8_t SFMLDisplay::toSRGB(float v)
{
    // IEC 61966-2-1 linear → sRGB
    if (v <= 0.0f) return 0;
    if (v >= 1.0f) return 255;
    const float s = (v <= 0.0031308f)
        ? 12.92f * v
        : 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
    return static_cast<std::uint8_t>(s * 255.0f + 0.5f);
}

void SFMLDisplay::update(const Image& img)
{
    const auto& pix = img.pixels();
    for (std::size_t i = 0; i < pix.size(); ++i) {
        rgba_[i * 4 + 0] = toSRGB(pix[i].r);
        rgba_[i * 4 + 1] = toSRGB(pix[i].g);
        rgba_[i * 4 + 2] = toSRGB(pix[i].b);
        rgba_[i * 4 + 3] = 255;
    }
    texture_.update(rgba_.data());
    window_.clear();
    window_.draw(sprite_);
    window_.display();
}

bool SFMLDisplay::pollEvents()
{
    while (const std::optional event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
            cancelled_ = true;
            return false;
        }
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                window_.close();
                cancelled_ = true;
                return false;
            }
        }
    }
    return window_.isOpen();
}

} // namespace rt
#endif // RAYTRACER_SFML
