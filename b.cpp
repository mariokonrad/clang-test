
int barfoo(int);

namespace org {
int foobar()
{
	int a = 0;
	int b = 0;
	int arr[10];

	for (int i = 0; i < 10; ++i) {
		if (barfoo(i) > a)
			a = 1;
		else
			b = 0;
	}

	if (a)
		b = 0;

	for (int i = 0; i < 10; ++i)
		barfoo(i);

	for (auto i : arr)
		i = 10;

	switch (a) {
		case 0: b = 1; break;
		case 1: b = 2; break;
		case 2: break;
		default: b = -1; break;
	}

	return a;
}
}

int barfoo(int a)
{
	return a * a;
}

void func1()
{
	int a = 10;

	if (a > 1)
		if (a > 2)
			if (a > 3)
				if (a > 4)
					if (a > 5)
						if (a > 6)
							if (a > 7)
								if (a > 8)
									a = 0;
}

void func2()
{
	int a = 10;

	if (a > 1) {
		if (a > 2)
			if (a > 3)
				a = -1;
	} else {
		if (a > 2)
			if (a > 3)
				if (a > 4)
					if (a > 5)
						if (a > 6)
							if (a > 7)
								if (a > 8)
									if (a > 9)
										a = 0;
	}
}

