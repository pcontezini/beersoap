/*
 *  WSDLPort.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"

WSDLPort::WSDLPort(std::string newName) {
	this->name = newName;
	binding = NULL;
}

WSDLPort::~WSDLPort() {
	binding = NULL;
}

std::string WSDLPort::getName() {
	return(name);
}

void WSDLPort::setLocation(std::string newLocation) {
	location = newLocation;
}

std::string WSDLPort::getLocation() {
	return(location);
}

void WSDLPort::setBinding(WSDLBinding *newBinding) {
	binding = newBinding;
}

WSDLBinding *WSDLPort::getBinding() {
	return(binding);
}