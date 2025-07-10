#include <reflexx/serializer.hpp>
#include <reflexx/type_handler.hpp>
#include <reflexx/backends/placeholder.hpp>

using MyBackend = reflexx::backends::placeholder_backend;
using StrictSerializer = reflexx::serializer<reflexx::serializer_settings::Strict(), MyBackend>;
using RelaxedSerializer = reflexx::serializer<reflexx::serializer_settings::Relaxed(), MyBackend>;