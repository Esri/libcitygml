#include "parser/textureelementparser.h"

#include "parser/textureassociationelementparser.h"

#include <unordered_map>

#include "parser/nodetypes.h"
#include "parser/attributes.h"
#include "parser/documentlocation.h"
#include "parser/parserutils.hpp"
#include "parser/skipelementparser.h"

#include <citygml/cityobject.h>
#include <citygml/citygmlfactory.h>
#include <citygml/citygmllogger.h>
#include <citygml/texture.h>
#include <citygml/texturetargetdefinition.h>
#include <citygml/texturecoordinates.h>

#include <stdexcept>

namespace citygml {

    TextureElementParser::TextureElementParser(CityGMLDocumentParser& documentParser, CityGMLFactory& factory, std::shared_ptr<CityGMLLogger> logger, std::function<void (std::shared_ptr<Texture>)> callback)
        : GMLObjectElementParser(documentParser, factory, logger)
    {
        m_callback = callback;
        m_model = nullptr;
        m_currentTexCoords = nullptr;
    }

    std::string TextureElementParser::elementParserName() const
    {
        return "TextureElementParser";
    }

    bool TextureElementParser::handlesElement(const NodeType::XMLNode& node) const
    {
        return node == NodeType::APP_ParameterizedTextureNode;
    }

    bool TextureElementParser::parseElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        if (node != NodeType::APP_ParameterizedTextureNode) {
            CITYGML_LOG_ERROR(m_logger, "Expected start tag <" << NodeType::APP_ParameterizedTextureNode.name() << "> got " << node << " at " << getDocumentLocation());
            throw std::runtime_error("Unexpected start tag found.");
        }

        m_model = m_factory.createTexture(attributes.getCityGMLIDAttribute());
        return true;
    }

    bool TextureElementParser::parseElementEndTag(const NodeType::XMLNode&, const std::string&)
    {
        m_callback(m_model);
        return true;
    }

    bool TextureElementParser::parseChildElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        if (m_model == nullptr) {
            throw std::runtime_error("TextureElementParser::parseChildElementStartTag called before TextureElementParser::parseElementStartTag");
        }

        if (node == NodeType::APP_imageURINode
            || node == NodeType::APP_textureTypeNode
            || node == NodeType::APP_wrapModeNode
            || node == NodeType::APP_borderColorNode
            || node == NodeType::APP_TexCoordListNode
            || node == NodeType::APP_isFrontNode
            || node == NodeType::APP_mimeTypeNode) {
            return true;
        } else if (node == NodeType::APP_targetNode) {
            if (m_currentTexTargetDef != nullptr) {
                CITYGML_LOG_WARN(m_logger, "Nested texture target definition detected at: " << getDocumentLocation());
            } else {
                m_currentTexTargetDef = m_factory.createTextureTargetDefinition(parseReference(attributes.getAttribute("uri"), m_logger, getDocumentLocation()), m_model, attributes.getCityGMLIDAttribute());
            }
            return true;
        } else if (node == NodeType::APP_textureCoordinatesNode) {
            if (m_currentTexTargetDef == nullptr) {
                CITYGML_LOG_WARN(m_logger, "Found texture coordinates node (" << NodeType::APP_textureCoordinatesNode << ") outside Texture target node at: " << getDocumentLocation());
            } else if (m_currentTexCoords != nullptr) {
                CITYGML_LOG_WARN(m_logger, "Nested texture coordinates definition detected at: " << getDocumentLocation());
            } else {
                m_currentTexCoords = std::make_shared<TextureCoordinates>(attributes.getCityGMLIDAttribute(), parseReference(attributes.getAttribute("ring"), m_logger, getDocumentLocation()));
            }
            return true;
        } else if (node == NodeType::APP_textureParameterizationNode) {
            setParserForNextElement(new TextureAssociationElementParser(m_documentParser, m_factory, m_logger, m_model));
            return true;
        }

        return GMLObjectElementParser::parseChildElementStartTag(node, attributes);
    }

    bool TextureElementParser::parseChildElementEndTag(const NodeType::XMLNode& node, const std::string& characters)
    {
        if (m_model == nullptr) {
            throw std::runtime_error("TextureElementParser::parseChildElementEndTag called before TextureElementParser::parseElementStartTag");
        }

        if (node == NodeType::APP_imageURINode) {

            m_model->setUrl(characters);
        } else if (node == NodeType::APP_textureTypeNode) {

            m_model->setAttribute(node.name(), characters);
        } else if (node == NodeType::APP_textureParameterizationNode) {
            // Do nothing (target and texture coords are set in child element)
        } else if (node == NodeType::APP_wrapModeNode) {

            if (!m_model->setWrapModeFromString(characters)) {
                CITYGML_LOG_WARN(m_logger, "Unknown texture wrap mode " << characters << " at: " << getDocumentLocation());
            }
        } else if (node == NodeType::APP_isFrontNode) {

            m_model->setIsFront(parseValue<bool>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_borderColorNode) {

            std::vector<float> colorValues = parseVecList<float>(characters, m_logger, getDocumentLocation());
            colorValues.push_back(1.f); // if 3 values are given, the fourth (alpha) is set to 1.0 by default
            if (colorValues.size() >= 4) {
                m_model->setBorderColor(TVec4f(colorValues[0], colorValues[1], colorValues[2], colorValues[3]));
            } else {
                CITYGML_LOG_WARN(m_logger, "Expected 3 or more float values in node " << NodeType::APP_borderColorNode << " but got " << colorValues.size() << " at: " << getDocumentLocation());
            }
        } else if (node == NodeType::APP_TexCoordListNode) {

            if (m_currentTexCoords != nullptr) {
                CITYGML_LOG_WARN(m_logger, "TexCoordList node finished before TextureCoordinates child is finished at " << getDocumentLocation());
                m_currentTexCoords = nullptr;
            }

        } else if (node == NodeType::APP_textureCoordinatesNode) {

            if (m_currentTexCoords != nullptr && m_currentTexTargetDef != nullptr) {
                m_currentTexCoords->setCoords(parseVecList<TVec2f>(characters, m_logger, getDocumentLocation()));
                m_currentTexTargetDef->addTexCoordinates(m_currentTexCoords);
                m_currentTexCoords = nullptr;
            } else {
                CITYGML_LOG_WARN(m_logger, "Unexpected end tag <" << NodeType::APP_textureCoordinatesNode << " at: " << getDocumentLocation());
            }
        } else if (node == NodeType::APP_targetNode) {

            m_currentTexTargetDef = nullptr;
        } else if (node == NodeType::APP_mimeTypeNode) {
            m_model->setAttribute(node.name(), characters);
        } else {
            return GMLObjectElementParser::parseChildElementEndTag(node, characters);
        }
        return true;
    }

    Object* TextureElementParser::getObject()
    {
        return m_model.get();
    }




}
