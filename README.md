# cpp26-reflection-examples

Examples covering C++26 static reflection (P2996).

## Topics
- `^^` operator and `display_string_of` / `identifier_of`
- Splicers (`typename [:r:]`)
- Type queries (`is_class_type`, `size_of`, `alignment_of`)
- Iterating struct members and enumerators via `template for`
- `enum_name` — consteval enum-to-string
- Generic struct printer

## Requirements
GCC 16+ with `-std=c++26 -freflection`

## License
MIT
