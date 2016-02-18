#include "docker.h"

bool Docker::isSupported(Description&)
{
	int retVal;

	retVal = system(CHECK_DOCKER);
	retVal = retVal >> 8;

	logger(ORCH_DEBUG, DOCKER_MODULE_NAME, __FILE__, __LINE__, "Script returned: %d\n",retVal);

	if(retVal > 0)
	{
		logger(ORCH_DEBUG_INFO, DOCKER_MODULE_NAME, __FILE__, __LINE__, "Docker deamon is running.");
		return true;
	}

	logger(ORCH_DEBUG_INFO, DOCKER_MODULE_NAME, __FILE__, __LINE__, "Docker deamon is not running (at least, it is not running with the LXC implementation).");
	return false;
}

bool Docker::startNF(StartNFIn sni)
{
	uint64_t lsiID = sni.getLsiID();
	string nf_name = sni.getNfName();
	
	string uri_image = description->getURI();
	
	map<unsigned int, string> namesOfPortsOnTheSwitch = sni.getNamesOfPortsOnTheSwitch();
	unsigned int n_ports = namesOfPortsOnTheSwitch.size();
	
	stringstream command;
	command << PULL_AND_RUN_DOCKER_NF << " " << lsiID << " " << nf_name << " " << uri_image << " " << n_ports;
		
	for(map<unsigned int, string>::iterator pn = namesOfPortsOnTheSwitch.begin(); pn != namesOfPortsOnTheSwitch.end(); pn++)
		command << " "  << pn->second;
		
	
	//The first element is the mac address, the second is the ip address
	//list<pair<string, string> > portsConfiguration = sni.getPortsConfiguration();
		
#ifdef ENABLE_UNIFY_PORTS_CONFIGURATION		
	list<pair<string, string> >  controlConnections = sni.getControlPorts();
	command << " " << controlConnections.size();
	if(controlConnections.size() != 0)
	{
		logger(ORCH_DEBUG, DOCKER_MODULE_NAME, __FILE__, __LINE__, "VNF '%s' requires %d control connections",nf_name.c_str(), controlConnections.size());
		for(list<pair<string, string> >::iterator control = controlConnections.begin(); control != controlConnections.end(); control++)
		{
			logger(ORCH_DEBUG, DOCKER_MODULE_NAME, __FILE__, __LINE__, "\t host TCP port: %s - VNF TCP port: %s",(control->first).c_str(), (control->second).c_str());
			command << " " << control->first << " " << control->second;
		}
	}
#else
	command << " 0";
#endif

	logger(ORCH_DEBUG_INFO, DOCKER_MODULE_NAME, __FILE__, __LINE__, "Executing command \"%s\"",command.str().c_str());

	int retVal = system(command.str().c_str());
	retVal = retVal >> 8;
	
	if(retVal == 0)
		return false;
		
	return true;
}

bool Docker::stopNF(StopNFIn sni)
{
	uint64_t lsiID = sni.getLsiID();
	string nf_name = sni.getNfName();

	stringstream command;
	command << STOP_DOCKER_NF << " " << lsiID << " " << nf_name;
	
	logger(ORCH_DEBUG_INFO, DOCKER_MODULE_NAME, __FILE__, __LINE__, "Executing command \"%s\"",command.str().c_str());
	int retVal = system(command.str().c_str());
	retVal = retVal >> 8;

	if(retVal == 0)
		return false;

	return true;
}

unsigned int Docker::convertNetmask(string netmask)
{
	unsigned int slash = 0;
	unsigned int mask;
	
	int first, second, third, fourth;
	sscanf(netmask.c_str(),"%d.%d.%d.%d",&first,&second,&third,&fourth);
	mask = (first << 24) + (second << 16) + (third << 8) + fourth;
	
	for(int i = 0; i < 32; i++)
	{
		if((mask & 0x1) == 1)
			slash++;
		mask = mask >> 1;
	}
	
	return slash;
}
