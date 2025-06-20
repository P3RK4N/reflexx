TEXTUAL SERIALIZER (TEXER) - SPEC:
----------------------------------------------------------
- FRONTEND (FROM WHAT) - IMPLEMENT EVERYTHING
    - consists of Settings, Serializer, Annotations
        - Settings -> contains: policies; optional custom serializers per GLOBAL type; ...
            - object custom serializers spec: 
                - void serialize<backend_t>(const auto& obj, backend_t& backend)
                - void deserialize<backend_t>(auto& obj, backend_t& backend)
        - Serializer<Backend, Settings> 
            - specs:
                - std::string serialize(const auto& obj)
                - void deserialize(std::string auto& obj)
        - Annotations: [[ignore]], [[serialized_name]], [[custom_serializer->only for that LOCAL type]]

    - Serialization Modes based on previous parts (ascending by precedence):
        - Implicit/Default: automatic, controlled by settings
        - Explicit/Global: custom funcs per object type (from setting)
        - Explicit/Local: as annotation on the type

- SERIALIZATOR BACKEND INTERFACE (map-ish simple interface, common across yaml json xml, static polymorphism) - ONLY ADD FEW INTERFACE IMPLS
    - format independent e.g. rapidjson_pretty (implements backend by using rapidjson library in pretty print mode), rapidjson_compact, rapidyaml, yyjson_pretty
    - we only need to implement some popular interfaces per each format
    - need to plug in backend libraries (automatically done via cmake)

- SERIALIZATOR BACKENDS (TO WHAT) - popular libraries, they contain implementations for serializing specific formats - AUTOMATICALLY ADD VIA CMAKE
    - JSON - simdjson, yyjson, rapidjson, nlohmann/json
    - YAML - yaml-cpp, libyaml, rapidyaml
    - XML - tinyxml2, pugixml, xerces-c
    - ...
    - Custom - network serializers?
    - nothing to do, just add to project

---------------------------------------------------------

WORKFLOW

- serialize/deserialize()
    - if annotation (use it)    // Used for easy ignoring of nonstatic data members (e.g. mutexes, pointers, references, whatever)
    - else if custom (use it)   // Used for custom serialization of nonstatic data members (e.g. you can handle heap objects here flexibly)
    - else serializeDefault()   // Used implicitly if nothing is provided, relies on settings. 
                                // Predefined settings: 
                                //  strict (doesnt allow almost anything at compile time except flat structs),
                                //      -> you should adapt code so that it works on this settings because it is the fastest 
                                //  relaxed (maybe ignores sussy members such as pointers mutexes), 
                                //      -> for people who are too lazy to put [[=txr::annotations::ignore]] on e.g. mutex members
                                //  full(handles heap jumps, cycles, std::any, ignores mutexes)
                                //      -> for autistic (interns) people... they wanna reflect whole fucking DB with a single ORM Entity serializer call?

----------------------------------------------------------

ASSUME

// INTERNAL ############################
+ using pretty_yyjson = yyjson_backend<Pretty=true>;

+ Serializer<BackendTypeable Backend, FrontendSettings> s;
+ 
+ using FullPrettyJsonSerializer = Serializer<pretty_yyjson, FrontendSettings::Full>;
+ using StrictPrettyJsonSerializer = Serializer<pretty_yyjson, FrontendSettings::Strict>;
+ using RelaxedPrettyJsonSerializer = Serializer<pretty_yyjson, FrontendSettings::Relaxed>;

+ using JsonSerializer = StrictCompactJsonSerializer;
// ######################################

----------------------------------------------------------

EASY EXAMPLE (99% use case)

struct S { ... };
S s1, s2;
JsonSerializer s;
std::string out = s.serialize(s1);
s.deserialize(out, s2);

-------------------------------------------------------------

COMPLEX EXAMPLE (1% rizz)

struct S
{
    int a = 2;
    [[=txr::annotations::ignore]] // Doesnt compile without this with StrictSettings
    std::mutex semafor;
    std::string b = "pero";
    Person p;
}

struct person_serializer
{
    template <typename backend_t>
    void serialize(const Person& p, backend_t& backend) { backend.serialize("a", p.a); }

    template <typename backend_t>
    void deserialize(const Person& p, backend_t& backend) { backend.deserialize("a", p.a); }
}




custom_serializer(const S obj, auto backend)
{
    backend.begin_object();
        backend.serialize("a", a);
        backend.serialize("b", b);
        backend.begin_array("arr");
            backend. p.year
        backend.end_array();
    backend.end_object()
}

constexpr FrontendSettings settings = Settings::default()
    .attachCustomSerializer<Person, person_serializer>()

Serializer<pretty_yyjson, settings> s;

S item, item2;

std::string out = s.serialize(item);
s.deserialize(out, item2);