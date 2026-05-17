#include <atomic>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <unistd.h>

#include "core/Camera.hpp"
#include "core/Exceptions.hpp"
#include "core/Image.hpp"
#include "core/Renderer.hpp"
#include "core/SceneBuilder.hpp"
#include "output/PPMWriter.hpp"
#include "parser/SceneParser.hpp"
#include "plugins/PluginLoader.hpp"

#ifdef RAYTRACER_SFML
#  include "output/SFMLDisplay.hpp"
#endif

namespace {

constexpr int kExitSuccess = 0;
constexpr int kExitFailure = 84;

constexpr std::string_view kUsage =
    "USAGE: ./raytracer <SCENE_FILE>\n"
    "  SCENE_FILE: scene configuration\n";

void printUsage(std::ostream& out) { out << kUsage; }

std::filesystem::path parseArguments(int argc, char* argv[])
{
    if (argc < 2)
        throw rt::CliError("missing argument: a scene file is required");
    if (argc > 2)
        throw rt::CliError("too many arguments: expected exactly one scene file");

    const std::string_view arg = argv[1];
    if (arg == "--help" || arg == "-h") {
        printUsage(std::cout);
        return {};
    }

    const std::filesystem::path p{arg};
    std::error_code ec;
    if (!std::filesystem::exists(p, ec) || ec)
        throw rt::IOError("scene file not found: " + std::string(arg));
    if (!std::filesystem::is_regular_file(p, ec) || ec)
        throw rt::IOError("scene path is not a regular file: " + std::string(arg));
    return p;
}

void runScene(const std::filesystem::path& scenePath)
{
    rt::PluginLoader loader("./plugins");

    rt::SceneParser  parser(scenePath);
    rt::SceneBuilder builder;
    builder.setCamera(parser.parseCamera());
    parser.parseMaterials(builder, loader);
    parser.parsePrimitives(builder, loader);
    parser.parseGroups(builder, loader);
    parser.parseLights(builder, loader);
    const rt::AASetting aa = parser.parseAntialiasing();
    rt::Scene scene = builder.build();

    rt::Camera   camera(scene.camera());
    rt::Image    img(camera.width(), camera.height());
    rt::Renderer renderer(scene, camera, aa);

#ifdef RAYTRACER_SFML
    // Interactive mode (terminal): open SFML window, save PPM to file.
    // Pipe mode (stdout redirected): render silently, write PPM to stdout.
    if (isatty(STDOUT_FILENO)) {
        rt::SFMLDisplay display(camera.width(), camera.height());
        std::atomic<bool> cancel{false};
        std::atomic<bool> done{false};

        std::thread renderThread([&]() {
            renderer.renderAll(img, &cancel);
            done.store(true);
        });

        while (display.isOpen() && !done.load()) {
            display.update(img);
            if (!display.pollEvents())
                cancel.store(true);
        }
        renderThread.join();

        if (display.isOpen()) {
            display.update(img);
            while (display.pollEvents()) {}
        }

        // Save PPM next to the scene file (avoids polluting the terminal).
        const auto outPath = scenePath.parent_path()
            / (scenePath.stem().string() + ".ppm");
        rt::PPMWriter::writeFile(outPath, img.width(), img.height(), img.pixels());
        std::cerr << "Saved: " << outPath.string() << '\n';
        return;
    }
    renderer.renderAll(img);
#else
    renderer.renderAll(img);
#endif

    rt::PPMWriter::write(std::cout, img.width(), img.height(), img.pixels());
}

} // namespace

int main(int argc, char* argv[])
{
    try {
        const auto scenePath = parseArguments(argc, argv);
        if (scenePath.empty()) return kExitSuccess;
        runScene(scenePath);
        return kExitSuccess;
    } catch (const rt::CliError& e) {
        std::cerr << "raytracer: " << e.what() << '\n';
        printUsage(std::cerr);
        return kExitFailure;
    } catch (const rt::RaytracerException& e) {
        std::cerr << "raytracer: " << e.what() << '\n';
        return kExitFailure;
    } catch (const std::exception& e) {
        std::cerr << "raytracer: unexpected error: " << e.what() << '\n';
        return kExitFailure;
    }
}
