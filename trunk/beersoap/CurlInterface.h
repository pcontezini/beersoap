/*
 *  CurlInterface.h
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string>
using namespace std;

struct MemoryStruct {
	char *memory;
	size_t size;
};

class CurlInterface {
private:
	CURL *curl;
	CURLcode res;
	struct curl_slist *headerList;
	struct MemoryStruct chunk;
	
public:
	CurlInterface();
	bool DoPost(string host, string postData);
	char *getData();
	void clearData();
};
