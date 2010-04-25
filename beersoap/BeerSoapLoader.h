/*
 *  BeerSoapLoader.h
 *  beersoap
 *
 *  Created by Piero Contezini on 02/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */
#ifndef BEERSOAPLOADER_H
#define BEERSOAPLOADER_H
#include "DomTree.h"
#include "BeerSoap.h"
#include "CurlInterface.h"
#include "WSDL.h"

// essa classe carrega um WSDL e mantem uma lista das interfaces bindeadas por ele, em listas
class BeerSoapLoader {
private:
	std::string WSDLUrl;
	
	DomTree *xmlWSDL;
	WSDL *WSDLData;
	
	BeerSoapService * service;
	std::vector<BeerSoapServiceMethod *> methods;
	
	CurlInterface *interface;

public:
	BeerSoapLoader(std::string endPointURL); // pode ser um arquivo, uma URL remota, ela é analisada pelo cURL
	~BeerSoapLoader();
	
	bool loadWSDL();
	void dump();
	
	BeerSoapService *getService(std::string name);
	void setService(BeerSoapService *service);
	std::vector<std::string> listServices();
	
	std::string getServiceURL(std::string serviceName);
	
	std::vector<std::string> listOperations();
	
	BeerSoapServiceMethod *getMethod(std::string name, BeerSoapService *service);
	std::string getMethodResponse(std::string name, BeerSoapService *service);
	std::vector<BeerSoapServiceMethod *> getMethods(); // cria uma copia, pois a original nao deve ser alterada	
	std::vector<std::string> describeMethod(std::string name); // retorna um vector de strings com os nomes dos parametros	
	std::string getParameterType(std::string method, std::string parameter); // retorna uma string com o tipo do parametro
	
	
};
#endif
