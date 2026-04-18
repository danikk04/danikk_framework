#pragma once

#include <danikk_framework/danikk_framework.h>
#include <danikk_framework/dynamic_array.h>
#include <danikk_framework/string.h>
#include <danikk_framework/format.h>
#include <danikk_framework/memory.h>
#include <gumbo.h>

#include "../static_string.h"
#include "array.h"
#include "cstring_functions.h"
#include "memory_buffer.h"

namespace danikk_framework
{
	namespace internal
	{
		const char* htmlTagToString(size_t tag);
		bool isTextHtmlType(size_t type);
		bool isUsedHtmlNode(GumboNode* ptr);

		template<class streamT> void htmlRecursivePrint(streamT& cout, GumboNode* node, size_t depth = 0)
		{
			if(node == NULL)
			{
				return;
			}
			size_t nodeType = node->type;
			if(nodeType == GUMBO_NODE_WHITESPACE)
			{
				return;
			}
			cout << indent(depth);
			if(nodeType == GUMBO_NODE_ELEMENT)
			{
				cout << "tag:" << htmlTagToString(node->v.element.tag);

				{
					GumboVector& attributes = node->v.element.attributes;

					GumboAttribute** current = (GumboAttribute**)attributes.data;
					GumboAttribute** end = current + attributes.length;

					while(current < end)
					{
						GumboAttribute* attribute = *current++;
						cout << ' ' << attribute->name << ' ' << attribute->value;
					}
				}

				cout << endl;


				GumboVector& childs = node->v.element.children;
				GumboNode** current = (GumboNode**)childs.data;
				GumboNode** end = current + childs.length;

				while(current < end)
				{
					htmlRecursivePrint(cout, *current, depth + 1);
					current++;
				}
			}
			else if(isTextHtmlType(nodeType))
			{
				const char* text = node->v.text.text;
				cout << "text: ";
				//Если строка содержит видимые символы
				if (!strcontainsonly(text, ' ', '\n', '\t'))
				{
					while(true)
					{
						char currentChar = *text++;
						if(currentChar == '\0')
						{
							break;
						}
						else if(currentChar != ' ' && currentChar != '\n' && currentChar != '\t')
						{
							cout << currentChar;
						}
					}
				}
				else
				{
					cout << "empty";
				}
				cout << endl;
			}
			else
			{

			}
		}
	}

	class HtmlNode
	{
	private:
		GumboNode* ptr;

		void checkNull();
	public:
		HtmlNode();

		HtmlNode(GumboNode* ptr);

		const char* text();

		size_t childcount();

		size_t type();

		size_t tag();

		//Возвращает массив дочерних узлов, но вырезает комментарии и пропуски строк.
		void getChilds(DynamicArray<HtmlNode>& result);

		class Iterator
		{
private:
			GumboNode** iterator;
			GumboNode** end;
public:
			Iterator();

			Iterator(GumboVector& childs);

			HtmlNode next();

			void skip();

			operator bool();
		};

		HtmlNode findByAttribute(const char* key, const char* value);

		HtmlNode recursiveFindByAttribute(const char* key, const char* value);

		HtmlNode findByClass(const char* value);

		HtmlNode recursiveFindByClass(const char* value);

		HtmlNode recursiveFindById(const char* value);

		HtmlNode findByTag(size_t tag);

		Iterator begin();

		const char* getAttribute(const char* name);

		const char* getClass();

		const char* getHref();

		bool isNull();

		template<class streamT> friend streamT& operator << (streamT& cout, HtmlNode node);

		HtmlNode operator[](size_t childIndex);
	};

	template<class streamT> streamT& operator <<(streamT& cout, HtmlNode node)
	{
		internal::htmlRecursivePrint(cout, node.ptr, 0);
		return cout;
	}

	class HtmlDocument
	{
	private:
		GumboOutput* ptr;
	public:
		HtmlDocument();

		HtmlDocument(MemoryBuffer& data);

		HtmlDocument(String& data);

		~HtmlDocument();

		HtmlNode operator[](size_t childIndex);

		HtmlNode body();
	};

	const char* htmlTagToString(size_t tag);

	const char* htmlNodeTypeToString(size_t type);


	/*void gumbo_recursive_print(std::ostream& cout,GumboNode* node, size_t depth = 0)
	{
		if(node == NULL)
		{
			return;
		}
		size_t nodeType = node->type;
		if(nodeType == GUMBO_NODE_WHITESPACE)
		{
			return;
		}
		cout << indent(depth);
		if(nodeType == GUMBO_NODE_ELEMENT)
		{
			cout << "tag:" << gumbo_tag_to_string(node->v.element.tag) << endl;
			GumboVector& childs = node->v.element.children;
			GumboNode** current = (GumboNode**)childs.data;
			GumboNode** end = current + childs.length;

			while(current < end)
			{
				gumbo_recursive_print(cout, *current, depth + 1);
				current++;
			}
		}
		else if(nodeType == GUMBO_NODE_TEXT 	||
				nodeType == GUMBO_NODE_COMMENT 	||
				nodeType == GUMBO_NODE_CDATA 	||
				nodeType == GUMBO_NODE_TEMPLATE	)
		{
			const char* text = node->v.text.text;
			cout << "text: ";
			//Если строка содержит видимые символы
			if (!strcontainsonly(text, ' ', '\n', '\t'))
			{
				cout << node->v.text.text;
			}
			else
			{
				cout << "empty";
			}
			cout << endl;
		}
		else
		{

		}
	}


	//Из множества дочерних объектов с тегом достаёт элемент по индексу.
	GumboNode* gumbo_get_node_by_tag_i_index(GumboNode* node, GumboTag tag, size_t index)
	{
		GumboVector& childs = node->v.element.children;
		GumboNode** current = (GumboNode**)childs.data;
		while(true)
		{
			if((*current)->type == GUMBO_NODE_ELEMENT && (*current)->v.element.tag == tag)
			{
				if(index == 0)
				{
					return *current;
				}
				else
				{
					index--;
				}
			}
			current++;
		}
	}*/
}

#include <danikk_framework/internal/html.inc>
