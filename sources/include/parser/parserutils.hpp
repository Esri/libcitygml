#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <parser/documentlocation.h>

#include <citygml/transformmatrix.h>
#include <citygml/citygmllogger.h>
#include <citygml/vecs.hpp>

namespace citygml {

    inline TransformationMatrix parseMatrix( const std::string &s, std::shared_ptr<citygml::CityGMLLogger>& logger, const DocumentLocation& location)
    {
        std::stringstream ss;
        ss << s;


        double matrix[16] = { 1.0, 0.0, 0.0, 0.0,
                              0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0,
                              0.0, 0.0, 0.0, 1.0 };

        for (size_t i = 0; i < 16; ++i)
        {
            if(ss.eof()) {
                CITYGML_LOG_WARN(logger, "Matrix with 16 elements expected, got '" << i + 1 << "' at " << location << ". Matrix may be invalid.");
                break;
            }

            ss >> matrix[i];
        }

        return TransformationMatrix(matrix);
    }

    template<class T> inline T parseNumber(const char* str, char** end);

    template<> inline float parseNumber<float>(const char* str, char** end) {
        return std::strtof(str, end);
    }

    template<> inline double parseNumber<double>(const char* str, char** end) {
        return std::strtod(str, end);
    }

    template<typename T>
    inline bool parseVec(T& result, const char* str, char** end) {
        result = parseNumber<T>(str, end);
        return str != *end;
    }

    template<typename T> inline bool parseVec(TVec2<T>& result, const char* str, char** end) {
        result.x = parseNumber<T>(str, end);
        if (str == *end) {
            return false;
        }
        str = *end;
        result.y = parseNumber<T>(str, end);
        return str != *end;
    }

    template<typename T> inline bool parseVec(TVec3<T>& result, const char* str, char** end) {
        result.x = parseNumber<T>(str, end);
        if (str == *end) {
            return false;
        }
        str = *end;
        result.y = parseNumber<T>(str, end);
        if (str == *end) {
            return false;
        }
        str = *end;
        result.z = parseNumber<T>(str, end);
        return str != *end;
    }

    template<class T> inline T parseValue( const std::string &s, std::shared_ptr<citygml::CityGMLLogger>&, const DocumentLocation&)
    {
        const char* str = s.c_str();
        char* end = nullptr;
        T value;
        parseVec(value, str, &end);
        return value;
    }

    template<> inline bool parseValue( const std::string &s, std::shared_ptr<citygml::CityGMLLogger>& logger, const DocumentLocation& location )
    {
        // parsing a bool is special because "true" and "1" are true while "false" and "0" are false
        if (s == "1" || s == "true") {
            return true;
        } else if (s == "0" || s == "false") {
            return false;
        } else {
            CITYGML_LOG_WARN(logger, "Boolean expected, got '" << s << "' at " << location << " set value to false.");
        }
        return false;
    }

    template<class T> inline std::vector<T> parseVecList( const std::string &s,  std::shared_ptr<citygml::CityGMLLogger>& logger, const DocumentLocation& location )
    {
        std::vector<T> vec;
        const char* str = s.c_str();

        char* end = nullptr;
        while (*str != '\0') {
            T value;
            if (!parseVec(value, str, &end)) {
                CITYGML_LOG_WARN(logger, "Mismatch type, list of " << typeid(T).name() << " expected at " << location << " Ring/Polygon may be incomplete!");
                break;
            }
            vec.push_back(value);
            str = end;
        }

        return vec;
    }

    inline std::string parseReference(const std::string& reference, std::shared_ptr<citygml::CityGMLLogger>& logger, const DocumentLocation& location) {
        if (reference.empty()) {
            CITYGML_LOG_WARN(logger, "Invalid reference value at " << location);
        }

        if (reference[0] == '#') {
            return reference.substr(1);
        } else {
            return reference;
        }
    }

}
