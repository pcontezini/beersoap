/*
 *  WSDL.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 05/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include <iostream>

#include "WSDL.h"


WSDL::WSDL(DomTree *wsdlFile) {
	
	primitives.push_back("xs:string");
	primitives.push_back("xs:int");
	primitives.push_back("xs:float");
	primitives.push_back("xs:long");

	std::vector<DomAttribute *> attributes = wsdlFile->getAttributes();
	for(unsigned int i = 0; i < attributes.size(); i++) {
		if(attributes[i]->getName() == "targetNamespace") {
			nameSpace = attributes[i]->getValue();
		}
	}
	
	std::vector<DomElement *> children = wsdlFile->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "types") {
//			printf("types\n");
			parseTypes(children[i]);
		}
		if(children[i]->getName() == "message") {
//			printf("messages\n");
			parseMessage(children[i]);
		}
		if(children[i]->getName() == "portType") {
//			printf("porttypes\n");
			parsePortType(children[i]);
		}
		if(children[i]->getName() == "binding") {
//			printf("bindings\n");
			parseBinding(children[i]);
		}
		if(children[i]->getName() == "service") {
//			printf("services\n");
			parseService(children[i]);
		}
	}
}

void WSDL::dump() {
	for(unsigned int i = 0; i < services.size(); i++) {
		std::vector<WSDLPort *> servicePorts = services[i]->getPorts();
		printf("Service: %s NameSpace: %s Ports size: %d\n", services[i]->getName().c_str(), services[i]->getNameSpace().c_str(),(int)servicePorts.size());
		for(unsigned int p = 0; p < servicePorts.size(); p++) {
			printf("	Port name: %s Location: %s Binding Name: %s\n",
				   servicePorts[p]->getName().c_str(),
				   servicePorts[p]->getLocation().c_str(),
				   servicePorts[p]->getBinding()->getName().c_str());
		}
	}
	
	for(unsigned int i = 0; i < bindings.size(); i++) {
		std::vector<WSDLOperation *> bindingOperations = bindings[i]->getOperations();
		printf("Binding: %s Operations: %d\n", bindings[i]->getName().c_str(), (int) bindingOperations.size());
		for(unsigned o = 0; o < bindingOperations.size(); o++) {
			std::string inputName, outputName, faultName;
			if(bindingOperations[o]->getInput()) {
				inputName = bindingOperations[o]->getInput()->getName();
			}
			if(bindingOperations[o]->getOutput()) {
				outputName = bindingOperations[o]->getOutput()->getName();
			}
			if(bindingOperations[o]->getFault()) {
				faultName = bindingOperations[o]->getFault()->getName();
			}
			printf("	Operation name: %s Parameter Order: %s Input: %s Output: %s Fault: %s\n",
				   bindingOperations[o]->getName().c_str(),
				   bindingOperations[o]->getParameterOrder().c_str(),
				   inputName.c_str(),
				   outputName.c_str(),
				   faultName.c_str());
		}
	}
	
	for(unsigned int i = 0; i < messages.size(); i++) {
		std::vector<WSDLPart *> messageParts = messages[i]->getParts();
		printf("Message: %s Parts: %d\n",
			   messages[i]->getName().c_str(), (int)messageParts.size());
		for(unsigned int p = 0; p < messageParts.size(); p++) {
			printf("	Part: %s Element: %s\n",
				   messageParts[p]->getName().c_str(),
				   messageParts[p]->getElement()->getName().c_str());
		}
	}
	
	for(unsigned int i = 0; i < types.size(); i++) {
		printf("Data Type: %s Type: %s Primitive: %d MinOccurs: %d\n",
			   types[i]->getName().c_str(),
			   types[i]->getType().c_str(),
			   types[i]->getPrimitive(),
			   types[i]->getMinOccurs());
			std::vector<WSDLType *> subTypes = types[i]->getTypes();
			for(unsigned int s = 0; s < subTypes.size(); s++) {
				printf("SubData Type: %s Type: %s Primitive: %d MinOccurs: %d\n",
					   subTypes[s]->getName().c_str(),
					   subTypes[s]->getType().c_str(),
					   subTypes[s]->getPrimitive(),
					   subTypes[s]->getMinOccurs());
			}
	}
}

bool WSDL::isPrimitive(std::string type) {
	for(unsigned int i = 0; i < primitives.size(); i++) {
		if(primitives[i] == type) {
			return(true);
		}
	}
	return(false);
}

void WSDL::parseTypes(DomElement *typesList) {
	std::vector<DomElement *> children = typesList->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "schema") { // a principio ignoramos o namespace, nas proximas versoes talvez
			std::vector<DomElement *> schemaChildren = children[i]->getChildren();
			for(unsigned int s = 0; s < schemaChildren.size(); s++) {
				if(schemaChildren[s]->getName() == "element") {
//					std::cout << "simple type: " << schemaChildren[s]->getAttribute("name") << std::endl;
					WSDLType *newType = parseElement(schemaChildren[s]);
					types.push_back(newType);
				}
				if(schemaChildren[s]->getName() == "complexType") {
//					std::cout << "complex type: " << schemaChildren[s]->getAttribute("name") << std::endl;
					WSDLType *type = NULL;
					for(unsigned int c = 0; c < types.size(); c++) {
						if(types[c]->getName() == schemaChildren[s]->getAttribute("name")) {
//							std::cout << "ja tem na lista: " << types[c]->getName() << std::endl;
							type = types[c];
						}
					}
					parseComplexType(schemaChildren[s], type);
				}
			}
		}
	}
}

WSDLType *WSDL::parseElement(DomElement *element) {
	WSDLType *newType = new WSDLType(element->getAttribute("name"), element->getAttribute("type"));
	unsigned int minOccurs = atoi((char *)element->getAttribute("minOccurs").c_str());
	if(minOccurs > 0) {
		newType->setMinOccurs(minOccurs);
	}
	if(isPrimitive(element->getAttribute("type"))) {
		newType->setPrimitive();
	}
	   
//	types.push_back(newType);
	return(newType);
}


void WSDL::parseComplexType(DomElement *element, WSDLType *type) {
	std::vector<DomElement *> children = element->getChildren();
	WSDLType *newType = NULL;
	if(type != NULL) {
//	        std::cout << "reutilizando type" << std::endl;
		newType = type;
	} else {
//		std::cout << "criando novo type" << std::endl;
		newType = new WSDLType(element->getAttribute("name"));
	}
	for(unsigned int i = 0; i < children.size(); i++) {
//		printf("name: %s\n", children[i]->getName().c_str());
		if(children[i]->getName() == "sequence") { // dados sao em sequencia
			std::vector<DomElement *> sequenceChildren = children[i]->getChildren();
			for(unsigned int s = 0; s < sequenceChildren.size(); s++) {
				if(sequenceChildren[s]->getName() == "element") {
//					printf("achou subelemento..\n");
					WSDLType *subType = parseElement(sequenceChildren[s]);
					newType->pushType(subType);
				}
			}
		}
	}
	types.push_back(newType);
}

void WSDL::parseMessage(DomElement *element) {
	WSDLMessage *newMessage = new WSDLMessage(element->getAttribute("name"));
	std::vector<DomElement *> children = element->getChildren();
	for ( unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "part") {
			WSDLPart *newPart = parsePart(children[i]);
			newMessage->pushPart(newPart);
		}
	}
	messages.push_back(newMessage);
}

WSDLPart *WSDL::parsePart(DomElement *element) {
	WSDLPart *newPart = new WSDLPart(element->getAttribute("name"));
	std::string elementName = element->getAttribute("element");
	elementName = elementName.substr(elementName.find(':')+1);
	for(unsigned int i = 0; i < types.size(); i++) {
		if(types[i]->getName() == elementName) {
			newPart->setElement(types[i]);
		}
	}
	parts.push_back(newPart);
	return(newPart);
}

void WSDL::parsePortType(DomElement *element) {
	WSDLPortType *newPort = new WSDLPortType(element->getAttribute("name"));
	std::vector<DomElement *> children = element->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "operation") {
			WSDLOperation *newOperation = parseOperation(children[i]);
			newPort->pushOperation(newOperation);
		}
	}
	portTypes.push_back(newPort);
}

WSDLOperation *WSDL::parseOperation(DomElement *element) {
	WSDLOperation *newOperation = new WSDLOperation(element->getAttribute("name"));
	
	newOperation->setParameterOrder(element->getAttribute("parameterOrder"));
	
	std::vector<DomElement *> children = element->getChildren();
	for (unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "input") {
			std::string inputName = children[i]->getAttribute("message");
			inputName = inputName.substr(inputName.find(':')+1);
//			printf("inputName: %s\n", inputName.c_str());
			for(unsigned int a = 0; a < messages.size(); a++) {
				if(messages[a]->getName() == inputName) {
//					printf("achou input\n");
					newOperation->setInput(messages[a]);
				}
			}
		}
		if(children[i]->getName() == "output") {
			std::string outputName = children[i]->getAttribute("message");
			outputName = outputName.substr(outputName.find(':')+1);			
//			printf("outputName: %s\n", outputName.c_str());			
			for(unsigned int a = 0; a < messages.size(); a++) {
				if(messages[a]->getName() == outputName) {
//					printf("achou output\n");					
					newOperation->setOutput(messages[a]);
				}
			}
		}
		if(children[i]->getName() == "fault") {
			std::string faultName = children[i]->getAttribute("message");
			faultName = faultName.substr(faultName.find(':')+1);			
//			printf("faultName: %s\n", faultName.c_str());						
			for(unsigned int a = 0; a < messages.size(); a++) {
				if(messages[a]->getName() == faultName) {
//					printf("achou fault\n");					
					newOperation->setFault(messages[a]);
				}
			}
		}
	}
	
	operations.push_back(newOperation);
	return(newOperation);
}

void WSDL::parseBinding(DomElement *element) {
	WSDLBinding *newBinding = new WSDLBinding(element->getAttribute("name"));
	std::string typeName = element->getAttribute("type");
	for(unsigned int i = 0; i < portTypes.size(); i++) {
		if(portTypes[i]->getName() == typeName) {
			newBinding->setType(portTypes[i]);
		}
	}
	std::vector<DomElement *> children = element->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "operation") {
			std::string operationName = children[i]->getAttribute("name");
			for(unsigned int a = 0; a < operations.size(); a++) {
				if(operations[a]->getName() == operationName) {
					newBinding->pushOperation(operations[a]);
				}
			}
		}
	}
	
	bindings.push_back(newBinding);
}

void WSDL::parseService(DomElement *element) {
	WSDLService *newService = new WSDLService(element->getAttribute("name"));
	newService->setNameSpace(element->getAttribute("targetNamespace"));
	
	std::vector<DomElement *> children = element->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "port") {
			WSDLPort *newPort = parsePort(children[i]);
			newService->pushPort(newPort);
		}
	}
	
	services.push_back(newService);
}

WSDLPort *WSDL::parsePort(DomElement *element) {
	WSDLPort *newPort = new WSDLPort(element->getAttribute("name"));
	std::string bindingName = element->getAttribute("binding");
	bindingName = bindingName.substr(bindingName.find(':')+1); // aqui é uma POG, o Jboss sempre manda um prefixo de namespace TNS, mas como é um atributo nao deveria existir..
//	printf("binding name: %s\n", bindingName.c_str());
	for(unsigned int i = 0; i < bindings.size(); i++) {
		if(bindings[i]->getName() == bindingName) {
			newPort->setBinding(bindings[i]);
		}
	}
	std::vector<DomElement *> children = element->getChildren();
	for(unsigned int i = 0; i < children.size(); i++) {
		if(children[i]->getName() == "address") {
			newPort->setLocation(children[i]->getAttribute("location"));
		}
	}
	ports.push_back(newPort);
	return(newPort);
}

std::vector<WSDLService *>& WSDL::getServices() {
	return(services);
}

std::vector<WSDLPort *>& WSDL::getPorts() {
	return(ports);
}

std::vector<WSDLBinding *>& WSDL::getBindings() {
	return(bindings);
}

std::vector<WSDLPortType *>& WSDL::getPortTypes() {
	return(portTypes);
}

std::vector<WSDLOperation *>& WSDL::getOperations() {
	return(operations);
}

std::vector<WSDLMessage *>& WSDL::getMessages() {
	return(messages);
}

std::vector<WSDLPart *>& WSDL::getParts() {
	return(parts);
}

std::vector<WSDLType *>& WSDL::getTypes() {
	return(types);
}

std::string WSDL::getNameSpace() {
	return(nameSpace);
}
