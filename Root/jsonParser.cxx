#include "METTriggerTools/jsonParser.h"
#include <sstream>
#include <tuple>

void JSONNode::recursivePrint(unsigned int depth) const {
  std::cout << std::string(depth, ' ') << this->name << ": ";
  if (this->subNodes.size() != 0) {
    std::cout << std::endl;
    for (const auto& pair : this->subNodes) pair.second.recursivePrint(depth + 2); }
  else if (this->vectorSubNodes.size() != 0) {
    std::cout << "[" << std::endl;
    for (const auto& node : this->vectorSubNodes) node.recursivePrint(depth + 2);
    std::cout << std::string(depth, ' ')  << "]" << std::endl;
  }
  //else if (this->vectorElements.size() != 0) {
    //std::cout << "[";
    //for (unsigned int i = 0; i < this->vectorElements.size() - 1; ++i) std::cout << this->vectorElements.at(i) << ", ";
    //std::cout << this->vectorElements.back() << ']' << std::endl;
  //}
  else {
    std::cout << this->element << std::endl;
  }
}

#if 0
bool JSONNode::get(const std::string& subNode, bool& element, bool erase){
  if (subNodes.count(subNode) ) {
    std::stringstream ss(subNodes.at(subNode).element);
    bool temp;
    std::cout << ss.str() << std::endl;
    ss >> std::boolalpha >> temp;
    std::cout << temp << std::endl;
    if (ss.eof() ) {
      std::cout << "Success!" << std::endl;
      element = temp;
      if (erase) subNodes.erase(subNode);
      return true;
    }
  }
  return false;
}
#endif

bool JSONNode::get(const std::string& subNode, JSONNode& node, bool erase){
  if (subNodes.count(subNode) ) {
    JSONNode& theNode(subNodes.at(subNode) );
    if (erase) { 
      //std::swap(node, subNodes.at(subNode) );
      std::swap(node, theNode);
      subNodes.erase(subNode);
    }
    //else node = subNodes.at(subNode);
    else node = theNode;
    return true;
  }
  return false;
}


JSONParser::JSONParser(std::string inText, bool verbose)
  : verbose(verbose),
    m_rootNode("root")
{
  if (inText.empty() ) {
    std::cout << "Warning: empty string provided!" << std::endl;
    return;
  }
  recurse(m_rootNode, inText);
}

JSONParser::JSONParser(std::ifstream& fin, bool verbose)
  : verbose(verbose),
    m_rootNode("root")
{
  if (fin.is_open() ) {
    std::string line;
    std::string inText;
    while (getline(fin, line) ) {
      size_t firstChar = line.find_first_not_of(" \t\r\n");
      if (firstChar == std::string::npos) continue;
      if (line.at(firstChar) == '#') continue;
      inText.append(line);
    }
    bool inQuotes = false;
    for (auto itr = inText.begin(); itr != inText.end(); ++itr) {
      if (inQuotes) {
        if (*itr == '"') inQuotes = false;
        continue;
      }
      if (*itr == '"') inQuotes = true;
      if (*itr == ' ') itr = inText.erase(itr) - 1;
    }
    if (inText.empty() ) {
      std::cout << "Warning: empty string provided!" << std::endl;
      return;
    }
    //recurse(m_rootNode, itr, inText);
    recurse(m_rootNode, inText);

    //std::cout << inText << std::endl;
  } else {
    std::cout << "ERROR: file not opened" << std::endl;
    throw;
  }
}


std::string getName(std::string& text)
{
  if (text.front() != '\"') {
    std::cerr << "Element names must be enclosed in double quotes" << std::endl;
    std::cerr << "Parsing name " << text << std::endl;
    throw;
  }
  text.erase(0,1);
  size_t pos = text.find('\"');
  if (pos == std::string::npos) {
    std::cerr << "Element names must be enclosed in double quotes" << std::endl;
    std::cerr << "Parsing name " << text << std::endl;
    throw;
  }
  std::string ret =  text.substr(0, pos);
  text.erase(0, pos + 1);
  return ret;
}


std::vector<std::string> splitStringOn(const std::string& text, char dlm) {
  std::stringstream ss(text);
  std::string entry;
  std::vector<std::string> ret;
  bool inQuotes = false;
  //std::vector<std::tuple<char, char, int>> bracketTuples({{'{', '}', 0}, {'[', ']', 0}, {'(', ')', 0}});
  std::vector<std::tuple<char, char, int>> bracketTuples;
  bracketTuples.push_back(std::make_tuple('{', '}', 0) );
  bracketTuples.push_back(std::make_tuple('[', ']', 0) );
  bracketTuples.push_back(std::make_tuple('(', ')', 0) );
  char c;
  while(ss.get(c) ){
    if (c == '\"') inQuotes = !inQuotes;
    bool skipDlm = inQuotes;
    if (!inQuotes) for (auto& tup : bracketTuples) {
      if (c == std::get<0>(tup) ) ++std::get<2>(tup);
      else if (c == std::get<1>(tup) ) --std::get<2>(tup);
      skipDlm = skipDlm || (std::get<2>(tup) != 0); // Check if we're inside some brackets (where the brackets aren't quoted)
    }
    if (!skipDlm && c == dlm) {
      ret.push_back(entry);
      entry.clear();
      continue;
    }
    entry += c;
    
  }
  ret.push_back(entry);
  return ret;
}

void JSONParser::recurse(JSONNode& node, std::string inText) {
  if (verbose) std::cout << "Parsing text: " << std::endl << inText << std::endl;
  if (inText.front() == '{') { // Parsing list of subnodes
    if (inText.back() != '}') {
      std::cerr << "Malformed JSON! Unclosed '{'" << std::endl;
      throw;
    }
    inText = inText.substr(1, inText.size() - 2);
    std::vector<std::string> elements = splitStringOn(inText, ',');
    for (std::string ele : elements) {
      std::string name = getName(ele);
      node.subNodes.emplace(name, name);
      if (ele.empty() ) node.subNodes.at(name).element = name;
      if (ele.front() != ':') {
        std::cerr << "Malformed JSON provided! Names and values must be separated by colons" << std::endl;
        throw;
      }
      ele.erase(0,1);
      recurse(node.subNodes.at(name), ele);
    }
  }
  else if (inText.front() == '[') { // Parsing list of elements
    if (inText.back() != ']') {
      std::cerr << "Malformed JSON! Unclosed '['" << std::endl;
      throw;
    }
    inText = inText.substr(1, inText.size() - 2);
    std::vector<std::string> elements = splitStringOn(inText, ',');
    for (std::string ele : elements) {
      node.vectorSubNodes.emplace_back(node.name+"_sub");
      recurse(node.vectorSubNodes.back(), ele);
    }
  }
  else {
    if (inText.front() == '"') {
      if (inText.back() != '"') {
        std::cerr << "Malformed JSON! Unmatched double quote" << std::endl;
      }
      inText = inText.substr(1, inText.size() - 2);
    }
    node.element = inText;
  }
}

