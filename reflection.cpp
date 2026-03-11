#include <meta>
#include <print>

struct Point { int x; double y; };

enum class Color { Red, Green, Blue };

void ex1_basics() {
  std::println("── 1. Basics ──────────────────────────");

  constexpr auto r = ^^int;
  constexpr auto name = std::meta::display_string_of(r);
  std::println("display_string_of(^^int)   = {}", name);
  std::println("display_string_of(^^Point) = {}", std::meta::display_string_of(^^Point));

  std::println("identifier_of(^^Point)     = {}", std::meta::identifier_of(^^Point));

}

void ex2_splicers() {
  std::println("── 2. Splicers ─────────────────────────");

  constexpr auto ri = ^^int;
  typename[:ri:] x = 42;
  std::println("typename [:^^int:] x = {}", x);

  constexpr auto rd = ^^double;
  typename[:rd:] d = 3.14;
  std::println("typename [:^^double:] d = {}", d);
}

void ex3_type_queries() {
  std::println("── 3. Type queries ─────────────────────");

  constexpr auto r = ^^Point;
  std::println("is_class_type:  {}", std::meta::is_class_type(r));
  std::println("is_const_type:  {}", std::meta::is_const_type(r));
  std::println("size_of:        {}", std::meta::size_of(r));
  std::println("alignment_of:   {}", std::meta::alignment_of(r));
}

template <typename T>
void ex4_members() {
  std::println("── 4. Struct members ───────────────────");

  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto m :
    std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))) {
    std::println("  member '{}' of type '{}'",
      std::meta::identifier_of(m),
      std::meta::display_string_of(std::meta::type_of(m))
    );
  }
}

void ex5_enumerators() {
  std::println("── 5. Enumerators ──────────────────────");

  template for (constexpr auto e :
    std::define_static_array(std::meta::enumerators_of(^^Color))) {
    std::println("  {}", std::meta::identifier_of(e));
  }
}

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
  static constexpr auto red = enum_name(Color::Red);
  static constexpr auto green = enum_name(Color::Green);
  static constexpr auto blue = enum_name(Color::Blue);
  std::println("  Color::Red   = {}", red);
  std::println("  Color::Green = {}", green);
  std::println("  Color::Blue  = {}", blue);
}

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
  ex4_members<Point>();
  struct Tmp { int x; }; ex4_members<Tmp>();
  ex5_enumerators();
  ex6_enum_to_string();
  ex7_generic_printer();
}