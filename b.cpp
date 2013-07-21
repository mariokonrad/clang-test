
int barfoo(int);

namespace org {
int foobar()
{
	int a = 0;
	int b = 0;

	for (int i = 0; i < 10; ++i) {
		if (barfoo(i) > a)
			a = 1;
		else
			b = 0;
	}

	if (a)
		b = 0;

	return a;
}
}

int barfoo(int a)
{
	return a * a;
}

