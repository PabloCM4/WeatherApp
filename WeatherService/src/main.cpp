#include <crow.h> // To receive HTTP requests and send responses
#include "WeatherService.h"

int main()
{
    crow::SimpleApp crow_app; // Create the web server object to receive HTTP requests and send responses

    CROW_ROUTE(crow_app, "/") // What to do when someone visits the root path ("/")
    ([]()                     // Lambda function to handle the request
     {
        crow::json::wvalue result_json;  // JSON object
        result_json["status"] = "ok";
        crow::response response(200, result_json);
        response.set_header("Access-Control-Allow-Origin", "*");
        return response; });

    CROW_ROUTE(crow_app, "/weather") // What to do when someone visits the path ("/weather")
    ([](const crow::request &req)
     {
        WeatherService weatherService;
        crow::json::wvalue result_json;  // JSON object

        const char* city = req.url_params.get("city"); // It will read the city name example: /weather?city=London
        if (!city)
        {
            result_json["error"] = "Missing city parameter";
            crow::response response(400, result_json);
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        }

        WeatherResult result_weather = weatherService.getWeather(city);

        crow::response response; // Create a response object to send back to the client

        // Create a response based on status of the weather service result
        switch(result_weather.status)
        {
            case WeatherResult::Status::JsonParsingError:
                result_json["error"] = "Error occurred while parsing weather data";
                response = crow::response(500, result_json); // Response 500 means Internal Server Error
                break;

            case WeatherResult::Status::ApiError:
                result_json["error"] = "Error occurred while fetching weather data";
                response = crow::response(500, result_json); // Response 500 means Internal Server Error
                break;

            case WeatherResult::Status::InvalidApiKey:
                result_json["error"] = "The weather service API key is missing or invalid";
                response = crow::response(500, result_json); // Response 500 means Internal Server Error
                break;

            case WeatherResult::Status::Success:
                result_json["temperature"] = result_weather.weather.temperature;
                result_json["city"] = result_weather.weather.city;
                result_json["description"] = result_weather.weather.description;
                result_json["feels_like"] = result_weather.weather.feels_like;
                result_json["wind_speed"] = result_weather.weather.wind_speed;
                response = crow::response(200, result_json); // Response 200 means OK, the request was successful
                break;

            default:
                result_json["error"] = "The weather service could not process the request";
                response = crow::response(500, result_json); // Response 500 means Internal Server Error
                break;
        }

        response.set_header("Access-Control-Allow-Origin", "*"); // Allow cross-origin requests from any domain, needed for the frontend to access the backend
        return response; });

    crow_app.port(8080).run(); // Start listening to requests on port 8080
}