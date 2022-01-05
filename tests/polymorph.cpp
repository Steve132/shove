
#include "containers/polymorph.hpp"
#include<vector>
#include<iostream>

struct Base
{
	virtual void foo()
	{
		std::cout << "Foo Base" << std::endl;
	}
	Base(const Base& b)
	{
		std::cout << "Copy Base" << std::endl;
	}
	Base()=default;

};

struct Derived: public Base
{
	virtual void foo()
	{
		std::cout << "Foo Derived" << std::endl;
	}
	Derived(const Derived& b)
	{
		std::cout << "Copy Derived" << std::endl;
	}
	Derived()=default;
};

void call_it(Base& obj)
{
	obj.foo();
}

int main()
{
	shv::polymorph<Base> b;
	b=Derived();
	call_it(b);
	b=Base();
	call_it(b);

	std::vector<shv::polymorph<Base>> objects;
	objects.push_back(Base{});
	objects.push_back(Derived{});

	std::cout << "Objects populated" << std::endl;
	std::vector<shv::polymorph<Base>> obj_copies=objects;

	return 0;
}
