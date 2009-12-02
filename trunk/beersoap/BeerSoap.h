/*
 *  BeerSoap.h
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include "DomTree.h"
#include "BeerSoapServiceMethod.h"
#include "CurlInterface.h"

#define BEER_DEBUG printf
//#define BEER_DEBUG

class BeerSoapService : public BeerSoapProtocol {
private:
	std::string serviceName;
	std::string serviceURL;
	std::string serviceNameSpace;
	
	DomTree *xmlDocOut;
	DomElement *rootIn;
	DomElement *bodyOut;
	
	DomTree *xmlDocIn;
	DomElement *rootOut;
	
	vector<BeerSoapServiceMethod *> methods;
	
	void *exceptionCustomPtr;
	
	
public:
	BeerSoapService();
	void(*exceptionCallback)(void *,DomElement *);	
	CurlInterface *http;

	void setServiceName(std::string serviceName);
	void setServiceURL(std::string serviceURL);
	void setServiceNameSpace(std::string serviceNameSpace);
	
	BeerSoapServiceMethod *callMethod(std::string method, std::string methodNameSpace);
//	void registerExceptionCallBack(void(*)(void *, element *), void *customPtr);
	void registerExceptionCallBack(void(*callback)(void *, DomElement *), void *customPtr);
	
	DomElement *createMethod(std::string methodName);
	
	int getMethodId(void *method);
	
	bool run();
	bool soapReceiveMessage();
	
	void registerXMLMethod();
	bool soapProcessResponse(DomElement *data, BeerSoapServiceMethod *receiver);
	bool soapProcessException(DomElement *data);
	
	BeerSoapServiceMethod *lookMethodResponse(std::string response);
	
	void methodRefresh();
	
	char *dump();
};
