#ifndef NATIVE_H_
#define NATIVE_H_ 1

#pragma once

#include "../../nfs_manager.h"
#include "native_constants.h"

#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>

class Native : public NFsManager
{
private:

	/**
	*	@brief: starting from a netmask, returns the /
	*
	*	@param:	netmask	Netmask to be converted
	*/
	unsigned int convertNetmask(string netmask);
	
	/**
	*	@brief: contains the list of capabilities available through the node
	*		name->path
	*/
	std::map<std::string, std::string> capabilities;
	
public:
	bool isSupported();
	
	bool startNF(StartNFIn sni);
	bool stopNF(StopNFIn sni);
};

#endif //NATIVE_H_
