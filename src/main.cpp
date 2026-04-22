// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// CLI entry point.
//
// Ticket history:
//   - T0.3   : initial CLI (argv parsing, --help, exit 84, stderr).
//   - T0.3.1 : refactor to use the rt::RaytracerException hierarchy so
//              that every domain error bubbles up to a single try/catch
//              here, and each category can be translated to an exit code
//              + message without scattering `if (error) return 84;`.
//
// Boundary contract: main() is the ONLY place that catches. Every other
// module throws a subclass of rt::RaytracerException on failure. Hot
// paths (intersect, shade, tile loop) stay exception-free.

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

#include "core/Camera.hpp"
#include "core/Exceptions.hpp"
#include "core/Image.hpp"
#include "core/Renderer.hpp"
#include "core/SceneBuilder.hpp"
#include "output/PPMWriter.hpp"
#include "parser/SceneParser.hpp"
#include "plugins/PluginLoader.hpp"

namespace {

constexpr int kExitSuccess = 0;
constexpr int kExitFailure = 84;

constexpr std::string_view kUsage =
    "USAGE: ./raytracer <SCENE_FILE>\n"
    "  SCENE_FILE: scene configuration\n";

void printUsage(std::ostream& out)
{
    out << kUsage;
}

bool isHelpFlag(std::string_view arg)
{
    return arg == "--help" || arg == "-h";
}

std::filesystem::path validateScenePath(std::string_view arg)
{
    const std::filesystem::path scenePath{arg};

    std::error_code ec;
    if (!std::filesystem::exists(scenePath, ec) || ec) {
        throw rt::IOError("scene file not found: " + std::string(arg));
    }
    if (!std::filesystem::is_regular_file(scenePath, ec) || ec) {
        throw rt::IOError("scene path is not a regular file: "
                          + std::string(arg));
    }
    return scenePath;
}

// Parses argv and returns the validated scene path, or std::nullopt-like
// sentinel (empty path) when the caller already handled the arguments
// (e.g. --help). Throws rt::CliError / rt::IOError on failure.
std::filesystem::path parseArguments(int argc, char* argv[])
{
    if (argc < 2) {
        throw rt::CliError("missing argument: a scene file is required");
    }
    if (argc > 2) {
        throw rt::CliError(
            "too many arguments: expected exactly one scene file");
    }

    const std::string_view arg = argv[1];
    if (isHelpFlag(arg)) {
        printUsage(std::cout);
        return {};  // empty path signals "handled, exit success"
    }

    return validateScenePath(arg);
}

void runScene(const std::filesystem::path& scenePath)
{
    // T0.9: load plugins from ./plugins/ (none yet in Phase 0).
    rt::PluginLoader loader("./plugins");

    // T0.10 + T0.11: parse → build immutable Scene.
    rt::SceneParser parser(scenePath);
    rt::SceneBuilder builder;
    builder.setCamera(parser.parseCamera());
    rt::Scene scene = builder.build();

    // T0.14: render via the tile-based Renderer.
    rt::Camera camera(scene.camera());
    rt::Image img(camera.width(), camera.height());
    rt::Renderer renderer(scene, camera);
    renderer.renderAll(img);
    rt::PPMWriter::write(std::cout, img.width(), img.height(), img.pixels());
}

}  // namespace

int main(int argc, char* argv[])
{
    try {
        const auto scenePath = parseArguments(argc, argv);
        if (scenePath.empty()) {
            return kExitSuccess;  // --help path
        }
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
        // Last-resort safety net: any non-domain std::exception still
        // yields exit 84 with a diagnostic, never std::terminate.
        std::cerr << "raytracer: unexpected error: " << e.what() << '\n';
        return kExitFailure;
    }
}
