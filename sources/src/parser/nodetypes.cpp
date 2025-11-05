#include "parser/nodetypes.h"
#include <citygml/utils.h>

namespace citygml {

    // declare static class members
    std::mutex NodeType::initializedMutex;
    bool NodeType::nodesInitialized = false;
    int NodeType::typeCount = -1;
    std::unordered_map<std::string, NodeType::XMLNode*> NodeType::nodeNameTypeMap;
    std::unordered_map<std::string, NodeType::XMLNode*> NodeType::nodeNameWithPrefixTypeMap;

    NodeType::XMLNode::XMLNode()
    {
        //
    }

    NodeType::XMLNode::XMLNode(std::string prefix, std::string name ) : m_name(name), m_prefix(toLower(prefix))
    {
        m_typeID = NodeType::typeCount++;
    }

    const std::string NodeType::XMLNode::name() const
    {
        return m_prefix + ":" + m_name;
    }

    const std::string& NodeType::XMLNode::prefix() const
    {
        return m_prefix;
    }

    const std::string& NodeType::XMLNode::baseName() const
    {
        return m_name;
    }

    int NodeType::XMLNode::typeID() const
    {
        return m_typeID;
    }

    bool NodeType::XMLNode::operator==(const NodeType::XMLNode& other) const
    {
        return typeID() == other.typeID();
    }

    bool NodeType::XMLNode::operator!=(const NodeType::XMLNode& other) const
    {
        return !(*this == other);
    }

    bool NodeType::XMLNode::valid() const
    {
        return !m_name.empty();
    }

    std::ostream& operator<<(std::ostream& os, const NodeType::XMLNode& o)
    {
        if (!o.valid()) {
            os << "InvalidNode";
        } else {
            os << o.name();
        }

        return os;
    }

    const NodeType::XMLNode NodeType::InvalidNode = XMLNode("", "");

#define INITIALIZE_NODE( prefix, elementname ) \
    NodeType::prefix ## _ ## elementname ## Node = XMLNode( #prefix , #elementname ); \
    NodeType::nodeNameTypeMap[toLower(#elementname)] = &NodeType::prefix ## _ ## elementname ## Node; \
    NodeType::nodeNameWithPrefixTypeMap[toLower(#prefix ":" #elementname)] = &NodeType::prefix ## _ ## elementname ## Node;

    void NodeType::initializeNodeTypes()
    {
        // double-checked locking to prevent synchronisation when the node types are already intialized
        if(!nodesInitialized) {
            std::lock_guard<std::mutex> lock(NodeType::initializedMutex);

            if (!nodesInitialized) {

                // CORE
                INITIALIZE_NODE( CORE, CityModel )
                INITIALIZE_NODE( CORE, cityObjectMember )
                INITIALIZE_NODE( CORE, creationDate )
                INITIALIZE_NODE( CORE, terminationDate )
                INITIALIZE_NODE( CORE, generalizesTo)
                INITIALIZE_NODE( CORE, PointCloud)

                INITIALIZE_NODE( CORE, ExternalReference)
                INITIALIZE_NODE( CORE, informationSystem)
                INITIALIZE_NODE( CORE, externalObject)

                INITIALIZE_NODE( CORE, uri)
                INITIALIZE_NODE( CORE, name)

                INITIALIZE_NODE( CORE, Address )
                INITIALIZE_NODE( CORE, xalAddress )

                INITIALIZE_NODE( CORE, ImplicitGeometry )
                INITIALIZE_NODE( CORE, relativeGMLGeometry )
                INITIALIZE_NODE( CORE, transformationMatrix )
                INITIALIZE_NODE( CORE, referencePoint)
                INITIALIZE_NODE( CORE, mimeType)
                INITIALIZE_NODE( CORE, libraryObject)
                INITIALIZE_NODE( CORE, genericAttribute)

                INITIALIZE_NODE( CORE, lod0MultiSurface)
                INITIALIZE_NODE( CORE, lod1MultiSurface)
                INITIALIZE_NODE( CORE, lod2MultiSurface)
                INITIALIZE_NODE( CORE, lod3MultiSurface)

                INITIALIZE_NODE( CORE, lod1Solid)
                INITIALIZE_NODE( CORE, lod2Solid)
                INITIALIZE_NODE( CORE, lod3Solid)

                INITIALIZE_NODE( CORE, boundary)
                INITIALIZE_NODE( CORE, relativeGeometry)
                INITIALIZE_NODE( CORE, ClosureSurface) // Should only be in BLDG namespace, but let's be lenient and read it from CORE as well.

                // GRP
                INITIALIZE_NODE( GRP, CityObjectGroup )
                INITIALIZE_NODE( GRP, groupMember )
                INITIALIZE_NODE( GRP, class )
                INITIALIZE_NODE( GRP, function )
                INITIALIZE_NODE( GRP, usage )
                INITIALIZE_NODE( GRP, parent )
                INITIALIZE_NODE( GRP, geometry )

                // GEN
                INITIALIZE_NODE( GEN, class )
                INITIALIZE_NODE( GEN, function )
                INITIALIZE_NODE( GEN, usage )
                INITIALIZE_NODE( GEN, GenericCityObject )
                INITIALIZE_NODE( GEN, stringAttribute )
                INITIALIZE_NODE( GEN, doubleAttribute )
                INITIALIZE_NODE( GEN, intAttribute )
                INITIALIZE_NODE( GEN, dateAttribute )
                INITIALIZE_NODE( GEN, uriAttribute )
                INITIALIZE_NODE( GEN, name )
                INITIALIZE_NODE( GEN, value )

                INITIALIZE_NODE( GEN, lod0Geometry )
                INITIALIZE_NODE( GEN, lod1Geometry )
                INITIALIZE_NODE( GEN, lod2Geometry )
                INITIALIZE_NODE( GEN, lod3Geometry )
                INITIALIZE_NODE( GEN, lod4Geometry )
                INITIALIZE_NODE( GEN, lod0TerrainIntersection )
                INITIALIZE_NODE( GEN, lod1TerrainIntersection )
                INITIALIZE_NODE( GEN, lod2TerrainIntersection )
                INITIALIZE_NODE( GEN, lod3TerrainIntersection )
                INITIALIZE_NODE( GEN, lod4TerrainIntersection )
                INITIALIZE_NODE( GEN, lod0ImplicitRepresentation )
                INITIALIZE_NODE( GEN, lod1ImplicitRepresentation )
                INITIALIZE_NODE( GEN, lod2ImplicitRepresentation )
                INITIALIZE_NODE( GEN, lod3ImplicitRepresentation )
                INITIALIZE_NODE( GEN, lod4ImplicitRepresentation )

                INITIALIZE_NODE( GEN, GenericOccupiedSpace )
                INITIALIZE_NODE( GEN, GenericUnoccupiedSpace )
                INITIALIZE_NODE( GEN, GenericLogicalSpace )
                INITIALIZE_NODE( GEN, GenericThematicSurface )

                INITIALIZE_NODE( GEN, area )
                INITIALIZE_NODE( GEN, spaceType )
                INITIALIZE_NODE( GEN, volume )

                // TEX
                // INITIALIZE_NODE( GML, TexturedSurface ) // Deprecated

                // GML
                INITIALIZE_NODE( GML, description )
                INITIALIZE_NODE( GML, identifier )
                INITIALIZE_NODE( GML, name )
                INITIALIZE_NODE( GML, descriptionReference )
                INITIALIZE_NODE( GML, metaDataProperty )
                INITIALIZE_NODE( GML, coordinates )
                INITIALIZE_NODE( GML, pos )
                INITIALIZE_NODE( GML, boundedBy )
                INITIALIZE_NODE( GML, Envelope )
                INITIALIZE_NODE( GML, lowerCorner )
                INITIALIZE_NODE( GML, upperCorner )
                INITIALIZE_NODE( GML, Solid )
                INITIALIZE_NODE( GML, surfaceMember )
                INITIALIZE_NODE( GML, baseSurface )
                INITIALIZE_NODE( GML, patches )
                INITIALIZE_NODE( GML, trianglePatches )
                INITIALIZE_NODE( GML, solidMember )
                INITIALIZE_NODE( GML, TriangulatedSurface )
                INITIALIZE_NODE( GML, Triangle )
                INITIALIZE_NODE( GML, Polygon )
                INITIALIZE_NODE( GML, Rectangle )
                INITIALIZE_NODE( GML, posList )
                INITIALIZE_NODE( GML, OrientableSurface )
                INITIALIZE_NODE( GML, LinearRing )
                INITIALIZE_NODE( GML, Shell )
                INITIALIZE_NODE( GML, PolyhedralSurface )
                INITIALIZE_NODE( GML, Surface )
                INITIALIZE_NODE( GML, PolygonPatch)
                INITIALIZE_NODE( GML, LineString)
                
                INITIALIZE_NODE( BLDG, lod0FootPrint )
                INITIALIZE_NODE( BLDG, lod0RoofEdge )
                INITIALIZE_NODE( BLDG, lod1Solid )
                INITIALIZE_NODE( BLDG, lod2Solid )
                INITIALIZE_NODE( BLDG, lod3Solid )
                INITIALIZE_NODE( BLDG, lod4Solid )
                INITIALIZE_NODE( BLDG, lod2Geometry )
                INITIALIZE_NODE( BLDG, lod3Geometry )
                INITIALIZE_NODE( BLDG, lod4Geometry )
                INITIALIZE_NODE( BLDG, lod1MultiCurve )
                INITIALIZE_NODE( BLDG, lod2MultiCurve )
                INITIALIZE_NODE( BLDG, lod3MultiCurve )
                INITIALIZE_NODE( BLDG, lod4MultiCurve )
                INITIALIZE_NODE( BLDG, lod1MultiSurface )
                INITIALIZE_NODE( BLDG, lod2MultiSurface )
                INITIALIZE_NODE( BLDG, lod3MultiSurface )
                INITIALIZE_NODE( BLDG, lod4MultiSurface )
                INITIALIZE_NODE( BLDG, lod1TerrainIntersection )
                INITIALIZE_NODE( BLDG, lod2TerrainIntersection )
                INITIALIZE_NODE( BLDG, lod3TerrainIntersection )
                INITIALIZE_NODE( BLDG, lod4TerrainIntersection )
                INITIALIZE_NODE( BLDG, buildingSubdivision )
                INITIALIZE_NODE( BLDG, Storey)

                INITIALIZE_NODE( GML, MultiPoint )
                INITIALIZE_NODE( GML, MultiCurve )
                INITIALIZE_NODE( GML, MultiSurface )
                INITIALIZE_NODE( GML, MultiSolid )
                INITIALIZE_NODE( GML, MultiGeometry )

                INITIALIZE_NODE( GML, CompositeCurve )
                INITIALIZE_NODE( GML, CompositeSurface )
                INITIALIZE_NODE( GML, CompositeSolid )

                INITIALIZE_NODE( GML, referencePoint )
                INITIALIZE_NODE( GML, Point )

                INITIALIZE_NODE( GML, interior )
                INITIALIZE_NODE( GML, exterior )

                // BLDG
                INITIALIZE_NODE( BLDG, Building )
                INITIALIZE_NODE( BLDG, BuildingPart )
                INITIALIZE_NODE( BLDG, Room )
                INITIALIZE_NODE( BLDG, Door )
                INITIALIZE_NODE( BLDG, Window )
                INITIALIZE_NODE( BLDG, BuildingInstallation )
                INITIALIZE_NODE( BLDG, measuredHeight )
                INITIALIZE_NODE( BLDG, class )
                INITIALIZE_NODE( BLDG, type ) // type does not exist in the BLDG namespace. Unclear what to make of it.
                INITIALIZE_NODE( BLDG, function )
                INITIALIZE_NODE( BLDG, usage )
                INITIALIZE_NODE( BLDG, yearOfConstruction )
                INITIALIZE_NODE( BLDG, yearOfDemolition )
                INITIALIZE_NODE( BLDG, storeysAboveGround )
                INITIALIZE_NODE( BLDG, storeysBelowGround )
                INITIALIZE_NODE( BLDG, storeyHeightsAboveGround )
                INITIALIZE_NODE( BLDG, storeyHeightsBelowGround )
                INITIALIZE_NODE( BLDG, boundedBy )
                INITIALIZE_NODE( BLDG, outerBuildingInstallation)
                INITIALIZE_NODE( BLDG, interiorBuildingInstallation)
                INITIALIZE_NODE( BLDG, interiorRoom)
                INITIALIZE_NODE( BLDG, interiorFurniture)
                INITIALIZE_NODE( BLDG, roomInstallation)
                INITIALIZE_NODE( BLDG, opening)
                INITIALIZE_NODE( BLDG, consistsOfBuildingPart )

                // CityFurniture
                INITIALIZE_NODE( FRN, class )
                INITIALIZE_NODE( FRN, function )
                INITIALIZE_NODE( FRN, CityFurniture )
                INITIALIZE_NODE( FRN, lod1Geometry )
                INITIALIZE_NODE( FRN, lod2Geometry )
                INITIALIZE_NODE( FRN, lod3Geometry )
                INITIALIZE_NODE( FRN, lod4Geometry )
                INITIALIZE_NODE( FRN, lod1TerrainIntersection )
                INITIALIZE_NODE( FRN, lod2TerrainIntersection )
                INITIALIZE_NODE( FRN, lod3TerrainIntersection )
                INITIALIZE_NODE( FRN, lod4TerrainIntersection )
                INITIALIZE_NODE( FRN, lod1ImplicitRepresentation )
                INITIALIZE_NODE( FRN, lod2ImplicitRepresentation )
                INITIALIZE_NODE( FRN, lod3ImplicitRepresentation )
                INITIALIZE_NODE( FRN, lod4ImplicitRepresentation )

                INITIALIZE_NODE( CON, fillingSurface )
                INITIALIZE_NODE( CON, WindowSurface )
                INITIALIZE_NODE( CON, DoorSurface )
                INITIALIZE_NODE( CON, OtherConstruction )

                // BoundarySurfaceType
                INITIALIZE_NODE( BLDG, WallSurface )
                INITIALIZE_NODE( BLDG, RoofSurface )
                INITIALIZE_NODE( BLDG, GroundSurface )
                INITIALIZE_NODE( BLDG, ClosureSurface )
                INITIALIZE_NODE( BLDG, FloorSurface )
                INITIALIZE_NODE( BLDG, InteriorWallSurface )
                INITIALIZE_NODE( BLDG, CeilingSurface )
                INITIALIZE_NODE( BLDG, OuterCeilingSurface )
                INITIALIZE_NODE( BLDG, OuterFloorSurface )

                INITIALIZE_NODE( CON, WallSurface )
                INITIALIZE_NODE( CON, RoofSurface )
                INITIALIZE_NODE( CON, GroundSurface )
                INITIALIZE_NODE( CON, FloorSurface )
                INITIALIZE_NODE( CON, InteriorWallSurface )
                INITIALIZE_NODE( CON, CeilingSurface )
                INITIALIZE_NODE( CON, OuterCeilingSurface )
                INITIALIZE_NODE( CON, OuterFloorSurface )
                INITIALIZE_NODE( BLDG, BuildingFurniture )
                INITIALIZE_NODE( BLDG, roofType)
                INITIALIZE_NODE( BLDG, IntBuildingInstallation)
                INITIALIZE_NODE( BLDG, BuildingConstructiveElement)
                INITIALIZE_NODE( BLDG, BuildingRoom)

                INITIALIZE_NODE( BLDG, CityFurniture )

                INITIALIZE_NODE( BLDG, address)

                // ADDRESS
                INITIALIZE_NODE( XAL, AddressDetails )
                INITIALIZE_NODE( XAL, Country )
                INITIALIZE_NODE( XAL, CountryName )
                INITIALIZE_NODE( XAL, CountryNameCode )
                INITIALIZE_NODE( XAL, AdministrativeArea )
                INITIALIZE_NODE( XAL, AdministrativeAreaName )
                INITIALIZE_NODE( XAL, Locality )
                INITIALIZE_NODE( XAL, LocalityName )
                INITIALIZE_NODE( XAL, PostalCode )
                INITIALIZE_NODE( XAL, PostalCodeNumber )
                INITIALIZE_NODE( XAL, Thoroughfare )
                INITIALIZE_NODE( XAL, ThoroughfareName )
                INITIALIZE_NODE( XAL, ThoroughfareNumber )

                // WTR
                INITIALIZE_NODE( WTR, WaterBody )
                INITIALIZE_NODE( WTR, WaterSurface )
                INITIALIZE_NODE( WTR, WaterGroundSurface )
                INITIALIZE_NODE( WTR, WaterClosureSurface )
                INITIALIZE_NODE( WTR, class )
                INITIALIZE_NODE( WTR, function )
                INITIALIZE_NODE( WTR, usage )
                INITIALIZE_NODE( WTR, waterLevel )
                INITIALIZE_NODE( WTR, lod0MultiCurve )
                INITIALIZE_NODE( WTR, lod0MultiSurface )
                INITIALIZE_NODE( WTR, lod1MultiCurve )
                INITIALIZE_NODE( WTR, lod1MultiSurface )
                INITIALIZE_NODE( WTR, lod1Solid )
                INITIALIZE_NODE( WTR, lod2Solid )
                INITIALIZE_NODE( WTR, lod3Solid )
                INITIALIZE_NODE( WTR, lod4Solid )
                INITIALIZE_NODE( WTR, lod2Surface )
                INITIALIZE_NODE( WTR, lod3Surface )
                INITIALIZE_NODE( WTR, lod4Surface )
                INITIALIZE_NODE( WTR, boundedBy )

                // VEG
                INITIALIZE_NODE( VEG, PlantCover )
                INITIALIZE_NODE( VEG, SolitaryVegetationObject )
                INITIALIZE_NODE( VEG, lod1ImplicitRepresentation )
                INITIALIZE_NODE( VEG, lod2ImplicitRepresentation )
                INITIALIZE_NODE( VEG, lod3ImplicitRepresentation )
                INITIALIZE_NODE( VEG, lod4ImplicitRepresentation )
                INITIALIZE_NODE( VEG, class )
                INITIALIZE_NODE( VEG, function )
                INITIALIZE_NODE( VEG, averageHeight )
                INITIALIZE_NODE( VEG, species )
                INITIALIZE_NODE( VEG, height )
                INITIALIZE_NODE( VEG, trunkDiameter )
                INITIALIZE_NODE( VEG, crownDiameter )

                INITIALIZE_NODE( VEG, lod0Geometry )
                INITIALIZE_NODE( VEG, lod1Geometry )
                INITIALIZE_NODE( VEG, lod2Geometry )
                INITIALIZE_NODE( VEG, lod3Geometry )
                INITIALIZE_NODE( VEG, lod4Geometry )
                INITIALIZE_NODE(VEG, lod1MultiSurface)
                INITIALIZE_NODE(VEG, lod2MultiSurface)
                INITIALIZE_NODE(VEG, lod3MultiSurface)
                INITIALIZE_NODE(VEG, lod4MultiSurface)

                // TRANS
                INITIALIZE_NODE( TRANS, TransportationComplex )
                INITIALIZE_NODE( TRANS, TrafficArea )
                INITIALIZE_NODE( TRANS, AuxiliaryTrafficArea )
                INITIALIZE_NODE( TRANS, TrafficSpace )
                INITIALIZE_NODE( TRANS, AuxiliaryTrafficSpace )
                INITIALIZE_NODE( TRANS, Track )
                INITIALIZE_NODE( TRANS, Road )
                INITIALIZE_NODE( TRANS, Railway )
                INITIALIZE_NODE( TRANS, Square )
                INITIALIZE_NODE( TRANS, Intersection )
                INITIALIZE_NODE( TRANS, Section )
                INITIALIZE_NODE( TRANS, Waterway )

                INITIALIZE_NODE( TRANS, usage )
                INITIALIZE_NODE( TRANS, function )
                INITIALIZE_NODE( TRANS, surfaceMaterial )
                INITIALIZE_NODE( TRANS, granularity )

                INITIALIZE_NODE( TRANS, lod0Network )
                INITIALIZE_NODE( TRANS, lod1MultiSurface )
                INITIALIZE_NODE( TRANS, lod2MultiSurface )
                INITIALIZE_NODE( TRANS, lod3MultiSurface )
                INITIALIZE_NODE( TRANS, lod4MultiSurface )

                // LUSE
                INITIALIZE_NODE( LUSE, LandUse )

                INITIALIZE_NODE( LUSE, class )
                INITIALIZE_NODE( LUSE, usage )
                INITIALIZE_NODE( LUSE, function )

                INITIALIZE_NODE( LUSE, lod1MultiSurface )
                INITIALIZE_NODE( LUSE, lod2MultiSurface )
                INITIALIZE_NODE( LUSE, lod3MultiSurface )
                INITIALIZE_NODE( LUSE, lod4MultiSurface )

                // DEM (Relief)
                INITIALIZE_NODE( DEM, ReliefFeature )
                INITIALIZE_NODE( DEM, TINRelief )
                INITIALIZE_NODE( DEM, RasterRelief )
                INITIALIZE_NODE( DEM, MassPointRelief )
                INITIALIZE_NODE( DEM, BreaklineRelief )
                INITIALIZE_NODE( DEM, lod )
                INITIALIZE_NODE( DEM, extent )
                INITIALIZE_NODE( DEM, reliefComponent )
                INITIALIZE_NODE( DEM, tin )
                INITIALIZE_NODE( DEM, grid )
                INITIALIZE_NODE( DEM, reliefPoints )
                INITIALIZE_NODE( DEM, ridgeOrValleyLines )
                INITIALIZE_NODE( DEM, breaklines )
                INITIALIZE_NODE( DEM, Elevation )

                // SUB
                INITIALIZE_NODE( SUB, Tunnel )
                INITIALIZE_NODE( SUB, RelativeToTerrain )

                // BRID
                INITIALIZE_NODE( BRID, Bridge )
                INITIALIZE_NODE( BRID, BridgeConstructionElement )
                INITIALIZE_NODE( BRID, BridgeInstallation )
                INITIALIZE_NODE( BRID, BridgePart )
                INITIALIZE_NODE( BRID, boundedBy )
                INITIALIZE_NODE( BRID, outerBridgeConstruction )
                INITIALIZE_NODE( BRID, outerBridgeInstallation )
                INITIALIZE_NODE( BRID, lod1Geometry )
                INITIALIZE_NODE( BRID, lod2Geometry )
                INITIALIZE_NODE( BRID, lod3Geometry )
                INITIALIZE_NODE( BRID, lod4Geometry )
                INITIALIZE_NODE( BRID, lod1MultiSurface)
                INITIALIZE_NODE( BRID, lod2MultiSurface)
                INITIALIZE_NODE( BRID, lod3MultiSurface)
                INITIALIZE_NODE( BRID, lod4MultiSurface)
                INITIALIZE_NODE( BRID, WallSurface)
                INITIALIZE_NODE( BRID, RoofSurface)
                INITIALIZE_NODE( BRID, GroundSurface)
                INITIALIZE_NODE( BRID, ClosureSurface)
                INITIALIZE_NODE( BRID, FloorSurface)
                INITIALIZE_NODE( BRID, InteriorWallSurface)
                INITIALIZE_NODE( BRID, CeilingSurface)
                INITIALIZE_NODE( BRID, OuterCeilingSurface)
                INITIALIZE_NODE( BRID, OuterFloorSurface)

                // APP
                INITIALIZE_NODE( APP, Appearance )
                INITIALIZE_NODE( APP, SimpleTexture )
                INITIALIZE_NODE( APP, ParameterizedTexture )
                INITIALIZE_NODE( APP, GeoreferencedTexture )
                INITIALIZE_NODE( APP, imageURI )
                INITIALIZE_NODE( APP, textureMap )
                INITIALIZE_NODE( APP, textureParameterization )
                INITIALIZE_NODE( APP, TextureAssociation )
                INITIALIZE_NODE( APP, target )
                INITIALIZE_NODE( APP, TexCoordList )
                INITIALIZE_NODE( APP, textureCoordinates )
                INITIALIZE_NODE( APP, ring )
                INITIALIZE_NODE( APP, textureType )
                INITIALIZE_NODE( APP, repeat )
                INITIALIZE_NODE( APP, wrapMode )
                INITIALIZE_NODE( APP, borderColor )
                INITIALIZE_NODE( APP, preferWorldFile )
                INITIALIZE_NODE( APP, referencePoint)
                INITIALIZE_NODE( APP, orientation)
                INITIALIZE_NODE( APP, isSmooth)

                INITIALIZE_NODE( APP, X3DMaterial )
                INITIALIZE_NODE( APP, Material )
                INITIALIZE_NODE( APP, appearanceMember )
                INITIALIZE_NODE( APP, SurfaceDataMember )
                INITIALIZE_NODE( APP, surfaceData )
                INITIALIZE_NODE( APP, shininess )
                INITIALIZE_NODE( APP, transparency )
                INITIALIZE_NODE( APP, specularColor )
                INITIALIZE_NODE( APP, diffuseColor )
                INITIALIZE_NODE( APP, emissiveColor )
                INITIALIZE_NODE( APP, ambientIntensity )
                INITIALIZE_NODE( APP, isFront )
                INITIALIZE_NODE( APP, theme )
                INITIALIZE_NODE( APP, mimeType )

                nodesInitialized = true;
            }
        }
    }

    const NodeType::XMLNode&NodeType::getXMLNodeFor(const std::string& name)
    {
        initializeNodeTypes();

        std::string lowerName = toLower(name);
        {
            auto it = nodeNameWithPrefixTypeMap.find(lowerName);
            if (it != nodeNameWithPrefixTypeMap.end()) {
                return *it->second;
            }
        }

        std::string nodeName = lowerName;

        size_t pos = nodeName.find_first_of( ":" );
        auto prefix = nodeName.substr(0, pos);
        if (pos != std::string::npos) {
            nodeName = nodeName.substr(pos + 1);
        } else {
            // node has no prefix... try with core prefix
            return getXMLNodeFor("core:" + name);
        }

        // It's technically possible to name the prefix anything
        // but adding handling for common alternatives used in test files
        // brg used instead of brid in Delft_3dfier_v2.gml
        if (prefix == "brg")
        {
            return getXMLNodeFor("brid:" + nodeName);
        }

        if (prefix == "tran")
        {
            return getXMLNodeFor("trans:" + nodeName);
        }

        auto it = nodeNameTypeMap.find(nodeName);

        if (it == nodeNameTypeMap.end()) {
            return InvalidNode;
        } else {
            return *it->second;
        }
    }

#define DEFINE_NODE( prefix, elementname ) NodeType::XMLNode NodeType::prefix ## _ ## elementname ## Node;

    // CORE
    DEFINE_NODE( CORE, CityModel )
    DEFINE_NODE( CORE, cityObjectMember )
    DEFINE_NODE( CORE, creationDate )
    DEFINE_NODE( CORE, terminationDate )
    DEFINE_NODE( CORE, generalizesTo)
    DEFINE_NODE( CORE, PointCloud)

    DEFINE_NODE( CORE, ExternalReference)
    DEFINE_NODE( CORE, informationSystem)
    DEFINE_NODE( CORE, externalObject)

    DEFINE_NODE( CORE, uri)
    DEFINE_NODE( CORE, name)

    DEFINE_NODE( CORE, Address )
    DEFINE_NODE( CORE, xalAddress )

    DEFINE_NODE( CORE, ImplicitGeometry )
    DEFINE_NODE( CORE, relativeGMLGeometry )
    DEFINE_NODE( CORE, transformationMatrix )
    DEFINE_NODE( CORE, referencePoint)
    DEFINE_NODE( CORE, mimeType)
    DEFINE_NODE( CORE, libraryObject)
    DEFINE_NODE( CORE, genericAttribute)

    DEFINE_NODE( CORE, lod0MultiSurface)
    DEFINE_NODE( CORE, lod1MultiSurface)
    DEFINE_NODE( CORE, lod2MultiSurface)
    DEFINE_NODE( CORE, lod3MultiSurface)

    DEFINE_NODE( CORE, lod1Solid)
    DEFINE_NODE( CORE, lod2Solid)
    DEFINE_NODE( CORE, lod3Solid)

    DEFINE_NODE( CORE, boundary)
    DEFINE_NODE( CORE, relativeGeometry)
    DEFINE_NODE( CORE, ClosureSurface)

    // GRP
    DEFINE_NODE( GRP, CityObjectGroup )
    DEFINE_NODE( GRP, groupMember )
    DEFINE_NODE( GRP, class )
    DEFINE_NODE( GRP, function )
    DEFINE_NODE( GRP, usage )
    DEFINE_NODE( GRP, parent )
    DEFINE_NODE( GRP, geometry )

    // GEN
    DEFINE_NODE( GEN, class )
    DEFINE_NODE( GEN, function )
    DEFINE_NODE( GEN, usage )
    DEFINE_NODE( GEN, GenericCityObject )
    DEFINE_NODE( GEN, stringAttribute )
    DEFINE_NODE( GEN, doubleAttribute )
    DEFINE_NODE( GEN, intAttribute )
    DEFINE_NODE( GEN, dateAttribute )
    DEFINE_NODE( GEN, uriAttribute )
    DEFINE_NODE( GEN, name )
    DEFINE_NODE( GEN, value )

    DEFINE_NODE( GEN, lod0Geometry )
    DEFINE_NODE( GEN, lod1Geometry )
    DEFINE_NODE( GEN, lod2Geometry )
    DEFINE_NODE( GEN, lod3Geometry )
    DEFINE_NODE( GEN, lod4Geometry )
    DEFINE_NODE( GEN, lod0TerrainIntersection )
    DEFINE_NODE( GEN, lod1TerrainIntersection )
    DEFINE_NODE( GEN, lod2TerrainIntersection )
    DEFINE_NODE( GEN, lod3TerrainIntersection )
    DEFINE_NODE( GEN, lod4TerrainIntersection )
    DEFINE_NODE( GEN, lod0ImplicitRepresentation )
    DEFINE_NODE( GEN, lod1ImplicitRepresentation )
    DEFINE_NODE( GEN, lod2ImplicitRepresentation )
    DEFINE_NODE( GEN, lod3ImplicitRepresentation )
    DEFINE_NODE( GEN, lod4ImplicitRepresentation )

    DEFINE_NODE( GEN, GenericOccupiedSpace )
    DEFINE_NODE( GEN, GenericUnoccupiedSpace )
    DEFINE_NODE( GEN, GenericLogicalSpace )
    DEFINE_NODE( GEN, GenericThematicSurface )

    DEFINE_NODE( GEN, Lod0MultiCurve )
    DEFINE_NODE( GEN, Lod2MultiCurve )
    DEFINE_NODE( GEN, Lod3MultiCurve )
    DEFINE_NODE( GEN, Lod0MultiSurface )
    DEFINE_NODE( GEN, Lod2MultiSurface )
    DEFINE_NODE( GEN, Lod3MultiSurface )

    DEFINE_NODE( GEN, area ) // These seem to be from the citygml 3.0 core. Unclear why they are in the GEN namespace here.
    DEFINE_NODE( GEN, spaceType )
    DEFINE_NODE( GEN, volume )

    // TEX
    // DEFINE_NODE( GML, TexturedSurface ) // Deprecated

    // GML
    DEFINE_NODE( GML, description )
    DEFINE_NODE( GML, identifier )
    DEFINE_NODE( GML, name )
    DEFINE_NODE( GML, descriptionReference )
    DEFINE_NODE( GML, metaDataProperty )
    DEFINE_NODE( GML, coordinates )
    DEFINE_NODE( GML, pos )
    DEFINE_NODE( GML, boundedBy )
    DEFINE_NODE( GML, Envelope )
    DEFINE_NODE( GML, lowerCorner )
    DEFINE_NODE( GML, upperCorner )
    DEFINE_NODE( GML, Solid )
    DEFINE_NODE( GML, surfaceMember )
    DEFINE_NODE( GML, baseSurface )
    DEFINE_NODE( GML, patches )
    DEFINE_NODE( GML, trianglePatches )
    DEFINE_NODE( GML, solidMember )
    DEFINE_NODE( GML, TriangulatedSurface )
    DEFINE_NODE( GML, Triangle )
    DEFINE_NODE( GML, Polygon )
    DEFINE_NODE( GML, Rectangle )
    DEFINE_NODE( GML, posList )
    DEFINE_NODE( GML, OrientableSurface )
    DEFINE_NODE( GML, LinearRing )

    DEFINE_NODE( BLDG, lod0FootPrint )
    DEFINE_NODE( BLDG, lod0RoofEdge )
    DEFINE_NODE( BLDG, lod1Solid )
    DEFINE_NODE( BLDG, lod2Solid )
    DEFINE_NODE( BLDG, lod3Solid )
    DEFINE_NODE( BLDG, lod4Solid )
    DEFINE_NODE( BLDG, lod2Geometry )
    DEFINE_NODE( BLDG, lod3Geometry )
    DEFINE_NODE( BLDG, lod4Geometry )
    DEFINE_NODE( BLDG, lod1MultiCurve )
    DEFINE_NODE( BLDG, lod2MultiCurve )
    DEFINE_NODE( BLDG, lod3MultiCurve )
    DEFINE_NODE( BLDG, lod4MultiCurve )
    DEFINE_NODE( BLDG, lod1MultiSurface )
    DEFINE_NODE( BLDG, lod2MultiSurface )
    DEFINE_NODE( BLDG, lod3MultiSurface )
    DEFINE_NODE( BLDG, lod4MultiSurface )
    DEFINE_NODE( BLDG, lod1TerrainIntersection )
    DEFINE_NODE( BLDG, lod2TerrainIntersection )
    DEFINE_NODE( BLDG, lod3TerrainIntersection )
    DEFINE_NODE( BLDG, lod4TerrainIntersection )
    DEFINE_NODE( BLDG, buildingSubdivision )
    DEFINE_NODE( BLDG, Storey )

    DEFINE_NODE( GML, MultiPoint )
    DEFINE_NODE( GML, MultiCurve )
    DEFINE_NODE( GML, MultiSurface )
    DEFINE_NODE( GML, MultiSolid )
    DEFINE_NODE( GML, MultiGeometry )

    DEFINE_NODE( GML, CompositeCurve )
    DEFINE_NODE( GML, CompositeSurface )
    DEFINE_NODE( GML, CompositeSolid )

    DEFINE_NODE( GML, referencePoint )
    DEFINE_NODE( GML, Point )

    DEFINE_NODE( GML, interior )
    DEFINE_NODE( GML, exterior )

    DEFINE_NODE( GML, Shell )
    DEFINE_NODE( GML, PolyhedralSurface )
    DEFINE_NODE( GML, Surface )
    DEFINE_NODE( GML, PolygonPatch )

    DEFINE_NODE( GML, LineString )

    DEFINE_NODE( GML, RectifiedGridCoverage )

    // BLDG
    DEFINE_NODE( BLDG, Building )
    DEFINE_NODE( BLDG, BuildingPart )
    DEFINE_NODE( BLDG, Room )
    DEFINE_NODE( BLDG, Door )
    DEFINE_NODE( BLDG, Window )
    DEFINE_NODE( BLDG, BuildingInstallation )
    DEFINE_NODE( BLDG, measuredHeight )
    DEFINE_NODE( BLDG, class )
    DEFINE_NODE( BLDG, type ) // Type does not exist in the BLDG namespace. Unclear what to make of it.
    DEFINE_NODE( BLDG, function )
    DEFINE_NODE( BLDG, usage )
    DEFINE_NODE( BLDG, yearOfConstruction )
    DEFINE_NODE( BLDG, yearOfDemolition )
    DEFINE_NODE( BLDG, storeysAboveGround )
    DEFINE_NODE( BLDG, storeysBelowGround )
    DEFINE_NODE( BLDG, storeyHeightsAboveGround )
    DEFINE_NODE( BLDG, storeyHeightsBelowGround )
    DEFINE_NODE( BLDG, boundedBy )
    DEFINE_NODE( BLDG, outerBuildingInstallation )
    DEFINE_NODE( BLDG, interiorBuildingInstallation )
    DEFINE_NODE( BLDG, interiorRoom )
    DEFINE_NODE( BLDG, interiorFurniture )
    DEFINE_NODE( BLDG, roomInstallation )
    DEFINE_NODE( BLDG, opening)
    DEFINE_NODE( BLDG, consistsOfBuildingPart )

    // CityFurniture
    DEFINE_NODE( FRN, class )
    DEFINE_NODE( FRN, function )
    DEFINE_NODE( FRN, CityFurniture )
    DEFINE_NODE( FRN, lod1Geometry )
    DEFINE_NODE( FRN, lod2Geometry )
    DEFINE_NODE( FRN, lod3Geometry )
    DEFINE_NODE( FRN, lod4Geometry )
    DEFINE_NODE( FRN, lod1TerrainIntersection )
    DEFINE_NODE( FRN, lod2TerrainIntersection )
    DEFINE_NODE( FRN, lod3TerrainIntersection )
    DEFINE_NODE( FRN, lod4TerrainIntersection )
    DEFINE_NODE( FRN, lod1ImplicitRepresentation )
    DEFINE_NODE( FRN, lod2ImplicitRepresentation )
    DEFINE_NODE( FRN, lod3ImplicitRepresentation )
    DEFINE_NODE( FRN, lod4ImplicitRepresentation )

    DEFINE_NODE( CON, fillingSurface )
    DEFINE_NODE( CON, WindowSurface )
    DEFINE_NODE( CON, DoorSurface )
    DEFINE_NODE( CON, OtherConstruction )

    // BoundarySurfaceType
    DEFINE_NODE( BLDG, WallSurface )
    DEFINE_NODE( BLDG, RoofSurface )
    DEFINE_NODE( BLDG, GroundSurface )
    DEFINE_NODE( BLDG, ClosureSurface )
    DEFINE_NODE( BLDG, FloorSurface )
    DEFINE_NODE( BLDG, InteriorWallSurface )
    DEFINE_NODE( BLDG, CeilingSurface )
    DEFINE_NODE( BLDG, OuterCeilingSurface )
    DEFINE_NODE( BLDG, OuterFloorSurface )

    DEFINE_NODE( CON, WallSurface )
    DEFINE_NODE( CON, RoofSurface )
    DEFINE_NODE( CON, GroundSurface )
    DEFINE_NODE( CON, FloorSurface )
    DEFINE_NODE( CON, InteriorWallSurface )
    DEFINE_NODE( CON, CeilingSurface )
    DEFINE_NODE( CON, OuterCeilingSurface )
    DEFINE_NODE( CON, OuterFloorSurface )

    DEFINE_NODE( BLDG, BuildingFurniture )
    DEFINE_NODE( BLDG, roofType)
    DEFINE_NODE( BLDG, IntBuildingInstallation)
    DEFINE_NODE( BLDG, BuildingConstructiveElement)
    DEFINE_NODE( BLDG, BuildingRoom)

    DEFINE_NODE( BLDG, CityFurniture )

    DEFINE_NODE( BLDG, address)

    // ADDRESS
    DEFINE_NODE( XAL, AddressDetails )
    DEFINE_NODE( XAL, Country )
    DEFINE_NODE( XAL, CountryName )
    DEFINE_NODE( XAL, CountryNameCode )
    DEFINE_NODE( XAL, AdministrativeArea )
    DEFINE_NODE( XAL, AdministrativeAreaName )
    DEFINE_NODE( XAL, Locality )
    DEFINE_NODE( XAL, LocalityName )
    DEFINE_NODE( XAL, PostalCode )
    DEFINE_NODE( XAL, PostalCodeNumber )
    DEFINE_NODE( XAL, Thoroughfare )
    DEFINE_NODE( XAL, ThoroughfareName )
    DEFINE_NODE( XAL, ThoroughfareNumber )

    // WTR
    DEFINE_NODE( WTR, WaterBody )
    DEFINE_NODE( WTR, WaterSurface )
    DEFINE_NODE( WTR, WaterGroundSurface )
    DEFINE_NODE( WTR, WaterClosureSurface )
    DEFINE_NODE( WTR, class )
    DEFINE_NODE( WTR, function )
    DEFINE_NODE( WTR, usage )
    DEFINE_NODE( WTR, waterLevel )
    DEFINE_NODE( WTR, lod0MultiCurve)
    DEFINE_NODE( WTR, lod1MultiCurve )
    DEFINE_NODE( WTR, lod0MultiSurface)
    DEFINE_NODE( WTR, lod1MultiSurface )
    DEFINE_NODE( WTR, lod1Solid )
    DEFINE_NODE( WTR, lod2Solid )
    DEFINE_NODE( WTR, lod3Solid )
    DEFINE_NODE( WTR, lod4Solid )
    DEFINE_NODE( WTR, lod2Surface )
    DEFINE_NODE( WTR, lod3Surface )
    DEFINE_NODE( WTR, lod4Surface )
    DEFINE_NODE( WTR, boundedBy )

    // VEG
    DEFINE_NODE( VEG, PlantCover )
    DEFINE_NODE( VEG, SolitaryVegetationObject )
    DEFINE_NODE( VEG, lod1ImplicitRepresentation )
    DEFINE_NODE( VEG, lod2ImplicitRepresentation )
    DEFINE_NODE( VEG, lod3ImplicitRepresentation )
    DEFINE_NODE( VEG, lod4ImplicitRepresentation )

    DEFINE_NODE( VEG, class )
    DEFINE_NODE( VEG, function )
    DEFINE_NODE( VEG, averageHeight )
    DEFINE_NODE( VEG, species )
    DEFINE_NODE( VEG, height )
    DEFINE_NODE( VEG, trunkDiameter )
    DEFINE_NODE( VEG, crownDiameter )

    DEFINE_NODE( VEG, lod0Geometry )
    DEFINE_NODE( VEG, lod1Geometry )
    DEFINE_NODE( VEG, lod2Geometry )
    DEFINE_NODE( VEG, lod3Geometry )
    DEFINE_NODE( VEG, lod4Geometry )
    DEFINE_NODE(VEG, lod1MultiSurface)
    DEFINE_NODE(VEG, lod2MultiSurface)
    DEFINE_NODE(VEG, lod3MultiSurface)
    DEFINE_NODE(VEG, lod4MultiSurface)

    // TRANS
    DEFINE_NODE( TRANS, TransportationComplex )
    DEFINE_NODE( TRANS, TrafficArea )
    DEFINE_NODE( TRANS, AuxiliaryTrafficArea )
    DEFINE_NODE( TRANS, TrafficSpace )
    DEFINE_NODE( TRANS, AuxiliaryTrafficSpace )
    DEFINE_NODE( TRANS, Track )
    DEFINE_NODE( TRANS, Road )
    DEFINE_NODE( TRANS, Railway )
    DEFINE_NODE( TRANS, Square )
    DEFINE_NODE( TRANS, Intersection )
    DEFINE_NODE( TRANS, Section )
    DEFINE_NODE( TRANS, Waterway )

    DEFINE_NODE( TRANS, usage )
    DEFINE_NODE( TRANS, function )
    DEFINE_NODE( TRANS, surfaceMaterial )
    DEFINE_NODE( TRANS, granularity )

    DEFINE_NODE( TRANS, lod0Network )
    DEFINE_NODE( TRANS, lod1MultiSurface )
    DEFINE_NODE( TRANS, lod2MultiSurface )
    DEFINE_NODE( TRANS, lod3MultiSurface )
    DEFINE_NODE( TRANS, lod4MultiSurface )

    // LUSE
    DEFINE_NODE( LUSE, LandUse )

    DEFINE_NODE( LUSE, class )
    DEFINE_NODE( LUSE, usage )
    DEFINE_NODE( LUSE, function )

    DEFINE_NODE( LUSE, lod1MultiSurface )
    DEFINE_NODE( LUSE, lod2MultiSurface )
    DEFINE_NODE( LUSE, lod3MultiSurface )
    DEFINE_NODE( LUSE, lod4MultiSurface )

    // DEM (Relief)
    DEFINE_NODE( DEM, ReliefFeature )
    DEFINE_NODE( DEM, TINRelief )
    DEFINE_NODE( DEM, RasterRelief )
    DEFINE_NODE( DEM, MassPointRelief )
    DEFINE_NODE( DEM, BreaklineRelief )
    DEFINE_NODE( DEM, lod )
    DEFINE_NODE( DEM, extent )
    DEFINE_NODE( DEM, reliefComponent )
    DEFINE_NODE( DEM, tin )
    DEFINE_NODE( DEM, grid )
    DEFINE_NODE( DEM, reliefPoints )
    DEFINE_NODE( DEM, ridgeOrValleyLines )
    DEFINE_NODE( DEM, breaklines )
    DEFINE_NODE( DEM, Elevation )

    // SUB
    DEFINE_NODE( SUB, Tunnel )
    DEFINE_NODE( SUB, RelativeToTerrain )

    // BRID
    DEFINE_NODE( BRID, Bridge )
    DEFINE_NODE( BRID, BridgeConstructionElement )
    DEFINE_NODE( BRID, BridgeInstallation )
    DEFINE_NODE( BRID, BridgePart )
    DEFINE_NODE( BRID, outerBridgeConstruction )
    DEFINE_NODE( BRID, outerBridgeInstallation )
    DEFINE_NODE( BRID, lod1Geometry )
    DEFINE_NODE( BRID, lod2Geometry )
    DEFINE_NODE( BRID, lod3Geometry ) 
    DEFINE_NODE( BRID, lod4Geometry )
    DEFINE_NODE( BRID, lod1MultiSurface)
    DEFINE_NODE( BRID, lod2MultiSurface)
    DEFINE_NODE( BRID, lod3MultiSurface)
    DEFINE_NODE( BRID, lod4MultiSurface)
    DEFINE_NODE( BRID, boundedBy )
    DEFINE_NODE( BRID, WallSurface)
    DEFINE_NODE( BRID, RoofSurface)
    DEFINE_NODE( BRID, GroundSurface)
    DEFINE_NODE( BRID, ClosureSurface)
    DEFINE_NODE( BRID, FloorSurface)
    DEFINE_NODE( BRID, InteriorWallSurface)
    DEFINE_NODE( BRID, CeilingSurface)
    DEFINE_NODE( BRID, OuterCeilingSurface)
    DEFINE_NODE( BRID, OuterFloorSurface)

    // APP
    DEFINE_NODE( APP, Appearance )
    DEFINE_NODE( APP, SimpleTexture )
    DEFINE_NODE( APP, ParameterizedTexture )
    DEFINE_NODE( APP, GeoreferencedTexture )
    DEFINE_NODE( APP, imageURI )
    DEFINE_NODE( APP, textureMap )
    DEFINE_NODE( APP, textureParameterization )
    DEFINE_NODE( APP, TextureAssociation )
    DEFINE_NODE( APP, target )
    DEFINE_NODE( APP, TexCoordList )
    DEFINE_NODE( APP, textureCoordinates )
    DEFINE_NODE( APP, ring )
    DEFINE_NODE( APP, textureType )
    DEFINE_NODE( APP, repeat )
    DEFINE_NODE( APP, wrapMode )
    DEFINE_NODE( APP, borderColor )
    DEFINE_NODE( APP, preferWorldFile )
    DEFINE_NODE( APP, referencePoint)
    DEFINE_NODE( APP, orientation)
    DEFINE_NODE( APP, isSmooth)

    DEFINE_NODE( APP, X3DMaterial )
    DEFINE_NODE( APP, Material )
    DEFINE_NODE( APP, appearanceMember )
    DEFINE_NODE( APP, SurfaceDataMember )
    DEFINE_NODE( APP, surfaceData )
    DEFINE_NODE( APP, shininess )
    DEFINE_NODE( APP, transparency )
    DEFINE_NODE( APP, specularColor )
    DEFINE_NODE( APP, diffuseColor )
    DEFINE_NODE( APP, emissiveColor )
    DEFINE_NODE( APP, ambientIntensity )
    DEFINE_NODE( APP, isFront )
    DEFINE_NODE( APP, theme )
    DEFINE_NODE( APP, mimeType )
}
