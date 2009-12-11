/*
 *  WSDLBinding.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"

WSDLBinding::WSDLBinding(std::string newName) {
	this->name = newName;
}

WSDLBinding::~WSDLBinding() {
	operations.clear();
}

std::string WSDLBinding::getName() {
	return(name);
}

void WSDLBinding::pushOperation(WSDLOperation *newOperation) {
	operations.push_back(newOperation);
}

WSDLOperation *WSDLBinding::getOperation(std::string name) {
	for(unsigned int i = 0; i < operations.size(); i++) {
		if(operations[i]->getName() == name) {
			return(operations[i]);
		}
	}
	return(NULL);
}

std::vector<WSDLOperation *>& WSDLBinding::getOperations() {
	return(operations);
}

void WSDLBinding::setType(WSDLPortType *newType) {
	type = newType;
}

WSDLPortType *WSDLBinding::getType() {
	return(type);
}