#include "high_level_graph_vnf.h"

namespace highlevel
{

#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION
VNFs::VNFs(string id, string name, list<string> groups, string vnf_template, list<vnf_port_t> ports, list<pair<string, string> > control_ports, list<string> environment_variables) :
	id(id), name(name), groups(groups), vnf_template(vnf_template)
#else
VNFs::VNFs(string id, string name, list<string> groups, string vnf_template, list<vnf_port_t> ports) :
	id(id), name(name), groups(groups), vnf_template(vnf_template)
#endif
{
	for(list<vnf_port_t>::iterator p = ports.begin(); p != ports.end(); p++)
	{
		this->ports.push_back((*p));
	}

#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION
	this->control_ports.insert(this->control_ports.end(),control_ports.begin(),control_ports.end());
	this->environment_variables.insert(this->environment_variables.end(),environment_variables.begin(),environment_variables.end());
#endif
}

VNFs::~VNFs()
{

}

bool VNFs::operator==(const VNFs &other) const
{
	if(id == other.id && name == other.name)
		return true;

	return false;
}

bool VNFs::addPort(vnf_port_t port)
{
	// the port is added if is not yet part of the VNF
	for(list<vnf_port_t>::iterator p = ports.begin(); p != ports.end(); p++)
	{
		vnf_port_t current = *p;
		if(current.id == port.id)
			// the port is already part of the graph
			return false;
	}
	ports.push_back(port);
	return true;
}

string VNFs::getId()
{
	return id;
}

string VNFs::getName()
{
	return name;
}

list<string> VNFs::getGroups()
{
	return groups;
}

string VNFs::getVnfTemplate()
{
	return vnf_template;
}

list <vnf_port_t> VNFs::getPorts()
{
	return ports;
}

Object VNFs::toJSON()
{
	Object vnf;
	Array portS,groups_Array;
#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION
	Array ctrl_ports;
	Array env_variables;
#endif

	vnf[_ID] = id.c_str();
	vnf[_NAME] = name.c_str();
	vnf[VNF_TEMPLATE] = vnf_template.c_str();
	for(list<string>::iterator it = groups.begin(); it != groups.end(); it++)
		groups_Array.push_back((*it).c_str());
	if(groups.size()!=0)
		vnf[VNF_GROUPS] = groups_Array;
	for(list<vnf_port_t>::iterator p = ports.begin(); p != ports.end(); p++)
	{
		Object pp;

		pp[_ID] = p->id.c_str();
		pp[_NAME] = p->name.c_str();
		if(strlen(p->mac_address.c_str()) != 0)
			pp[PORT_MAC] = p->mac_address.c_str();
#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION
		if(strlen(p->ip_address.c_str()) != 0)
			pp[PORT_IP] = p->ip_address.c_str();
#endif

		portS.push_back(pp);
	}
	vnf[VNF_PORTS] = portS;

#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION
	for(list<pair<string, string> >::iterator c = control_ports.begin(); c != control_ports.end(); c++)
	{
		Object cc;

		cc[HOST_PORT] = atoi((*c).first.c_str());
		cc[VNF_PORT] = atoi((*c).second.c_str());

		ctrl_ports.push_back(cc);
	}
	vnf[UNIFY_CONTROL] = ctrl_ports;

	for(list<string>::iterator ev = environment_variables.begin(); ev != environment_variables.end(); ev++)
	{
		Object var;

		var[VARIABLE] = ev->c_str();
		env_variables.push_back(var);
	}
	vnf[UNIFY_ENV_VARIABLES] = env_variables;
#endif

	return vnf;
}

#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION
list<pair<string, string> >  VNFs::getControlPorts()
{
	return control_ports;
}

list<string> VNFs::getEnvironmentVariables()
{
	return environment_variables;
}
#endif

}
