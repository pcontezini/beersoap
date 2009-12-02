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
	BeerSoapService *soapService = new BeerSoapService;
	soapService->setServiceURL("http://200.195.63.49:8080/service/MobusServer");
	soapService->setServiceName("http://acesso.informant.com.br/");
	soapService->setServiceNameSpace("http://acesso.informant.com.br");
	
	soapService->registerExceptionCallBack(ExceptionCallBack,NULL);
	
	BeerSoapServiceMethod *authenticate = soapService->callMethod("Authenticate","http://server.mobus.com.br/");
	authenticate->pushParameter("terminal_id","blah");
	authenticate->pushParameter("terminal_key","blah");
	authenticate->setMethodResponse("AuthenticateResponse");
	authenticate->registerCallBack(RegisterUserCallBack,NULL);
	authenticate->run();
	
	
}
