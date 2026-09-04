[![experimental-x86_64-unknown-linux-gnu](https://github.com/P3RK4N/reflexx/actions/workflows/linux.yml/badge.svg)](https://github.com/P3RK4N/reflexx/actions/workflows/linux.yml)

# reflexx

C++26 reflection serializer. One line to serialize any struct.

Swap the output format — or the whole target — without touching your types.

Built in 2025 on Bloomberg’s experimental P2996 Clang, before reflection shipped in real compilers.

Private WIP, paused due to lack of time.

---

## Use it

```cpp
struct Player {
    std::string name;
    int         level;
    bool        online;
};

using S = relaxed_serializer;   // ready-made: relaxed rules + JSON

Player p{ "ana", 7, true };

std::string_view json = S::serialize(p);      // -> {"name":"ana","level":7,"online":true}
Player back = S::deserialize<Player>(json);   // parse into a new Player
S::deserialize(p, json);                      // or parse in-place
```

That is the whole API: `serialize`, `deserialize`. No macros, no `REGISTER(...)`, no describing your fields twice.

## Numbers

Release / O3, Celero, 30 samples, x86_64 Linux (emulated on ARM — read the ratios, not the absolute µs). Lower is better.

**Benchmark glossary**

- **Raw yyjson:** manual walk, direct JSON parser calls.
- **Backend only:** manual walk through the Reflexx backend adapter.
- **Reflexx serializer:** reflected walk through the same pluggable backend.
- **Zero-copy:** views point into the input buffer, which must outlive the result.

benchmark/data/citm_catalog.json (1.73 MB), deserialize:

| Path | µs | vs raw yyjson |
|---|---:|---:|
| Raw yyjson (manual) | 4,109 | — |
| Raw yyjson (manual, zero-copy) | 3,987 | -3% |
| Glaze | 4,451 | +8% |
| Glaze (zero-copy) | 4,349 | +6% |
| **Reflexx serializer (yyjson backend)** | **4,700** | **+14%** |
| **=> Reflexx serializer (yyjson backend, zero-copy)** | **4,596** | **+12%** |
| Reflexx backend only (manual walk) | 4,771 | +16% |
| Reflexx backend only (manual walk, zero-copy) | 4,673 | +14% |
| reflect-cpp | 7,562 | +84% |

**Reflexx lands ~1.5% **faster** than the handwritten traversal on the same backend: the reflection layer costs nothing, the backend is the only real cost.**

benchmark/data/profiles.json (5.1 MB), deserialize:

| Path | µs | vs raw yyjson |
|---|---:|---:|
| Raw yyjson (manual) | 16,721 | — |
| Raw yyjson (manual, zero-copy) | 7,466 | -55% |
| Glaze | 17,701 | +6% |
| Glaze (zero-copy) | 9,483 | -43% |
| **Reflexx serializer (yyjson backend)** | **14,002** | **-16%** |
| **=> Reflexx serializer (yyjson backend, zero-copy)** | **8,371** | **-50%** |
| reflect-cpp | 26,474 | +58% |

**On Profiles, Reflexx is **21% faster than Glaze** and **47% faster than reflect-cpp** on the owning-string path.**

---

## Build your own serializer type

A serializer is just three choices: **settings + backend + handlers**.

```cpp
template <serializer_settings Settings, IsBackendType Backend, IsTypeHandlerList Handlers>
class serializer;
```

`relaxed_serializer` and `strict_serializer` are presets.

When you want your own, start from a preset and override one knob. Settings are `consteval`, so this is all resolved at compile time:

```cpp
constexpr auto settings = serializer_settings::Relaxed()
    .set_enum_format_policy(enum_format_policy::integral)      // enums serialized as numbers, not strings
    .set_missing_field_policy(missing_field_policy::disallow); // or you can treat them as null

using MyJson = serializer<settings, backends::yyjson_backend>;
```

Want YAML instead of JSON? Change one word — same types, same call sites:

```cpp
using MyYaml = serializer<settings, backends::ryml_backend>;

MyYaml::serialize(p);   // name: ana\nlevel: 7\nonline: true
```

---

## Custom handlers

Sometimes one field should not follow the default layout. Say a `Vec2` you want written as `[x, y]`, not `{"x":..,"y":..}`:

```cpp
struct Vec2 { float x, y; };

struct Sprite {
    std::string texture;
    Vec2        position;   // <-- wants special treatment
};
```

Write a handler for just that type. It talks to the backend through small verbs (`begin_array`, `serialize_number`, …) and the same function works for both reading and writing:

```cpp
template <typename S, bool IsReading>
struct vec2_handler : type_handler<S, IsReading> {
    void serialize(Vec2& v) {
        this->begin_array();
            this->serialize_number(v.x);
            this->serialize_number(v.y);
        this->end_array();
    }
};
```

Plug it in front of the defaults. Reflexx uses the first handler that matches a type, so `Vec2` hits yours and everything else falls through to normal reflection:

```cpp
using Handlers = type_handler_list<vec2_handler, std_handler, default_handler>;
using S = serializer<serializer_settings::Relaxed(), backends::yyjson_backend, Handlers>;

S::serialize(Sprite{ "hero.png", { 4, 2 } });
// {"texture":"hero.png","position":[4,2]}
```

---

## Backends: the actual idea

Most serializers **are** their format. [Glaze](https://github.com/stephenberry/glaze) is superb, but JSON is welded into the core — parse, types, and output are one thing. Change the target and you change the library.

Reflexx keeps the walk over your data and the destination separate:

```
your struct ──► frontend (reflection) ──► handlers ──► BACKEND ──► anywhere
                 never sees JSON         never see JSON   the only format-aware part
```

A backend is a tiny contract: “open an object, write a key, write a number, write a string…”. yyjson and YAML are two backends. **The destination does not have to be text.**

### “I want my structs in an inspector, but I don’t want to write GUI code”

The verbs `write_number`, `write_string`, `write_bool` map perfectly onto ImGui widgets. So make ImGui a backend, and your existing struct walk paints itself:

```cpp
struct imgui_backend {
    std::string key_;

    void write_begin_object()             { ImGui::TreeNode(key_.c_str()); }
    void write_end_object()               { ImGui::TreePop(); }
    void write_key(std::string_view k)    { key_ = k; }
    void write_number(float f)            { ImGui::InputFloat(key_.c_str(), &f); }
    void write_number(std::int32_t n)     { ImGui::InputInt(key_.c_str(), &n); }
    void write_bool(bool b)               { ImGui::Checkbox(key_.c_str(), &b); }
    void write_string(std::string_view s) { ImGui::TextUnformatted(s.data()); }
    // ... rest of the IsBackendType contract ...
};

using Inspector = serializer<serializer_settings::Relaxed(), imgui_backend>;

Inspector::serialize(player);   // renders an editable panel — no per-field UI code
```

Same `Player`, same one-line call. JSON, YAML, and a live editor are just three backends behind one reflection walk.

---

## Build

x86_64 Linux, experimental Clang:

```sh
python3 setup_env.py use_prebuilt_clang
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=cmake/clang.toolchain.cmake -B build
cmake --build build --target reflexx_benchmark
./build/benchmark/reflexx_benchmark
```

— Bruno Perković
