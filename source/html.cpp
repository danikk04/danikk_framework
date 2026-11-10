#include <danikk_framework/misc/html.h>

namespace danikk_framework
{
	namespace internal
	{
		bool isUsedHtmlNode(GumboNode* ptr)
		{
			size_t type = ptr->type;
			return type != GUMBO_NODE_WHITESPACE && type != GUMBO_NODE_COMMENT;
		}
		bool isTextHtmlType(size_t type)
		{
			return 	type == GUMBO_NODE_TEXT 	||
					type == GUMBO_NODE_COMMENT 	||
					type == GUMBO_NODE_CDATA 	||
					type == GUMBO_NODE_TEMPLATE;
		}
		GumboNode* getHtmlChild(GumboNode* node, size_t index)
		{
			GumboVector& childs = node->v.element.children;
			GumboNode** current = (GumboNode**)childs.data;
			while(true)
			{
				if(isUsedHtmlNode(*current))
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
		}

#undef case
#define case(type) case type: return #type + 10;

	const char* htmlTagToString(size_t tag)
	{
		switch(tag)
		{
			case(GUMBO_TAG_HTML)
			case(GUMBO_TAG_HEAD)
			case(GUMBO_TAG_TITLE)
			case(GUMBO_TAG_BASE)
			case(GUMBO_TAG_LINK)
			case(GUMBO_TAG_META)
			case(GUMBO_TAG_STYLE)
			case(GUMBO_TAG_SCRIPT)
			case(GUMBO_TAG_NOSCRIPT)
			case(GUMBO_TAG_TEMPLATE)
			case(GUMBO_TAG_BODY)
			case(GUMBO_TAG_ARTICLE)
			case(GUMBO_TAG_SECTION)
			case(GUMBO_TAG_NAV)
			case(GUMBO_TAG_ASIDE)
			case(GUMBO_TAG_H1)
			case(GUMBO_TAG_H2)
			case(GUMBO_TAG_H3)
			case(GUMBO_TAG_H4)
			case(GUMBO_TAG_H5)
			case(GUMBO_TAG_H6)
			case(GUMBO_TAG_HGROUP)
			case(GUMBO_TAG_HEADER)
			case(GUMBO_TAG_FOOTER)
			case(GUMBO_TAG_ADDRESS)
			case(GUMBO_TAG_P)
			case(GUMBO_TAG_HR)
			case(GUMBO_TAG_PRE)
			case(GUMBO_TAG_BLOCKQUOTE)
			case(GUMBO_TAG_OL)
			case(GUMBO_TAG_UL)
			case(GUMBO_TAG_LI)
			case(GUMBO_TAG_DL)
			case(GUMBO_TAG_DT)
			case(GUMBO_TAG_DD)
			case(GUMBO_TAG_FIGURE)
			case(GUMBO_TAG_FIGCAPTION)
			case(GUMBO_TAG_MAIN)
			case(GUMBO_TAG_DIV)
			case(GUMBO_TAG_A)
			case(GUMBO_TAG_EM)
			case(GUMBO_TAG_STRONG)
			case(GUMBO_TAG_SMALL)
			case(GUMBO_TAG_S)
			case(GUMBO_TAG_CITE)
			case(GUMBO_TAG_Q)
			case(GUMBO_TAG_DFN)
			case(GUMBO_TAG_ABBR)
			case(GUMBO_TAG_DATA)
			case(GUMBO_TAG_TIME)
			case(GUMBO_TAG_CODE)
			case(GUMBO_TAG_VAR)
			case(GUMBO_TAG_SAMP)
			case(GUMBO_TAG_KBD)
			case(GUMBO_TAG_SUB)
			case(GUMBO_TAG_SUP)
			case(GUMBO_TAG_I)
			case(GUMBO_TAG_B)
			case(GUMBO_TAG_U)
			case(GUMBO_TAG_MARK)
			case(GUMBO_TAG_RUBY)
			case(GUMBO_TAG_RT)
			case(GUMBO_TAG_RP)
			case(GUMBO_TAG_BDI)
			case(GUMBO_TAG_BDO)
			case(GUMBO_TAG_SPAN)
			case(GUMBO_TAG_BR)
			case(GUMBO_TAG_WBR)
			case(GUMBO_TAG_INS)
			case(GUMBO_TAG_DEL)
			case(GUMBO_TAG_IMAGE)
			case(GUMBO_TAG_IMG)
			case(GUMBO_TAG_IFRAME)
			case(GUMBO_TAG_EMBED)
			case(GUMBO_TAG_OBJECT)
			case(GUMBO_TAG_PARAM)
			case(GUMBO_TAG_VIDEO)
			case(GUMBO_TAG_AUDIO)
			case(GUMBO_TAG_SOURCE)
			case(GUMBO_TAG_TRACK)
			case(GUMBO_TAG_CANVAS)
			case(GUMBO_TAG_MAP)
			case(GUMBO_TAG_AREA)
			case(GUMBO_TAG_MATH)
			case(GUMBO_TAG_MI)
			case(GUMBO_TAG_MO)
			case(GUMBO_TAG_MN)
			case(GUMBO_TAG_MS)
			case(GUMBO_TAG_MTEXT)
			case(GUMBO_TAG_MGLYPH)
			case(GUMBO_TAG_MALIGNMARK)
			case(GUMBO_TAG_ANNOTATION_XML)
			case(GUMBO_TAG_SVG)
			case(GUMBO_TAG_FOREIGNOBJECT)
			case(GUMBO_TAG_DESC)
			case(GUMBO_TAG_TABLE)
			case(GUMBO_TAG_CAPTION)
			case(GUMBO_TAG_COLGROUP)
			case(GUMBO_TAG_COL)
			case(GUMBO_TAG_TBODY)
			case(GUMBO_TAG_THEAD)
			case(GUMBO_TAG_TFOOT)
			case(GUMBO_TAG_TR)
			case(GUMBO_TAG_TD)
			case(GUMBO_TAG_TH)
			case(GUMBO_TAG_FORM)
			case(GUMBO_TAG_FIELDSET)
			case(GUMBO_TAG_LEGEND)
			case(GUMBO_TAG_LABEL)
			case(GUMBO_TAG_INPUT)
			case(GUMBO_TAG_BUTTON)
			case(GUMBO_TAG_SELECT)
			case(GUMBO_TAG_DATALIST)
			case(GUMBO_TAG_OPTGROUP)
			case(GUMBO_TAG_OPTION)
			case(GUMBO_TAG_TEXTAREA)
			case(GUMBO_TAG_KEYGEN)
			case(GUMBO_TAG_OUTPUT)
			case(GUMBO_TAG_PROGRESS)
			case(GUMBO_TAG_METER)
			case(GUMBO_TAG_DETAILS)
			case(GUMBO_TAG_SUMMARY)
			case(GUMBO_TAG_MENU)
			case(GUMBO_TAG_MENUITEM)
			case(GUMBO_TAG_APPLET)
			case(GUMBO_TAG_ACRONYM)
			case(GUMBO_TAG_BGSOUND)
			case(GUMBO_TAG_DIR)
			case(GUMBO_TAG_FRAME)
			case(GUMBO_TAG_FRAMESET)
			case(GUMBO_TAG_NOFRAMES)
			case(GUMBO_TAG_ISINDEX)
			case(GUMBO_TAG_LISTING)
			case(GUMBO_TAG_XMP)
			case(GUMBO_TAG_NEXTID)
			case(GUMBO_TAG_NOEMBED)
			case(GUMBO_TAG_PLAINTEXT)
			case(GUMBO_TAG_RB)
			case(GUMBO_TAG_STRIKE)
			case(GUMBO_TAG_BASEFONT)
			case(GUMBO_TAG_BIG)
			case(GUMBO_TAG_BLINK)
			case(GUMBO_TAG_CENTER)
			case(GUMBO_TAG_FONT)
			case(GUMBO_TAG_MARQUEE)
			case(GUMBO_TAG_MULTICOL)
			case(GUMBO_TAG_NOBR)
			case(GUMBO_TAG_SPACER)
			case(GUMBO_TAG_TT)
			case(GUMBO_TAG_RTC)
			case(GUMBO_TAG_UNKNOWN)
			default: return "NOT_EXITS";
		}
	}

	#undef case
	}

	
	void HtmlNode::checkNull()
	{
		if(isNull())
		{
			throw "HtmlNode::isNull";
		}
	}

	HtmlNode::HtmlNode(GumboNode* ptr):ptr(ptr)
	{

	}

	const char* HtmlNode::text()
	{
		checkNull();
		if(internal::isTextHtmlType(ptr->type))
		{
			return ptr->v.text.text;
		}
		else
		{
			return HtmlNode(ptr)[0].text();
		}
	}

	size_t HtmlNode::childcount()
	{
		checkNull();

		return ptr->v.element.children.length;
	}

	size_t HtmlNode::type()
	{
		checkNull();
		return ptr->type;
	}

	size_t HtmlNode::tag()
	{
		checkNull();
		return ptr->v.element.tag;
	}

	void HtmlNode::getChilds(DynamicArray<HtmlNode>& result)
	{
		GumboVector* childs = &ptr->v.element.children;
		GumboNode** iterator = (GumboNode**)childs->data;
		GumboNode** end = iterator + childs->length;
		while(iterator != end)
		{
			GumboNode* current = *iterator;
			if(internal::isUsedHtmlNode(current))
			{
				result.push(current);
			}
			iterator++;
		}
	}

	using Iterator = HtmlNode::Iterator;

	Iterator::Iterator(GumboVector& childs)
	{
		iterator = (GumboNode**)childs.data;
		end = iterator + childs.length;

		while(true)
		{
			GumboNode* node = *iterator;
			if(iterator == end)
			{
				return;
			}
			else if(!internal::isUsedHtmlNode(node))
			{
				iterator++;
			}
			else
			{
				return;
			}
		}
	}

	HtmlNode Iterator::next()
	{
		GumboNode* result = *iterator;
		iterator++;

		if(iterator == end)
		{
			return result;
		}

		while(!internal::isUsedHtmlNode(*iterator))
		{
			iterator++;
			if(iterator == end)
			{
				break;
			}
		}

		return result;
	}

	void Iterator::skip()
	{
		next();
	}

	Iterator::operator bool()
	{
		return iterator != end;
	}

	Iterator HtmlNode::begin()
	{
		if(isNull())
		{
			throw "[danikk_framework::HtmlNode::begin] isNull";
		}
		return Iterator(ptr->v.element.children);
	}

	const char* HtmlNode::getAttribute(const char* name)
	{
		GumboVector* attributes = &ptr->v.element.attributes;
		GumboAttribute** iterator = (GumboAttribute**)attributes->data;
		GumboAttribute** end = iterator + attributes->length;
		while(iterator != end)
		{
			GumboAttribute* current = *iterator;
			if(strequal(current->name, name))
			{
				return current->value;
			}
			iterator++;
		}
		return "";
	}

	const char* HtmlNode::getClass()
	{
		return this->getAttribute("class");
	}

	const char* HtmlNode::getHref()
	{
		return this->getAttribute("href");
	}

	bool HtmlNode::isNull()
	{
		return ptr == NULL;
	}

	HtmlNode HtmlNode::operator[](size_t childIndex)
	{
		return HtmlNode(internal::getHtmlChild(ptr, childIndex));
	}

	HtmlNode HtmlNode::findByAttribute(const char* key, const char* value)
	{
		checkNull();

		Iterator thisIterator = this->begin();

		while(thisIterator)
		{
			HtmlNode currentNode = thisIterator.next();

			if(strequal(currentNode.getAttribute(key),value))
			{
				return currentNode;
			}
		}
		return HtmlNode(NULL);
	}

	HtmlNode HtmlNode::recursiveFindByAttribute(const char* key, const char* value)
	{
		checkNull();

		Iterator thisIterator = begin();

		while(thisIterator)
		{
			HtmlNode currentNode = thisIterator.next();

			if(currentNode.type() == GUMBO_NODE_ELEMENT)
			{
				if(strequal(currentNode.getAttribute(key),value))
				{
					return currentNode;
				}

				HtmlNode recursiveResult = currentNode.recursiveFindByAttribute(key, value);
				if(!recursiveResult.isNull())
				{
					return recursiveResult;
				}
			}
		}
		return HtmlNode(NULL);
	}

	HtmlNode HtmlNode::findByTag(size_t tag)
	{
		checkNull();
		Iterator thisIterator = this->begin();
		while(thisIterator)
		{
			HtmlNode currentNode = thisIterator.next();
			if(currentNode.tag() == tag)
			{
				return currentNode;
			}
		}
		return HtmlNode(NULL);
	}

	HtmlNode HtmlNode::findByClass(const char* value)
	{
		return findByAttribute("class", value);
	}

	HtmlNode HtmlNode::recursiveFindByClass(const char* value)
	{
		return recursiveFindByAttribute("class", value);
	}

	HtmlNode HtmlNode::recursiveFindById(const char* value)
	{
		return recursiveFindByAttribute("id", value);
	}

	HtmlDocument::HtmlDocument()
	{
		ptr = NULL;
	}

	HtmlDocument::HtmlDocument(MemoryBuffer& data)
	{
		ptr = gumbo_parse_with_options(&kGumboDefaultOptions, data.data(), data.size());

		if(ptr == NULL)
		{
			throw "[danikk_framework::HtmlDocument::ctor] isNull";
		}
	}

	HtmlDocument::HtmlDocument(String& data)
	{
		ptr = gumbo_parse_with_options(&kGumboDefaultOptions, data.data(), data.size());

		if(ptr == NULL)
		{
			throw "[danikk_framework::HtmlDocument::ctor] isNull";
		}
	}

	HtmlDocument::~HtmlDocument()
	{
		gumbo_destroy_output(&kGumboDefaultOptions, ptr);
	}

	HtmlNode HtmlDocument::operator[](size_t childIndex)
	{
		return HtmlNode(internal::getHtmlChild(ptr->document, childIndex));
	}

	HtmlNode HtmlDocument::body()
	{
		HtmlNode root = ptr->document;

		return root.findByTag(GUMBO_TAG_HTML).findByTag(GUMBO_TAG_BODY);
	}

	//Убираем GUMBO_NODE_ смещением указателя.
	#define case(type) case type: return #type + 11;

	const char* htmlNodeTypeToString(size_t type)
	{
		switch(type)
		{
			case(GUMBO_NODE_DOCUMENT)
			case(GUMBO_NODE_ELEMENT)
			case(GUMBO_NODE_TEXT)
			case(GUMBO_NODE_CDATA)
			case(GUMBO_NODE_COMMENT)
			case(GUMBO_NODE_WHITESPACE)
			case(GUMBO_NODE_TEMPLATE)
			default: return "NOT_EXITS";
		}
	}
	#undef case

	const char* htmlTagToString(size_t tag)
	{
		return internal::htmlTagToString(tag);
	}
}
