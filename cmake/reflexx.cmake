include(reflexx_compiler_check)

reflexx_check_feature_compiles(
    ReflectionTest
    "
    #include <experimental/meta>
    #include <type_traits>
    #include <concepts>
    int main() { std::meta::display_string_of(^^int); }
    "
    REQUIRED
)