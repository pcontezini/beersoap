/*
 *  BearSoapServiceMethod.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include "BeerSoapServiceMethod.h"

BeerSoapServiceMethod::BeerSoapServiceMethod(std::string methodName, std::string methodNameSpace, BeerSoapProtocol *parent) {
	this->method = methodName;
	this->methodNameSpace = methodNameSpace;
	this->parent = parent;
	this->exceptioncallback = NULL;
	this->exceptionPtr = NULL;
}

void BeerSoapServiceMethod::pushParameter(std::string name, std::string value) {
	for(unsigned int i = 0; i < parameters.size(); i++) {
		if(parameters[i].name == name) {
			parameters[i].value = value;
			return;
		}
	}
	BeerSoapParameter newParameter;
	newParameter.name = name;
	newParameter.value = value;
	parameters.push_back(newParameter);
}

void BeerSoapServiceMethod::registerCallBack(void(*callback)(void *, DomElement *), void *customPtr) {
	this->customPtr = customPtr;
	this->callback = callback;
}

bool BeerSoapServiceMethod::run() {
	int methodID = parent->getMethodId(this);
	
	parent->methodRefresh();
	
	char URIStringStr[255];
	snprintf(URIStringStr, 254, "bssm%d", methodID);
	std::string URIString = URIStringStr;
	
	DomElement *methodNode = parent->createMethod(URIString + ":" + method);
	std::string attr = "xmlns:" + URIString;
	methodNode->setAttribute(attr, methodNameSpace);
	
	for(unsigned int i = 0; i < parameters.size(); i++) {
		DomElement *node = methodNode->newElement(parameters[i].name);
		node->setValue(parameters[i].value);
	}
	
	return(parent->run());
}

void BeerSoapServiceMethod::setMethodResponse(std::string response) {
	this->methodResponse = response;
}

std::string BeerSoapServiceMethod::getMethodResponse() {
	return(this->methodResponse);
}

bool BeerSoapServiceMethod::isResponseFor(std::string response) {
	if(this->methodResponse == response) {
		return(true);
	} else {
		return(false);
	}
}

bool BeerSoapServiceMethod::callCallBack(DomElement *data) {
	if(callback) {
		(*callback)(customPtr, data);
		return(true);
	} else {
		return(false);
	}
}

std::string BeerSoapServiceMethod::getMethodName() {
	return(method);
}

bool BeerSoapServiceMethod::hasExceptionCallback() {
	if(exceptioncallback) {
		return(true);
	} else {
		return(false);
	}
}

BeerSoapCallBackDef BeerSoapServiceMethod::getExceptionCallback() {
	return(exceptioncallback);
}

void *BeerSoapServiceMethod::getExceptionPointer() {
	return(exceptionPtr);
}

void BeerSoapServiceMethod::registerExceptionCallback(BeerSoapCallBackDef exceptioncallback, void *exceptionPtr) {
	this->exceptioncallback = exceptioncallback;
	this->exceptionPtr = exceptionPtr;
}