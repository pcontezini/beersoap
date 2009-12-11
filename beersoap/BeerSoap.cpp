/*
 *  BeerSoap.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include "BeerSoap.h"

BeerSoapService::BeerSoapService() {
	exceptionCallBack = NULL;
	xmlDocOut = new DomTree("SOAP-ENV:Envelope","UTF-8","1.0");
	xmlDocOut->setAttribute("xmlns:SOAP-ENV","http://schemas.xmlsoap.org/soap/envelope/");
	xmlDocOut->setAttribute("xmlns:soap","http://schemas.xmlsoap.org/wsdl/soap/");
	xmlDocOut->setAttribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");
	xmlDocOut->setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	
	bodyOut = xmlDocOut->newElement("SOAP-ENV:Body");
	
	xmlDocIn = new DomTree("env:Envelope","UTF-8","1.0");
	
	http = new CurlInterface();
}

void BeerSoapService::setName(string serviceName) {
	this->name = serviceName;
}

std::string BeerSoapService::getName() {
	return(name);
}

void BeerSoapService::setURL(string serviceURL) {
	this->URL = serviceURL;
}

std::string BeerSoapService::getURL() {
	return(URL);
}

void BeerSoapService::setNameSpace(string serviceNameSpace) {
	this->nameSpace = serviceNameSpace;
}

std::string BeerSoapService::getNameSpace() {
	return(nameSpace);
}

BeerSoapServiceMethod *BeerSoapService::callMethod(string method, string methodNameSpace) {
	for(unsigned int i = 0; i < methods.size(); i++) {
		if(methods[i]->getMethodName() == method) {
			return(methods[i]);
		}
	}
	BeerSoapServiceMethod *newMethod = new BeerSoapServiceMethod(method, methodNameSpace, this);
	methods.push_back(newMethod);
	return(newMethod);
}

void BeerSoapService::methodRefresh() {
	xmlDocOut->removeElements(bodyOut->getChildren());
	return;
}

char *BeerSoapService::dump() {
	char *p = xmlDocOut->xml(true);
	return(p);
}


bool BeerSoapService::run() {
	if(!URL.size()) {
		return(false);
	}
	char *data = dump();
	
	BEER_DEBUG("DATA: %s\n",data);
	
	if (!http->DoPost(URL.c_str(), data)) {
		BEER_DEBUG("erro ao conectar\n");
		free(data);
		return(false);
	}
	
	if(!soapReceiveMessage()) {
		BEER_DEBUG("erro ao processar mensagem SOAP\n");
		return(false);
	}
	
	
	free(data);
	return(true);
}

bool BeerSoapService::soapReceiveMessage() {
	xmlDocIn->removeElements(xmlDocIn->getChildren());
	
	
	char *p = (char *)http->getData();
//	BEER_DEBUG("RESPOSTA: %s\n", p);
	if(!p) {
		return(false);
	}
	if(!xmlDocIn->load(p,strlen(p))) {
		http->clearData();
		return(false);
	}
	http->clearData();
	
	BeerSoapServiceMethod *method;
	
	vector<DomElement *> children = xmlDocIn->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		BEER_DEBUG("NAME: %s\n",children[i]->getName().c_str());
		if(children[i]->getName() == "Body") {
			vector<DomElement *> bchildren = children[i]->getChildren();
			for(unsigned int c = 0; c < bchildren.size(); c++) {
				BEER_DEBUG("SNAME: %s\n",bchildren[c]->getName().c_str());
				if(bchildren[c]->getName() == "Fault") {
					BEER_DEBUG("exception\n");
					soapProcessException(bchildren[c]);
					return(true);
				}
				if ( (method = lookMethodResponse(bchildren[c]->getName())) ) {
					soapProcessResponse(bchildren[c],method);
					return(true);
				} 	
			}
		}
	}
	
	return(false);
		
}

BeerSoapServiceMethod *BeerSoapService::lookMethodResponse(string response) {
	for(unsigned int i = 0; i < methods.size(); i++) {
		if(methods[i]->isResponseFor(response)) {
			return(methods[i]);
		}
	}
	return(NULL);
}

int BeerSoapService::getMethodId(void *method) {
	for(unsigned int i = 0; i < methods.size(); i++) {
		if(methods[i] == method) {
			return(i);
		}
	}
	return(-1);
}

DomElement *BeerSoapService::createMethod(string methodName) {
	return(bodyOut->newElement(methodName.c_str()));	
}

//bool BeerSoapService::

bool BeerSoapService::soapProcessResponse(DomElement *data, BeerSoapServiceMethod *receiver) {
	receiver->callCallBack(data);
	return(true);
}
bool BeerSoapService::soapProcessException(DomElement *data) {
	if(exceptionCallBack) {
		(*exceptionCallBack)(exceptionCustomPtr, data);
		return(true);
	}
	return(false);
}

void BeerSoapService::registerExceptionCallBack(void(*callback)(void *, DomElement *), void *customPtr) {
	this->exceptionCustomPtr = customPtr;
	this->exceptionCallBack = callback;
}

