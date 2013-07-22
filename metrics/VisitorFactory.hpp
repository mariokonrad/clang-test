#ifndef __VISITORFACTORY__HPP__
#define __VISITORFACTORY__HPP__

#include "VisitorDescriptor.hpp"
#include <functional>
#include <map>
#include <vector>

class Visitor;

class VisitorFactory
{
	private:
		struct Item
		{
			VisitorDescriptor desc;
			std::function<Visitor*(void)> create;
		};

		typedef std::map<std::string, Item> CreatorMap;

		CreatorMap creators;

	public:
		bool add(
				const VisitorDescriptor & desc,
				std::function<Visitor*(void)> create);

		Visitor * create(const std::string & id) const;

		std::vector<VisitorDescriptor> get_visitor_desc() const;

		bool exists(const std::string & id) const;
		size_t size() const;
};

#endif
