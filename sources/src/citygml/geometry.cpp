#include <citygml/geometry.h>

#include <citygml/polygon.h>
#include <citygml/appearancemanager.h>
#include <citygml/appearance.h>
#include <citygml/citygmllogger.h>

#include <algorithm>

namespace citygml {

    Geometry::Geometry(const std::string& id, Geometry::GeometryType type, unsigned int lod, std::string srsName)
        : AppearanceTarget( id ), m_finished(false), m_type( type ), m_lod( lod ), m_srsName( srsName )
    {

    }

    unsigned int Geometry::getLOD() const
    {
        return m_lod;
    }

    unsigned int Geometry::getPolygonsCount() const
    {
        return m_polygons.size();
    }

    std::shared_ptr<Polygon> Geometry::getPolygon(unsigned int i)
    {
        return m_polygons.at(i);
    }

    std::shared_ptr<const Polygon> Geometry::getPolygon(unsigned int i) const
    {
        return m_polygons.at(i);
    }

    unsigned int Geometry::getLineStringCount() const
    {
        return m_lineStrings.size();
    }

    std::shared_ptr<LineString> Geometry::getLineString(unsigned int i)
    {
        return m_lineStrings.at(i);
    }

    std::shared_ptr<const LineString> Geometry::getLineString(unsigned int i) const
    {
        return m_lineStrings.at(i);
    }

    unsigned int Geometry::getGeometriesCount() const
    {
        return m_childGeometries.size();
    }

    const Geometry& Geometry::getGeometry(unsigned int i) const
    {
        return *m_childGeometries.at(i);
    }

    Geometry& Geometry::getGeometry(unsigned int i)
    {
        return *m_childGeometries.at(i);
    }

    void Geometry::addGeometry(Geometry* geom)
    {
        m_childGeometries.push_back(std::unique_ptr<Geometry>(geom));
    }
    
    std::string Geometry::getPreviousParentId(std::string currentParentId, std::shared_ptr<citygml::CityGMLLogger> logger) const
    {
        for (auto& pair : m_NodeStack)
        {
            if (std::find_if(pair.second.begin(), pair.second.end(), 
                [currentParentId](const IntermediateNode& node) 
                { return currentParentId == node.id(); }) != pair.second.end())
                return pair.first;
        }

        if (currentParentId != "root")
        {
            CITYGML_LOG_ERROR(logger, "Failed to find parent id of " + currentParentId);
            return currentParentId;
        }
        
        return "";
    }

    std::string Geometry::getNodeStackPath(const std::string& startNodeId, std::shared_ptr<citygml::CityGMLLogger> logger) const
    {
        // searches up from the input id to the root
        std::string pathToRoot = "";
        std::string currentId = startNodeId;
        
        while (!currentId.empty() || currentId == "root")
        {
            std::string parentId = getPreviousParentId(currentId, logger);
            if (parentId.empty())
                return pathToRoot;

            // Now look for the details of the node from the parent
            auto parentChildren = m_NodeStack.at(parentId);
            auto nodeIt = std::find_if(parentChildren.begin(), parentChildren.end(), [currentId](const IntermediateNode& node) {
                return node.id() == currentId;
            });
            if (nodeIt != parentChildren.end())
            {
                // Found the node in the parent's children
                std::string idBlock = nodeIt->id().empty() ? "" : "[" + nodeIt->id() + "]";
                pathToRoot = nodeIt->name() + idBlock + "\\" + pathToRoot;
                currentId = parentId;
            }
            else
            {
                // force search to end
                currentId = "";
            }
        }

        return pathToRoot;
    }

    void Geometry::pushIntermediateNode(const IntermediateNode& node, const std::string& parentId, bool toBack)
    {
        if(m_NodeStack.find(parentId) == m_NodeStack.end())
            m_NodeStack.insert(std::make_pair(parentId, std::deque<citygml::IntermediateNode>()));

        // if replacing the root then root elements are moved to become children of the new element
        if (parentId == "root")
        {
            if (m_NodeStack.find(node.id()) == m_NodeStack.end())
                m_NodeStack.insert(std::make_pair(node.id(), std::deque<citygml::IntermediateNode>()));

            auto rootElements = m_NodeStack.at("root");
            for (auto& rootElement : rootElements)
            {
                m_NodeStack.at(node.id()).push_back(rootElement);
            }

            m_NodeStack.at("root").clear();
        }

        if (toBack)
        {
            m_NodeStack.at(parentId).push_back(node);
        }
        else
        {
            m_NodeStack.at(parentId).push_front(node);
        }
    }

    Geometry::GeometryType Geometry::getType() const
    {
        return m_type;
    }

    std::string Geometry::getTypeAsString() const
    {
        switch (m_type) {
        case GeometryType::GT_Unknown:
            return "Unknown";
        case GeometryType::GT_Roof:
            return "Roof";
        case GeometryType::GT_Wall:
            return "Wall";
        case GeometryType::GT_Ground:
            return "Ground";
        case GeometryType::GT_Closure:
            return "Closure";
        case GeometryType::GT_Floor:
            return "Floor";
        case GeometryType::GT_InteriorWall:
            return "InteriorWall";
        case GeometryType::GT_Ceiling:
            return "Ceiling";
        case GeometryType::GT_OuterCeiling:
            return "OuterCeiling";
        case GeometryType::GT_OuterFloor:
            return "OuterFloor";
        case GeometryType::GT_Tin:
            return "Tin";
        default:
            return "Unknown";
        }
    }

    Geometry::~Geometry()
    {
    }

    unsigned int Geometry::lod() const
    {
        return m_lod;
    }

    void Geometry::setLod(unsigned int lod)
    {
        m_lod = lod;
    }

    std::string Geometry::getSRSName() const
    {
        return m_srsName;
    }

    void Geometry::setSRSName(const std::string& srsName)
    {
        m_srsName = srsName;
    }

    void Geometry::addPolygon( std::shared_ptr<Polygon> p )
    {
        m_polygons.push_back(p);
    }

    void Geometry::addLineString(std::shared_ptr<LineString> l)
    {
        m_lineStrings.push_back(l);
    }

    void Geometry::finish(TesselatorBase* tesselator, bool optimize, std::shared_ptr<CityGMLLogger> logger)
    {
        // only need to finish geometry once
        if (m_finished) {
            return;
        }

        m_finished = true;

        for (std::shared_ptr<Geometry>&  child : m_childGeometries) {
            child->addTargetDefinitionsOf(*this);
            child->finish(tesselator, optimize, logger);
        }

        for (std::shared_ptr<Polygon>& polygon : m_polygons) {
            polygon->addTargetDefinitionsOf(*this);
            polygon->finish(tesselator, optimize, logger);
        }

    }

    std::ostream& operator<<( std::ostream& os, const citygml::Geometry& s )
    {
        unsigned int count = 0;
        for ( unsigned int i = 0; i < s.getPolygonsCount(); i++ )
        {
            os << s.getPolygon(i);
            count += s.getPolygon(i)->getVertices().size();
        }

        os << "  @ " << s.getPolygonsCount() << " polys [" << count << " vertices]" << std::endl;

        return os;
    }

}
