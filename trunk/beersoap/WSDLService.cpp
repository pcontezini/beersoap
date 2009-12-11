/*
 *  WSDLService.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"


WSDLService::WSDLService(std::string newName) {
	this->name = newName;
}

WSDLService::~WSDLService() {
	ports.clear();
}

std::string WSDLService::getName() {
	return(name);
}

void WSDLService::setNameSpace(std::string newNameSpace) {
	nameSpace = newNameSpace;
}

std::string WSDLService::getNameSpace() {
	return(nameSpace);
}

void WSDLService::pushPort(WSDLPort *newPort) {
	ports.push_back(newPort);
}

WSDLPort *WSDLService::getPort(std::string name) {
	for(unsigned int i = 0; i < ports.size(); i++) {
		if(ports[i]->getName() == name) {
			return(ports[i]);
		}
	}
	return(NULL);
}

std::vector<WSDLPort *>& WSDLService::getPorts() {
	return(ports);
}
