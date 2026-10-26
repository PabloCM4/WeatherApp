#include <crow.h>
#include "FavouritesService.hpp"
#include <string>
#include <mongocxx/instance.hpp>

int main()
{
    crow::SimpleApp crow_app;
    mongocxx::instance instance_; // This should be created only once in the application
    FavouritesService favouritesService;

    CROW_ROUTE(crow_app, "/") // What to do when someone visits the root path ("/")
    ([]()                     // Lambda function to handle the request
     {
        crow::json::wvalue result;  // JSON object
        result["status"] = "ok";
        crow::response response(200, result);
        response.set_header("Access-Control-Allow-Origin", "*");
        return response; });

    CROW_ROUTE(crow_app, "/addFavourite") // What to do when someone visits the path ("/addFavourite")
    //.methods(crow::HTTPMethod::POST) // Specify that this route only accepts POST requests (disabled for testing purposes)
    ([&favouritesService](const crow::request &req)
     {
        
        crow::json::wvalue result; // JSON object

        const char *city = req.url_params.get("city"); // It will read the city from /weather?city=London
        const char *user_param = req.url_params.get("user"); // It will read the user from /weather?user=John
        const std::string user = user_param != nullptr ? user_param : "";
        if (city != nullptr)
        {
            favouritesService.addFavourite(user, city);
            result["message"] = "Favourite added";
        }
        else
        {
            result["error"] = "City parameter is required";
            crow::response response(400, result);
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        }
        crow::response response(200, result);
        response.set_header("Access-Control-Allow-Origin", "*");
        return response; });

    CROW_ROUTE(crow_app, "/removeFavourite") // What to do when someone visits the path ("/removeFavourite")
    //.methods(crow::HTTPMethod::DELETE) // Specify that this route only accepts POST requests (disabled for testing purposes)
    ([&favouritesService](const crow::request &req)
     {
        crow::json::wvalue result; // JSON object

        const char *city = req.url_params.get("city"); // It will read the city from /weather?city=London
        const char *user_param = req.url_params.get("user"); // It will read the user from /weather?user=John
        const std::string user = user_param != nullptr ? user_param : "";
        if (city != nullptr)
        {
            favouritesService.removeFavourite(user, city);
            result["message"] = "Favourite removed";
        }
        else
        {
            result["error"] = "City parameter is required";
            crow::response response(400, result);
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        }
        crow::response response(200, result);
        response.set_header("Access-Control-Allow-Origin", "*");
        return response; });

    CROW_ROUTE(crow_app, "/isFavourite") // What to do when someone visits the path ("/isFavourite")
        .methods(crow::HTTPMethod::GET)  // Specify that this route only accepts GET requests
        ([&favouritesService](const crow::request &req)
         {
        crow::json::wvalue result; // JSON object

        const char *city = req.url_params.get("city"); // It will read the city from /weather?city=London 
        const char *user_param = req.url_params.get("user"); // It will read the user from /weather?user=John
        const std::string user = user_param != nullptr ? user_param : "";
        if (city != nullptr)
        {
            if (favouritesService.isFavourite(user, city))
            {
                result["message"] = "Favourite is present";
            }
            else
            {
                result["message"] = "Favourite is not present";
            }
        }
        else
        {
            result["error"] = "City parameter is required";
            crow::response response(400, result);
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        }
        crow::response response(200, result);
        response.set_header("Access-Control-Allow-Origin", "*");
        return response; });

    CROW_ROUTE(crow_app, "/getFavourites") // What to do when someone visits the path ("/getFavourites")
        .methods(crow::HTTPMethod::GET)    // Specify that this route only accepts GET requests
        ([&favouritesService](const crow::request &req)
         {
        crow::json::wvalue result; // JSON object
        const char *user_param = req.url_params.get("user"); // It will read the user from /weather?user=John
        const std::string user = user_param != nullptr ? user_param : "";
        std::vector<std::string> favouritesFromStore = favouritesService.getFavourites(user);
        crow::json::wvalue::list favouritesFromStoreList;
        for (const std::string &city : favouritesFromStore)
        {
            favouritesFromStoreList.emplace_back(city);
        }
        result["favouritesFromStore"] = std::move(favouritesFromStoreList); // Move the list instead of copying it

        crow::response response(200, result);
        response.set_header("Access-Control-Allow-Origin", "*");
        return response; });

    crow_app.port(8081).run();
}