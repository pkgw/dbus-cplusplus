#include "generator_utils.h"

#include <iostream>

using namespace std;

const char *atomic_type_to_string(char t)
{
	static struct { char type; const char *name; } atos[] =
	{
		{ 'y', "uint8_t" },
		{ 'b', "bool" },
		{ 'n', "int16_t" },
		{ 'q', "uint16_t" },
		{ 'i', "int32_t" },
		{ 'u', "uint32_t" },
		{ 'x', "int64_t" },
		{ 't', "uint64_t" },
		{ 'd', "double" },
		{ 's', "std::string" },
		{ 'o', "::DBus::Path" },
		{ 'g', "::DBus::Signature" },
		{ 'v', "::DBus::Variant" },
		{ '\0', "" }
	};
	int i;

	for (i = 0; atos[i].type; ++i)
	{
		if (atos[i].type == t) break;
	}
	return atos[i].name;
}

string stub_name(string name)
{
	underscorize(name);

	return "_" + name + "_stub";
}

void _parse_signature(const string &signature, string &type, unsigned int &i)
{
	for (; i < signature.length(); ++i)
	{
		switch (signature[i])
		{
			case 'a':
			{
				switch (signature[++i])
				{
					case '{':
					{
						type += "std::map< ";

						const char *atom = atomic_type_to_string(signature[++i]);
						if (!atom)
						{
							cerr << "invalid signature" << endl;
							exit(-1);
						}
						type += atom;
						type += ", ";
						++i;
						break;
					}
					default:
					{
						type += "std::vector< ";
						break;
					}
				}
				_parse_signature(signature, type, i);
				type += " >";
				continue;
			}
			case '(':	
			{
				type += "::DBus::Struct< ";
				++i;
				_parse_signature(signature, type, i);
				type += " >";
				if (signature[i+1])
				{
					type += ", ";
				}
				continue;
			}
			case ')':
			case '}':	
			{
				return;
			}
			default:
			{
				const char *atom = atomic_type_to_string(signature[i]);
				if (!atom)
				{
					cerr << "invalid signature" << endl;
					exit(-1);
				}
				type += atom;

				if (signature[i+1] != ')' && signature[i+1] != '}' && i+1 < signature.length())
				{
					type += ", ";
				}
				break;
			}
		}
	}
}

string signature_to_type(const string &signature)
{
	string type;
	unsigned int i = 0;
	_parse_signature(signature, type, i);
	return type;
}

void underscorize(string &str)
{
	for (unsigned int i = 0; i < str.length(); ++i)
	{
		if (!isalpha(str[i]) && !isdigit(str[i])) str[i] = '_';
	}
}
