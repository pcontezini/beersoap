/*
 *  BeerSoapProtocol.h
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */
#ifndef BEERSOAPPROTOCOL_H
#define BEERSOAPPROTOCOL_H
#include "DomTree.h"

class BeerSoapProtocol {
public:
	virtual ~BeerSoapProtocol() {};
	virtual int getMethodId(void *method) = 0;
	virtual void methodRefresh() = 0;
	virtual DomElement *createMethod(std::string methodName) = 0;
	virtual bool run() = 0;
};

#endif

