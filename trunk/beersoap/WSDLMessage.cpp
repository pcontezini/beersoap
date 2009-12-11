/*
 *  WSDLMessage.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"

WSDLMessage::WSDLMessage(std::string newName) {
	this->name = newName;
}

WSDLMessage::~WSDLMessage() {
	parts.clear();
}

std::string WSDLMessage::getName() {
	return(name);
}

void WSDLMessage::pushPart(WSDLPart *newPart) {
	parts.push_back(newPart);
}

WSDLPart *WSDLMessage::getPart(std::string name) {
	for(unsigned int i = 0; i < parts.size(); i++) {
		if(parts[i]->getName() == name) {
			return(parts[i]);
		}
	}
	return(NULL);
}

std::vector<WSDLPart *>& WSDLMessage::getParts() {
	return(parts);
}
