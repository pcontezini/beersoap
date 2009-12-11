/*
 *  CurlInterface.cpp
 *  beersoap
 *
 *  Created by Piero Contezini on 04/12/08.
 *  Copyright 2008 Informant. All rights reserved.
 *
 */

#include <CurlInterface.h>

static void *curlRealloc(void *ptr, size_t size) {
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}

static size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;
	 
	mem->memory = (char *)curlRealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}


CurlInterface::CurlInterface() {
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	chunk.memory = NULL;
	chunk.size = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	
	headerList = NULL;
	headerList = curl_slist_append(headerList, "Content-type: text/xml");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);	
}

CurlInterface::~CurlInterface() {
	if(chunk.memory) {
		free(chunk.memory);
	}
	curl_easy_cleanup(curl);
	curl_slist_free_all(headerList);
}

bool CurlInterface::DoPost(std::string host, std::string postData) {
	curl_easy_setopt(curl, CURLOPT_URL, (unsigned char *) host.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (unsigned char *) postData.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());	
	res = curl_easy_perform(curl);
	if(res == 0) {
		return(true);
	} else {
		return(false);
	}
}

bool CurlInterface::doGet(std::string url) {
//	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_URL, (unsigned char *) url.c_str());
	res = curl_easy_perform(curl);
	if(res == 0) {
		return(true);
	} else {
		return(false);
	}
}	

char *CurlInterface::getData() {
	return(chunk.memory);
}

void CurlInterface::clearData() {
	free(chunk.memory);
	chunk.memory = NULL;
	chunk.size = 0;
}
