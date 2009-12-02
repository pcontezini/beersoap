//
//  BeerSoapServiceMethod.h
//  beersoap
//
//  Created by Piero Contezini on 11/09/08.
//  Copyright 2008 Informant. All rights reserved.
//
//#include "plataform.h"
#ifdef MACOSX_X86
#import <Cocoa/Cocoa.h>
#endif

#ifdef USE_LIBXML2
#import "xmldata.h"
#endif


// esta interface devine um metodo de um Soap Web Service e permite registrar uma callback para manipular a resposta
@interface BearSoapServiceMethod : NSObject {
	NSMutableArray *Parameters;
	NSString *method;
	NSString *NS;
	id parent;
#ifndef USE_LIBXML2	
	void(*callback)(void *,NSArray *); // callback, ainda nao sei como castear aqui..
#else
	void(*callback)(void *,element *); // callback, ainda nao sei como castear aqui..
#endif
	void *CustomPtr;
	NSString *methodResponse; // resposta que o método precisa .. 
}

- (id) initWithParameters: (id) daddy MethodName: (NSString *)MethodName MethodNS: (NSString *)MethodNS;
- (void) pushParameter: (NSString *) name value: (NSString *) value;
- (BOOL) run;
- (BOOL) scheduleRun;
- (BOOL) setMethodResponse: (NSString *)response;
- (NSString *)getMethodResponse;
- (BOOL) isResponseFor: (NSString *)response;
- (void) cleanParameters;

#ifndef USE_LIBXML2
- (BOOL) registerCallback: (void(*)(void *,NSArray *)) callbackPtr customPtr: (void *)customPtr;
- (BOOL) callCallBack: (NSArray *) ServiceData;
#else
- (BOOL) registerCallback: (void(*)(void *,element *)) callbackPtr customPtr: (void *)customPtr;
- (BOOL) callCallBack: (element *) ServiceData;

#endif
@end
