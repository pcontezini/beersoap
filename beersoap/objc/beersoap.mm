//
//  beersoap.m
//  beersoap
//
//  Created by Piero Contezini on 10/09/08.
//  Copyright 2008 Informant. All rights reserved.
//

#import "beersoap.h"
#import "BeerSoapServiceMethod.h"
#import "BeerHttp.h"

@implementation BeerSoapService
- (id)init {
	self = [super init];
#ifndef USE_LIBXML2
	rootOut = (NSXMLElement *)[NSXMLNode elementWithName:@"SOAP-ENV:Envelope" URI: @"SOAP-ENV"];
	xmlDocOut = [[NSXMLDocument alloc] initWithRootElement:rootOut];
	
	[xmlDocOut setVersion:@"1.0"];
	
	[xmlDocOut setCharacterEncoding:@"UTF-8"];	
	
	NSXMLNode *SoapEnvNS = [NSXMLNode namespaceWithName: @"SOAP-ENV" stringValue: @"http://schemas.xmlsoap.org/soap/envelope/"];
	[rootOut addNamespace: SoapEnvNS];

	NSXMLNode *SoapNS = [NSXMLNode namespaceWithName: @"soap" stringValue: @"http://schemas.xmlsoap.org/wsdl/soap/"];
	[rootOut addNamespace: SoapNS];

	NSXMLNode *XsdNS = [NSXMLNode namespaceWithName: @"xsd" stringValue: @"http://www.w3.org/2001/XMLSchema"];
	[rootOut addNamespace: XsdNS];

	NSXMLNode *XsiNS = [NSXMLNode namespaceWithName: @"xsi" stringValue: @"http://www.w3.org/2001/XMLSchema-instance"];
	[rootOut addNamespace: XsiNS];
	
	bodyOut = (NSXMLElement *)[NSXMLNode elementWithName: @"SOAP-ENV:Body" URI: @"SOAP-ENV"];
	[rootOut addChild: (NSXMLNode *) bodyOut ];

#else
	xmlDocOut = new xml_tree("SOAP-ENV:Envelope","UTF-8","1.0");
	xmlDocOut->new_attribute("xmlns:SOAP-ENV","http://schemas.xmlsoap.org/soap/envelope/");
	xmlDocOut->new_attribute("xmlns:soap","http://schemas.xmlsoap.org/wsdl/soap/");
	xmlDocOut->new_attribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");
	xmlDocOut->new_attribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	
	bodyOut = xmlDocOut->new_element("SOAP-ENV:Body");
	
	
	xmlDocIn = new xml_tree("env:Envelope","UTF-8","1.0");
	
#endif

	Methods = [ NSMutableArray arrayWithCapacity: 10 ]; // capacidade inicial, ela aumenta com a necessidade
	
	http = [[ BeerHttp alloc ] init];
	
	return(self);
}

- (void)dealloc {
	delete xmlDocOut;
	delete xmlDocIn;
	[super dealloc];
}

- (void) methodRefresh {
#ifndef USE_LIBXML2	
	[ bodyOut setChildren: nil ];
#else
//	printf("DUH %d\n", bodyOut->get_children().size());
	xmlDocOut->remove_elements(bodyOut->get_children());
//	printf("DUH2 %d\n", bodyOut->get_children().size());	
#endif

}


- (void) setServiceName: (NSString *) name {
	serviceName = name;
}

- (void) setTnsNS: (NSString *)Tns {
	tNS = Tns;
#ifndef USE_LIBXML2
	NSXMLNode *TnsNS = [NSXMLNode namespaceWithName: @"tns" stringValue: Tns];
	[rootOut addNamespace: TnsNS];
#else
	xmlDocOut->new_attribute("xmlns:tns", [Tns UTF8String] );
#endif
	
}

- (void) setServiceURL: (NSString *) URL {
	serviceURL = URL;
}

#ifndef USE_LIBXML2
- (void) RegisterXMLMethod: (NSXMLNode *)methodXmlNode
#else
- (void) RegisterXMLMethod
#endif
{
	[self methodRefresh ];
#ifndef USE_LIBXML2
	[ bodyOut addChild: methodXmlNode ];
#endif
}



- (void) dumpOut {
#ifndef USE_LIBXML2
	NSData *xmlData = [xmlDocOut XMLDataWithOptions:NSXMLNodePrettyPrint];
	printf("DumpOut\n %s\n",[xmlData bytes]);
	[xmlData release];
#else 
	char *p = xmlDocOut->xml(true);
	printf("DumpOut: %s\n",p);
	free(p);
#endif
}

- (unsigned char *) dump {
#ifndef USE_LIBXML2
	NSData *xmlData = [xmlDocOut XMLDataWithOptions:NSXMLNodePrettyPrint];
	return ((unsigned char *)[xmlData bytes]);
#else
	char *p = xmlDocOut->xml(true);
	return((unsigned char *)p);
#endif
}


- (BearSoapServiceMethod *) callMethod: (NSString *) method methodns: (NSString *) methodns {
	BearSoapServiceMethod *newMethod = [[BearSoapServiceMethod alloc] initWithParameters:self MethodName: method MethodNS: methodns];
	[ Methods addObject: newMethod ];
	return(newMethod);
}

- (int) getMethodId: (BearSoapServiceMethod *) methodPtr {
	return ( (int) [ Methods indexOfObject: methodPtr ] );
}

- (BOOL) run {
	if(!serviceURL) {
		return(FALSE);
	}
	// aqui pode ter leak, pra onde vai esse pointer??
	if(![http requestPostHttpMessage: serviceURL PostData: [NSString stringWithUTF8String:(char *)[self dump]]]) {
		return(FALSE);
	}
	
	[http run];
	
	if(![http requestCompleted]) {
		return(FALSE);
	}
	

	printf("DEBUG: %s\n", [http requestGetData] );
	
	[self soapReceiveMessage ];
	
	return(TRUE);
	
}

- (BOOL) soapReceiveMessage {
#ifndef USE_LIBXML2
	NSError *err;
	xmlDocIn = [[NSXMLDocument alloc] initWithXMLString: [NSString stringWithUTF8String: (char *)[http requestGetData]] options: 0 error: &err ];
//	xmlDocIn = [[NSXMLDocument alloc] initWithContentsOfURL: [NSURL fileURLWithPath: @"/Users/pcontezini/devel/geotagger/beersoap/tests/sample_answer.xml" ] options: 0 error: &err ];
	if ( [err code] ) {
		printf("ERRO: %s\n",[[err localizedDescription]UTF8String] );
		return(FALSE);
	}
	printf("recepcao de mensagem codigo: %d\n", [err code]);
	rootIn = [xmlDocIn rootElement];
	
	
	NSXMLNode *node = [rootIn nextNode];
	BearSoapServiceMethod *method;
	
	while ( node = [node nextNode] ) {
		if(! [node name] ) {
			continue;
		}
		if ( [[node localName] hasSuffix: @"Fault"]) {
//			printf("DEU ERRO! Fault via SOAP!\n");
			[self soapProcessException: [node children]];
			return(TRUE); // mesmo assim true afinal a mensagem veio
		}
		if ( method = [self lookMethodResponse: [node localName]] ) { // o primeiro que retorna true nos paramos, só uma resposta ..
			[self soapProcessResponse: [node children] methodReceiver: method];
			return(TRUE);
		}
//		printf("Node name: %s localname: %s %s\n", [[node name] UTF8String], [[node localName] UTF8String]);
	}

	return(FALSE);
#else
	xmlDocIn->remove_elements(xmlDocIn->get_children());

	char *p = (char *)[http requestGetData];
	if(!p) {
		return(FALSE);
	}
	if(!xmlDocIn->load(p,strlen(p))) {
		return(FALSE);
	}
	
	BearSoapServiceMethod *method;
	
	vector<element *> children = xmlDocIn->get_children();
	for(int i = 0; i < children.size(); i++) {
		printf("NAME: %s\n",children[i]->get_name().c_str());
		if(children[i]->get_name() == "Body") {
			vector<element *> bchildren = children[i]->get_children();
			for(int c = 0; c < bchildren.size(); c++) {
				printf("SNAME: %s\n",bchildren[c]->get_name().c_str());
				if(bchildren[c]->get_name() == "Fault") {
					[self soapProcessException: bchildren[c] ];
					return(TRUE);
				}
				if ( method = [self lookMethodResponse: [NSString stringWithUTF8String: bchildren[c]->get_name().c_str()]] ) { // Objective-C C++ POG
					[self soapProcessResponse: bchildren[c] methodReceiver: method];
					return(TRUE);
				} 
					
			}
		}
/*		if(children[i]->get_name().) {
			[self soapProcessException: children[i]];
			return(TRUE);
		}
		if ( method = [self lookMethodResponse: [NSString stringWithUTF8String: children[i]->get_value().c_str()]] ) { // Objective-C C++ POG
			[self soapProcessResponse: children[i] methodReceiver: method];
			return(TRUE);
		} */
	}
	
	return(FALSE);
	
#endif
}

#ifndef USE_LIBXML2
- (BOOL) soapProcessResponse: (NSArray *) data methodReceiver: (BearSoapServiceMethod *) methodReceiver {
#else
- (BOOL) soapProcessResponse: (element *) data methodReceiver: (BearSoapServiceMethod *) methodReceiver {
#endif
	[ methodReceiver callCallBack: data ];
	return(TRUE);
	
}

#ifndef USE_LIBXML2
- (BOOL) soapProcessException: (NSArray *) data {
#else 
- (BOOL) soapProcessException: (element *) data {	
#endif
	if(exceptionCallback) {
		(*exceptionCallback)(exceptionCustomPtr, data);
		return(TRUE);
	} else {
		return(FALSE);
	}
}

- (BearSoapServiceMethod *) lookMethodResponse: (NSString *)methodResponseName {
	BearSoapServiceMethod *lookup;
	for ( lookup in Methods ) {
//		printf("passando na array: %s %s\n", [[lookup getMethodResponse] UTF8String], [methodResponseName UTF8String]);
		if ( [ lookup isResponseFor: methodResponseName ] ) {
//			printf("metodo querendo a resposta encontrado!!\n");
			return(lookup);
		}
	}
	return(NULL);
}

#ifndef USE_LIBXML2
- (void) registerExceptionCallBack: (void(*)(void *,NSArray *)) callbackPtr customPtr: (void *)customPtr {
#else
- (void) registerExceptionCallBack: (void(*)(void *,element *)) callbackPtr customPtr: (void *)customPtr {
#endif
	exceptionCustomPtr = customPtr;
	exceptionCallback = callbackPtr;
}

#ifdef USE_LIBXML2
- (element *) createMethod: (NSString *) name {
	return(bodyOut->new_child([name UTF8String]));
}
#endif



@end

@implementation BeerSoap
- (BOOL) enumarateSoapServicesFromWSDL: (NSString *) URL {
	return(FALSE);
}
- (BeerSoapService *)CreateBearSoapService {
	return(NULL);
}
- (BeerSoapService *)getBeerSoapService: (NSString *) name {
	return(NULL);

}

@end
