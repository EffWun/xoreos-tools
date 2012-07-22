/* libeos 0.0.1
 * Copyright (c) 2005 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include <eos.h>

#include <cstdlib>

Variable::Variable()
{
	type = VAR_TYPE_INT;
	value.tint = 0;
	value.tstr = 0;
}

Variable::Variable(const Variable &other)
{
	*this = other;
}

Variable::~Variable()
{
//	fprintf(stderr, "~Variable(): %p, %p\n", this, value.tstr);
	if((type == VAR_TYPE_STRING) && value.tstr) free(value.tstr);
}

Variable &Variable::operator=(const Variable &src)
{
	name = src.name;
	type = src.type;
	switch(type)
	{
		case VAR_TYPE_INT:
			value.tint = src.value.tint;
			break;
		case VAR_TYPE_FLOAT:
			value.tfloat = src.value.tfloat;
			break;
		case VAR_TYPE_STRING:
			value.tstr = strdup(src.value.tstr);
			break;
		case VAR_TYPE_OBJECT:
			value.tobj = src.value.tobj;
			break;
		case VAR_TYPE_LOCATION:
			value.tloc = src.value.tloc;
			break;
	}
	return *this;
}

void Variable::set(std::string name, int32 value)
{
	if((type == VAR_TYPE_STRING) && this->value.tstr) free(this->value.tstr);
	type = VAR_TYPE_INT;
	this->name = name;
	this->value.tint = value;
}

void Variable::set(std::string name, float32 value)
{
	if((type == VAR_TYPE_STRING) && this->value.tstr) free(this->value.tstr);
	type = VAR_TYPE_FLOAT;
	this->name = name;
	this->value.tfloat = value;
}

void Variable::set(std::string name, std::string value)
{
	if((type == VAR_TYPE_STRING) && this->value.tstr) free(this->value.tstr);
	type = VAR_TYPE_STRING;
	this->name = name;
	this->value.tstr = strdup(value.c_str());
}

void Variable::set(std::string name, uint32 value)
{
	if((type == VAR_TYPE_STRING) && this->value.tstr) free(this->value.tstr);
	type = VAR_TYPE_OBJECT;
	this->name = name;
	this->value.tobj = value;
}

void Variable::set(std::string name, Location value)
{
	if((type == VAR_TYPE_STRING) && this->value.tstr) free(this->value.tstr);
	type = VAR_TYPE_LOCATION;
	this->name = name;
	this->value.tloc = value;
}

void Variable::get(int32 &value)
{
	switch(type)
	{
		case VAR_TYPE_INT:
			value = this->value.tint;
			break;
		case VAR_TYPE_FLOAT:
			value = (int32) this->value.tfloat;
			break;
		case VAR_TYPE_STRING:
			value = 0;
			break;
		case VAR_TYPE_OBJECT:
			value = this->value.tobj;
			break;
		case VAR_TYPE_LOCATION:
			value = 0;
			break;
	}
}

void Variable::get(float32 &value)
{
	switch(type)
	{
		case VAR_TYPE_INT:
			value = this->value.tint;
			break;
		case VAR_TYPE_FLOAT:
			value = this->value.tfloat;
			break;
		case VAR_TYPE_STRING:
			value = 0;
			break;
		case VAR_TYPE_OBJECT:
			value = this->value.tobj;
			break;
		case VAR_TYPE_LOCATION:
			value = 0;
			break;
	}
}

void Variable::get(std::string &value)
{
	if(type == VAR_TYPE_STRING) value = this->value.tstr;
	else value = "";
}

void Variable::get(uint32 &value)
{
	switch(type)
	{
		case VAR_TYPE_INT:
			value = this->value.tint;
			break;
		case VAR_TYPE_FLOAT:
			value = (uint32) this->value.tfloat;
			break;
		case VAR_TYPE_STRING:
			value = 0;
			break;
		case VAR_TYPE_OBJECT:
			value = this->value.tobj;
			break;
		case VAR_TYPE_LOCATION:
			value = 0;
			break;
	}
}

void Variable::get(Location &value)
{
	if(type == VAR_TYPE_LOCATION) value = this->value.tloc;
	else
	{
		Location loc;
		memset(&loc, 0, sizeof(Location));
		value = loc;
	}
}

// vim:tabstop=2
