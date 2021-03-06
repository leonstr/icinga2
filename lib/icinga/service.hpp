/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2018 Icinga Development Team (https://icinga.com/)      *
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

#ifndef SERVICE_H
#define SERVICE_H

#include "icinga/i2-icinga.hpp"
#include "icinga/service-ti.hpp"
#include "icinga/macroresolver.hpp"
#include "icinga/host.hpp"
#include <tuple>

using std::tie;

namespace icinga
{

/**
 * An Icinga service.
 *
 * @ingroup icinga
 */
class Service final : public ObjectImpl<Service>, public MacroResolver
{
public:
	DECLARE_OBJECT(Service);
	DECLARE_OBJECTNAME(Service);

	static Service::Ptr GetByNamePair(const String& hostName, const String& serviceName);

	Host::Ptr GetHost() const override;
	int GetSeverity() const override;

	bool ResolveMacro(const String& macro, const CheckResult::Ptr& cr, Value *result) const override;

	bool IsStateOK(ServiceState state) override;
	void SaveLastState(ServiceState state, double timestamp) override;

	static ServiceState StateFromString(const String& state);
	static String StateToString(ServiceState state);

	static StateType StateTypeFromString(const String& state);
	static String StateTypeToString(StateType state);

	static void EvaluateApplyRules(const Host::Ptr& host);

protected:
	void OnAllConfigLoaded() override;
	void CreateChildObjects(const Type::Ptr& childType) override;

private:
	Host::Ptr m_Host;

	static bool EvaluateApplyRuleInstance(const Host::Ptr& host, const String& name, ScriptFrame& frame, const ApplyRule& rule);
	static bool EvaluateApplyRule(const Host::Ptr& host, const ApplyRule& rule);
};

std::pair<Host::Ptr, Service::Ptr> GetHostService(const Checkable::Ptr& checkable);

}

#endif /* SERVICE_H */
