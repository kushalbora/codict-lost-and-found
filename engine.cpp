#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::getline;

inline void error(const string& s) {
    throw std::runtime_error(s);
}

struct Item {
    string description;
    string date; 
    string category;
    string color;
    string location;
    string status;
    string image_name;
};

int calc_score(const Item& a, const Item& b) {
    int score = 0; 
    
    if (a.category == b.category) score += 40;
    if (a.color == b.color) score += 30;
    if (a.location == b.location) score += 25;

    return score; 
}

int main() 
try {
    ifstream ifs{"data.csv"};
    if (!ifs) error("Can't open input file");

    vector<Item> lost_items;
    vector<Item> found_items;

    ofstream ofs{"matches.txt"};
    if (!ofs) error("Cannot open output file");

    string dummy;
    getline(ifs, dummy); // Skip the header row we created in Python

    string str;
    while (getline(ifs, str)) {
        if (str.empty()) continue;

        std::istringstream istr{str}; 
        Item item;
        
        getline(istr, item.status, ','); 
        getline(istr, item.date, ',');
        getline(istr, item.category, ','); 
        getline(istr, item.color, ',');
        getline(istr, item.location, ',');
        getline(istr, item.description, ',');
        getline(istr, item.image_name); // No comma here, reads the rest of the line

        // Fix for Windows hidden carriage returns (\r)
        if (!item.image_name.empty() && item.image_name.back() == '\r') {
            item.image_name.pop_back();
        }

        if (item.status == "Lost") lost_items.push_back(item);
        else if (item.status == "Found") found_items.push_back(item);
    }

    bool matched = false;

    for (size_t i = 0; i < lost_items.size(); i++) {
        for (size_t j = 0; j < found_items.size(); j++) {
            int score = calc_score(lost_items[i], found_items[j]);
            
            if (score >= 65) {
                matched = true;
                
                string out_res;
                out_res += "<div style='border-bottom: 1px solid #444; padding-bottom: 10px; margin-bottom: 10px;'>"
                         + string("<h3 style='color: #00ff00;'> Match (") + std::to_string(score) + "%)</h3>"
                         + "<strong>Lost item:</strong> " + lost_items[i].category + " | <strong>Color:</strong> " + lost_items[i].color + " | <strong>Location:</strong> " + lost_items[i].location + " <br>"
                         + "<em>Lost on: " + lost_items[i].date + "</em><br><br>"
                         + "<strong>Found item:</strong> " + found_items[j].category + " | <strong>Color:</strong> " + found_items[j].color + " | <strong>Location:</strong> " + found_items[j].location + " <br>"
                         + "<em>Found on: " + found_items[j].date + "</em>"
                         + "</div>";
                ofs << out_res << '\n';  
            }
        }
    }

    if (!matched) ofs << "<p>No high confidence matches found yet...</p>\n";

    return 0;
}
catch (std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return -1;
}
catch (...) {
    std::cerr << "Oops, unknown error detected\n";
    return -2;
}