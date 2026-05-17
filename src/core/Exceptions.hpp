#ifndef RAYTRACER_CORE_EXCEPTIONS_HPP
#define RAYTRACER_CORE_EXCEPTIONS_HPP

#include <stdexcept>

namespace rt {

class RaytracerException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class CliError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

class IOError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

class SceneParseError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

class InvalidConfigError : public SceneParseError {
public:
    using SceneParseError::SceneParseError;
};

class UnknownTypeError : public SceneParseError {
public:
    using SceneParseError::SceneParseError;
};

class PluginError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

class PluginLoadError : public PluginError {
public:
    using PluginError::PluginError;
};

class PluginAbiError : public PluginError {
public:
    using PluginError::PluginError;
};

class MathError : public RaytracerException {
public:
    using RaytracerException::RaytracerException;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_EXCEPTIONS_HPP
