
namespace sys {
class A { public: int f(int a, int b) const; void g(float c) {} };
int A::f(int a, int b) const { return 0; }
}

namespace util {
namespace special {
class B {};
}}

namespace param {
class C : public util::special::B {};
}

class D : public param::C {};

namespace test {
class E : public param::C { public: class E1 : public sys::A {}; };
}

namespace org {
class Base : virtual public sys::A, virtual public util::special::B, public D
{
	public:
		test::E e;
		test::E::E1 e1;
	public:
		test::E & get();
};

test::E & Base::get()
{
	return e;
}
}

struct XX {};

union YY {};

class AA;

class AA {};

int main()
{
	return 0;
}

