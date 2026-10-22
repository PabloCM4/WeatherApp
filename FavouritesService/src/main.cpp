#include <crow.h>
#include "FavouritesService.hpp"
#include <string>
#include <unordered_set>
#include <mongocxx/instance.hpp>

int main()
{
    crow::SimpleApp crow_app;
    mongocxx::instance instance_; // This should be created only once in the application

    CROW_ROUTE(crow_app, "/") // What to do when someone visits the root path ("/")
    ([]()                     // Lambda function to handle the request
     {
        crow::json::wvalue result;  // JSON object
        result["status"] = "ok";
        return crow::response(200, result); });

    CROW_ROUTE(crow_app, "/addFavourite") // What to do when someone visits the path ("/addFavourite")
    ([](const crow::request &req)
     {
        FavouritesService favouritesService;
        crow::json::wvalue result; // JSON object

        const char *city = req.url_params.get("city"); // It will read the city from /weather?city=London 
        if (city != nullptr)
        {
            favouritesService.addFavourite(city);
            favouritesService.addFavouriteToStore(city);
            result["message"] = "Favourite added";
        }
        else
        {
            result["error"] = "City parameter is required";
            return crow::response(400, result);
        }
        return crow::response(200, result); });

    CROW_ROUTE(crow_app, "/removeFavourite") // What to do when someone visits the path ("/removeFavourite")
    ([](const crow::request &req)
     {
        FavouritesService favouritesService;
        crow::json::wvalue result; // JSON object

        const char *city = req.url_params.get("city"); // It will read the city from /weather?city=London 
        if (city != nullptr)
        {
            favouritesService.removeFavourite(city);
            result["message"] = "Favourite removed";
        }
        else
        {
            result["error"] = "City parameter is required";
            return crow::response(400, result);
        }
        return crow::response(200, result); });
    CROW_ROUTE(crow_app, "/isFavourite") // What to do when someone visits the path ("/isFavourite")
    ([](const crow::request &req)
     {
        FavouritesService favouritesService;
        crow::json::wvalue result; // JSON object

        const char *city = req.url_params.get("city"); // It will read the city from /weather?city=London 
        if (city != nullptr)
        {
            if (favouritesService.isFavourite(city))
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
            return crow::response(400, result);
        }
        return crow::response(200, result); });

    CROW_ROUTE(crow_app, "/getFavourites") // What to do when someone visits the path ("/getFavourites")
    ([]()
     {
        FavouritesService favouritesService;
        crow::json::wvalue result; // JSON object
        std::unordered_set<std::string> favourites = favouritesService.getFavourites();
        std::vector<std::string> favouritesFromStore = favouritesService.getFavouritesFromStore();
        crow::json::wvalue::list favouritesList, favouritesFromStoreList;
        for (const std::string &city : favourites)
        {
            favouritesList.emplace_back(city);
        }
        for (const std::string &city : favouritesFromStore)
        {
            favouritesFromStoreList.emplace_back(city);
        }
        result["favourites"] = std::move(favouritesList); // Move the list instead of copying it
        result["favouritesFromStore"] = std::move(favouritesFromStoreList); // Move the list instead of copying it

        return crow::response(200, result); });

    crow_app.port(8081).run();
}