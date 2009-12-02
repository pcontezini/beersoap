//
//  BeerHttp.m
//  beersoap
//
//  Created by Piero Contezini on 10/09/08.
//  Copyright 2008 Informant. All rights reserved.
//

#import "BeerHttp.h"

static const CFOptionFlags kNetworkEvents = kCFStreamEventOpenCompleted |
kCFStreamEventHasBytesAvailable |
kCFStreamEventEndEncountered |
kCFStreamEventErrorOccurred;


static void ReadStreamClientCallBack(CFReadStreamRef readStream, CFStreamEventType type, void *clientCallBackInfo) {
    [((BeerHttp*)clientCallBackInfo) handleNetworkEvent: type];
}


#define STATICBUFFERSIZE 2048

@implementation BeerHttp

- (id)init {
	self = [super init];
	pool = [[NSAutoreleasePool alloc] init];
	StaticBuffer = (unsigned char *)malloc(STATICBUFFERSIZE);
	StaticBufferSize = STATICBUFFERSIZE;
	return(self);
};

- (void)dealloc {
	free(StaticBuffer);
	[super dealloc];
	[pool release];
}

- (BOOL)requestGetHttpMessage: (NSString *) URL {
	static	CFURLRef			urlRef;

	urlRef= CFURLCreateWithString( kCFAllocatorDefault, (CFStringRef) URL, NULL );
	
	if(HttpMessage)  // se ja temos então precisamos matar isto primeiro pra dai poder alocar outro request
		CFRelease(HttpMessage);
	HttpMessage = CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("GET"), urlRef, kCFHTTPVersion1_1);
	if(HttpMessage == NULL) 
		goto Error;
		
	if(ReadStreamBuffer) { // aqui precisamos pedir pra fechar o socket também
		CFReadStreamClose(ReadStreamBuffer);
		CFRelease(ReadStreamBuffer);
	}
	
	ReadStreamBuffer = CFReadStreamCreateForHTTPRequest( kCFAllocatorDefault, HttpMessage );
	if ( ReadStreamBuffer == NULL ) 
		goto Error;
	

	CFStreamClientContext ctxt = {0, self, NULL, NULL, NULL};
	
    if (!CFReadStreamSetClient(ReadStreamBuffer, kNetworkEvents, ReadStreamClientCallBack, &ctxt)) {
        CFRelease(ReadStreamBuffer);
        ReadStreamBuffer = NULL;
        return(FALSE);
    }
    
    // Schedule the stream
    CFReadStreamScheduleWithRunLoop(ReadStreamBuffer, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	

	if ( CFReadStreamOpen( ReadStreamBuffer ) == FALSE ) // somente deus entende por que isto retorna FALSE! API fodida do caralho
	    goto Error;

	if ( IncomeData != NULL) { // se há algo do ultimo request mandamos ele pro espeto
		[IncomeData release];
		IncomeData = NULL;
	}
	
	complete = FALSE;
	
	return(TRUE);
	
Error:
	if ( urlRef != NULL )
		CFRelease(urlRef);
		return(FALSE);

}

- (BOOL)requestPostHttpMessage: (NSString *) URL PostData: (NSString *) PostData {
	static	CFURLRef			urlRef;
	CFDataRef					cfPostData;
	
	urlRef= CFURLCreateWithString( kCFAllocatorDefault, (CFStringRef)URL , NULL );
	
	if(HttpMessage)  // se ja temos então precisamos matar isto primeiro pra dai poder alocar outro request
		CFRelease(HttpMessage);
	HttpMessage = CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("POST"), urlRef, kCFHTTPVersion1_1);
	if(HttpMessage == NULL) 
		goto Error;
	
	cfPostData = CFDataCreate(kCFAllocatorDefault, (const unsigned char *)[PostData UTF8String], [PostData length]);
	
	CFHTTPMessageSetBody(HttpMessage,cfPostData);
	
	if(ReadStreamBuffer) { // aqui precisamos pedir pra fechar o socket também
		CFReadStreamClose(ReadStreamBuffer);
		CFRelease(ReadStreamBuffer);
	}
	
	ReadStreamBuffer = CFReadStreamCreateForHTTPRequest( kCFAllocatorDefault, HttpMessage );
	if ( ReadStreamBuffer == NULL ) 
		goto Error;
	
	
	CFStreamClientContext ctxt = {0, self, NULL, NULL, NULL};
	
    if (!CFReadStreamSetClient(ReadStreamBuffer, kNetworkEvents, ReadStreamClientCallBack, &ctxt)) {
        CFRelease(ReadStreamBuffer);
        ReadStreamBuffer = NULL;
        return(FALSE);
    }
    
    // Schedule the stream
    CFReadStreamScheduleWithRunLoop(ReadStreamBuffer, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
	
	if ( CFReadStreamOpen( ReadStreamBuffer ) == FALSE ) // somente deus entende por que isto retorna FALSE! API fodida do caralho
	    goto Error;
	
	if ( IncomeData != NULL) { // se há algo do ultimo request mandamos ele pro espeto
		[IncomeData release];
		IncomeData = NULL;
	}
	
	complete = FALSE;	
	return(TRUE);
	
Error:
	if ( urlRef != NULL )
		CFRelease(urlRef);
	if ( cfPostData != NULL )
		CFRelease(cfPostData);
	
	return(FALSE);
	
}





- (void)handleNetworkEvent:(CFStreamEventType)type
{
	switch (type) {
		case kCFStreamEventHasBytesAvailable:
//			printf("kCFStreamEventHasBytesAvailable\n");
		[self handleHttpData];
		break;
					
		case kCFStreamEventEndEncountered:
//			printf("kCFStreamEventEndEncountered\n");
		[self handleHttpDataComplete];
		break;
					
		case kCFStreamEventErrorOccurred:
//			printf("kCFStreamEventErrorOccurred\n");
		[self handleHttpDataError];
		break;
					
		default:
		break;
	}
}
		

- (void)handleHttpData {
	CFIndex ReadLen = CFReadStreamRead(ReadStreamBuffer, StaticBuffer, StaticBufferSize ); 
	if ( ReadLen < 0 ) {
		[self handleHttpDataError];
	} else if( ReadLen > 0 ) { 
		if ( IncomeData == NULL) // ele ta vazio vamos aloca-lo
			IncomeData = [ NSMutableData dataWithBytes:StaticBuffer length:ReadLen ];
		else
			[ IncomeData appendBytes:StaticBuffer length: ReadLen ];
	}
}
		

- (void)handleHttpDataComplete {
	complete = TRUE;
	const char pad_data[] = { '\0' };
	[ IncomeData appendBytes: pad_data length: 1];
		if(ReadStreamBuffer) {
			CFReadStreamClose(ReadStreamBuffer);
			CFReadStreamSetClient(ReadStreamBuffer, kCFStreamEventNone, NULL, NULL);
			CFReadStreamUnscheduleFromRunLoop(ReadStreamBuffer, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
			CFRelease(ReadStreamBuffer);
			ReadStreamBuffer = NULL;
			
		}
	CFRunLoopStop(CFRunLoopGetCurrent());
}

- (void)handleHttpDataError {
	complete = TRUE;
	NSError *err = [(NSError *)CFReadStreamCopyError(ReadStreamBuffer) autorelease];
	printf("ERRO: %s\n",[[err localizedDescription]UTF8String]);
	CFRunLoopStop(CFRunLoopGetCurrent());
}
			

- (unsigned char *) requestGetData {
	if(IncomeData)
		return( (unsigned char *) [IncomeData mutableBytes ] );
	else
		return(NULL);
}


- (int) requestGetDataLen {
	return( (int) [IncomeData length] );
}

- (BOOL) requestCompleted {
	return(complete);
}

- (void) run {
	CFRunLoopRun();
}

@end
