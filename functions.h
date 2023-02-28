#pragma once

#include "list.h"

struct value
{
	enum
	{
		value_valuetype_i64,
	}
	valuetype;
	union
	{
		int64_t i64;
	}
	value;
};
struct values
{
	list(struct value)
};
struct function;
struct functionptrs
{
	list(struct function *)
};
struct function
{
	char *name;
	struct values values;
	enum
	{
		function_definitiontype_native,
		function_definitiontype_functionptrs,
	}
	definitiontype;
	union
	{
		struct value (*native)(struct values);
		struct functionptrs functionptrs;
	}
	definition;
};
struct functions
{
	list(struct function)
};
struct value constant_one_definition_native(struct values values)
{
	return (struct value){  };
}
struct function constant_one = (struct function)
{
	.name = "1",
	.values = list_empty(),
	.definitiontype = function_definitiontype_native,
	.definition.native = &constant_one_definition_native,
};
