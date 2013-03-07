/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012 Icinga Development Team (http://www.icinga.org/)        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "i2-config.h"

using std::ifstream;

using namespace icinga;

ConfigCompilerContext *ConfigCompilerContext::m_Context = NULL;

ConfigCompilerContext::ConfigCompilerContext(void)
	: m_Unit(Utility::NewUUID()), m_Flags(0)
{ }

void ConfigCompilerContext::AddItem(const ConfigItem::Ptr& item)
{
	m_Items.push_back(item);
	m_ItemsMap[make_pair(item->GetType(), item->GetName())] = item;
}

ConfigItem::Ptr ConfigCompilerContext::GetItem(const String& type, const String& name) const
{
	map<pair<String, String>, ConfigItem::Ptr>::const_iterator it;

	it = m_ItemsMap.find(make_pair(type, name));

	if (it == m_ItemsMap.end())
		return ConfigItem::Ptr();

	return it->second;
}

vector<ConfigItem::Ptr> ConfigCompilerContext::GetItems(void) const
{
	return m_Items;
}

void ConfigCompilerContext::AddType(const ConfigType::Ptr& type)
{
	m_Types[type->GetName()] = type;
}

ConfigType::Ptr ConfigCompilerContext::GetType(const String& name) const
{
	map<String, ConfigType::Ptr>::const_iterator it;

	it = m_Types.find(name);

	if (it == m_Types.end())
		return ConfigType::Ptr();

	return it->second;
}

void ConfigCompilerContext::AddError(bool warning, const String& message)
{
	m_Errors.push_back(ConfigCompilerError(warning, message));
}

vector<ConfigCompilerError> ConfigCompilerContext::GetErrors(void) const
{
	return m_Errors;
}

void ConfigCompilerContext::SetFlags(int flags)
{
	m_Flags = flags;
}

int ConfigCompilerContext::GetFlags(void) const
{
	return m_Flags;
}

void ConfigCompilerContext::SetContext(ConfigCompilerContext *context)
{
	ASSERT(m_Context == NULL || context == NULL);

	m_Context = context;
}

ConfigCompilerContext *ConfigCompilerContext::GetContext(void)
{
	return m_Context;
}

String ConfigCompilerContext::GetUnit(void) const
{
	return m_Unit;
}

void ConfigCompilerContext::Validate(void)
{
	SetContext(this);

	BOOST_FOREACH(const ConfigItem::Ptr& item, m_Items) {
		ConfigType::Ptr ctype;

		{
			ctype = GetType(item->GetType());

			if (!ctype) {
				AddError(true, "No validation type found for object '" + item->GetName() + "' of type '" + item->GetType() + "'");

				continue;
			}
		}

		ctype->ValidateItem(item);
	}

	SetContext(NULL);
}

void ConfigCompilerContext::ActivateItems(void)
{
	ASSERT(m_Context == NULL);

	Logger::Write(LogInformation, "config", "Activating config items in compilation unit '" + m_Unit + "'");
	BOOST_FOREACH(const ConfigItem::Ptr& item, m_Items) {
		item->Commit();
	}
}
