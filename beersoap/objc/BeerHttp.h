//
//  BeerHttp.h
//  beersoap
//
//  Created by Piero Contezini on 10/09/08.
//  Copyright 2008 Informant. All rights reserved.
//

//#include "plataform.h"

#import <CoreFoundation/CoreFoundation.h>

#ifdef IPHONE_HW
#import "CFNetwork/CFNetwork.h"
#endif

#ifdef MACOSX_X86
#import <Cocoa/Cocoa.h>
#endif

@interface BeerHttp : NSObject {
	
	CFHTTPMessageRef HttpMessage;
	CFReadStreamRef	ReadStreamBuffer;
	
	unsigned char *StaticBuffer;
	int StaticBufferSize;
	NSMutableData *IncomeData;
	
	BOOL complete;
	
	NSAutoreleasePool *pool;
	
}

- (id) init;
- (void) dealloc;
- (BOOL) requestPostHttpMessage: (NSString *) URL PostData: (NSString *) PostData;
- (BOOL) requestGetHttpMessage: (NSString *) URL;
- (void) handleNetworkEvent:(CFStreamEventType)type;
- (void) handleHttpData;
- (void) handleHttpDataComplete;
- (void) handleHttpDataError;
- (unsigned char *) requestGetData; // não aloca memoria dinamica
//- (unsigned char *) requestGetStringData; // aloca memoria dinamica e coloca um \0 no fim, pra facilitar..
- (int) requestGetDataLen;
- (BOOL) requestCompleted;
- (void) run;

@end
