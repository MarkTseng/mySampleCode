#include <stdio.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

// "//tr/lang[@id=\"es\"]/str"
// "//tr/@id"


int main(int argc, char *argv[])
{
   const char *file_locale = "./locales.xml";
   const char *xpath = "//tr/@id"; // "//tr/lang[@id=\"es\"]/str"
   xmlDocPtr doc = xmlParseFile(file_locale);
   if (doc != NULL) 
   {
      xmlXPathContextPtr context = xmlXPathNewContext(doc);
      xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
      xmlNodeSetPtr nodeset;
      xmlChar *keyword;
      if(xmlXPathNodeSetIsEmpty(result->nodesetval)) printf("Error en expath\n");
      nodeset = result->nodesetval;
      keyword = xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
      printf("Hola => %s\n", keyword);
      xmlFree(keyword);
      xmlFreeDoc(doc);
      xmlXPathFreeContext(context);
      xmlXPathFreeObject(result);
      xmlCleanupParser();
   }
   else
   {
      printf("Error al abrir => %s\n", file_locale);
   }
   return 0;
}
