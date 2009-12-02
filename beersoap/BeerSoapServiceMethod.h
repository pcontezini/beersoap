/*
 *  BearSoapServiceMethod.h
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include "DomTree.h"
#include "BeerSoapProtocol.h"

typedef void (*BeerSoapCallBackDef)(void *, DomElement *);

class BeerSoapParameter {
public:
	std::string name;
	std::string value;
};

class BeerSoapServiceMethod {
private:
	std::vector<BeerSoapParameter> parameters;
	std::string method;
	std::string methodNameSpace;
	std::string methodResponse;
	
	BeerSoapProtocol *parent;
	
public:	
	BeerSoapServiceMethod(std::string methodName, std::string methodNameSpace, BeerSoapProtocol *parent);
	void(*callback)(void *, DomElement *);
	void *customPtr;
	
	void(*exceptioncallback)(void *, DomElement *);
	void *exceptionPtr;
	
	void registerExceptionCallback(BeerSoapCallBackDef exceptioncallback, void *exceptionPtr);
	bool hasExceptionCallback();
	BeerSoapCallBackDef getExceptionCallback();
	void *getExceptionPointer();
	
	void pushParameter(std::string name, std::string value);
	void registerCallBack(void(*)(void *, DomElement *), void *customPtr);
	
	bool run();
	
	void setMethodResponse(std::string response);
	std::string getMethodResponse();
	bool isResponseFor(std::string response);
	bool callCallBack(DomElement *data);
	
	std::string getMethodName();
	
};
