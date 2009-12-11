/*
 *  test.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include "test.h"

static void RegisterUserCallBack(void *customPtr, DomElement *Data) {
	printf("CALLBACK RECEBIDA COM SUCESSO!!\n");
}

static void ExceptionCallBack(void *customPtr,DomElement *Data) {
	printf("CALLBACK de Exception Recebida também!!\n");
}


int main(int argc, char *argv[]) {
/*	BeerSoapService *soapService = new BeerSoapService;
	soapService->setServiceURL("http://200.195.63.49:8080/service/MobusServer");
	soapService->setServiceName("http://acesso.informant.com.br/");
	soapService->setServiceNameSpace("http://acesso.informant.com.br");
	
	soapService->registerExceptionCallBack(ExceptionCallBack,NULL);
	
	BeerSoapServiceMethod *authenticate = soapService->callMethod("Authenticate","http://server.mobus.com.br/");
	authenticate->pushParameter("terminal_id","blah");
	authenticate->pushParameter("terminal_key","blah");
	authenticate->setMethodResponse("AuthenticateResponse");
	authenticate->registerCallBack(RegisterUserCallBack,NULL);
	authenticate->run();  */
	BeerSoapLoader *loader;
	try {
		loader = new BeerSoapLoader("http://200.195.63.49:8080/service/MobusServer?WSDL");
	} catch (int e) {
		printf("ERRO!!\n");
	}
	
	BeerSoapService *service = loader->getService("MobusServer");

	if(!service) {
		printf("ERRO SERVIÇO NÃO ENCONTRADO!!\n");
	}

	service->registerExceptionCallBack(ExceptionCallBack,NULL);
	
	BeerSoapServiceMethod *authenticate = loader->getMethod("Authenticate",service);

	std::vector<std::string > parameterList = loader->describeMethod("Authenticate");
	
	for(unsigned int i = 0; i < parameterList.size(); i++) {
		cout << "Parameter: " << parameterList[i] << " Type: " << loader->getParameterType("Authenticate", parameterList[i]) << endl;
	}

	for(;;) {

	std::string terminalLogin;
	std::string terminalPassword;

	std::cout << "login: ";
	std::cin >> terminalLogin;
	std::cout << "password: ";
	std::cin >> terminalPassword;


	authenticate->pushParameter("terminal_id", terminalLogin);
	authenticate->pushParameter("terminal_key",terminalPassword);
	authenticate->setMethodResponse("AuthenticateResponse");
	authenticate->registerCallBack(RegisterUserCallBack,NULL);
//	authenticate->registerExceptionCallback(ExceptionCallBack,NULL);
	authenticate->run(); 

	}

	
}
