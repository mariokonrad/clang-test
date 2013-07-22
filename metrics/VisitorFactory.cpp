#include "VisitorFactory.hpp"

bool VisitorFactory::add(
		const VisitorDescriptor & desc,
		std::function<Visitor*(void)> create)
{
	auto i = creators.find(desc.id);
	if (i != creators.end())
		return false;

	creators[desc.id] = { desc, create };
	return true;
}

Visitor * VisitorFactory::create(const std::string & id) const
{
	auto i = creators.find(id);
	return (i == creators.end())
		? nullptr
		: i->second.create();
}

std::vector<VisitorDescriptor> VisitorFactory::get_visitor_desc() const
{
	std::vector<VisitorDescriptor> v;
	v.reserve(creators.size());
	for (auto i : creators)
		v.push_back(i.second.desc);
	return v;
}

bool VisitorFactory::exists(const std::string & id) const
{
	return creators.find(id) != creators.end();
}

size_t VisitorFactory::size() const
{
	return creators.size();
}

