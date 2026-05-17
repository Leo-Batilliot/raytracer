#pragma once
#ifdef RAYTRACER_SFML

#include <atomic>
#include <cstdint>
#include <vector>
#include <SFML/Graphics.hpp>
#include "core/Image.hpp"

namespace rt {

// Live preview window — updated tile-by-tile during rendering (T4.18).
// Built with SFML 3.x API.
class SFMLDisplay {
public:
    SFMLDisplay(int width, int height);

    // Copy Image pixels (linear→sRGB) into the window. Call from main thread.
    void update(const Image& img);

    // Process pending events. Returns false if window was closed / ESC pressed.
    bool pollEvents();

    bool isOpen()    const { return window_.isOpen(); }
    bool cancelled() const { return cancelled_; }

private:
    sf::RenderWindow             window_;
    sf::Texture                  texture_;
    sf::Sprite                   sprite_;
    std::vector<std::uint8_t>    rgba_;
    bool                         cancelled_ = false;

    static std::uint8_t toSRGB(float linear);
};

} // namespace rt
#endif // RAYTRACER_SFML
