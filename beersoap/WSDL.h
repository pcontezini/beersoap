#ifndef WSDL_H
#define WSDL_H
/*
 *  WSDL.h
 *  beersoap
 *
 *  Created by Piero Contezini on 05/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 *
 * De acordo com a especificacao encontrada em: http://www.w3.org/TR/wsdl
 *
 */

#include <string>
#include <vector>

#include "DomTree.h"


class WSDLType {
	bool primitive;
	std::string name;
	std::string type;
	unsigned int minOccurs;
	
	std::vector<WSDLType *> types;
public:
	WSDLType(std::string newName = "", std::string newType = "");
	~WSDLType();
	
	void setPrimitive();
	bool getPrimitive();
	
	std::string getName();
	std::string getType();
	
	unsigned int getMinOccurs();
	void setMinOccurs(unsigned int newMinOccurs);
	
	void pushType(WSDLType *newType);
	
	void setTypes(std::vector<WSDLType *> newTypes);
	std::vector<WSDLType *>& getTypes();
};

class WSDLPart {
	WSDLType *element;
	std::string name;
public:
	WSDLPart(std::string newName);
	~WSDLPart();
	
	std::string getName();
	
	void setElement(WSDLType *type);
	WSDLType *getElement();
};

class WSDLMessage {
	std::string name;
	std::vector<WSDLPart *> parts;
public:
	WSDLMessage(std::string newName);
	~WSDLMessage();
	
	std::string getName();
	
	void pushPart(WSDLPart *newPart);
	WSDLPart *getPart(std::string name);
	
	std::vector<WSDLPart *>& getParts();
	
};

class WSDLOperation {
	std::string name;
	std::string parameterOrder;
	
	WSDLMessage *input;
	WSDLMessage *output;
	WSDLMessage *fault;
public:
	WSDLOperation(std::string name);
	~WSDLOperation();
	
	std::string getName();
	
	void setParameterOrder(std::string newParameterOrder);
	std::string getParameterOrder();
	
	void setInput(WSDLMessage *message);
	WSDLMessage *getInput();
	void setOutput(WSDLMessage *message);
	WSDLMessage *getOutput();
	void setFault(WSDLMessage *message);
	WSDLMessage *getFault();
	
};

class WSDLPortType {
	std::string name;
	std::vector<WSDLOperation *> operations;
public:
	WSDLPortType(std::string newName);
	~WSDLPortType();
	
	std::string getName();
	
	void pushOperation(WSDLOperation *newOperation);
	WSDLOperation *getOperation(std::string name);
	std::vector<WSDLOperation *> getOperations();
};


class WSDLBinding {
	std::string name;
	WSDLPortType *type;
	std::vector<WSDLOperation *> operations;
public:
	WSDLBinding(std::string newName);
	~WSDLBinding();
	
	std::string getName();
	void pushOperation(WSDLOperation *newOperation);
	WSDLOperation *getOperation(std::string name);
	std::vector<WSDLOperation *>& getOperations();

	void setType(WSDLPortType *newType);
	WSDLPortType *getType();
	
};

class WSDLPort {
	std::string name;
	WSDLBinding *binding;
	std::string location;
public:
	WSDLPort(std::string name);
	~WSDLPort();
	
	std::string getName();
	
	void setLocation(std::string newLocation);
	std::string getLocation();
	
	void setBinding(WSDLBinding *newBinding);
	WSDLBinding *getBinding();
	
};


class WSDLService {
	std::string name;
	std::string nameSpace;
	std::vector<WSDLPort *> ports;
public:
	WSDLService(std::string newName);
	~WSDLService();
	
	std::string getName();
	void setNameSpace(std::string newNameSpace);
	std::string getNameSpace();
	
	void pushPort(WSDLPort *newPort);
	WSDLPort *getPort(std::string name);
	
	std::vector<WSDLPort *>& getPorts();
};

class WSDL {
	std::vector<WSDLService *> services;
	std::vector<WSDLPort *> ports;
	std::vector<WSDLBinding *> bindings;
	std::vector<WSDLPortType *> portTypes;
	std::vector<WSDLOperation *> operations;
	std::vector<WSDLMessage *> messages;
	std::vector<WSDLPart *> parts;
	std::vector<WSDLType *> types;
	std::string nameSpace;
	
	std::vector<std::string> primitives; // tipos de primitivos conhecidos
public:
	WSDL(DomTree *wsdlFile);

	void dump();
	
	bool isPrimitive(std::string type);
	void parseTypes(DomElement *typesList);
	WSDLType *parseElement(DomElement *element);
	void parseComplexType(DomElement *element, WSDLType *type);
	void parseMessage(DomElement *element);
	WSDLPart *parsePart(DomElement *element);
	void parsePortType(DomElement *element);
	WSDLOperation *parseOperation(DomElement *element);
	void parseBinding(DomElement *element);
	void parseService(DomElement *element);
	WSDLPort *parsePort(DomElement *element);

	std::vector<WSDLService *>& getServices();
	std::vector<WSDLPort *>& getPorts();
	std::vector<WSDLBinding *>& getBindings();
	std::vector<WSDLPortType *>& getPortTypes();
	std::vector<WSDLOperation *>& getOperations();
	std::vector<WSDLMessage *>& getMessages();
	std::vector<WSDLPart *>& getParts();
	std::vector<WSDLType *>& getTypes();

	std::string getNameSpace();

};


#endif
