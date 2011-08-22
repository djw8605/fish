/*
    Copyright (C) 2007  Derek Weitzel
    This file is part of Fish.

    Fish is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Fish is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

/*
Author: Derek Weitzel
*/



#include "aquarium.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
//#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;
XERCES_CPP_NAMESPACE_USE


extern int numClusters;
extern ClusterInfo* clusters;
extern Fish* fish;
extern int FISHCOUNT;
extern Stats stats;
extern int statFields;



//yet to implement, should test if file gangout exists, and whether to come up with another name for the file
void secure_tmpFile() {

//	fstream tmpFile("/tmp/gangout");
//	tmpFile.



}

float* colorTranslate(char* color) {

	float* toReturn = 0;
	if(!color) {
		return 0;
	}
	if(!strcmp(color, "red"))
	{
		toReturn = new float[16];
		//ambient
		toReturn[0] = 1.0; toReturn[1] = 0.0; toReturn[2] = 0.0; toReturn[3] = 1.0;
		//diffuse
		toReturn[4] = 1.0; toReturn[5] = 0.0; toReturn[6] = 0.0; toReturn[7] = 1.0;
		//specular
		toReturn[8] = 0.0; toReturn[9] = 0.0; toReturn[10] = 0.0; toReturn[11] = 1.0;
		//emissive, maybe don't need
		toReturn[12] = 0.0; toReturn[13] = 0.0; toReturn[14] = 0.0; toReturn[15] = 1.0;
	} else if(!strcmp(color, "blue"))
	{
		toReturn = new float[16];
		//ambient
		toReturn[0] = 0.0; toReturn[1] = 0.0; toReturn[2] = 1.0; toReturn[3] = 1.0;
		//diffuse
		toReturn[4] = 0.0; toReturn[5] = 0.0; toReturn[6] = 1.0; toReturn[7] = 1.0;
		//specular
		toReturn[8] = 0.0; toReturn[9] = 0.0; toReturn[10] = 0.0; toReturn[11] = 1.0;
		//emissive, maybe don't need
		toReturn[12] = 0.0; toReturn[13] = 0.0; toReturn[14] = 0.0; toReturn[15] = 1.0;


	} else if(!strcmp(color, "green"))
	{
		toReturn = new float[16];
		//ambient
		toReturn[0] = 0.0; toReturn[1] = 1.0; toReturn[2] = 0.0; toReturn[3] = 1.0;
		//diffuse
		toReturn[4] = 0.0; toReturn[5] = 1.0; toReturn[6] = 0.0; toReturn[7] = 1.0;
		//specular
		toReturn[8] = 0.0; toReturn[9] = 0.0; toReturn[10] = 0.0; toReturn[11] = 1.0;
		//emissive, maybe don't need
		toReturn[12] = 0.0; toReturn[13] = 0.0; toReturn[14] = 0.0; toReturn[15] = 1.0;
	}

	return toReturn;
}



void parse_config() {

	XercesDOMParser* parser;
	XMLPlatformUtils::Initialize();
	parser = new XercesDOMParser;
	parser->setValidationScheme(XercesDOMParser::Val_Auto);
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setValidationSchemaFullChecking(false);
    parser->setCreateEntityReferenceNodes(false);
    parser->setIncludeIgnorableWhitespace(false);


	FILE* configFile = fopen("config.xml", "r");
	if(configFile==0) {
		printf("ERROR, config.xml was not found\n");
		exit(1);
	}
	fclose(configFile);
	parser->parse("./config.xml");
	DOMDocument* doc = parser->getDocument();

	XMLCh* cluster = XMLString::transcode("CLUSTER");
	DOMNodeList* list = doc->getElementsByTagName(cluster);
	delete cluster;
	numClusters = list->getLength();
	printf("number of clusters = %i\n", numClusters);
	clusters = new ClusterInfo[numClusters];
	int i = 0, lowerCounter = 0, a = 0;
	//get cluster information

	for(i = 0; i < list->getLength(); i++) {

		DOMNamedNodeMap *attr = list->item(i)->getAttributes();
		if(!attr)
		    continue;
		DOMNode* clusterName = attr->getNamedItem(XMLString::transcode("NAME"));
		DOMNode* clusterIP = attr->getNamedItem(XMLString::transcode("IP"));
		DOMNode* clusterColor = attr->getNamedItem(XMLString::transcode("COLOR"));
		DOMNode* clusterWebsite = attr->getNamedItem(XMLString::transcode("WEBSITE"));
		DOMNode* clusterCommand = attr->getNamedItem(XMLString::transcode("COMMAND"));
		if(clusterName)
			clusters[i].name = XMLString::transcode(clusterName->getNodeValue());
		else
			clusters[i].name = 0;
		if(clusterIP)
		{
			// Seperate the port number
			char* tmpip = XMLString::transcode(clusterIP->getNodeValue());
			char* str_port = strchr(tmpip, ':');
			if(str_port)
			{
				int ip_length = str_port - tmpip;
				clusters[i].ip = new char[ip_length + 1];
				memcpy(clusters[i].ip, tmpip, ip_length);
				clusters[i].ip[ip_length+1] = '\0';

				sscanf(str_port+1, "%i", &clusters[i].port);

			} else {
				clusters[i].ip = XMLString::transcode(clusterIP->getNodeValue());
				clusters[i].port = 0;
			}
			printf("IP = %s; port = %i\n", clusters[i].ip, clusters[i].port);

		} else {
			clusters[i].ip = 0;
			clusters[i].port = 0;
		}
		if(clusterColor)
			clusters[i].color = colorTranslate(XMLString::transcode(clusterColor->getNodeValue()));
		else
			clusters[i].color = 0;
		if(clusterWebsite)
			clusters[i].website = XMLString::transcode(clusterWebsite->getNodeValue());
		else
			clusters[i].website = 0;
		if(clusterCommand)
                        clusters[i].command = XMLString::transcode(clusterCommand->getNodeValue());
		else
                        clusters[i].command = 0;
		//DOMNodeList* childList = list->item(i)->getChildNodes();
		//clusters[i].numNodes = childList->getLength();
		//printf("cluster %s has %i nodes\n", clusters[i].name, clusters[i].numNodes);
		lowerCounter += clusters[i].numNodes;

	}

	//get the info that we want to store with the fish
	XMLCh* stat = XMLString::transcode("STATS");
	DOMNamedNodeMap* attr;		//for use as temp storage of information
	DOMNode* tempNode;
	DOMNodeList* statsList = doc->getElementsByTagName(stat);
	if(statsList && statsList->getLength()) {
	delete stat;
	DOMNodeList* metaList = statsList->item(0)->getChildNodes();
        //first, count the number of actual element nodes and not text nodes
        statFields = 0;
	for(i = 0; i < metaList->getLength(); i++) {
                if(metaList->item(i)->getNodeType() == DOMNode::ELEMENT_NODE)
                        statFields++;
        }

//	statFields = metaList->getLength();
	//allocate the memory for the structure
	stats.names = new char*[statFields];
	stats.print = new char*[statFields];
	stats.order = new char*[statFields];
	printf("%i stats found\n", statFields);

	int skipped = 0;
	for(i = 0; i < metaList->getLength(); i++) {
		if((metaList->item(i)->getNodeType() == DOMNode::TEXT_NODE)) {
			skipped++;
			continue;
		}

		attr = metaList->item(i)->getAttributes();
		if(!attr) {
		    skipped++;
		    continue;
		}

		tempNode = attr->getNamedItem(XMLString::transcode("NAME"));

		if(tempNode)
		{
			stats.names[i-skipped] = XMLString::transcode(tempNode->getNodeValue());
		}
		else
		{
			stats.names[i-skipped] = 0;
		}

		tempNode = attr->getNamedItem(XMLString::transcode("PRINT"));
		if(tempNode)
			stats.print[i-skipped] = XMLString::transcode(tempNode->getNodeValue());
		else
			stats.print[i-skipped] = 0;

		tempNode = attr->getNamedItem(XMLString::transcode("ORDER"));
		if(tempNode)
			stats.order[i-skipped] = XMLString::transcode(tempNode->getNodeValue());
		else
			stats.order[i-skipped] = 0;



	}
	}

	//count the total number of fish described in the config xml file
	lowerCounter = 0;
	for(i = 0; i < list->getLength(); i++) {
		DOMNodeList* childList = list->item(i)->getChildNodes();
		for(a = 0; a < childList->getLength(); a++) {
			if(childList->item(a)->getNodeType() == DOMNode::ELEMENT_NODE) {
				lowerCounter++;
			}
		}
	}

	fish = new Fish[lowerCounter];

//	printf("sizeof fish = %i", sizeof(Fish));
	FISHCOUNT = lowerCounter;
	printf("number of fish = %i\n", lowerCounter);
	lowerCounter = 0;

	//loop through all of the clusters
	for(i = 0; i < list->getLength(); i++) {
		DOMNodeList* childList = list->item(i)->getChildNodes();
		clusters[i].lowerFish = lowerCounter;
		//loop through all of the fish
		for(a = 0; a < childList->getLength(); a++) {
				if((childList->item(a)->getNodeType() == DOMNode::TEXT_NODE)) { // || (childList->item(a)->getNodeType() != DOMNode::ELEMENT_NODE)) {
					continue;
				}
				attr = childList->item(a)->getAttributes();
				//if(!attr)
				 //   continue;
				tempNode = attr->getNamedItem(XMLString::transcode("NAME"));
				//fish[lowerCounter].fishname = XMLString::transcode(tempNode->getNodeValue());
				fish[lowerCounter].fishname = new char[strlen(XMLString::transcode(tempNode->getNodeValue()))];
				strcpy(fish[lowerCounter].fishname, XMLString::transcode(tempNode->getNodeValue()));
				fish[lowerCounter].fishcluster = clusters[i].name;
				fish[lowerCounter].clusterPtr = clusters+i;
				tempNode = attr->getNamedItem(XMLString::transcode("PRINT"));
				fish[lowerCounter].print = new char[strlen(XMLString::transcode(tempNode->getNodeValue()))];
				strcpy(fish[lowerCounter].print, XMLString::transcode(tempNode->getNodeValue()));
				tempNode = attr->getNamedItem(XMLString::transcode("TARGET"));
				if(tempNode)
					fish[lowerCounter].targetLoad = atof(XMLString::transcode(tempNode->getNodeValue()));
//				for(int c = 0; c < 3; c++) {
//					if(fish[lowerCounter].print[c] > 57 || fish[lowerCounter].print[c] < 48)
//						printf("%s fish has invalid print value %c", fish[lowerCounter].fishname, fish[lowerCounter].print[c]);

//				}

				//depreciated
				fish[lowerCounter].picking[0] = lowerCounter%255;
				fish[lowerCounter].picking[1] = lowerCounter/255;
				//doubt i'd ever use more than this
				fish[lowerCounter].picking[2] = 0;

				fish[lowerCounter].nodeinfo.init(statFields);

				lowerCounter++;



		}





	}

	delete parser;
	XMLPlatformUtils::Terminate();

}
