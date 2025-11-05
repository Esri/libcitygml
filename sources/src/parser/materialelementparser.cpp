#include "parser/materialelementparser.h"

#include <unordered_map>

#include "parser/nodetypes.h"
#include "parser/attributes.h"
#include "parser/documentlocation.h"
#include "parser/parserutils.hpp"
#include "parser/skipelementparser.h"

#include <citygml/material.h>
#include <citygml/citygmlfactory.h>
#include <citygml/citygmllogger.h>

#include <stdexcept>

namespace citygml {

    MaterialElementParser::MaterialElementParser(CityGMLDocumentParser& documentParser, CityGMLFactory& factory, std::shared_ptr<CityGMLLogger> logger, std::function<void(std::shared_ptr<Material>)> callback)
        : GMLObjectElementParser(documentParser, factory, logger)
    {
        m_model = nullptr;
        m_callback = callback;
    }

    std::string MaterialElementParser::elementParserName() const
    {
        return "MaterialElementParser";
    }

    bool MaterialElementParser::handlesElement(const NodeType::XMLNode& node) const
    {
        return node == NodeType::APP_MaterialNode || node == NodeType::APP_X3DMaterialNode;
    }

    bool MaterialElementParser::parseElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        if (!handlesElement(node)) {
            CITYGML_LOG_ERROR(m_logger, "Expected start tag <" << NodeType::APP_MaterialNode.name() << "> got " << node << " at " << getDocumentLocation());
            throw std::runtime_error("Unexpected start tag found.");
        }

        m_model = std::shared_ptr<Material>(m_factory.createMaterial(attributes.getCityGMLIDAttribute()));

        return true;
    }

    bool MaterialElementParser::parseElementEndTag(const NodeType::XMLNode&, const std::string&)
    {
        m_callback(m_model);
        return true;
    }

    bool MaterialElementParser::parseChildElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        if (m_model == nullptr) {
            throw std::runtime_error("MaterialElementParser::parseChildElementStartTag called before MaterialElementParser::parseElementStartTag");
        }

        if (node == NodeType::APP_diffuseColorNode
            || node == NodeType::APP_emissiveColorNode
            || node == NodeType::APP_specularColorNode
            || node == NodeType::APP_shininessNode
            || node == NodeType::APP_transparencyNode
            || node == NodeType::APP_ambientIntensityNode
            || node == NodeType::APP_isFrontNode
            || node == NodeType::APP_isSmoothNode) {
            return true;
        } else if (node == NodeType::APP_targetNode) {
            m_lastTargetDefinitionID = attributes.getCityGMLIDAttribute();
            return true;
        }

        return GMLObjectElementParser::parseChildElementStartTag(node, attributes);
    }

    bool MaterialElementParser::parseChildElementEndTag(const NodeType::XMLNode& node, const std::string& characters)
    {
        if (m_model == nullptr) {
            throw std::runtime_error("MaterialElementParser::parseChildElementEndTag called before MaterialElementParser::parseElementStartTag");
        }

        if (node == NodeType::APP_diffuseColorNode) {

            m_model->setDiffuse(parseValue<TVec3f>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_emissiveColorNode) {

            m_model->setEmissive(parseValue<TVec3f>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_specularColorNode) {

            m_model->setSpecular(parseValue<TVec3f>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_shininessNode) {

            m_model->setShininess(parseValue<float>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_transparencyNode) {

            m_model->setTransparency(parseValue<float>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_ambientIntensityNode) {

            m_model->setAmbientIntensity(parseValue<float>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_isFrontNode) {

            m_model->setIsFront(parseValue<bool>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_isSmoothNode) {

            m_model->setIsSmooth(parseValue<bool>(characters, m_logger, getDocumentLocation()));
        } else if (node == NodeType::APP_targetNode) {

            m_factory.createMaterialTargetDefinition(parseReference(characters, m_logger, getDocumentLocation()), m_model, m_lastTargetDefinitionID);
            m_lastTargetDefinitionID = "";
        }  else {
            return GMLObjectElementParser::parseChildElementEndTag(node, characters);
        }
        return true;
    }

    Object* MaterialElementParser::getObject()
    {
        return m_model.get();
    }


}
