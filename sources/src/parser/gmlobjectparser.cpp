#include "parser/gmlobjectparser.h"

#include <citygml/object.h>

namespace citygml {

    GMLObjectElementParser::GMLObjectElementParser(CityGMLDocumentParser& documentParser, CityGMLFactory& factory, std::shared_ptr<CityGMLLogger> logger)
        : CityGMLElementParser(documentParser, factory, logger)
    {

    }

    bool GMLObjectElementParser::parseChildElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        if (getObject() == nullptr) {
            throw std::runtime_error("Invalid call to GMLObjectElementParser::parseChildElementStartTag");
        }

        if (   node == NodeType::GML_descriptionNode
            || node == NodeType::GML_identifierNode
            || node == NodeType::GML_nameNode
            || node == NodeType::GML_descriptionReferenceNode
            || node == NodeType::GML_metaDataPropertyNode) {

            return true;
        }

        return false;
    }

    bool GMLObjectElementParser::parseChildElementEndTag(const NodeType::XMLNode& node, const std::string& characters)
    {
        if (getObject() == nullptr) {
            throw std::runtime_error("Invalid call to GMLObjectElementParser::parseChildElementEndTag");
        }

        if (   node == NodeType::GML_descriptionNode
            || node == NodeType::GML_identifierNode
            || node == NodeType::GML_nameNode
            || node == NodeType::GML_descriptionReferenceNode
            || node == NodeType::GML_metaDataPropertyNode) {

                getObject()->setAttribute(node.name(), characters);
                return true;
        }

        return false;
    }

}
