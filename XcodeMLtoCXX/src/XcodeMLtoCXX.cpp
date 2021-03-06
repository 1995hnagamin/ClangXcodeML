#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <sstream>
#include <cassert>
#include <memory>
#include <vector>
#include "llvm/ADT/Optional.h"
#include "Stream.h"
#include "StringTree.h"
#include "XMLString.h"
#include "XcodeMlNns.h"
#include "XcodeMlType.h"
#include "XcodeMlEnvironment.h"
#include "XMLWalker.h"
#include "TypeAnalyzer.h"
#include "SourceInfo.h"
#include "CodeBuilder.h"

int
main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <filename>" << std::endl;
    return 0;
  }
  std::string filename(argv[1]);
  xmlDocPtr doc = xmlParseFile(filename.c_str());
  xmlNodePtr root = xmlDocGetRootElement(doc);
  xmlXPathContextPtr ctxt = xmlXPathNewContext(doc);
  std::stringstream ss;
  buildCode(root, ctxt, ss);
  std::cout << ss.str() << std::endl;
  xmlXPathFreeContext(ctxt);
  xmlFreeDoc(doc);
  return 0;
}
