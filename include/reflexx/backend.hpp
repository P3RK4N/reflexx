#ifndef REFLEXX_BACKEND_HPP
#define REFLEXX_BACKEND_HPP

#include <cstdint>
#include <span>
#include <string_view>

namespace reflexx {

// TODO: Ifstream
// TODO: u8string?
// TODO: fixed floating types
// TODO: Allow only fixed types to be (de)serialized?

template <typename T>
concept IsBackendType =
    std::is_move_constructible_v<T> &&
    std::is_move_assignable_v<T> && 
    requires
    (
        T backend,
        
        // For read input
        std::span<const char> input,
        std::span<char> mutable_input,

        // Strings
        std::string str,
        std::string_view sv,

        // For writing
        bool b,
        char c,
        std::int8_t i8, std::int16_t i16, std::int32_t i32, std::int64_t i64,
        std::uint8_t u8, std::uint16_t u16, std::uint32_t u32, std::uint64_t u64,
        float f, double d,
        
        // For reading
        bool& b_ref,
        char& c_ref,
        std::int8_t& i8_ref, std::int16_t& i16_ref, std::int32_t& i32_ref, std::int64_t& i64_ref,
        std::uint8_t& u8_ref, std::uint16_t& u16_ref, std::uint32_t& u32_ref, std::uint64_t& u64_ref,
        float& f_ref, double& d_ref
    )
    {
        T {};                   /* Constructor for write mode           */

        T { input };            /* Constructor for immutable read mode  */
        T { mutable_input };    /* Constructor for mutable read mode    */

        /**
         *  #################################################################
         *  #                                                               #
         *  #                        WRITE MODE                             #
         *  #      Defined behaviour when initialized for writing           #
         *  #                                                               #
         *  #################################################################
         */

        { backend.write_key(sv)         } -> std::same_as<void>;
        { backend.write_begin_array()   } -> std::same_as<void>;
        { backend.write_end_array()     } -> std::same_as<void>;
        { backend.write_begin_object()  } -> std::same_as<void>;
        { backend.write_end_object()    } -> std::same_as<void>;
        { backend.write_number(i8)      } -> std::same_as<void>;
        { backend.write_number(i16)     } -> std::same_as<void>;
        { backend.write_number(i32)     } -> std::same_as<void>;
        { backend.write_number(i64)     } -> std::same_as<void>;
        { backend.write_number(u8)      } -> std::same_as<void>;
        { backend.write_number(u16)     } -> std::same_as<void>;
        { backend.write_number(u32)     } -> std::same_as<void>;
        { backend.write_number(u64)     } -> std::same_as<void>;
        { backend.write_number(f)       } -> std::same_as<void>;
        { backend.write_number(d)       } -> std::same_as<void>;
        { backend.write_char(c)         } -> std::same_as<void>;
        { backend.write_bool(b)         } -> std::same_as<void>;
        { backend.write_string(sv)      } -> std::same_as<void>;
        { backend.write_null()          } -> std::same_as<void>;
        // Output -> defined only after a well formed write sequence
        { backend.get()                 } -> std::same_as<std::string_view>;

        /**
         *  #################################################################
         *  #                                                               #
         *  #                        READ MODE                              #
         *  #      Defined behaviour when initialized for reading           #
         *  #                                                               #
         *  #################################################################
         */

        { backend.read_key(sv)          } -> std::same_as<void>;
        { backend.read_key()            } -> std::same_as<std::string_view>;
        { backend.read_begin_array()    } -> std::same_as<void>;
        { backend.read_end_array()      } -> std::same_as<void>;
        { backend.read_begin_object()   } -> std::same_as<void>;
        { backend.read_end_object()     } -> std::same_as<void>;
        { backend.read_number(i8_ref)   } -> std::same_as<void>;
        { backend.read_number(i16_ref)  } -> std::same_as<void>;
        { backend.read_number(i32_ref)  } -> std::same_as<void>;
        { backend.read_number(i64_ref)  } -> std::same_as<void>;
        { backend.read_number(u8_ref)   } -> std::same_as<void>;
        { backend.read_number(u16_ref)  } -> std::same_as<void>;
        { backend.read_number(u32_ref)  } -> std::same_as<void>;
        { backend.read_number(u64_ref)  } -> std::same_as<void>;
        { backend.read_number(f_ref)    } -> std::same_as<void>;
        { backend.read_number(d_ref)    } -> std::same_as<void>;
        { backend.read_char(c_ref)      } -> std::same_as<void>;
        { backend.read_bool(b_ref)      } -> std::same_as<void>;
        { backend.read_string()         } -> std::same_as<std::string_view>;
        { backend.read_is_null()        } -> std::same_as<bool>;
        { backend.read_skip()           } -> std::same_as<void>;
        { backend.read_has_next()       } -> std::same_as<bool>;
    };

} // reflexx


#endif