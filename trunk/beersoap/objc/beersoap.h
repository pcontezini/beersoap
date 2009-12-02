//
//  beersoap.h
//  beersoap
//
//  Created by Piero Contezini on 10/09/08.
//  Copyright 2008 Informant. All rights reserved.
//
//#include "plataform.h"

#ifdef MACOSX_X86
#import <Cocoa/Cocoa.h>
#endif

#ifdef USE_LIBXML2
#import "xmldata.h"
#endif

// esta classe define um serviço SOAP disponível e registrado, ela manipula o envio e recepção de dados
@class BearSoapServiceMethod;
@class BeerHttp;

@interface BeerSoapService : NSObject {
	@public NSString *serviceName;
	// saida de dados
#ifndef USE_LIBXML2
	NSXMLDocument *xmlDocOut;
	NSXMLElement *rootOut;
	NSXMLElement *bodyOut;
	
	// entrada de dados
	NSXMLDocument *xmlDocIn;
	NSXMLElement *rootIn;
	void(*exceptionCallback)(void *,NSArray *);
#else
	xml_tree *xmlDocOut;
	element *rootIn;
	element *bodyOut;
	
	xml_tree *xmlDocIn;
	element *rootOut;
	void(*exceptionCallback)(void *,element *);	
#endif
	NSMutableArray *Methods;
	
	BeerHttp *http; // componente HTTP
	NSString *serviceURL;
	NSString *tNS;
	
	
	void *exceptionCustomPtr;
	
}
- (id) init;
- (void) methodRefresh;
- (void) setServiceName: (NSString *) name;
- (void) setTnsNS: (NSString *)Tns;
- (void) setServiceURL: (NSString *) URL;

- (BearSoapServiceMethod *) callMethod: (NSString *) method methodns: (NSString *) methodns;
- (int) getMethodId: (BearSoapServiceMethod *) methodPtr;


- (void) dumpOut;
- (unsigned char *) dump;
- (BOOL) run;
- (BOOL) soapReceiveMessage;
//- (BOOL) soapProcessResponse: (NSArray *) node;

- (BearSoapServiceMethod *) lookMethodResponse: (NSString *)methodResponseName;

#ifndef USE_LIBXML2
- (void) RegisterXMLMethod: (NSXMLNode *)methodXmlNode;
- (BOOL) soapProcessResponse: (NSArray *) data methodReceiver: (BearSoapServiceMethod *) methodReceiver;
- (void) registerExceptionCallBack: (void(*)(void *,NSArray *)) callbackPtr customPtr: (void *)customPtr;
- (BOOL) soapProcessException: (NSArray *) data;
#else
- (void) RegisterXMLMethod;
- (BOOL) soapProcessResponse: (element *) data methodReceiver: (BearSoapServiceMethod *) methodReceiver;
- (void) registerExceptionCallBack: (void(*)(void *,element *)) callbackPtr customPtr: (void *)customPtr;
- (BOOL) soapProcessException: (element *) data;
- (element *) createMethod: (NSString *) name;
#endif

@end


@interface BeerSoap : NSObject {
	
}
- (BOOL) enumarateSoapServicesFromWSDL: (NSString *) URL; // esta funcao deverá funcionar uma hora pra criar todos os BearSoapServices pra cada serviço
- (BeerSoapService *)CreateBearSoapService;
- (BeerSoapService *)getBeerSoapService: (NSString *)name;
@end
