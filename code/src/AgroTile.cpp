# include <map>
# include <sstream>
# include <vector>
# include "AgroTile.h"
# include "MeshUtils.h"

// Map from Node IDs to a vector of moisture readings
static std::map<string, std::vector<Moisture *>* > *entries;

// Constructs a moisture object
Moisture::Moisture(unsigned long timestamp, unsigned short int moisture)
: _timestamp(timestamp), _moisture(moisture) {}

//Allocate moisture
void allocate() {
    entries = new std::map<string, std::vector<Moisture *>* >();
}

// There's probably a cleaner way to do this, but it seems memory efficient
const char js_part1[] PROGMEM = "=new CanvasJS.Chart(\"";
const char js_part2[] PROGMEM = "\",{animationEnabled:true,theme:\"light2\",title:{text:\"";
const char js_part3[] PROGMEM = "\"},axisY:{includeZero:true},data:[{type:\"spline\",dataPoints:[";
const char js_part4[] PROGMEM = "]}]});";

// Checks if a string contains only numbers
boolean isValidNumber(String str) {
    for(byte i = 0; i < str.length(); ++ i)
        if(!isDigit(str.charAt(i))) return false;
   return true;
}

// Convert the moisture readings and convert to a readable HTML graph
String processor(const String& var) {
    Serial.print("In processor with var='");
    Serial.print(var);
    Serial.println("'");
    if (var == "SCRIPT") {
        std::stringstream ss;

        for(auto const &entry : *entries) {
            ss << "var " << entry.first << js_part1;
            ss << entry.first << js_part2;
            ss << entry.first << " Moisture" << js_part3;
            for (auto const &moisture : *entry.second) {
                ss << "{x:" << moisture->getTimestamp();
                Serial.print("Moisture");
                ss << ",y:" << moisture->getMoisture() << "},";
            }
            ss << js_part4 << entry.first << ".render();";
        }
        return ss.str().c_str();
    } else if (var == "BODY") {
        std::stringstream ss;

        for(auto const &entry : *entries) {
            ss << "<div id=\"" << entry.first << "\" class=\"canvclass\" style=\"height: 300px; width: 100% ;\"></div>";
        }

        return ss.str().c_str();
    } else if (var == "STYLE") {
        return "";
    }

    return "html processing error";
}

/* If we do not have record for node, create a new map entry.
 * Then we can add the new record to the our records. */
void recordMoisture(const String &name, Moisture *moisture) {
    string record = name.c_str();
    //Serial.println(record);
    if (entries->count(record) == 0) {
        (*entries)[record] = new std::vector<Moisture *>();
    }

    unsigned long delta = RECORD_DELTA;
    if ((*entries)[record]->size() != 0)
        unsigned long delta =
            moisture->getTimestamp() - (*entries)[record]->back()->getTimestamp();
    if (delta >= RECORD_DELTA)
        (*entries)[record]->push_back(moisture);
    else
        Serial.println("Duplicate detected, throwing out message.");
}
