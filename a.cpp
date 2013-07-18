
namespace sys {
class A { public: void f(int a, int b) {} void g(float c) {} };
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
};
}

int main()
{
	return 0;
}

