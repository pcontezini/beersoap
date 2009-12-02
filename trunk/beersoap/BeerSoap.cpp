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
	xmlDocOut = new DomTree("SOAP-ENV:Envelope","UTF-8","1.0");
	xmlDocOut->setAttribute("xmlns:SOAP-ENV","http://schemas.xmlsoap.org/soap/envelope/");
	xmlDocOut->setAttribute("xmlns:soap","http://schemas.xmlsoap.org/wsdl/soap/");
	xmlDocOut->setAttribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");
	xmlDocOut->setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	
	bodyOut = xmlDocOut->newElement("SOAP-ENV:Body");
	
	xmlDocIn = new DomTree("env:Envelope","UTF-8","1.0");
	
	http = new CurlInterface();
}

void BeerSoapService::setServiceName(string serviceName) {
	this->serviceName = serviceName;
}

void BeerSoapService::setServiceURL(string serviceURL) {
	this->serviceURL = serviceURL;
}

void BeerSoapService::setServiceNameSpace(string serviceNameSpace) {
	this->serviceNameSpace = serviceNameSpace;
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
	if(!serviceURL.size()) {
		return(false);
	}
	char *data = dump();
	
	BEER_DEBUG("DATA: %s\n",data);
	
	if (!http->DoPost(serviceURL.c_str(), data)) {
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
	BEER_DEBUG("RESPOSTA: %s\n", p);
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
	if(exceptionCallback) {
		(*exceptionCallback)(exceptionCustomPtr, data);
		return(true);
	}
	return(false);
}

void BeerSoapService::registerExceptionCallBack(void(*callback)(void *, DomElement *), void *customPtr) {
	this->exceptionCustomPtr = customPtr;
	this->exceptionCallback = callback;
}

