/*
 *  WSDLPart.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"

WSDLPart::WSDLPart(std::string newName) {
	name = newName;
	element = NULL;
}

WSDLPart::~WSDLPart() {
	
}

std::string WSDLPart::getName() {
	return(name);
}

void WSDLPart::setElement(WSDLType *type) {
	element = type;
}

WSDLType *WSDLPart::getElement() {
	return(element);
}
