// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Domain exception hierarchy (ticket T0.3.1).
//
// All raytracer errors inherit from rt::RaytracerException, itself derived
// from std::runtime_error. This gives us:
//   - A single `catch (const rt::RaytracerException&)` in main() that
//     captures every domain error and translates it into exit 84 + stderr.
//   - Rich categorisation: the renderer can distinguish a CLI error from
//     a plugin ABI mismatch from a scene parse failure without string
//     sniffing.
//   - Polymorphic error handling: specialised catch blocks can react to
//     a specific category when that becomes useful (e.g. plugin errors
//     may suggest running `make re` on ./plugins).
//
// Hot-path code (IPrimitive::intersect, IMaterial::shade, the tile loop)
// must NOT throw. Exceptions live at the boundaries: CLI, parser, plugin
// loader, IO. Plugins themselves expose a C ABI — exceptions cannot cross
// the .so frontier, so each plugin captures internally and returns an
// error code that the core reconverts into one of these exceptions.

#ifndef RAYTRACER_CORE_EXCEPTIONS_HPP
#define RAYTRACER_CORE_EXCEPTIONS_HPP

#include <stdexcept>

namespace rt {

// Root of the domain hierarchy. Catch this in main() to handle every
// raytracer-specific error uniformly.
class RaytracerException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// ---------------------------------------------------------------------
// CLI / arguments
// ---------------------------------------------------------------------
// Thrown from argv parsing: missing scene argument, too many arguments,
// unknown flag, etc.
class CliError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

// ---------------------------------------------------------------------
// IO / filesystem
// ---------------------------------------------------------------------
// Thrown when a file is missing, not a regular file, or unreadable.
class IOError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

// ---------------------------------------------------------------------
// Scene parsing (libconfig++)
// ---------------------------------------------------------------------
// Base class for every error produced while reading a .cfg file.
class SceneParseError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

// The config file is syntactically or semantically invalid (missing
// required field, wrong type, out-of-range value).
class InvalidConfigError : public SceneParseError {
public:
    using SceneParseError::SceneParseError;
};

// A scene references a primitive / light / material name that no loaded
// plugin provides.
class UnknownTypeError : public SceneParseError {
public:
    using SceneParseError::SceneParseError;
};

// ---------------------------------------------------------------------
// Plugins (.so loading / ABI contract)
// ---------------------------------------------------------------------
class PluginError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

// dlopen / dlsym failure, or plugin file not found.
class PluginLoadError : public PluginError {
public:
    using PluginError::PluginError;
};

// Plugin loaded correctly but its PluginDescriptor declares an ABI version
// incompatible with RAYTRACER_ABI_VERSION.
class PluginAbiError : public PluginError {
public:
    using PluginError::PluginError;
};

// ---------------------------------------------------------------------
// Math
// ---------------------------------------------------------------------
// e.g. Vector3::normalize() on a zero-length vector, singular matrix
// inversion, invalid Transform composition.
class MathError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_EXCEPTIONS_HPP
