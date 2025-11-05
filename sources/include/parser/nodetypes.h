#pragma once

#include <string>
#include <ostream>
#include <mutex>
#include <unordered_map>

namespace citygml {


    class NodeType {
    public:

        class XMLNode {
        public:
            XMLNode();
            XMLNode(std::string prefix, std::string name);

            const std::string name() const;
            const std::string& prefix() const;
            const std::string& baseName() const;
            int typeID() const;

            bool operator==(const XMLNode& other) const;

            bool operator!=(const XMLNode& other) const;

            bool valid() const;
        private:
            std::string m_name;
            std::string m_prefix;
            int m_typeID;
        };

        static const XMLNode& getXMLNodeFor(const std::string& name);

        static const XMLNode InvalidNode;

        #define NODETYPE( prefix, elementName ) static XMLNode prefix ## _ ## elementName ## Node;

        // CORE
        NODETYPE( CORE, CityModel )
        NODETYPE( CORE, cityObjectMember )
        NODETYPE( CORE, creationDate )
        NODETYPE( CORE, terminationDate )
        NODETYPE( CORE, generalizesTo)
        NODETYPE( CORE, PointCloud)

        NODETYPE( CORE, ExternalReference)
        NODETYPE( CORE, informationSystem)
        NODETYPE( CORE, externalObject)

        NODETYPE( CORE, uri)
        NODETYPE( CORE, name)

        NODETYPE( CORE, Address )
        NODETYPE( CORE, xalAddress )

        NODETYPE( CORE, ImplicitGeometry )
        NODETYPE( CORE, relativeGMLGeometry )
        NODETYPE( CORE, transformationMatrix )
        NODETYPE( CORE, referencePoint)
        NODETYPE( CORE, mimeType)
        NODETYPE( CORE, libraryObject)
        NODETYPE( CORE, genericAttribute)

        NODETYPE( CORE, lod0MultiSurface)
        NODETYPE( CORE, lod1MultiSurface)
        NODETYPE( CORE, lod2MultiSurface)
        NODETYPE( CORE, lod3MultiSurface)

        NODETYPE( CORE, lod1Solid )
        NODETYPE( CORE, lod2Solid )
        NODETYPE( CORE, lod3Solid )

        NODETYPE( CORE, boundary )
        NODETYPE( CORE, relativeGeometry )
        NODETYPE( CORE, ClosureSurface)

        // GRP
        NODETYPE( GRP, CityObjectGroup )
        NODETYPE( GRP, groupMember )
        NODETYPE( GRP, class )
        NODETYPE( GRP, function )
        NODETYPE( GRP, usage )
        NODETYPE( GRP, parent )
        NODETYPE( GRP, geometry )

        // GEN
        NODETYPE( GEN, class )
        NODETYPE( GEN, function )
        NODETYPE( GEN, usage )
        NODETYPE( GEN, GenericCityObject )
        NODETYPE( GEN, stringAttribute )
        NODETYPE( GEN, doubleAttribute )
        NODETYPE( GEN, intAttribute )
        NODETYPE( GEN, dateAttribute )
        NODETYPE( GEN, uriAttribute )
        NODETYPE( GEN, name )
        NODETYPE( GEN, value )

        NODETYPE( GEN, lod0Geometry )
        NODETYPE( GEN, lod1Geometry )
        NODETYPE( GEN, lod2Geometry )
        NODETYPE( GEN, lod3Geometry )
        NODETYPE( GEN, lod4Geometry )
        NODETYPE( GEN, lod0TerrainIntersection )
        NODETYPE( GEN, lod1TerrainIntersection )
        NODETYPE( GEN, lod2TerrainIntersection )
        NODETYPE( GEN, lod3TerrainIntersection )
        NODETYPE( GEN, lod4TerrainIntersection )
        NODETYPE( GEN, lod0ImplicitRepresentation )
        NODETYPE( GEN, lod1ImplicitRepresentation )
        NODETYPE( GEN, lod2ImplicitRepresentation )
        NODETYPE( GEN, lod3ImplicitRepresentation )
        NODETYPE( GEN, lod4ImplicitRepresentation )

        NODETYPE( GEN, GenericOccupiedSpace )
        NODETYPE( GEN, GenericUnoccupiedSpace )
        NODETYPE( GEN, GenericLogicalSpace )
        NODETYPE( GEN, GenericThematicSurface )

        NODETYPE( GEN, Lod0MultiCurve )
        NODETYPE( GEN, Lod2MultiCurve )
        NODETYPE( GEN, Lod3MultiCurve )
        NODETYPE( GEN, Lod0MultiSurface )
        NODETYPE( GEN, Lod2MultiSurface )
        NODETYPE( GEN, Lod3MultiSurface )

        NODETYPE( GEN, area )
        NODETYPE( GEN, spaceType )
        NODETYPE( GEN, volume )

        // TEX
        // NODETYPE( GML, TexturedSurface ) // Deprecated

        // GML
        NODETYPE( GML, description )
        NODETYPE( GML, identifier )
        NODETYPE( GML, name )
        NODETYPE( GML, descriptionReference )
        NODETYPE( GML, metaDataProperty )
        NODETYPE( GML, coordinates )
        NODETYPE( GML, pos )
        NODETYPE( GML, boundedBy )
        NODETYPE( GML, Envelope )
        NODETYPE( GML, lowerCorner )
        NODETYPE( GML, upperCorner )
        NODETYPE( GML, Solid )
        NODETYPE( GML, surfaceMember )
        NODETYPE( GML, baseSurface )
        NODETYPE( GML, patches )
        NODETYPE( GML, trianglePatches )
        NODETYPE( GML, solidMember )
        NODETYPE( GML, TriangulatedSurface )
        NODETYPE( GML, Triangle )
        NODETYPE( GML, Polygon )
        NODETYPE( GML, Rectangle )
        NODETYPE( GML, posList )
        NODETYPE( GML, OrientableSurface )
        NODETYPE( GML, LinearRing )

        NODETYPE( GML, MultiPoint )
        NODETYPE( GML, MultiCurve )
        NODETYPE( GML, MultiSurface )
        NODETYPE( GML, MultiSolid )
        NODETYPE( GML, MultiGeometry )

        NODETYPE( GML, CompositeCurve )
        NODETYPE( GML, CompositeSurface )
        NODETYPE( GML, CompositeSolid )

        NODETYPE( GML, referencePoint ) // This is theoretically citygml core. But let's leave it here to be lenient.
        NODETYPE( GML, Point )

        NODETYPE( GML, interior )
        NODETYPE( GML, exterior )

        NODETYPE( GML, Shell )
        NODETYPE( GML, PolyhedralSurface )
        NODETYPE( GML, Surface )
        NODETYPE( GML, PolygonPatch )

        NODETYPE( GML, LineString )
        
        NODETYPE( GML, RectifiedGridCoverage )

        // BLDG
        NODETYPE( BLDG, Building )
        NODETYPE( BLDG, BuildingPart )
        NODETYPE( BLDG, Room )
        NODETYPE( BLDG, Door )
        NODETYPE( BLDG, Window )
        NODETYPE( BLDG, BuildingInstallation )
        NODETYPE( BLDG, measuredHeight )
        NODETYPE( BLDG, class )
        NODETYPE( BLDG, type ) // type does not exist in the BLDG namespace. Unclear what to make of it.
        NODETYPE( BLDG, function )
        NODETYPE( BLDG, usage )
        NODETYPE( BLDG, yearOfConstruction )
        NODETYPE( BLDG, yearOfDemolition )
        NODETYPE( BLDG, storeysAboveGround )
        NODETYPE( BLDG, storeysBelowGround )
        NODETYPE( BLDG, storeyHeightsAboveGround )
        NODETYPE( BLDG, storeyHeightsBelowGround )
        NODETYPE( BLDG, boundedBy )
        NODETYPE( BLDG, outerBuildingInstallation)
        NODETYPE( BLDG, interiorBuildingInstallation)
        NODETYPE( BLDG, interiorRoom)
        NODETYPE( BLDG, interiorFurniture)
        NODETYPE( BLDG, roomInstallation)
        NODETYPE( BLDG, opening)
        NODETYPE( BLDG, roofType)
        NODETYPE( BLDG, buildingSubdivision)
        NODETYPE( BLDG, Storey)
        
        NODETYPE( BLDG, lod0FootPrint )
        NODETYPE( BLDG, lod0RoofEdge )
        NODETYPE( BLDG, lod1Solid )
        NODETYPE( BLDG, lod2Solid )
        NODETYPE( BLDG, lod3Solid )
        NODETYPE( BLDG, lod4Solid )
        NODETYPE( BLDG, lod2Geometry )
        NODETYPE( BLDG, lod3Geometry )
        NODETYPE( BLDG, lod4Geometry )
        NODETYPE( BLDG, lod1MultiCurve ) // Theoretically MultiCurves only exist for LoD 2, 3, and 4 but let's be lenient
        NODETYPE( BLDG, lod2MultiCurve )
        NODETYPE( BLDG, lod3MultiCurve )
        NODETYPE( BLDG, lod4MultiCurve )
        NODETYPE( BLDG, lod1MultiSurface )
        NODETYPE( BLDG, lod2MultiSurface )
        NODETYPE( BLDG, lod3MultiSurface )
        NODETYPE( BLDG, lod4MultiSurface )
        NODETYPE( BLDG, lod1TerrainIntersection )
        NODETYPE( BLDG, lod2TerrainIntersection )
        NODETYPE( BLDG, lod3TerrainIntersection )
        NODETYPE( BLDG, lod4TerrainIntersection )
        NODETYPE( BLDG, consistsOfBuildingPart )

        NODETYPE( CON, fillingSurface )
        NODETYPE( CON, WindowSurface )
        NODETYPE( CON, DoorSurface )
        NODETYPE( CON, OtherConstruction )

        // BoundarySurfaceType
        NODETYPE( BLDG, WallSurface )
        NODETYPE( BLDG, RoofSurface )
        NODETYPE( BLDG, GroundSurface )
        NODETYPE( BLDG, ClosureSurface )
        NODETYPE( BLDG, FloorSurface )
        NODETYPE( BLDG, InteriorWallSurface )
        NODETYPE( BLDG, CeilingSurface )
        NODETYPE( BLDG, OuterCeilingSurface )
        NODETYPE( BLDG, OuterFloorSurface )

        NODETYPE( CON, WallSurface )
        NODETYPE( CON, RoofSurface )
        NODETYPE( CON, GroundSurface )
        NODETYPE( CON, FloorSurface )
        NODETYPE( CON, InteriorWallSurface )
        NODETYPE( CON, CeilingSurface )
        NODETYPE( CON, OuterCeilingSurface )
        NODETYPE( CON, OuterFloorSurface )
        NODETYPE( BLDG, BuildingFurniture )
        NODETYPE( BLDG, IntBuildingInstallation)
        NODETYPE( BLDG, BuildingConstructiveElement) // Can occur both upper and lowercase in different situations
        NODETYPE( BLDG, BuildingRoom) // Can occur both upper and lowercase in different situations
            
        NODETYPE( BLDG, CityFurniture ) // This belongs in the FRN namespace, but let's be lenient.
        NODETYPE( BLDG, address)

        // CityFurniture
        NODETYPE( FRN, class )
        NODETYPE( FRN, function )
        NODETYPE( FRN, CityFurniture )
        NODETYPE( FRN, lod1Geometry )
        NODETYPE( FRN, lod2Geometry )
        NODETYPE( FRN, lod3Geometry )
        NODETYPE( FRN, lod4Geometry )
        NODETYPE( FRN, lod1TerrainIntersection )
        NODETYPE( FRN, lod2TerrainIntersection )
        NODETYPE( FRN, lod3TerrainIntersection )
        NODETYPE( FRN, lod4TerrainIntersection )
        NODETYPE( FRN, lod1ImplicitRepresentation )
        NODETYPE( FRN, lod2ImplicitRepresentation )
        NODETYPE( FRN, lod3ImplicitRepresentation )
        NODETYPE( FRN, lod4ImplicitRepresentation )

        // ADDRESS
        NODETYPE( XAL, AddressDetails )
        NODETYPE( XAL, Country )
        NODETYPE( XAL, CountryName )
        NODETYPE( XAL, CountryNameCode )
        NODETYPE( XAL, AdministrativeArea )
        NODETYPE( XAL, AdministrativeAreaName )
        NODETYPE( XAL, Locality )
        NODETYPE( XAL, LocalityName )
        NODETYPE( XAL, PostalCode )
        NODETYPE( XAL, PostalCodeNumber )
        NODETYPE( XAL, Thoroughfare )
        NODETYPE( XAL, ThoroughfareName )
        NODETYPE( XAL, ThoroughfareNumber )

        // WTR
        NODETYPE( WTR, WaterBody )
        NODETYPE( WTR, WaterSurface )
        NODETYPE( WTR, WaterGroundSurface )
        NODETYPE( WTR, WaterClosureSurface )
        NODETYPE( WTR, class )
        NODETYPE( WTR, function )
        NODETYPE( WTR, usage )
        NODETYPE( WTR, waterLevel )
        NODETYPE( WTR, lod0MultiCurve )
        NODETYPE( WTR, lod0MultiSurface )
        NODETYPE( WTR, lod1MultiCurve )
        NODETYPE( WTR, lod1MultiSurface )
        NODETYPE( WTR, lod1Solid )
        NODETYPE( WTR, lod2Solid )
        NODETYPE( WTR, lod3Solid )
        NODETYPE( WTR, lod4Solid )
        NODETYPE( WTR, lod2Surface )
        NODETYPE( WTR, lod3Surface )
        NODETYPE( WTR, lod4Surface )
        NODETYPE( WTR, boundedBy )

        // VEG
        NODETYPE( VEG, PlantCover )
        NODETYPE( VEG, SolitaryVegetationObject )
        NODETYPE( VEG, lod1ImplicitRepresentation )
        NODETYPE( VEG, lod2ImplicitRepresentation )
        NODETYPE( VEG, lod3ImplicitRepresentation )
        NODETYPE( VEG, lod4ImplicitRepresentation )

        NODETYPE( VEG, class )
        NODETYPE( VEG, function )
        NODETYPE( VEG, averageHeight )
        NODETYPE( VEG, species )
        NODETYPE( VEG, height )
        NODETYPE( VEG, trunkDiameter )
        NODETYPE( VEG, crownDiameter )
        NODETYPE( VEG, lod0Geometry )
        NODETYPE( VEG, lod1Geometry )
        NODETYPE( VEG, lod2Geometry )
        NODETYPE( VEG, lod3Geometry )
        NODETYPE( VEG, lod4Geometry )
        NODETYPE( VEG, lod1MultiSurface )
        NODETYPE( VEG, lod2MultiSurface )
        NODETYPE( VEG, lod3MultiSurface )
        NODETYPE( VEG, lod4MultiSurface )

        // TRANS
        NODETYPE( TRANS, TransportationComplex )
        NODETYPE( TRANS, TrafficArea )
        NODETYPE( TRANS, AuxiliaryTrafficArea )
        NODETYPE( TRANS, TrafficSpace ) // Exists in both upper and lowercase forms in different situations
        NODETYPE( TRANS, AuxiliaryTrafficSpace )
        NODETYPE( TRANS, Track )
        NODETYPE( TRANS, Road )
        NODETYPE( TRANS, Railway )
        NODETYPE( TRANS, Square )
        NODETYPE( TRANS, Intersection ) // Exists in both upper and lowercase forms in different situations
        NODETYPE( TRANS, Section ) // Exists in both upper and lowercase forms in different situations
        NODETYPE( TRANS, Waterway )

        NODETYPE( TRANS, usage )
        NODETYPE( TRANS, function )
        NODETYPE( TRANS, surfaceMaterial )
        NODETYPE( TRANS, granularity )

        NODETYPE( TRANS, lod0Network )
        NODETYPE( TRANS, lod1MultiSurface )
        NODETYPE( TRANS, lod2MultiSurface )
        NODETYPE( TRANS, lod3MultiSurface )
        NODETYPE( TRANS, lod4MultiSurface )


        // LUSE
        NODETYPE( LUSE, LandUse )

        NODETYPE( LUSE, class )
        NODETYPE( LUSE, usage )
        NODETYPE( LUSE, function )

        NODETYPE( LUSE, lod1MultiSurface )
        NODETYPE( LUSE, lod2MultiSurface )
        NODETYPE( LUSE, lod3MultiSurface )
        NODETYPE( LUSE, lod4MultiSurface )

        // DEM (Relief)
        NODETYPE( DEM, ReliefFeature )
        NODETYPE( DEM, TINRelief )
        NODETYPE( DEM, RasterRelief )
        NODETYPE( DEM, MassPointRelief )
        NODETYPE( DEM, BreaklineRelief )
        NODETYPE( DEM, lod )
        NODETYPE( DEM, extent )
        NODETYPE( DEM, reliefComponent )
        NODETYPE( DEM, tin )
        NODETYPE( DEM, grid )
        NODETYPE( DEM, reliefPoints )
        NODETYPE( DEM, ridgeOrValleyLines )
        NODETYPE( DEM, breaklines )
        NODETYPE( DEM, Elevation )

        // SUB
        NODETYPE( SUB, Tunnel )
        NODETYPE( SUB, RelativeToTerrain ) // Exists in both upper and lowercase forms in different situations

        // BRID
        NODETYPE( BRID, Bridge )
        NODETYPE( BRID, BridgeConstructionElement )
        NODETYPE( BRID, BridgeInstallation )
        NODETYPE( BRID, BridgePart )
        NODETYPE( BRID, boundedBy)
        NODETYPE( BRID, outerBridgeConstruction )
        NODETYPE( BRID, outerBridgeInstallation )
        NODETYPE( BRID, lod1Geometry)
        NODETYPE( BRID, lod2Geometry)
        NODETYPE( BRID, lod3Geometry)
        NODETYPE( BRID, lod4Geometry)
        NODETYPE( BRID, lod1MultiSurface) // Theoretically MultiSurfaces only exist for LoD 2, 3, and 4 but let's be lenient
        NODETYPE( BRID, lod2MultiSurface)
        NODETYPE( BRID, lod3MultiSurface)
        NODETYPE( BRID, lod4MultiSurface)

        NODETYPE( BRID, WallSurface)
        NODETYPE( BRID, RoofSurface)
        NODETYPE( BRID, GroundSurface)
        NODETYPE( BRID, ClosureSurface)
        NODETYPE( BRID, FloorSurface)
        NODETYPE( BRID, InteriorWallSurface)
        NODETYPE( BRID, CeilingSurface)
        NODETYPE( BRID, OuterCeilingSurface)
        NODETYPE( BRID, OuterFloorSurface)

        // APP
        NODETYPE( APP, Appearance )
        NODETYPE( APP, appearanceMember )
        NODETYPE( APP, SimpleTexture )
        NODETYPE( APP, ParameterizedTexture )
        NODETYPE( APP, GeoreferencedTexture )
        NODETYPE( APP, imageURI )
        NODETYPE( APP, textureMap )
        NODETYPE( APP, textureParameterization )
        NODETYPE( APP, TextureAssociation )
        NODETYPE( APP, target )
        NODETYPE( APP, TexCoordList )
        NODETYPE( APP, TexCoordGen )
        NODETYPE( APP, textureCoordinates )
        NODETYPE( APP, ring )
        NODETYPE( APP, worldToTexture )
        NODETYPE( APP, textureType )
        NODETYPE( APP, repeat )
        NODETYPE( APP, wrapMode )
        NODETYPE( APP, borderColor )
        NODETYPE( APP, preferWorldFile )
        NODETYPE( APP, referencePoint)
        NODETYPE( APP, orientation)
        NODETYPE( APP, isSmooth)

        NODETYPE( APP, X3DMaterial )
        NODETYPE( APP, Material )
        NODETYPE( APP, SurfaceDataMember ) // This doesn't exist in the APP namespace, but let's be lenient.
        NODETYPE( APP, surfaceData )
        NODETYPE( APP, shininess )
        NODETYPE( APP, transparency )
        NODETYPE( APP, specularColor )
        NODETYPE( APP, diffuseColor )
        NODETYPE( APP, emissiveColor )
        NODETYPE( APP, ambientIntensity )
        NODETYPE( APP, isFront )
        NODETYPE( APP, theme )
        NODETYPE( APP, mimeType )

    private:
        static void initializeNodeTypes();

        static std::mutex initializedMutex;
        static bool nodesInitialized;
        static int typeCount;
        static std::unordered_map<std::string, XMLNode*> nodeNameTypeMap;
        static std::unordered_map<std::string, XMLNode*> nodeNameWithPrefixTypeMap;
    };

    std::ostream& operator<<( std::ostream& os, const NodeType::XMLNode& o );
}

namespace std {

template <>
struct hash<citygml::NodeType::XMLNode> {
    size_t operator()(const citygml::NodeType::XMLNode& node) const
    {
        return node.typeID();
    }
};

template <>
struct hash<const citygml::NodeType::XMLNode&> {
    size_t operator()(const citygml::NodeType::XMLNode& node) const
    {
        return node.typeID();
    }
};

}  // namespace std
