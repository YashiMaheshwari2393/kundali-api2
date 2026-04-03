#include "httplib.h"
#include "json.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;
using json = nlohmann::json;

string calculateZodiac(int day, int month) {
    if ((month == 3 && day >= 21) || (month == 4 && day <= 19))   return "Aries (Mesh)";
    if ((month == 4 && day >= 20) || (month == 5 && day <= 20))   return "Taurus (Vrishabh)";
    if ((month == 5 && day >= 21) || (month == 6 && day <= 20))   return "Gemini (Mithun)";
    if ((month == 6 && day >= 21) || (month == 7 && day <= 22))   return "Cancer (Kark)";
    if ((month == 7 && day >= 23) || (month == 8 && day <= 22))   return "Leo (Singh)";
    if ((month == 8 && day >= 23) || (month == 9 && day <= 22))   return "Virgo (Kanya)";
    if ((month == 9 && day >= 23) || (month == 10 && day <= 22))  return "Libra (Tula)";
    if ((month == 10 && day >= 23) || (month == 11 && day <= 21)) return "Scorpio (Vrishchik)";
    if ((month == 11 && day >= 22) || (month == 12 && day <= 21)) return "Sagittarius (Dhanu)";
    if ((month == 12 && day >= 22) || (month == 1 && day <= 19))  return "Capricorn (Makar)";
    if ((month == 1 && day >= 20) || (month == 2 && day <= 18))   return "Aquarius (Kumbh)";
    return "Pisces (Meen)";
}

vector<string> getRandomQuotes() {
    vector<string> quotes = {
        "Believe in yourself and shine bright!",
        "Success comes from consistency.",
        "Your stars support your hard work.",
        "Today is a new opportunity to grow.",
        "Confidence is your true power.",
        "The cosmos aligns in your favour.",
        "Trust the journey the universe has laid for you.",
        "Your potential is written in the stars."
    };
    srand((unsigned)time(nullptr));
    vector<string> chosen;
    vector<int> used;
    while ((int)chosen.size() < 3) {
        int idx = rand() % quotes.size();
        bool duplicate = false;
        for (int u : used) if (u == idx) { duplicate = true; break; }
        if (!duplicate) { chosen.push_back(quotes[idx]); used.push_back(idx); }
    }
    return chosen;
}

json buildKundali(const json& body) {
    string name  = body["name"];
    int    day   = body["day"];
    int    month = body["month"];
    int    year  = body["year"];
    string place = body["place"];
    string zodiac = calculateZodiac(day, month);
    vector<string> quotes = getRandomQuotes();
    json result;
    result["name"]            = name;
    result["place_of_birth"]  = place;
    result["date_of_birth"]   = to_string(day) + "-" + to_string(month) + "-" + to_string(year);
    result["zodiac_sign"]     = zodiac;
    result["lucky_quotes"]    = quotes;
    result["special_message"] = "The universe has great plans for you, " + name + "!";
    return result;
}

bool validateBody(const json& body, json& err) {
    if (!body.contains("name") || !body.contains("day") ||
        !body.contains("month") || !body.contains("year") || !body.contains("place")) {
        err["error"] = "Missing fields. Required: name, day, month, year, place";
        return false;
    }
    int day = body["day"], month = body["month"], year = body["year"];
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
        err["error"] = "Invalid date values.";
        return false;
    }
    return true;
}

string generateTXT(const json& k) {
    ostringstream oss;
    oss << "=========================================\n";
    oss << "           ASTROKUNDALI REPORT           \n";
    oss << "=========================================\n\n";
    oss << "Name            : " << k["name"].get<string>() << "\n";
    oss << "Date of Birth   : " << k["date_of_birth"].get<string>() << "\n";
    oss << "Place of Birth  : " << k["place_of_birth"].get<string>() << "\n";
    oss << "Zodiac Sign     : " << k["zodiac_sign"].get<string>() << "\n\n";
    oss << "-----------------------------------------\n";
    oss << "Lucky Quotes:\n";
    for (auto& q : k["lucky_quotes"]) oss << "  * " << q.get<string>() << "\n";
    oss << "\n-----------------------------------------\n";
    oss << "Special Message :\n  " << k["special_message"].get<string>() << "\n";
    oss << "=========================================\n";
    return oss.str();
}

string generatePDFHTML(const json& k) {
    ostringstream html;
    html << R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>AstroKundali Report</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Cinzel:wght@400;700&family=Lato:wght@300;400&display=swap');
  * { margin: 0; padding: 0; box-sizing: border-box; }
  body { font-family: 'Lato', sans-serif; background: #0d0d1a; color: #e8d9b5;
    min-height: 100vh; display: flex; align-items: center; justify-content: center; padding: 40px 20px; }
  .card { background: linear-gradient(145deg, #1a1a2e, #16213e); border: 1px solid #c9a84c;
    border-radius: 16px; padding: 50px 60px; max-width: 680px; width: 100%;
    box-shadow: 0 0 60px rgba(201,168,76,0.15); }
  .header { text-align: center; margin-bottom: 36px; border-bottom: 1px solid rgba(201,168,76,0.3); padding-bottom: 24px; }
  .header h1 { font-family: 'Cinzel', serif; font-size: 2rem; color: #c9a84c; letter-spacing: 4px; text-transform: uppercase; }
  .header p { font-size: 0.85rem; color: #888; margin-top: 6px; letter-spacing: 2px; text-transform: uppercase; }
  .section { margin-bottom: 28px; }
  .section label { font-size: 0.7rem; letter-spacing: 3px; text-transform: uppercase;
    color: #c9a84c; opacity: 0.7; display: block; margin-bottom: 6px; }
  .section .value { font-size: 1.1rem; font-weight: 400; color: #f0e6cc; }
  .zodiac-badge { display: inline-block; background: linear-gradient(135deg, #c9a84c, #e8c96a);
    color: #0d0d1a; font-family: 'Cinzel', serif; font-weight: 700;
    padding: 8px 22px; border-radius: 50px; font-size: 1rem; letter-spacing: 1px; }
  .quotes { list-style: none; }
  .quotes li { padding: 10px 0 10px 20px; border-left: 2px solid rgba(201,168,76,0.4);
    margin-bottom: 10px; font-style: italic; color: #ccc; font-size: 0.95rem; }
  .special { background: rgba(201,168,76,0.08); border: 1px solid rgba(201,168,76,0.25);
    border-radius: 10px; padding: 18px 24px; font-size: 1rem; text-align: center;
    color: #e8d9b5; font-style: italic; }
  .footer { text-align: center; margin-top: 36px; font-size: 0.75rem; color: #555; letter-spacing: 1px; }
  .print-btn { display: block; margin: 30px auto 0; padding: 12px 36px;
    background: linear-gradient(135deg, #c9a84c, #e8c96a); color: #0d0d1a; border: none;
    border-radius: 50px; font-family: 'Cinzel', serif; font-size: 0.9rem; font-weight: 700;
    cursor: pointer; letter-spacing: 2px; }
  @media print { body { background: white; } .card { box-shadow: none; border-color: #ccc; } .print-btn { display: none; } }
</style>
</head>
<body>
<div class="card">
  <div class="header"><h1>&#9733; AstroKundali</h1><p>Celestial Birth Report</p></div>
  <div class="section"><label>Full Name</label><div class="value">)";
    html << k["name"].get<string>();
    html << R"(</div></div>
  <div class="section"><label>Date of Birth</label><div class="value">)";
    html << k["date_of_birth"].get<string>();
    html << R"(</div></div>
  <div class="section"><label>Place of Birth</label><div class="value">)";
    html << k["place_of_birth"].get<string>();
    html << R"(</div></div>
  <div class="section"><label>Zodiac Sign</label><div><span class="zodiac-badge">)";
    html << k["zodiac_sign"].get<string>();
    html << R"(</span></div></div>
  <div class="section"><label>Lucky Quotes</label><ul class="quotes">)";
    for (auto& q : k["lucky_quotes"]) html << "<li>" << q.get<string>() << "</li>";
    html << R"(</ul></div>
  <div class="special">)";
    html << k["special_message"].get<string>();
    html << R"(</div>
  <button class="print-btn" onclick="window.print()">Save as PDF</button>
  <div class="footer">Generated by AstroKundali &bull; The stars never lie</div>
</div>
</body>
</html>)";
    return html.str();
}

bool saveTXTtoDisk(const json& k, string& filepath) {
    filesystem::create_directories("kundali_files");
    string name = k["name"].get<string>();
    string safe = name;
    for (char& c : safe) if (!isalnum(c)) c = '_';
    filepath = "kundali_files/" + safe + "_kundali.txt";
    ofstream f(filepath);
    if (!f.is_open()) return false;
    f << generateTXT(k);
    f.close();
    return true;
}

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            "Kundali API is running!\n"
            "Endpoints:\n"
            "  POST /kundali          -> JSON response\n"
            "  POST /kundali/txt      -> Download .txt file\n"
            "  POST /kundali/pdf      -> HTML page (Ctrl+P to save PDF)\n"
            "  POST /kundali/save     -> Save .txt on server + JSON response\n",
            "text/plain");
    });

    svr.Get("/kundali", [](const httplib::Request&, httplib::Response& res) {
        json hint;
        hint["usage"]   = "Send a POST request to /kundali";
        hint["example"] = {{"name","Arjun"},{"day",15},{"month",8},{"year",2000},{"place","Delhi"}};
        res.set_content(hint.dump(2), "application/json");
    });

    svr.Post("/kundali", [](const httplib::Request& req, httplib::Response& res) {
        json body, err;
        try { body = json::parse(req.body); }
        catch (...) { json e; e["error"]="Invalid JSON body"; res.status=400; res.set_content(e.dump(),"application/json"); return; }
        if (!validateBody(body, err)) { res.status=400; res.set_content(err.dump(),"application/json"); return; }
        res.set_content(buildKundali(body).dump(2), "application/json");
    });

    svr.Post("/kundali/txt", [](const httplib::Request& req, httplib::Response& res) {
        json body, err;
        try { body = json::parse(req.body); }
        catch (...) { json e; e["error"]="Invalid JSON body"; res.status=400; res.set_content(e.dump(),"application/json"); return; }
        if (!validateBody(body, err)) { res.status=400; res.set_content(err.dump(),"application/json"); return; }
        json k = buildKundali(body);
        string safe = k["name"].get<string>();
        for (char& c : safe) if (!isalnum(c)) c = '_';
        res.set_header("Content-Disposition", "attachment; filename=\"" + safe + "_kundali.txt\"");
        res.set_content(generateTXT(k), "text/plain");
    });

    svr.Post("/kundali/pdf", [](const httplib::Request& req, httplib::Response& res) {
        json body, err;
        try { body = json::parse(req.body); }
        catch (...) { json e; e["error"]="Invalid JSON body"; res.status=400; res.set_content(e.dump(),"application/json"); return; }
        if (!validateBody(body, err)) { res.status=400; res.set_content(err.dump(),"application/json"); return; }
        res.set_content(generatePDFHTML(buildKundali(body)), "text/html");
    });

    svr.Post("/kundali/save", [](const httplib::Request& req, httplib::Response& res) {
        json body, err;
        try { body = json::parse(req.body); }
        catch (...) { json e; e["error"]="Invalid JSON body"; res.status=400; res.set_content(e.dump(),"application/json"); return; }
        if (!validateBody(body, err)) { res.status=400; res.set_content(err.dump(),"application/json"); return; }
        json k = buildKundali(body);
        string filepath;
        bool saved = saveTXTtoDisk(k, filepath);
        k["saved_to"] = saved ? filepath : "Error saving file";
        k["status"]   = saved ? "File saved on server" : "File save failed";
        res.set_content(k.dump(2), "application/json");
    });

    const char* port_env = getenv("PORT");
    int port = port_env ? atoi(port_env) : 8080;
    cout << "Kundali API running on port " << port << endl;
    svr.listen("0.0.0.0", port);
    return 0;
}

