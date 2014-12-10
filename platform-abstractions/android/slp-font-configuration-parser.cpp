/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include "slp-font-configuration-parser.h"

#include <sstream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <dali/integration-api/debug.h>

namespace Dali
{

namespace SlpPlatform
{

namespace FontConfigurationParser
{

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SLP_FONT_CONFIGURATION_PARSER");
}
#endif

void Parse(std::string confFile, std::string& fontFamily, std::string& fontStyle)
{
  bool familyParsed(false);
  bool styleParsed(false);

  if ( !confFile.empty() )
  {
    xmlDocPtr doc = xmlParseFile(confFile.c_str());
    if(doc == NULL)
    {
      DALI_LOG_ERROR("Document %s not parsed successfully.\n", confFile.c_str());
      return;
    }

    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if(cur == NULL)
    {
      DALI_LOG_ERROR("%s is Empty\n", confFile.c_str());
      xmlFreeDoc(doc);
      return;
    }

    if(xmlStrcmp(cur->name, (const xmlChar *)"fontconfig"))
    {
      DALI_LOG_ERROR("Document %s is of the wrong type, root node != fontconfig\n", confFile.c_str());
      xmlFreeDoc(doc);
      return;
    }

    cur = cur->xmlChildrenNode;

    while(cur != NULL && !(familyParsed && styleParsed))
    {
      if((!xmlStrcmp(cur->name, (const xmlChar *)"match")))
      {
        xmlNodePtr cur2 = cur->xmlChildrenNode;
        while(cur2 != NULL && !(familyParsed && styleParsed))
        {
          if((!xmlStrcmp(cur2->name, (const xmlChar *)"edit")))
          {
            xmlChar* xmlValue = xmlGetProp( cur2, (const xmlChar *)"name" );
            if((!xmlStrcmp(xmlValue, (const xmlChar *)"family")))
            {
              xmlNodePtr cur3 = cur2->xmlChildrenNode;
              while(cur3 != NULL && !(familyParsed && styleParsed))
              {
                if((!xmlStrcmp(cur3->name, (const xmlChar *)"string")))
                {
                  xmlChar* key = xmlNodeListGetString(doc, cur3->xmlChildrenNode, 1);
                  DALI_LOG_INFO(gLogFilter,Debug::Concise, "Document %s uses the following font family as default: %s\n", confFile.c_str(), key);
                  std::stringstream str;
                  str << key;
                  fontFamily = str.str();
                  xmlFree(key);
                  familyParsed = true;
                  break;
                }
                cur3 = cur3->next;
              }
            }
            else if((!xmlStrcmp(xmlValue, (const xmlChar *)"style")))
            {
              xmlNodePtr cur3 = cur2->xmlChildrenNode;
              while(cur3 != NULL && !(familyParsed && styleParsed))
              {
                if((!xmlStrcmp(cur3->name, (const xmlChar *)"string")))
                {
                  xmlChar* key = xmlNodeListGetString(doc, cur3->xmlChildrenNode, 1);
                  DALI_LOG_INFO(gLogFilter,Debug::Concise, "Document %s uses the following font style as default: %s\n", confFile.c_str(), key);
                  std::stringstream str;
                  str << key;
                  fontStyle = str.str();
                  xmlFree(key);
                  styleParsed = true;
                  break;
                }
                cur3 = cur3->next;
              }
            }
          }
          cur2 = cur2->next;
        }
      }
      cur = cur->next;
    }

    xmlFreeDoc(doc);
  }
}

} // namespace FontConfParser

} // namespace SlpPlatform

} // namespace Dali
