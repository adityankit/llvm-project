// RUN: %clang_cc1 -fsyntax-only -std=c++17 -verify %s
// RUN: %clang_cc1 -fsyntax-only -std=c++17 -verify %s -fexperimental-new-constant-interpreter
// RUN: %clang_cc1 -fsyntax-only -std=c++20 -verify %s -fexperimental-new-constant-interpreter

// p1099 'using enum ELABORATED-ENUM-SPECIFIER ;'

namespace One {
namespace Bob {
enum A { a, // expected-note{{declared here}}
         b,
         c };
class C;
enum class D : int;
enum class D { d,
               e,
               f };
enum class D : int;
} // namespace Bob

using enum Bob::A;
#if __cplusplus < 202002
// expected-warning@-2{{is a C++20 extension}}
#endif
using enum Bob::B; // expected-error{{unknown type name B}}
#if __cplusplus < 202002
// expected-warning@-2{{is a C++20 extension}}
#endif
using enum Bob::C; // expected-error{{'Bob::C' is not an enumerated type}}
#if __cplusplus < 202002
// expected-warning@-2{{is a C++20 extension}}
#endif
auto v = a;

A g; // expected-error{{unknown type name 'A'}}

int A;

using enum Bob::D;
#if __cplusplus < 202002
// expected-warning@-2{{is a C++20 extension}}
#endif

void DR2621() {
  using A_t = Bob::A;
  using enum A_t;
#if __cplusplus < 202002
// expected-warning@-2{{is a C++20 extension}}
#endif
  A_t x = a;
}

} // namespace One

namespace Two {
namespace Kevin {
enum class B { d,
               e,
               f };
}

using enum Kevin::B;
#if __cplusplus < 202002
// expected-warning@-2{{is a C++20 extension}}
#endif
auto w = e;

} // namespace Two

#if __cplusplus >= 202002
// Now only check c++20 onwards

namespace Three {
namespace Stuart {
enum class C : int; // expected-note{{declared here}}
}

using enum Stuart::C; // expected-error{{is incomplete}}
} // namespace Three

namespace Four {
class Dave {
public:
  enum D { a,
           b,
           c };

private:
  enum class E { d, // expected-note{{declared private here}}
                 e,
                 f };
};

using enum Dave::D;
using enum Dave::E; // expected-error{{is a private member}}

} // namespace Four

namespace Five {
enum class A { b,
               c };
class Dave {
public:
  using enum A;
  A f = b;
};

} // namespace Five

namespace Six {
template <typename T> class TPL;
template <> class TPL<int> {
public:
  enum A { a };
};

template <typename T> class USR {
  using enum TPL<T>::B; // expected-error{{cannot name a dependent type}}
  using enum TPL<int>::A;
};
} // namespace Six

// Now instantiate things
namespace Seven {
namespace Stuart {
enum class A { a,
               b,
               c };
}

static_assert(!int(Stuart::A::a));
constexpr int Bar() {
  using enum Stuart::A;
  return int(b);
}
static_assert(Bar() == 1);

template <int I> constexpr int Foo() {
  using enum Stuart::A;
  return int(b) + I;
}

static_assert(Foo<10>() == 11);

template <int I> struct C {
  using enum Stuart::A;
  static constexpr int V = int(c) + I;

  enum class D { d,
                 e,
                 f };
  using enum D; // expected-error {{using-enum cannot name a dependent type}}
};

static_assert(C<2>::V == 4);

} // namespace Seven

namespace Eight {
enum class Bob : int {};
using enum Bob;
} // namespace Eight

namespace Nine {
template <int I> struct C {
  enum class D { i = I };
  enum class E : int; // expected-note{{declared here}}
};

using enum C<2>::D;

constexpr auto d = i;
static_assert(unsigned(d) == 2);

using enum C<2>::E; // expected-error{{instantiation of undefined member}}
} // namespace Nine

namespace Ten {
enum class Bob { a };

void Foo() {
  extern void a();
}

// We don't see the hidden extern a fn!
using enum Bob;

auto v = a;
} // namespace Ten

namespace Eleven {
enum class Bob { a }; // expected-note{{conflicting declaration}}

struct Base {
  enum { a }; // expected-note{{target of using}}
};

template <typename B>
class TPLa : B {
  using enum Bob;
  using B::a; // expected-error{{target of using declaration}}
};

TPLa<Base> a; // expected-note{{in instantiation}}

} // namespace Eleven

namespace Twelve {
enum class Bob { a }; // expected-note{{target of using}}

struct Base {
  enum { a };
};

template <typename B>
class TPLb : B {
  using B::a;     // expected-note{{conflicting declaration}}
  using enum Bob; // expected-error{{target of using declaration}}
};

TPLb<Base> b;

} // namespace Twelve

namespace Thirteen {
enum class Bob { a };
class Foo {
  using enum Bob; // expected-note{{previous using-enum}}
  using enum Bob; // expected-error{{redeclaration of using-enum}}
};

template <typename B>
class TPLa {
  using enum Bob; // expected-note{{previous using-enum}}
  using enum Bob; // expected-error{{redeclaration of using-enum}}
};

TPLa<int> a;

} // namespace Thirteen

namespace Fourteen {
template<typename T>
int A = T();

using enum A<int>; // expected-error {{A is not an enumerated type}}
} // namespace Fourteen

namespace GH58057 {
struct Wrap {
enum Things {
  Value1,
  Value2
};
};

using enum Wrap::Things;

int f() {
  return (Value1 | Value2);
}
}

namespace GH59014 {
struct X {
  enum Masks {Mask = 1,Shift = 0};
};

void f(int a) {
  using enum X::Masks;

  auto u = (Mask);
  auto v = (Mask << Shift);
  void (~(Mask));
}
}

namespace GH147495 {
struct S {
  enum class E { A };
  using enum E;

  struct S1 {
    using enum E;
  };

  struct S2 {
    using E::A;
  };
};
}
#endif
