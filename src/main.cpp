// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// CLI entry point (ticket T0.3).
// Responsibilities handled here:
//   - parse argv
//   - print usage on --help (stdout, exit 0)
//   - report errors on stderr and exit with code 84 on any failure
//   - delegate scene loading / rendering to future modules (T0.10+)

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

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

int fail(std::string_view message)
{
    std::cerr << "raytracer: " << message << '\n';
    printUsage(std::cerr);
    return kExitFailure;
}

bool isHelpFlag(std::string_view arg)
{
    return arg == "--help" || arg == "-h";
}

int runScene(const std::filesystem::path& scenePath)
{
    // Ticket T0.3 scope ends here: we only confirm the file is readable.
    // The actual parser (libconfig++) lands in T0.10, and the renderer
    // pipeline in T0.11+ (Scene, SceneBuilder, Camera, Renderer).
    std::cout << "raytracer: scene file accepted: "
              << scenePath.string() << '\n'
              << "raytracer: rendering pipeline not implemented yet "
              << "(see PLAN.md, tickets T0.10+)\n";
    return kExitSuccess;
}

}  // namespace

int main(int argc, char* argv[])
{
    if (argc < 2) {
        return fail("missing argument: a scene file is required");
    }
    if (argc > 2) {
        return fail("too many arguments: expected exactly one scene file");
    }

    const std::string_view arg = argv[1];

    if (isHelpFlag(arg)) {
        printUsage(std::cout);
        return kExitSuccess;
    }

    const std::filesystem::path scenePath{arg};

    std::error_code ec;
    if (!std::filesystem::exists(scenePath, ec) || ec) {
        return fail("scene file not found: " + std::string(arg));
    }
    if (!std::filesystem::is_regular_file(scenePath, ec) || ec) {
        return fail("scene path is not a regular file: " + std::string(arg));
    }

    return runScene(scenePath);
}
