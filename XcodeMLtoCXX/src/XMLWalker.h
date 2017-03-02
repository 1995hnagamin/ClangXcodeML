#ifndef XMLWALKER_H
#define XMLWALKER_H

/*!
 * \brief A class that combines procedures into a single one
 * that traverses XML node and visits each element.
 * \tparam ...T parameter type required by procedures in XMLWalker.
 *
 * It has a mapping from XML element names to procedures
 * (std::function<void(xmlNodePtr, const XMLWalker&, T...)>). Once
 * XMLWalker<T...>::walkAll() runs, it performs pre-order traversal of
 * given XML elements and their descendants until it finds an element
 * whose name is registered with the map. Finally it executes
 * a corresponding procedure.
 */
template<typename... T>
class XMLWalker {
public:
  /*!
   * \brief Procedure to be registered with XMLWalker.
   */
  using Procedure = std::function<void(const XMLWalker&, xmlNodePtr, T...)>;

  XMLWalker() = default;

  XMLWalker(std::initializer_list<std::tuple<std::string, Procedure>> pairs):
    map()
  {
    for (auto p : pairs) {
      registerProc(std::get<0>(p), std::get<1>(p));
    }
  }

  XMLWalker(std::map<std::string, Procedure>&& initMap):
    map(initMap)
  {}

  const Procedure& operator[](const std::string& key) const {
    map.at(key);
  }

  /*!
   * \brief Traverse a given XML element and subsequent elements.
   * \param node XML element to traverse first
   * \param args... Arguments to be passed to registered procedures.
   */
  void walkAll(xmlNodePtr node, T... args) const {
    if (!node) {
      return;
    }
    for (xmlNodePtr cur =
          node->type == XML_ELEMENT_NODE ?
            node : xmlNextElementSibling(node) ;
         cur;
         cur = xmlNextElementSibling(cur))
    {
      walk(cur, args...);
    }
  }

  void walkChildren(xmlNodePtr node, T... args) const {
    if (node) {
      walkAll(node->children, args...);
    }
  }

  /*!
   * \brief Traverse an XML element.
   * \param node XML element to traverse
   * \param args... Arguments to be passed to registered procedures.
   * \pre \c node is not null.
   * \pre \c node is an XML element node.
   */
  void walk(xmlNodePtr node, T... args) const {
    assert(node && node->type == XML_ELEMENT_NODE);
    bool traverseChildren = true;
    XMLString elemName = node->name;
    auto iter = map.find(elemName);
    if (iter != map.end()) {
      (iter->second)(*this, node, args...);
      traverseChildren = false;
    }
    if (traverseChildren) {
      walkAll(node->children, args...);
    }
  }

/*!
 * \brief Register a procedure. If \c key already exists,
 * do nothing.
 * \param key The name of XML element which \c proc should process.
 * \param value Procedure to run.
 * \return false if \c key already exists.
 */
  bool registerProc(std::string key, Procedure value) {
    auto iter = map.find(key);
    if (iter != map.end()) {
      return false;
    }
    map[key] = value;
    return true;
  }

private:
  std::map<std::string, Procedure> map;
};

#endif /* !XMLWALKER_H */
