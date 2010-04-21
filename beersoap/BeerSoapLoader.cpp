/*
 *  BeerSoapLoader.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 02/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "BeerSoapLoader.h"
#include "BeerSoapExceptions.h"

using namespace std;

BeerSoapLoader::BeerSoapLoader(std::string endPointURL) {
	WSDLData = NULL;
	interface = new CurlInterface();

	if(!interface->doGet(endPointURL)) {
		throw BeerSoapInvalidEndpoint;
	}
	this->WSDLUrl = endPointURL;
	xmlWSDL = new DomTree("definitions","UTF-8","1.0");
	if(!xmlWSDL) {
		throw BeerSoapAllocError;
	}
	char *data = interface->getData();
	if(!xmlWSDL->load(data,strlen(data))) {
		free(data);
		throw BeerSoapInvalidXML;
	}
	free(data);
	if(!loadWSDL()) {
		throw BeerSoapInvalidWSDL;
	}
}

bool BeerSoapLoader::loadWSDL() {

	WSDLData = new WSDL(xmlWSDL);
	
//	WSDLData->dump();
	
	return(true);
	
		
}


BeerSoapLoader::~BeerSoapLoader() {
	free(interface);
	if(xmlWSDL) {
		free(xmlWSDL);
	}
	if(WSDLData)
		delete WSDLData;
}

BeerSoapService *BeerSoapLoader::getService(std::string name) {
	BeerSoapService *newService = new BeerSoapService;
	newService->setName(name);
	std::vector<WSDLService *> services = WSDLData->getServices();
	for(unsigned int i = 0; i < services.size(); i++) {
		if(services[i]->getName() == name) {
			cout << "Location: " << services[i]->getPorts()[0]->getLocation() << endl;
			newService->setURL(services[i]->getPorts()[0]->getLocation());
			return(newService);
		}
	}
	return(NULL);
}

void BeerSoapLoader::setService(BeerSoapService *service) {

}

std::string BeerSoapLoader::getServiceURL(std::string serviceName) {
	std::vector<WSDLService *> services = WSDLData->getServices();
	for(unsigned int i = 0; i < services.size(); i++) {
		if(services[i]->getName() == serviceName) {
			if(services[i]->getPorts().size()) {
				return(services[i]->getPorts()[0]->getLocation());
			}
		}
	}
	return("");
}

std::vector<std::string> BeerSoapLoader::listServices() {
	std::vector<WSDLService *> services = WSDLData->getServices();
	std::vector<std::string> serviceList;
	for(unsigned int i = 0; i < services.size(); i++) {
		serviceList.push_back(services[i]->getName());
	}
	return(serviceList);
}

std::vector<std::string> BeerSoapLoader::listOperations() {
	std::vector<WSDLOperation *> operations = WSDLData->getOperations();
	std::vector<std::string> operationList;
	for(unsigned int i = 0; i < operations.size(); i++) {
		operationList.push_back(operations[i]->getName());
	}
	return(operationList);
}


BeerSoapServiceMethod *BeerSoapLoader::getMethod(std::string name, BeerSoapService *service) {
	std::vector<WSDLService *> services = WSDLData->getServices();
	std::vector<WSDLOperation *> operations = WSDLData->getOperations();
	BeerSoapServiceMethod *newMethod = NULL;
	for(unsigned int s = 0; s < services.size(); s++) {
		if(services[s]->getName() == service->getName()) {
			for(unsigned int i = 0; i < operations.size(); i++) {
				if(operations[i]->getName() == name) {
					newMethod = service->callMethod(name,WSDLData->getNameSpace());
				}
			}
		}
	}
	return(newMethod);
}

// aqui deveria pegar os metodos apenas do servico selecionado. mas enfim..
std::vector<BeerSoapServiceMethod *> BeerSoapLoader::getMethods() {
	return(methods);
}

std::vector<std::string> BeerSoapLoader::describeMethod(std::string name) {
	std::vector<std::string> parameterList;
	std::vector<WSDLOperation *> operations = WSDLData->getOperations();

	for(unsigned int i = 0; i < operations.size(); i++) {
		if(operations[i]->getName() == name) {
			WSDLMessage *message = operations[i]->getInput();
//			cout << "Input name:"<< message->getName() << endl;
			std::vector<WSDLPart *> parts = message->getParts();
			for(unsigned p = 0; p < parts.size(); p++) {
				std::vector<WSDLType *> types = parts[i]->getElement()->getTypes();
//				cout << "Part name:" << parts[i]->getName() << " " << parts[i]->getElement()->getName()  << " size: " << types.size() << endl;
				for(unsigned t = 0; t < types.size(); t++) {
//					cout << "Tipo nome: " << types[t]->getName() << " Classe: " << types[t]->getType() << endl;
					parameterList.push_back(types[t]->getName());
				}
			}

		}
	}
	return(parameterList);
}

std::string BeerSoapLoader::getParameterType(std::string method, std::string parameter) {
	std::vector<std::string> parameterList;
	std::vector<WSDLOperation *> operations = WSDLData->getOperations();
	
	for(unsigned int i = 0; i < operations.size(); i++) {
		if(operations[i]->getName() == method) {
			WSDLMessage *message = operations[i]->getInput();
			std::vector<WSDLPart *> parts = message->getParts();
			for(unsigned p = 0; p < parts.size(); p++) {
				std::vector<WSDLType *> types = parts[i]->getElement()->getTypes();
				for(unsigned t = 0; t < types.size(); t++) {
					if(types[t]->getName() == parameter) {
						return(types[t]->getType());
					}
					
				}
			}
			
		}
	}
	return("");
}
