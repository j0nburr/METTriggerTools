#ifndef METTRIGGERTOOLS_JSONPARSER_H
#define METTRIGGERTOOLS_JSONPARSER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

class JSONNode
{
public:
  JSONNode(const std::string& name = "blank") : name(name) {}
  JSONNode(const JSONNode& node) : name(node.name), subNodes(node.subNodes), vectorSubNodes(node.vectorSubNodes), /*vectorElements(node.vectorElements), */element(node.element) {}
  std::string name;
  std::map<std::string, JSONNode> subNodes;
  std::vector<JSONNode> vectorSubNodes;
  //std::vector<std::string> vectorElements;
  std::string element;
  void recursivePrint(unsigned int depth = 0) const;
  template <typename T>
  bool get(const std::string& subNode, T& element, bool erase = true);
  template <typename T>
  bool get(const std::string& subNode, std::vector<T>& element, bool erase = true);
  bool get(const std::string& subNode, JSONNode& node, bool erase = true);
  template <typename T>
  T getDefault(const std::string& subNode, const T& dflt, bool erase = true);

};


template <typename T>
bool JSONNode::get(const std::string& subNode, T& element, bool erase){
  if (subNodes.count(subNode) ) {
    std::istringstream iss(subNodes.at(subNode).element.c_str());
    T temp;
    iss >> std::boolalpha >> temp;
    T eofTest;
    iss >> eofTest;
    if (iss.eof() ) {
    //if (iss.tellg() == iss.str().size() ) {
      element = temp;
      if (erase) subNodes.erase(subNode);
      return true;
    }
    std::string remaining;
    iss >> remaining;
    std::cout << (iss.eof() ? "EOF, " : "No EOF, ") << (iss.good() ? "good, " : "not good, ") << (iss.fail() ? "fail, " : "not fail, ") << (iss.bad() ? "bad" : "not bad") << std::endl;
    std::cout << remaining << ": " << remaining.size() << std::endl;
    std::cout << subNodes.at(subNode).element << std::endl;
    std::cout << subNodes.at(subNode).element.size() << std::endl;
    std::cout << temp << std::endl;
  }
  return false;
}

template <typename T>
bool JSONNode::get(const std::string& subNode, std::vector<T>& element, bool erase) {
  if (subNodes.count(subNode) ) {
    std::vector<T> tempVec;
    for (const auto& node : subNodes.at(subNode).vectorSubNodes) {
      std::istringstream iss(node.element);
      T temp;
      iss >> std::boolalpha >> temp;
      T eofTest;
      iss >>  eofTest;
      if (iss.eof() ) {
      //if (iss.tellg() == iss.str().size() ) {
        tempVec.push_back(temp);
      }
      else return false;
    }
    std::swap(tempVec, element);
    if (erase) subNodes.erase(subNode);
    return true;
  }
  return false;
}

    

template <typename T>
T JSONNode::getDefault(const std::string& subNode, const T& dflt, bool erase){
  T temp;
  if (get(subNode, temp, erase) ) return temp;
  else return dflt;
}


class JSONParser
{
public:
  JSONParser(std::string inText, bool verbose = false);
  JSONParser(std::ifstream& fin, bool verbose = false);
  
  JSONNode& rootNode() { return m_rootNode; }

private:
  bool verbose;
  bool m_inQuotes;
  bool m_inName;
  JSONNode m_rootNode;

  void recurse(JSONNode& node, std::string inText);
};

#endif
