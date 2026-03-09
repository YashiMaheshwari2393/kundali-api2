#include "httplib.h"
#include "json.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

using namespace std;
using json = nlohmann::json;

// ─── Zodiac Calculator ────────────────────────────────────────────────────────
string calculateZodiac(int day, int month) {
    if ((month == 3 && day >= 21) || (month == 4 && day <= 19))  return "Aries (Mesh)";
    if ((month == 4 && day >= 20) || (month == 5 && day <= 20))  return "Taurus (Vrishabh)";
    if ((month == 5 && day >= 21) || (month == 6 && day <= 20))  return "Gemini (Mithun)";
    if ((month == 6 && day >= 21) || (month == 7 && day <= 22))  return "Cancer (Kark)";
    if ((month == 7 && day >= 23) || (month == 8 && day <= 22))  return "Leo (Singh)";
    if ((month == 8 && day >= 23) || (month == 9 && day <= 22))  return "Virgo (Kanya)";
    if ((month == 9 && day >= 23) || (month == 10 && day <= 22)) return "Libra (Tula)";
    if ((month == 10 && day >= 23) || (month == 11 && day <= 21)) return "Scorpio (Vrishchik)";
    if ((month == 11 && day >= 22) || (month == 12 && day <= 21)) return "Sagittarius (Dhanu)";
    if ((month == 12 && day >= 22) || (month == 1 && day <= 19)) return "Capricorn (Makar)";
    if ((month == 1 && day >= 20) || (month == 2 && day <= 18))  return "Aquarius (Kumbh)";
    return "Pisces (Meen)";
}

// ─── Lucky Quotes ─────────────────────────────────────────────────────────────
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
        if (!duplicate) {
            chosen.push_back(quotes[idx]);
            used.push_back(idx);
        }
    }
    return chosen;
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    httplib::Server svr;

    // Health check
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Kundali API is running! POST to /kundali", "text/plain");
    });

    // GET /kundali - usage hint
    svr.Get("/kundali", [](const httplib::Request&, httplib::Response& res) {
        json hint;
        hint["usage"]   = "Send a POST request to /kundali";
        hint["example"] = {{"name","Arjun"},{"day",15},{"month",8},{"year",2000},{"place","Delhi"}};
        res.set_content(hint.dump(2), "application/json");
    });

    // POST /kundali
    svr.Post("/kundali", [](const httplib::Request& req, httplib::Response& res) {
        json body;
        try {
            body = json::parse(req.body);
        } catch (...) {
            json err;
            err["error"] = "Invalid JSON body";
            res.status = 400;
            res.set_content(err.dump(), "application/json");
            return;
        }

        if (!body.contains("name") || !body.contains("day") ||
            !body.contains("month") || !body.contains("year") || !body.contains("place")) {
            json err;
            err["error"] = "Missing fields. Required: name, day, month, year, place";
            res.status = 400;
            res.set_content(err.dump(), "application/json");
            return;
        }

        string name  = body["name"];
        int    day   = body["day"];
        int    month = body["month"];
        int    year  = body["year"];
        string place = body["place"];

        if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
            json err;
            err["error"] = "Invalid date values.";
            res.status = 400;
            res.set_content(err.dump(), "application/json");
            return;
        }

        string zodiac = calculateZodiac(day, month);
        vector<string> quotes = getRandomQuotes();

        json result;
        result["name"]           = name;
        result["place_of_birth"] = place;
        result["date_of_birth"]  = to_string(day) + "-" + to_string(month) + "-" + to_string(year);
        result["zodiac_sign"]    = zodiac;
        result["lucky_quotes"]   = quotes;
        result["special_message"] = "The universe has great plans for you, " + name + "!";

        res.set_content(result.dump(2), "application/json");
    });

    const char* port_env = getenv("PORT");
    int port = port_env ? atoi(port_env) : 8080;

    cout << "Kundali API running on port " << port << endl;
    svr.listen("0.0.0.0", port);
    return 0;
}
