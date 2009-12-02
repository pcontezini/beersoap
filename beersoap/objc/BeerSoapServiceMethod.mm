//
//  BeerSoapServiceMethod.m
//  beersoap
//
//  Created by Piero Contezini on 11/09/08.
//  Copyright 2008 Informant. All rights reserved.
//

#import "BeerSoapServiceMethod.h"
#import "beersoap.h"

@class BeerSoapService;

@implementation BearSoapServiceMethod
- (id) initWithParameters: (id) daddy MethodName: (NSString *)MethodName MethodNS: (NSString *)MethodNS {
	self = [super init];
	//	pool = [[NSAutoreleasePool alloc] init];
	Parameters = [ NSMutableArray arrayWithCapacity: 10 ]; // capacidade inicial, ela aumenta com a necessidade
	method = MethodName;
	NS = MethodNS;
	parent = daddy;
	return self; 
}

- (void) pushParameter: (NSString *) name value: (NSString *) value {
	NSArray *newParameter = [NSArray arrayWithObjects: name, value, nil];
	
	[ Parameters addObject:newParameter ];
}

#ifndef USE_LIBXML2
- (BOOL) registerCallback: (void(*)(void *,NSArray *)) callbackPtr customPtr: (void *)customPtr {
#else
- (BOOL) registerCallback: (void(*)(void *,element *)) callbackPtr customPtr: (void *)customPtr {	
#endif
	CustomPtr = customPtr;
	if(callback != callbackPtr) {
		callback = callbackPtr;
		return(TRUE);
	} else {
		return(FALSE);
	}
}

// rodar imediatamente o metodo
- (BOOL) run {
	BeerSoapService *Service = (BeerSoapService *) parent;
	int methodID = [ Service getMethodId: self];
//	printf("METHOD ID: %d\n", methodID);
	NSString *URIString = [ NSString stringWithFormat: @"bssm%d" , methodID ];
#ifndef USE_LIBXML2
	NSXMLNode *methodNode = [ NSXMLNode elementWithName: [ NSString stringWithFormat:@"%@:%@" , URIString, method ] URI: URIString ];
	
	NSXMLNode *methodNSNode = [ NSXMLNode namespaceWithName: URIString stringValue: NS ];
	[ (NSXMLElement *)methodNode addNamespace: methodNSNode];
	NSEnumerator *enumerator = [ Parameters reverseObjectEnumerator]; 
	
	for (NSArray *parameter in enumerator ) {
		NSXMLNode *parameterNode = [ NSXMLNode elementWithName: [ parameter objectAtIndex: 0 ] ];
		[ parameterNode setStringValue: [ parameter objectAtIndex: 1 ] ];
		[ (NSXMLElement *)methodNode addChild: parameterNode ];
	} 
	[ Service RegisterXMLMethod: methodNode ];
#else
	[Service methodRefresh];
	element *methodNode = [Service createMethod: [ NSString stringWithFormat:@"%@:%@", URIString, method ] ];
	NSString *attr = [ NSString stringWithFormat:@"%@:%@", @"xmlns", URIString ];
	methodNode->add_attribute([ attr UTF8String] , [NS UTF8String]);

	NSEnumerator *enumerator = [ Parameters reverseObjectEnumerator]; 
	
	for (NSArray *parameter in enumerator ) {
		element *node = methodNode->new_child( [ [ parameter objectAtIndex: 0 ] UTF8String]  );
		node->data([[ parameter objectAtIndex: 1] UTF8String] );
	}
	
#endif

	[ Service run ];
	
	return(TRUE);
}

// agendar para multiplas chamadas num mesmo POST
- (BOOL) scheduleRun {
	return(TRUE);
}

- (BOOL) setMethodResponse: (NSString *)response {
	methodResponse = response;
	return(TRUE);
}

- (NSString *)getMethodResponse {
	return(methodResponse);
}

- (BOOL) isResponseFor: (NSString *)response {
	if ( [response isEqualToString: methodResponse] ) {
		return(TRUE);
	} else {
		return(FALSE);
	}
}

#ifndef USE_LIBXML2
- (BOOL) callCallBack: (NSArray *) ServiceData {
#else
- (BOOL) callCallBack: (element *) ServiceData {
#endif
	(*callback)(CustomPtr, ServiceData);
	return(TRUE);
}

- (void) cleanParameters {
	[ Parameters removeAllObjects ];
}

@end
