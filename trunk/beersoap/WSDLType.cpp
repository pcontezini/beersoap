/*
 *  WSDLType.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"


WSDLType::WSDLType(std::string newName, std::string newType) {
	this->name = newName;
	this->type = newType;
	
	this->primitive = false;
	this->minOccurs = 0;
}

WSDLType::~WSDLType() {
	types.clear();
}

void WSDLType::setPrimitive() {
	primitive = true;
}

bool WSDLType::getPrimitive() {
	return(primitive);
}

std::string WSDLType::getName() {
	return(name);
}

std::string WSDLType::getType() {
	return(type);
}

unsigned int WSDLType::getMinOccurs() {
	return(minOccurs);
}

void WSDLType::setMinOccurs(unsigned int newMinOccurs) {
	this->minOccurs = newMinOccurs;
}

void WSDLType::pushType(WSDLType *newType) {
	types.push_back(newType);
}

void WSDLType::setTypes(std::vector<WSDLType *> newTypes) {
	types = newTypes;
}

std::vector<WSDLType *>& WSDLType::getTypes() {
	return(types);
}
