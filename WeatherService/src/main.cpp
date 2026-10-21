#include <crow.h> // To receive HTTP requests and send responses
#include "WeatherService.h"

int main()
{
    crow::SimpleApp crow_app; // Create the web server object to receive HTTP requests and send responses

    CROW_ROUTE(crow_app, "/") // What to do when someone visits the root path ("/")
    ([]()   // Lambda function to handle the request
     {
        crow::json::wvalue result;  // JSON object
        result["status"] = "ok";
        return crow::response(200, result); });

    CROW_ROUTE(crow_app, "/weather") // What to do when someone visits the path ("/weather")
    ([](const crow::request &req)
     {
        WeatherService weatherService;
        crow::json::wvalue result;  // JSON object

        const char* city = req.url_params.get("city"); // It will read the city from /weather?city=London
        if (!city)
        {
            result["error"] = "Missing city parameter";
            return crow::response(400, result);
        }

        Weather weather = weatherService.getTemperature(city);
        if (weather.temperature == -997) // Check if a JSON parsing error occurred
        {
            result["error"] = "Error occurred while parsing weather data";
            return crow::response(500, result);
        }
        if (weather.temperature == -998) // Check if an error occurred
        {
            result["error"] = "Error occurred while fetching weather data";
            return crow::response(500, result);
        }
        if (weather.temperature == -999) // Check if the city was not found
        {
            result["error"] = "City not found";
            return crow::response(404, result);
        }
        result["temperature"] = weather.temperature;
        result["city"] = weather.city;

        return crow::response(200, result); });

    crow_app.port(8080).run(); // Start listening to requests on port 8080
}