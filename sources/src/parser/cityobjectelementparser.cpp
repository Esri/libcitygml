#include "parser/cityobjectelementparser.h"
#include "parser/citygmldocumentparser.h"
#include "parser/nodetypes.h"
#include "parser/attributes.h"
#include "parser/documentlocation.h"
#include "parser/appearanceelementparser.h"
#include "parser/geometryelementparser.h"
#include "parser/implicitgeometryelementparser.h"
#include "parser/polygonelementparser.h"
#include "parser/skipelementparser.h"
#include "parser/delayedchoiceelementparser.h"
#include "parser/linestringelementparser.h"
#include "parser/addressparser.h"
#include "parser/rectifiedgridcoverageparser.h"
#include "parser/externalreferenceparser.h"

#include <citygml/citygmlfactory.h>
#include <citygml/citygmllogger.h>
#include <citygml/address.h>
#include <citygml/intermediateNode.h>

#include <stdexcept>
#include <iostream>

namespace citygml {

    bool CityObjectElementParser::typeIDTypeMapInitialized = false;
    bool CityObjectElementParser::attributesSetInitialized = false;

    std::unordered_map<int, CityObject::CityObjectsType> CityObjectElementParser::typeIDTypeMap = std::unordered_map<int, CityObject::CityObjectsType>();
    std::unordered_set<int> CityObjectElementParser::attributesSet = std::unordered_set<int>();
    std::unordered_map<int, AttributeType> CityObjectElementParser::attributeTypeMap;

    std::mutex CityObjectElementParser::initializedTypeIDMutex;
    std::mutex CityObjectElementParser::initializedAttributeSetMutex;

    #define HANDLE_TYPE( prefix, elementName ) std::pair<int, CityObject::CityObjectsType>(NodeType::prefix ## _ ## elementName ## Node.typeID(), CityObject::CityObjectsType::COT_## elementName)
    #define HANDLE_GROUP_TYPE( prefix, elementName, enumtype ) std::pair<int, CityObject::CityObjectsType>(NodeType::prefix ## _ ## elementName ## Node.typeID(), enumtype)
    #define HANDLE_ATTR( prefix, elementName ) NodeType::prefix ## _ ## elementName ## Node.typeID()

    CityObjectElementParser::CityObjectElementParser(CityGMLDocumentParser& documentParser, CityGMLFactory& factory, std::shared_ptr<CityGMLLogger> logger, std::function<void (CityObject*)> callback)
        : GMLFeatureCollectionElementParser(documentParser, factory, logger)
        , m_lastGenericAttributeType(AttributeType::String), m_lastAttributeType(AttributeType::String)
        , m_typeMask(documentParser.getParserParams().objectsMask.get())
        , m_skipped(false)
    {
        m_callback = callback;
		m_model = nullptr;
        m_currentParentId = "root";
    }

    std::string CityObjectElementParser::elementParserName() const
    {
        return "CityObjectElementParser";
    }

    void CityObjectElementParser::initializeTypeIDTypeMap()
    {
        // double-checked lock
        if (!typeIDTypeMapInitialized) {
            std::lock_guard<std::mutex> lock(CityObjectElementParser::initializedTypeIDMutex);

            if (!typeIDTypeMapInitialized) {
                typeIDTypeMap.insert(HANDLE_TYPE(GEN, GenericCityObject));
                typeIDTypeMap.insert(HANDLE_TYPE(GEN, GenericOccupiedSpace));
                typeIDTypeMap.insert(HANDLE_TYPE(GEN, GenericUnoccupiedSpace));
                typeIDTypeMap.insert(HANDLE_TYPE(GEN, GenericLogicalSpace));
                typeIDTypeMap.insert(HANDLE_TYPE(GEN, GenericThematicSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, Building));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, BuildingPart));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, Room));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, BuildingInstallation));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, BuildingFurniture));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, BuildingConstructiveElement));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, BuildingRoom));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, Door));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, Window));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, CityFurniture));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, Storey));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, buildingSubdivision));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, BuildingRoom));
                typeIDTypeMap.insert(HANDLE_TYPE(CORE, PointCloud));
                typeIDTypeMap.insert(HANDLE_TYPE(CORE, ClosureSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(FRN, CityFurniture));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Track));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Road));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Railway));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Square));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Intersection));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Section));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, Waterway));
                typeIDTypeMap.insert(HANDLE_GROUP_TYPE(TRANS, TransportationComplex, CityObject::CityObjectsType::COT_TransportationObject));
                typeIDTypeMap.insert(HANDLE_GROUP_TYPE(TRANS, TrafficArea, CityObject::CityObjectsType::COT_TransportationObject));
                typeIDTypeMap.insert(HANDLE_GROUP_TYPE(TRANS, AuxiliaryTrafficArea, CityObject::CityObjectsType::COT_TransportationObject));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, TrafficSpace));
                typeIDTypeMap.insert(HANDLE_TYPE(TRANS, AuxiliaryTrafficSpace));
                typeIDTypeMap.insert(HANDLE_TYPE(VEG, PlantCover));
                typeIDTypeMap.insert(HANDLE_TYPE(VEG, SolitaryVegetationObject));
                typeIDTypeMap.insert(HANDLE_TYPE(WTR, WaterBody));
                typeIDTypeMap.insert(HANDLE_GROUP_TYPE(WTR, WaterSurface, CityObject::CityObjectsType::COT_WaterBody));
                typeIDTypeMap.insert(HANDLE_GROUP_TYPE(WTR, WaterGroundSurface, CityObject::CityObjectsType::COT_WaterBody));
                typeIDTypeMap.insert(HANDLE_GROUP_TYPE(WTR, WaterClosureSurface, CityObject::CityObjectsType::COT_WaterBody));
                typeIDTypeMap.insert(HANDLE_TYPE(LUSE, LandUse));
                typeIDTypeMap.insert(HANDLE_TYPE(SUB, Tunnel));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, Bridge));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, BridgeConstructionElement));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, BridgeInstallation));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, BridgePart));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, outerBridgeConstruction));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, outerBridgeInstallation));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, WallSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, RoofSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, GroundSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, ClosureSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, FloorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, InteriorWallSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, CeilingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, OuterCeilingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BRID, OuterFloorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, fillingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, WindowSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, DoorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, WallSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, RoofSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, GroundSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, WallSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, RoofSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, GroundSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, ClosureSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, FloorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, InteriorWallSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, CeilingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, OuterCeilingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, OuterFloorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, FloorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, InteriorWallSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, CeilingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, OuterCeilingSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, OuterFloorSurface));
                typeIDTypeMap.insert(HANDLE_TYPE(CON, OtherConstruction));
                typeIDTypeMap.insert(HANDLE_TYPE(GRP, CityObjectGroup));
                typeIDTypeMap.insert(HANDLE_TYPE(DEM, ReliefFeature));
                typeIDTypeMap.insert(HANDLE_TYPE(DEM, reliefComponent));
                typeIDTypeMap.insert(HANDLE_TYPE(DEM, TINRelief));
                typeIDTypeMap.insert(HANDLE_TYPE(DEM, MassPointRelief));
                typeIDTypeMap.insert(HANDLE_TYPE(DEM, BreaklineRelief));
                typeIDTypeMap.insert(HANDLE_TYPE(DEM, RasterRelief));
                typeIDTypeMap.insert(HANDLE_TYPE(BLDG, IntBuildingInstallation));

                typeIDTypeMapInitialized = true;
            }
        }
    }

    void CityObjectElementParser::initializeAttributesSet()
    {
        // double-checked lock
        if (!attributesSetInitialized) {
            std::lock_guard<std::mutex> lock(CityObjectElementParser::initializedAttributeSetMutex);

            if (!attributesSetInitialized) {
                attributesSet.insert(HANDLE_ATTR(CORE, creationDate));
                attributesSet.insert(HANDLE_ATTR(CORE, terminationDate));
                attributesSet.insert(HANDLE_ATTR(BLDG, type));
                attributesSet.insert(HANDLE_ATTR(BLDG, class));
                attributesSet.insert(HANDLE_ATTR(BLDG, function));
                attributesSet.insert(HANDLE_ATTR(BLDG, usage));
                attributesSet.insert(HANDLE_ATTR(BLDG, yearOfConstruction));
                attributesSet.insert(HANDLE_ATTR(BLDG, yearOfDemolition));
                attributesSet.insert(HANDLE_ATTR(BLDG, storeyHeightsAboveGround));
                attributesSet.insert(HANDLE_ATTR(BLDG, storeyHeightsBelowGround));
                attributesSet.insert(HANDLE_ATTR(BLDG, storeysBelowGround));
                attributesSet.insert(HANDLE_ATTR(BLDG, storeysAboveGround));
                attributesSet.insert(HANDLE_ATTR(BLDG, measuredHeight));
                attributesSet.insert(HANDLE_ATTR(BLDG, roofType));
                attributesSet.insert(HANDLE_ATTR(VEG, class ));
                attributesSet.insert(HANDLE_ATTR(VEG, function ));
                attributesSet.insert(HANDLE_ATTR(VEG, averageHeight ));
                attributesSet.insert(HANDLE_ATTR(VEG, species ));
                attributesSet.insert(HANDLE_ATTR(VEG, height ));
                attributesSet.insert(HANDLE_ATTR(VEG, trunkDiameter ));
                attributesSet.insert(HANDLE_ATTR(VEG, crownDiameter ));
                attributesSet.insert(HANDLE_ATTR(FRN, class));
                attributesSet.insert(HANDLE_ATTR(FRN, function));
                attributesSet.insert(HANDLE_ATTR(GRP, class));
                attributesSet.insert(HANDLE_ATTR(GRP, function));
                attributesSet.insert(HANDLE_ATTR(GRP, usage));
                attributesSet.insert(HANDLE_ATTR(GEN, class));
                attributesSet.insert(HANDLE_ATTR(GEN, function));
                attributesSet.insert(HANDLE_ATTR(GEN, usage));
                attributesSet.insert(HANDLE_ATTR(GEN, area));
                attributesSet.insert(HANDLE_ATTR(GEN, spaceType));
                attributesSet.insert(HANDLE_ATTR(GEN, volume));
                attributesSet.insert(HANDLE_ATTR(LUSE, class));
                attributesSet.insert(HANDLE_ATTR(LUSE, function));
                attributesSet.insert(HANDLE_ATTR(LUSE, usage));
                attributesSet.insert(HANDLE_ATTR(DEM, lod));
                attributesSet.insert(HANDLE_ATTR(TRANS, usage));
                attributesSet.insert(HANDLE_ATTR(TRANS, function));
                attributesSet.insert(HANDLE_ATTR(TRANS, surfaceMaterial));
                attributesSet.insert(HANDLE_ATTR(TRANS, granularity));
                attributesSet.insert(HANDLE_ATTR(WTR, class));
                attributesSet.insert(HANDLE_ATTR(WTR, function));
                attributesSet.insert(HANDLE_ATTR(WTR, usage));
                attributesSet.insert(HANDLE_ATTR(WTR, waterLevel));


                attributeTypeMap[HANDLE_ATTR(CORE, creationDate)] = AttributeType::Date;
                attributeTypeMap[HANDLE_ATTR(CORE, terminationDate)] = AttributeType::Date;
                attributeTypeMap[HANDLE_ATTR(BLDG, type)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(BLDG, class)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(BLDG, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(BLDG, usage)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(BLDG, yearOfConstruction)] = AttributeType::Date;
                attributeTypeMap[HANDLE_ATTR(BLDG, yearOfDemolition)] = AttributeType::Date;
                attributeTypeMap[HANDLE_ATTR(BLDG, storeyHeightsAboveGround)] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(BLDG, storeyHeightsBelowGround)] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(BLDG, storeysBelowGround)] = AttributeType::Integer;
                attributeTypeMap[HANDLE_ATTR(BLDG, storeysAboveGround)] = AttributeType::Integer;
                attributeTypeMap[HANDLE_ATTR(BLDG, measuredHeight)] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(BLDG, roofType)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(VEG, class )] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(VEG, function )] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(VEG, averageHeight )] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(VEG, species )] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(VEG, height )] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(VEG, trunkDiameter )] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(VEG, crownDiameter )] = AttributeType::Double;
                attributeTypeMap[HANDLE_ATTR(FRN, class)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(FRN, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GRP, class)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GRP, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GRP, usage)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GEN, class)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GEN, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GEN, usage)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GEN, area)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GEN, spaceType)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(GEN, volume)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(LUSE, class)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(LUSE, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(LUSE, usage)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(DEM, lod)] = AttributeType::Integer;
                attributeTypeMap[HANDLE_ATTR(TRANS, usage)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(TRANS, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(TRANS, surfaceMaterial)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(TRANS, granularity)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(WTR, class)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(WTR, function)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(WTR, usage)] = AttributeType::String;
                attributeTypeMap[HANDLE_ATTR(WTR, waterLevel)] = AttributeType::Double;

                attributesSetInitialized = true;
            }
        }
    }

    AttributeType CityObjectElementParser::getAttributeType(const NodeType::XMLNode& node)
    {
        if (node == NodeType::GEN_stringAttributeNode) {
            return AttributeType::String;
        } else if (node == NodeType::GEN_doubleAttributeNode) {
            return AttributeType::Double;
        } else if (node == NodeType::GEN_intAttributeNode) {
            return AttributeType::Integer;
        } else if (node == NodeType::GEN_dateAttributeNode) {
            return AttributeType::Date;
        } else if (node == NodeType::GEN_uriAttributeNode) {
            return AttributeType::Uri;
        } else if (node == NodeType::CORE_genericAttributeNode) {
            return AttributeType::Generic;
        } else {
            // fallback to string for other types
            return AttributeType::String;
        }
    }

    bool CityObjectElementParser::handlesElement(const NodeType::XMLNode& node) const
    {
        initializeTypeIDTypeMap();

        return typeIDTypeMap.count(node.typeID()) > 0;
    }

    bool CityObjectElementParser::parseElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        initializeTypeIDTypeMap();

        auto it = typeIDTypeMap.find(node.typeID());

        if (it == typeIDTypeMap.end()) {
            CITYGML_LOG_ERROR(m_logger, "Expected start tag of CityObject but got <" << node.name() << "> at " << getDocumentLocation());
            throw std::runtime_error("Unexpected start tag found.");
        }

        auto const type = static_cast<CityObject::CityObjectsType>(it->second);
        if (m_typeMask.test(static_cast<size_t>(type))) {
            m_model = m_factory.createCityObject(attributes.getCityGMLIDAttribute(), type);

            std::string nodeId = attributes.getCityGMLIDAttribute();
            IntermediateNode intermediateNode(node.prefix(), node.baseName(), nodeId);
            m_model->pushIntermediateNode(intermediateNode, m_currentParentId);
            m_currentParentId = nodeId;

            return true;
        } else {
            m_skipped = true;//skipUnknownOrUnexpectedElement(node.name());//m_skipped = true;
            CITYGML_LOG_TRACE(m_logger, "Ignoring start tag <" << node.name() << "> because it is ignored at " << getDocumentLocation());
            return false;
        }

    }

    bool CityObjectElementParser::parseElementEndTag(const NodeType::XMLNode&, const std::string&)
    {
        if (getSourceSRSOverride()) {
            Envelope *envelope = new Envelope(getEnvelope().srsName());
            envelope->setLowerBound(m_model->getEnvelope().getLowerBound());
            envelope->setUpperBound(m_model->getEnvelope().getUpperBound());
            m_model->setEnvelope(envelope);
        }
        m_callback(m_model);
        m_model = nullptr;
        return true;
    }

    bool CityObjectElementParser::parseChildElementStartTag(const NodeType::XMLNode& node, Attributes& attributes)
    {
        if (m_skipped) {
            return false;
        }
        initializeAttributesSet();

        if (m_model == nullptr) {
            throw std::runtime_error("CityObjectElementParser::parseChildElementStartTag called before CityObjectElementParser::parseElementStartTag");
        }

        if (    node == NodeType::GEN_stringAttributeNode
             || node == NodeType::GEN_doubleAttributeNode
             || node == NodeType::GEN_intAttributeNode
             || node == NodeType::GEN_dateAttributeNode
             || node == NodeType::GEN_uriAttributeNode) {

            if (m_lastAttributeType == AttributeType::Generic) {
                m_lastGenericAttributeType = getAttributeType(node);
            } else {
                m_lastAttributeName = attributes.getAttribute("name");
                m_lastAttributeType = getAttributeType(node);
            }
        } else if (node == NodeType::CORE_genericAttributeNode) {
            m_lastAttributeType = getAttributeType(node);
        } else if (attributesSet.count(node.typeID()) > 0 
            || node == NodeType::GEN_nameNode
            || node == NodeType::GEN_valueNode) {

            return true;
        } else if (node == NodeType::GML_RectifiedGridCoverageNode) {
            
            setParserForNextElement(new RectifiedGridCoverageParser(m_documentParser, m_factory, m_logger, [this](RectifiedGridCoverage * rectifiedGridCoverage) {
                m_model->setRectifiedGridCoverage(rectifiedGridCoverage);
            }));
        } else if (node == NodeType::BLDG_boundedByNode
                   || node == NodeType::BLDG_outerBuildingInstallationNode
                   || node == NodeType::BLDG_interiorBuildingInstallationNode
                   || node == NodeType::BLDG_interiorFurnitureNode
                   || node == NodeType::BLDG_roomInstallationNode
                   || node == NodeType::BLDG_interiorRoomNode
                   || node == NodeType::BLDG_openingNode
                   || node == NodeType::BLDG_consistsOfBuildingPartNode
                   || node == NodeType::BLDG_BuildingPartNode
                   || node == NodeType::BLDG_BuildingConstructiveElementNode
                   || node == NodeType::BLDG_BuildingRoomNode
                   || node == NodeType::BLDG_BuildingInstallationNode
                   || node == NodeType::BLDG_buildingSubdivisionNode
                   || node == NodeType::BLDG_StoreyNode
                   || node == NodeType::GRP_groupMemberNode
                   || node == NodeType::GRP_parentNode
                   || node == NodeType::TRANS_TrafficAreaNode
                   || node == NodeType::TRANS_AuxiliaryTrafficAreaNode
                   || node == NodeType::TRANS_TrafficSpaceNode
                   || node == NodeType::TRANS_AuxiliaryTrafficSpaceNode
                   || node == NodeType::TRANS_IntersectionNode
                   || node == NodeType::TRANS_SectionNode
                   || node == NodeType::TRANS_WaterwayNode
                   || node == NodeType::WTR_boundedByNode
                   || node == NodeType::DEM_reliefComponentNode
                   || node == NodeType::DEM_TINReliefNode
                   || node == NodeType::DEM_MassPointReliefNode
                   || node == NodeType::DEM_BreaklineReliefNode
                   || node == NodeType::DEM_RasterReliefNode
                   || node == NodeType::DEM_gridNode
                   || node == NodeType::CORE_generalizesToNode
                   || node == NodeType::GEN_GenericOccupiedSpaceNode
                   || node == NodeType::GEN_GenericUnoccupiedSpaceNode
                   || node == NodeType::GEN_GenericLogicalSpaceNode
                   || node == NodeType::GEN_GenericThematicSurfaceNode
                   || node == NodeType::CORE_boundaryNode
                   || node == NodeType::CON_fillingSurfaceNode
                   || node == NodeType::CON_WindowSurfaceNode
                   || node == NodeType::CON_DoorSurfaceNode
                   || node == NodeType::CORE_PointCloudNode
                   || node == NodeType::CON_OtherConstructionNode
                   || node == NodeType::BRID_outerBridgeConstructionNode
                   || node == NodeType::BRID_outerBridgeInstallationNode
                   || node == NodeType::BRID_BridgeConstructionElementNode
                   || node == NodeType::BRID_BridgeInstallationNode
                   || node == NodeType::BRID_BridgePartNode
                   || node == NodeType::BRID_boundedByNode) {
            std::string nodeId = attributes.getCityGMLIDAttribute();
            setParserForNextElement(new CityObjectElementParser(m_documentParser, m_factory, m_logger, [this, node, nodeId](CityObject* obj) {
                                        m_model->addChildCityObject(obj);
                                        IntermediateNode intermediateNode(node.prefix(), node.baseName(), nodeId);
                                        obj->pushIntermediateNode(intermediateNode, "root");
                                    }));
        } else if (node == NodeType::APP_AppearanceNode // Compatibility with CityGML 1.0 (in CityGML 2 CityObjects can only contain appearanceMember elements)
                   || node == NodeType::APP_appearanceMemberNode) {

            setParserForNextElement(new AppearanceElementParser(m_documentParser, m_factory, m_logger));
        } else if (node == NodeType::DEM_extentNode
                   || node == NodeType::DEM_tinNode
                   || node == NodeType::DEM_reliefPointsNode
                   || node == NodeType::DEM_ridgeOrValleyLinesNode
                   || node == NodeType::DEM_breaklinesNode) {
            
            std::string lod = m_model->getAttribute("dem:lod");
            if (!lod.empty()) {
                parseGeometryForLODLevel(node, std::stoi(lod), attributes);
            } else {
                parseGeometryForLODLevel(node, 0, attributes);
            }
        } else if (node == NodeType::GEN_lod0TerrainIntersectionNode
                   || node == NodeType::WTR_lod0MultiCurveNode
                   || node == NodeType::WTR_lod0MultiSurfaceNode
                   || node == NodeType::CORE_lod0MultiSurfaceNode) {
            
            parseGeometryForLODLevel(node, 0, attributes);
        } else if (node == NodeType::BLDG_lod0FootPrintNode) {
            // for Lod0 footprint, we must explicitly set the City Object Type, because the parent type is "Building", and it doesn't allow to discriminate between the ground and roof surface
            parseGeometryForLODLevel(node, 0, CityObject::CityObjectsType::COT_GroundSurface, attributes);
        } else if (node == NodeType::BLDG_lod0RoofEdgeNode) {
            // for Lod0 roof edge, we must explicitly set the City Object Type, because the parent type is "Building", and it doesn't allow to discriminate between the ground and roof surface
            parseGeometryForLODLevel(node, 0, CityObject::CityObjectsType::COT_RoofSurface, attributes);
        } else if (node == NodeType::BLDG_lod1MultiCurveNode
                   || node == NodeType::BLDG_lod1MultiSurfaceNode
                   || node == NodeType::BRID_lod1MultiSurfaceNode
                   || node == NodeType::VEG_lod1MultiSurfaceNode
                   || node == NodeType::BLDG_lod1SolidNode
                   || node == NodeType::BLDG_lod1TerrainIntersectionNode
                   || node == NodeType::GEN_lod1TerrainIntersectionNode
                   || node == NodeType::FRN_lod1TerrainIntersectionNode
                   || node == NodeType::LUSE_lod1MultiSurfaceNode
                   || node == NodeType::TRANS_lod1MultiSurfaceNode
                   || node == NodeType::WTR_lod1MultiCurveNode
                   || node == NodeType::WTR_lod1MultiSurfaceNode
                   || node == NodeType::CORE_lod0MultiSurfaceNode
                   || node == NodeType::CORE_lod1MultiSurfaceNode
                   || node == NodeType::WTR_lod1SolidNode) {

            parseGeometryForLODLevel(node, 1, attributes);
        } else if (node == NodeType::BLDG_lod2MultiCurveNode
                   || node == NodeType::BLDG_lod2MultiSurfaceNode
                   || node == NodeType::BRID_lod2MultiSurfaceNode
                   || node == NodeType::VEG_lod2MultiSurfaceNode
                   || node == NodeType::BLDG_lod2SolidNode
                   || node == NodeType::BLDG_lod2TerrainIntersectionNode
                   || node == NodeType::GEN_lod2TerrainIntersectionNode
                   || node == NodeType::FRN_lod2TerrainIntersectionNode
                   || node == NodeType::LUSE_lod2MultiSurfaceNode
                   || node == NodeType::TRANS_lod2MultiSurfaceNode
                   || node == NodeType::WTR_lod2SolidNode
                   || node == NodeType::WTR_lod2SurfaceNode
                   || node == NodeType::GEN_Lod0MultiCurveNode
                   || node == NodeType::GEN_Lod2MultiCurveNode
                   || node == NodeType::GEN_Lod3MultiCurveNode
                   || node == NodeType::GEN_Lod0MultiSurfaceNode
                   || node == NodeType::GEN_Lod2MultiSurfaceNode
                   || node == NodeType::GEN_Lod3MultiSurfaceNode
                   || node == NodeType::CORE_lod0MultiSurfaceNode
                   || node == NodeType::CORE_lod2MultiSurfaceNode
                   || node == NodeType::CORE_lod3MultiSurfaceNode
                   || node == NodeType::CORE_lod1SolidNode
                   || node == NodeType::CORE_lod2SolidNode
                   || node == NodeType::CORE_lod3SolidNode) {

            parseGeometryForLODLevel(node, 2, attributes);
        } else if (node == NodeType::BLDG_lod3MultiCurveNode
                   || node == NodeType::BLDG_lod3MultiSurfaceNode
                   || node == NodeType::BRID_lod3MultiSurfaceNode
                   || node == NodeType::VEG_lod3MultiSurfaceNode
                   || node == NodeType::CORE_lod3MultiSurfaceNode
                   || node == NodeType::BLDG_lod3SolidNode
                   || node == NodeType::BLDG_lod3TerrainIntersectionNode
                   || node == NodeType::GEN_lod3TerrainIntersectionNode
                   || node == NodeType::FRN_lod3TerrainIntersectionNode
                   || node == NodeType::LUSE_lod3MultiSurfaceNode
                   || node == NodeType::TRANS_lod3MultiSurfaceNode
                   || node == NodeType::WTR_lod3SolidNode
                   || node == NodeType::WTR_lod3SurfaceNode) {

            parseGeometryForLODLevel(node, 3, attributes);
        } else if (node == NodeType::BLDG_lod4MultiCurveNode
                   || node == NodeType::BLDG_lod4SolidNode
                   || node == NodeType::VEG_lod4MultiSurfaceNode
                   || node == NodeType::BLDG_lod4MultiSurfaceNode
                   || node == NodeType::BRID_lod4MultiSurfaceNode
                   || node == NodeType::BLDG_lod4SolidNode
                   || node == NodeType::BLDG_lod4TerrainIntersectionNode
                   || node == NodeType::GEN_lod4TerrainIntersectionNode
                   || node == NodeType::FRN_lod4TerrainIntersectionNode
                   || node == NodeType::LUSE_lod4MultiSurfaceNode
                   || node == NodeType::TRANS_lod4MultiSurfaceNode
                   || node == NodeType::WTR_lod4SolidNode
                   || node == NodeType::WTR_lod4SurfaceNode) {

            parseGeometryForLODLevel(node, 4, attributes);
        } else if (node == NodeType::GEN_lod0GeometryNode) {
            parseGeometryPropertyElementForLODLevel(node, 0, attributes.getCityGMLIDAttribute());
        }
        else if (node == NodeType::GEN_lod1GeometryNode
                   || node == NodeType::FRN_lod1GeometryNode
                   || node == NodeType::VEG_lod1GeometryNode
                   || node == NodeType::BRID_lod1GeometryNode) {
            parseGeometryPropertyElementForLODLevel(node, 1, attributes.getCityGMLIDAttribute());
        } else if (node == NodeType::GEN_lod2GeometryNode
                   || node == NodeType::FRN_lod2GeometryNode
                   || node == NodeType::BLDG_lod2GeometryNode
                   || node == NodeType::VEG_lod2GeometryNode
                   || node == NodeType::BRID_lod2GeometryNode ) {
            parseGeometryPropertyElementForLODLevel(node, 2, attributes.getCityGMLIDAttribute());
        } else if (node == NodeType::GEN_lod3GeometryNode
                   || node == NodeType::FRN_lod3GeometryNode
                   || node == NodeType::BLDG_lod3GeometryNode
                   || node == NodeType::VEG_lod3GeometryNode
                   || node == NodeType::BRID_lod3GeometryNode) {
            parseGeometryPropertyElementForLODLevel(node, 3, attributes.getCityGMLIDAttribute());
        } else if (node == NodeType::GEN_lod4GeometryNode
                   || node == NodeType::FRN_lod4GeometryNode
                   || node == NodeType::BLDG_lod4GeometryNode
                   || node == NodeType::VEG_lod4GeometryNode
                   || node == NodeType::BRID_lod4GeometryNode) {
            parseGeometryPropertyElementForLODLevel(node, 4, attributes.getCityGMLIDAttribute());
        } else if (node == NodeType::GEN_lod0ImplicitRepresentationNode) {
          
            parseImplicitGeometryForLODLevel(0);
        } else if (node == NodeType::VEG_lod1ImplicitRepresentationNode
                   || node == NodeType::FRN_lod1ImplicitRepresentationNode
                   || node == NodeType::GEN_lod1ImplicitRepresentationNode) {

            parseImplicitGeometryForLODLevel(1);
        } else if (node == NodeType::VEG_lod2ImplicitRepresentationNode
                   || node == NodeType::FRN_lod2ImplicitRepresentationNode
                   || node == NodeType::GEN_lod2ImplicitRepresentationNode) {

            parseImplicitGeometryForLODLevel(2);
        } else if (node == NodeType::VEG_lod3ImplicitRepresentationNode
                   || node == NodeType::FRN_lod3ImplicitRepresentationNode
                   || node == NodeType::GEN_lod3ImplicitRepresentationNode) {

            parseImplicitGeometryForLODLevel(3);
        } else if (node == NodeType::VEG_lod4ImplicitRepresentationNode
                   || node == NodeType::FRN_lod4ImplicitRepresentationNode
                   || node == NodeType::GEN_lod4ImplicitRepresentationNode) {

            parseImplicitGeometryForLODLevel(4);
        } else if (node == NodeType::CORE_ExternalReferenceNode) {
            
            setParserForNextElement(new ExternalReferenceParser(m_documentParser, m_factory, m_logger, [this](ExternalReference * externalReference){
                m_model->setExternalReference(externalReference);
            }));
        } else if (node == NodeType::GML_MultiPointNode
                   || node == NodeType::GRP_geometryNode
                   || node == NodeType::TRANS_lod0NetworkNode) {
            CITYGML_LOG_INFO(m_logger, "Skipping CityObject child element <" << node  << ">  at " << getDocumentLocation() << " (Currently not supported!)");
            setParserForNextElement(new SkipElementParser(m_documentParser, m_logger, node));
            return true;
        } else if (node == NodeType::BLDG_addressNode
                   || node == NodeType::CORE_AddressNode
                   || node == NodeType::CORE_xalAddressNode) {
            setParserForNextElement(new AddressParser(m_documentParser, m_factory, m_logger, [this](std::unique_ptr<Address>&& address) {
                m_model->setAddress(std::move(address));
            }));
            return true;
        } else {
            return GMLFeatureCollectionElementParser::parseChildElementStartTag(node, attributes);
        }

        return true;

    }

    bool CityObjectElementParser::parseChildElementEndTag(const NodeType::XMLNode& node, const std::string& characters)
    {
        if (m_skipped) {
            return false;
        } else if (m_model == nullptr) {
            throw std::runtime_error("CityObjectElementParser::parseChildElementEndTag called before CityObjectElementParser::parseElementStartTag");
        }

        initializeAttributesSet();

        if (    node == NodeType::GEN_stringAttributeNode
             || node == NodeType::GEN_doubleAttributeNode
             || node == NodeType::GEN_intAttributeNode
             || node == NodeType::GEN_dateAttributeNode
             || node == NodeType::GEN_uriAttributeNode) {

            if (m_lastAttributeType != AttributeType::Generic) {
                m_lastAttributeName = "";
                m_lastAttributeType = AttributeType::String;
            }

            return true;
        } else if (node == NodeType::CORE_genericAttributeNode) {
            if(!m_lastAttributeName.empty())
                m_model->setAttribute(m_lastAttributeName, m_lastGenericAttributeValue, m_lastGenericAttributeType);
            m_lastAttributeName = "";
            m_lastGenericAttributeValue = "";
            m_lastAttributeType = AttributeType::String;
            m_lastGenericAttributeType = AttributeType::String;
            return true;
        } else if (node == NodeType::GEN_nameNode) {
            m_lastAttributeName = characters;
            return true;
        } else if (node == NodeType::GEN_valueNode) {

            if (m_lastAttributeType == AttributeType::Generic) {
                m_lastGenericAttributeValue = characters;
            } else {
                if (!m_lastAttributeName.empty()) {
                    m_model->setAttribute(m_lastAttributeName, characters, m_lastAttributeType);
                } else {
                    CITYGML_LOG_WARN(m_logger, "Found value node (" << NodeType::GEN_valueNode << ") outside attribute node... ignore.");
                }
            }

            return true;
        } else if (attributesSet.count(node.typeID()) > 0) {
            if (!characters.empty()) {
                m_model->setAttribute(node.name(), characters, attributeTypeMap.at(node.typeID()));
            }
            return true;
        } else if (node == NodeType::BLDG_boundedByNode
                    || node == NodeType::BLDG_outerBuildingInstallationNode
                    || node == NodeType::BLDG_interiorBuildingInstallationNode
                    || node == NodeType::BLDG_interiorFurnitureNode
                    || node == NodeType::BLDG_roomInstallationNode
                    || node == NodeType::BLDG_interiorRoomNode
                    || node == NodeType::BLDG_openingNode
                    || node == NodeType::APP_AppearanceNode
                    || node == NodeType::APP_appearanceMemberNode
                    || node == NodeType::BLDG_lod0FootPrintNode
                    || node == NodeType::BLDG_lod0RoofEdgeNode
                    || node == NodeType::BLDG_lod1MultiCurveNode
                    || node == NodeType::BLDG_lod1MultiSurfaceNode
                    || node == NodeType::BLDG_lod1SolidNode
                    || node == NodeType::BLDG_lod1TerrainIntersectionNode
                    || node == NodeType::BLDG_lod2GeometryNode
                    || node == NodeType::BLDG_lod2MultiCurveNode
                    || node == NodeType::BLDG_lod2MultiSurfaceNode
                    || node == NodeType::BLDG_lod2SolidNode
                    || node == NodeType::BLDG_lod2TerrainIntersectionNode
                    || node == NodeType::BLDG_lod3GeometryNode
                    || node == NodeType::BLDG_lod3MultiCurveNode
                    || node == NodeType::BLDG_lod3MultiSurfaceNode
                    || node == NodeType::BLDG_lod3SolidNode
                    || node == NodeType::BLDG_lod3TerrainIntersectionNode
                    || node == NodeType::BLDG_lod4GeometryNode
                    || node == NodeType::BLDG_lod4MultiCurveNode
                    || node == NodeType::BLDG_lod4MultiSurfaceNode
                    || node == NodeType::BLDG_lod4SolidNode
                    || node == NodeType::BLDG_lod4TerrainIntersectionNode
                    || node == NodeType::BLDG_BuildingPartNode
                    || node == NodeType::BLDG_BuildingConstructiveElementNode
                    || node == NodeType::BLDG_BuildingRoomNode
                    || node == NodeType::BLDG_BuildingInstallationNode
                    || node == NodeType::CON_fillingSurfaceNode
                    || node == NodeType::CON_WindowSurfaceNode
                    || node == NodeType::CON_DoorSurfaceNode
                    || node == NodeType::BLDG_buildingSubdivisionNode
                    || node == NodeType::BLDG_StoreyNode
                    || node == NodeType::CON_OtherConstructionNode
                    || node == NodeType::GEN_lod1GeometryNode
                    || node == NodeType::GEN_lod2GeometryNode
                    || node == NodeType::GEN_lod3GeometryNode
                    || node == NodeType::GEN_lod4GeometryNode
                    || node == NodeType::GEN_lod1TerrainIntersectionNode
                    || node == NodeType::GEN_lod2TerrainIntersectionNode
                    || node == NodeType::GEN_lod3TerrainIntersectionNode
                    || node == NodeType::GEN_lod4TerrainIntersectionNode
                    || node == NodeType::GEN_lod1ImplicitRepresentationNode
                    || node == NodeType::GEN_lod2ImplicitRepresentationNode
                    || node == NodeType::GEN_lod3ImplicitRepresentationNode
                    || node == NodeType::GEN_lod4ImplicitRepresentationNode
                    || node == NodeType::VEG_lod1ImplicitRepresentationNode
                    || node == NodeType::VEG_lod2ImplicitRepresentationNode
                    || node == NodeType::VEG_lod3ImplicitRepresentationNode
                    || node == NodeType::VEG_lod4ImplicitRepresentationNode
                    || node == NodeType::VEG_lod2GeometryNode
                    || node == NodeType::CORE_ExternalReferenceNode
                    || node == NodeType::BLDG_consistsOfBuildingPartNode
                    || node == NodeType::FRN_lod1GeometryNode
                    || node == NodeType::FRN_lod1TerrainIntersectionNode
                    || node == NodeType::FRN_lod1ImplicitRepresentationNode
                    || node == NodeType::FRN_lod2GeometryNode
                    || node == NodeType::FRN_lod2TerrainIntersectionNode
                    || node == NodeType::FRN_lod2ImplicitRepresentationNode
                    || node == NodeType::FRN_lod3GeometryNode
                    || node == NodeType::FRN_lod3TerrainIntersectionNode
                    || node == NodeType::FRN_lod3ImplicitRepresentationNode
                    || node == NodeType::FRN_lod4GeometryNode
                    || node == NodeType::FRN_lod4TerrainIntersectionNode
                    || node == NodeType::FRN_lod4ImplicitRepresentationNode
                    || node == NodeType::CORE_generalizesToNode
                    || node == NodeType::CORE_boundaryNode
                    || node == NodeType::CORE_PointCloudNode
                    || node == NodeType::GML_MultiPointNode
                    || node == NodeType::GRP_groupMemberNode
                    || node == NodeType::GRP_parentNode
                    || node == NodeType::LUSE_lod1MultiSurfaceNode
                    || node == NodeType::LUSE_lod2MultiSurfaceNode
                    || node == NodeType::LUSE_lod3MultiSurfaceNode
                    || node == NodeType::LUSE_lod4MultiSurfaceNode
                    || node == NodeType::DEM_reliefComponentNode
                    || node == NodeType::DEM_TINReliefNode
                    || node == NodeType::DEM_MassPointReliefNode
                    || node == NodeType::DEM_BreaklineReliefNode
                    || node == NodeType::DEM_RasterReliefNode
                    || node == NodeType::DEM_tinNode
                    || node == NodeType::DEM_reliefPointsNode
                    || node == NodeType::DEM_ridgeOrValleyLinesNode
                    || node == NodeType::DEM_breaklinesNode
                    || node == NodeType::DEM_gridNode
                    || node == NodeType::GEN_lod0GeometryNode
                    || node == NodeType::GEN_lod0ImplicitRepresentationNode
                    || node == NodeType::GEN_lod0TerrainIntersectionNode
                    || node == NodeType::TRANS_lod0NetworkNode
                    || node == NodeType::TRANS_TrafficAreaNode
                    || node == NodeType::TRANS_AuxiliaryTrafficAreaNode
                    || node == NodeType::TRANS_TrafficSpaceNode
                    || node == NodeType::TRANS_AuxiliaryTrafficSpaceNode
                    || node == NodeType::TRANS_IntersectionNode
                    || node == NodeType::TRANS_SectionNode
                    || node == NodeType::TRANS_WaterwayNode
                    || node == NodeType::TRANS_lod1MultiSurfaceNode
                    || node == NodeType::TRANS_lod2MultiSurfaceNode
                    || node == NodeType::TRANS_lod3MultiSurfaceNode
                    || node == NodeType::TRANS_lod4MultiSurfaceNode
                    || node == NodeType::WTR_lod0MultiCurveNode
                    || node == NodeType::WTR_lod0MultiSurfaceNode
                    || node == NodeType::WTR_lod1MultiCurveNode
                    || node == NodeType::WTR_lod1MultiSurfaceNode
                    || node == NodeType::WTR_lod1SolidNode
                    || node == NodeType::WTR_lod2SolidNode
                    || node == NodeType::WTR_lod3SolidNode
                    || node == NodeType::WTR_lod4SolidNode
                    || node == NodeType::WTR_lod2SurfaceNode
                    || node == NodeType::WTR_lod3SurfaceNode
                    || node == NodeType::WTR_lod4SurfaceNode
                    || node == NodeType::WTR_boundedByNode
                    || node == NodeType::BLDG_addressNode
                    || node == NodeType::CORE_AddressNode
                    || node == NodeType::CORE_xalAddressNode
                    || node == NodeType::GEN_GenericOccupiedSpaceNode
                    || node == NodeType::GEN_GenericUnoccupiedSpaceNode
                    || node == NodeType::GEN_GenericLogicalSpaceNode
                    || node == NodeType::GEN_GenericThematicSurfaceNode
                    || node == NodeType::GEN_Lod0MultiCurveNode
                    || node == NodeType::GEN_Lod2MultiCurveNode
                    || node == NodeType::GEN_Lod3MultiCurveNode
                    || node == NodeType::GEN_Lod0MultiSurfaceNode
                    || node == NodeType::GEN_Lod2MultiSurfaceNode
                    || node == NodeType::GEN_Lod3MultiSurfaceNode
                    || node == NodeType::CORE_lod0MultiSurfaceNode
                    || node == NodeType::CORE_lod1MultiSurfaceNode
                    || node == NodeType::CORE_lod2MultiSurfaceNode
                    || node == NodeType::CORE_lod3MultiSurfaceNode
                    || node == NodeType::CORE_lod1SolidNode
                    || node == NodeType::CORE_lod2SolidNode
                    || node == NodeType::CORE_lod3SolidNode
                    || node == NodeType::CORE_boundaryNode
                    || node == NodeType::BRID_outerBridgeConstructionNode
                    || node == NodeType::BRID_outerBridgeInstallationNode
                    || node == NodeType::BRID_BridgeConstructionElementNode
                    || node == NodeType::BRID_BridgeInstallationNode
                    || node == NodeType::BRID_BridgePartNode
                    || node == NodeType::BRID_boundedByNode
                    || node == NodeType::BRID_lod1GeometryNode
                    || node == NodeType::BRID_lod2GeometryNode
                    || node == NodeType::BRID_lod3GeometryNode
                    || node == NodeType::BRID_lod4GeometryNode
                    || node == NodeType::BRID_lod1MultiSurfaceNode
                    || node == NodeType::BRID_lod2MultiSurfaceNode
                    || node == NodeType::BRID_lod3MultiSurfaceNode
                    || node == NodeType::BRID_lod4MultiSurfaceNode
                    || node == NodeType::VEG_lod1MultiSurfaceNode
                    || node == NodeType::VEG_lod2MultiSurfaceNode
                    || node == NodeType::VEG_lod3MultiSurfaceNode
                    || node == NodeType::VEG_lod4MultiSurfaceNode) {

            return true;
        }

        return GMLFeatureCollectionElementParser::parseChildElementEndTag(node, characters);

    }

    FeatureObject* CityObjectElementParser::getFeatureObject()
    {
        return m_model;
    }
    
    void CityObjectElementParser::parseGeometryForLODLevel(const citygml::NodeType::XMLNode& node, int lod, CityObject::CityObjectsType parentType, const Attributes& attributes)
    {
        std::string nodeId = attributes.getCityGMLIDAttribute();
        setParserForNextElement(new GeometryElementParser(m_documentParser, m_factory, m_logger, lod, parentType, [this, node, nodeId](Geometry* geom) {
            m_model->addGeometry(geom);
            // Need to push to back as this is the top level tag of the geometry
            IntermediateNode intermediateNode(node.prefix(), node.baseName(), nodeId);
            geom->pushIntermediateNode(intermediateNode, "root");
        }));
    }
    
    void CityObjectElementParser::parseGeometryForLODLevel(const citygml::NodeType::XMLNode& node, int lod, const Attributes& attributes)
    {
        parseGeometryForLODLevel(node, lod, m_model->getType(), attributes);
    }

    void CityObjectElementParser::parseImplicitGeometryForLODLevel(int lod)
    {
        setParserForNextElement(new ImplicitGeometryElementParser(m_documentParser, m_factory, m_logger, lod, m_model->getType(), [this](ImplicitGeometry* imp) {
            m_model->addImplictGeometry(imp);
        }));
    }

    void CityObjectElementParser::parseGeometryPropertyElementForLODLevel(const citygml::NodeType::XMLNode& node, int lod, const std::string& id)
    {
        setParserForNextElement(new DelayedChoiceElementParser(m_documentParser, m_logger, {
            new PolygonElementParser(m_documentParser, m_factory, m_logger, [id, lod, node, this](std::shared_ptr<Polygon> p) {
                                                                       Geometry* geom = m_factory.createGeometry(id, m_model->getType(), lod);
                                                                       geom->addPolygon(p);
                                                                       m_model->addGeometry(geom);
                                                                       IntermediateNode intermediateNode(node.prefix(), node.baseName(), id);
                                                                       geom->pushIntermediateNode(intermediateNode, "root");
                                                                   }),
            new LineStringElementParser(m_documentParser, m_factory, m_logger, [id, lod, node, this](std::shared_ptr<LineString> l) {
                                                                       Geometry* geom = m_factory.createGeometry(id, m_model->getType(), lod);
                                                                       geom->addLineString(l);
                                                                       m_model->addGeometry(geom);
                                                                       IntermediateNode intermediateNode(node.prefix(), node.baseName(), id);
                                                                       geom->pushIntermediateNode(intermediateNode, "root");
                                                                   }),
            new GeometryElementParser(m_documentParser, m_factory, m_logger, lod, m_model->getType(), [this, node, id](Geometry* geom) {
                                                                       m_model->addGeometry(geom);
                                                                       IntermediateNode intermediateNode(node.prefix(), node.baseName(), id);
                                                                       geom->pushIntermediateNode(intermediateNode, "root");
                                                                   })
        }));

    }

}
