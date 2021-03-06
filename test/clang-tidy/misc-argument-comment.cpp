// RUN: %check_clang_tidy %s misc-argument-comment %t

// FIXME: clang-tidy should provide a -verify mode to make writing these checks
// easier and more accurate.

void ffff(int xxxx, int yyyy);

void f(int x, int y);
void g() {
  // CHECK-MESSAGES: [[@LINE+4]]:5: warning: argument name 'y' in comment does not match parameter name 'x'
  // CHECK-MESSAGES: :[[@LINE-3]]:12: note: 'x' declared here
  // CHECK-MESSAGES: [[@LINE+2]]:14: warning: argument name 'z' in comment does not match parameter name 'y'
  // CHECK-MESSAGES: :[[@LINE-5]]:19: note: 'y' declared here
  f(/*y=*/0, /*z=*/0);
  // CHECK-FIXES: {{^}}  f(/*y=*/0, /*z=*/0);

  ffff(0 /*aaaa=*/, /*bbbb*/ 0); // Unsupported formats.
}

struct Closure {};

template <typename T1, typename T2>
Closure *NewCallback(void (*f)(T1, T2), T1 arg1, T2 arg2) { return nullptr; }

template <typename T1, typename T2>
Closure *NewPermanentCallback(void (*f)(T1, T2), T1 arg1, T2 arg2) { return nullptr; }

void h() {
  (void)NewCallback(&ffff, /*xxxx=*/11, /*yyyy=*/22);
  (void)NewPermanentCallback(&ffff, /*xxxx=*/11, /*yyyy=*/22);
}

template<typename... Args>
void variadic(Args&&... args);

template<typename... Args>
void variadic2(int zzz, Args&&... args);

void templates() {
  variadic(/*xxx=*/0, /*yyy=*/1);
  variadic2(/*zzU=*/0, /*xxx=*/1, /*yyy=*/2);
  // CHECK-MESSAGES: [[@LINE-1]]:13: warning: argument name 'zzU' in comment does not match parameter name 'zzz'
  // CHECK-FIXES: variadic2(/*zzz=*/0, /*xxx=*/1, /*yyy=*/2);
}

#define FALSE 0
void qqq(bool aaa);
void f() { qqq(/*bbb=*/FALSE); }
// CHECK-MESSAGES: [[@LINE-1]]:16: warning: argument name 'bbb' in comment does not match parameter name 'aaa'
// CHECK-FIXES: void f() { qqq(/*bbb=*/FALSE); }

void f(bool _with_underscores_);
void ignores_underscores() {
  f(/*With_Underscores=*/false);
}

// gmock
namespace testing {
namespace internal {

template <typename F>
struct Function;

template <typename R>
struct Function<R()> {
  typedef R Result;
};

template <typename R, typename A1>
struct Function<R(A1)>
    : Function<R()> {
  typedef A1 Argument1;
};

template <typename R, typename A1, typename A2>
struct Function<R(A1, A2)>
    : Function<R(A1)> {
  typedef A2 Argument2;
};
} // namespace internal
} // namespace testing

#define GMOCK_RESULT_(tn, ...) \
    tn ::testing::internal::Function<__VA_ARGS__>::Result
#define GMOCK_ARG_(tn, N, ...) \
    tn ::testing::internal::Function<__VA_ARGS__>::Argument##N
#define GMOCK_METHOD2_(tn, constness, ct, Method, ...) \
  GMOCK_RESULT_(tn, __VA_ARGS__) ct Method( \
      GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
      GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2) constness
#define MOCK_METHOD2(m, ...) GMOCK_METHOD2_(, , , m, __VA_ARGS__)

class Base {
 public:
  virtual void Method(int param_one_base, int param_two_base);
};
class Derived : public Base {
 public:
  virtual void Method(int param_one, int param_two);
};
class MockDerived : public Derived {
 public:
  MOCK_METHOD2(Method, void(int, int));
};

void test_gmock() {
  MockDerived m;
  m.Method(/*param_one=*/1, /*param_tw=*/2);
// CHECK-MESSAGES: [[@LINE-1]]:29: warning: argument name 'param_tw' in comment does not match parameter name 'param_two'
// CHECK-FIXES:   m.Method(/*param_one=*/1, /*param_two=*/2);
}
