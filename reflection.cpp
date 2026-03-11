#include <meta>
#include <print>

struct Point { int x; double y; };

enum class Color { Red, Green, Blue };

void ex1_basics() {
  std::println("── 1. Basics ──────────────────────────");

  // ^^ is the reflection operator — produces a std::meta::info handle for the given entity
  constexpr auto r = ^^int;
  constexpr auto name = std::meta::display_string_of(r);
  /*
    display_string_of — human-readable string, no guarantees on format, meant for display/debugging.
    Could return "int", "Point", or even something like "struct Point" depending on the implementation.
  */
  std::println("display_string_of(^^int)   = {}", name);
  std::println("display_string_of(^^Point) = {}", std::meta::display_string_of(^^Point));

  /*
    identifier_of — the exact declared name only, guaranteed to be just the identifier token as written in source.
    So for struct Point it returns "Point", nothing else.
    Note: identifier_of(^^int) would be ill-formed — int is a keyword, not a declared identifier.
  */
  std::println("identifier_of(^^Point)     = {}", std::meta::identifier_of(^^Point));
}

void ex2_splicers() {
  std::println("── 2. Splicers ─────────────────────────");
  constexpr auto ri = ^^int;
  /*
    [:ri:] splices the reflection back into actual code — not a string, but the real type.
    typename[:ri:] x = 42; is equivalent to writing int x = 42; directly.
    splicers go in the opposite direction of ^^: entity -> ^^ -> info -> [:] -> entity
  */
  typename[:ri:] x = 42;
  std::println("typename [:^^int:] x = {}", x);
  constexpr auto rd = ^^double;
  /*
    Same pattern with double — the splicer injects the type into the declaration.
    typename[:rd:] d = 3.14; is equivalent to double d = 3.14;
  */
  typename[:rd:] d = 3.14;
  std::println("typename [:^^double:] d = {}", d);
}

void ex3_type_queries() {
  std::println("── 3. Type queries ─────────────────────");

  // reflect Point and query properties — all resolved at compile time
  constexpr auto r = ^^Point;
  // is_class_type — true for structs and classes
  std::println("is_class_type:  {}", std::meta::is_class_type(r));
  // is_const_type — true if the type is const-qualified
  std::println("is_const_type:  {}", std::meta::is_const_type(r));
  // size_of / alignment_of — same as sizeof(Point) and alignof(Point) but via reflection
  std::println("size_of:        {}", std::meta::size_of(r));
  std::println("alignment_of:   {}", std::meta::alignment_of(r));
}

template <typename T>
void ex4_members(const T& obj) {
  std::println("── 4. Struct members ───────────────────");

  // access_context controls which members are visible (public/private/etc.)
  // ::current() means "use the access rules of the current call site"
  constexpr auto ctx = std::meta::access_context::current();

  // nonstatic_data_members_of returns a std::vector<std::meta::info> at consteval time
  /*
    struct Foo {
      int x;        // included
      static int y; // excluded
    };
  */

  // define_static_array converts it to a static array so template for can iterate it
  // template for is a compile-time loop — each iteration is a separate instantiation
  template for (constexpr auto m :
    std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {

    // obj.[:m:] splices the member handle back into real code — directly accesses the field
    std::println("  member '{}' of type '{}' = {}",
      std::meta::identifier_of(m),
      std::meta::display_string_of(std::meta::type_of(m)),
      obj.[:m:]
    );
  }
}

void ex5_enumerators() {
  std::println("── 5. Enumerators ──────────────────────");

  // enum class Color { Red, Green, Blue };
  // enumerators_of returns all enumerator values of an enum as std::meta::info handles
  // identifier_of gives back the declared name of each enumerator ("Red", "Green", "Blue")
  template for (constexpr auto e :
    std::define_static_array(std::meta::enumerators_of(^^Color))) {
    std::println("  {}", std::meta::identifier_of(e));
  }
}

// consteval — runs entirely at compile time, result is a compile-time constant
// iterates all enumerators of E and returns the name of the one matching value
// [:e:] splices the enumerator back into a value so it can be compared with ==
template<typename E>
consteval std::string_view enum_name(E value) {
  template for (constexpr auto e :
    std::define_static_array(std::meta::enumerators_of(^^E))) {
    if ([:e:] == value)
      return std::meta::identifier_of(e);
  }
  return "<unknown>";
}

void ex6_enum_to_string() {
  std::println("── 6. Enum to string ───────────────────");
  // enum_name is consteval so results are compile-time string_views baked into the binary
  static constexpr auto red = enum_name(Color::Red);
  static constexpr auto green = enum_name(Color::Green);
  static constexpr auto blue = enum_name(Color::Blue);
  std::println("  Color::Red   = {}", red);
  std::println("  Color::Green = {}", green);
  std::println("  Color::Blue  = {}", blue);

  std::println("\n  ────── Enum iteration ────────────────");
  template for (constexpr auto e :
    std::define_static_array(std::meta::enumerators_of(^^Color))) {
    std::println("  Color::{} = {}", std::meta::identifier_of(e), enum_name([:e:]));
  }
}

// generic printer — works on any struct, prints all member names and values
// uses both identifier_of (name as string) and obj.[:m:] (actual value via splice)
template<typename T>
void print_struct(const T& obj) {
  std::println("  {} {{", std::meta::display_string_of(^^T));
  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto m :
    std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {
    std::println("    {} = {}", std::meta::identifier_of(m), obj.[:m:]);
  }
  std::println("  }}");
}

void ex7_generic_printer() {
  std::println("── 7. Generic struct printer ───────────");
  Point p {3, 4.5};
  print_struct(p);
}

int main() {
  ex1_basics();
  ex2_splicers();
  ex3_type_queries();
  Point p {3, 4.5};
  ex4_members(p);
  struct Tmp { int x; }; ex4_members(Tmp{42});
  ex5_enumerators();
  ex6_enum_to_string();
  ex7_generic_printer();
}