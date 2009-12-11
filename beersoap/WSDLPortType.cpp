/*
 *  WSDLPortType.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"

WSDLPortType::WSDLPortType(std::string newName) {
	name = newName;
}

WSDLPortType::~WSDLPortType() {
	operations.clear();
}

std::string WSDLPortType::getName() {
	return(name);
}

void WSDLPortType::pushOperation(WSDLOperation *newOperation) {
	operations.push_back(newOperation);
}

WSDLOperation *WSDLPortType::getOperation(std::string name) {
	for(unsigned int i = 0; i < operations.size(); i++) {
		if(operations[i]->getName() == name) {
			return(operations[i]);
		}
	}
	return(NULL);
}

std::vector<WSDLOperation *> WSDLPortType::getOperations() {
	return(operations);
}