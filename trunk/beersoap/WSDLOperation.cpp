/*
 *  WSDLOperation.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 06/12/09.
 *  Copyright 2009 Informant. All rights reserved.
 *
 */

#include "WSDL.h"

WSDLOperation::WSDLOperation(std::string newName) {
	name = newName;
	input = NULL;
	output = NULL;
	fault = NULL;	
}

WSDLOperation::~WSDLOperation() {
	input = NULL;
	output = NULL;
	fault = NULL;
}

std::string WSDLOperation::getName() {
	return(name);
}

void WSDLOperation::setParameterOrder(std::string newParameterOrder) {
	parameterOrder = newParameterOrder;
}

std::string WSDLOperation::getParameterOrder() {
	return(parameterOrder);
}

void WSDLOperation::setInput(WSDLMessage *message) {
	this->input = message;
}

WSDLMessage *WSDLOperation::getInput() {
	return(input);
}

void WSDLOperation::setOutput(WSDLMessage *message) {
	this->output = message;
}

WSDLMessage *WSDLOperation::getOutput() {
	return(output);
}

void WSDLOperation::setFault(WSDLMessage *message) {
	this->fault = message;
}

WSDLMessage *WSDLOperation::getFault() {
	return(fault);
}