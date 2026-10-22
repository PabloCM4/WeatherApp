#include "FavouritesStore.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

FavouritesStore::FavouritesStore() : client_{mongocxx::uri{"mongodb://localhost:27017"}},
                                     db_{client_["weather-mongo"]},
                                     users_{db_["users"]}
{
}

void FavouritesStore::addFavourite(
    const std::string &userId,
    const std::string &city)
{
    // Update the user's document to add the city to their favourites
    users_.update_one(
        document{} // First parameter, filter to find the document (i.e., the user) to update
            << "userId" << userId
            << finalize,

        document{}                  // Second parameter, update operation
            << "$addToSet"          // Operation: $addToSet (to avoid duplicates)
            << open_document        // Open a nested document for the update operation (since it is a list of favourites cities)
            << "favourites" << city // Add the city to the favourites list
            << close_document       // Close the nested document
            << finalize,            // Finalize the update operation document

        mongocxx::options::update{}.upsert(true)); // Upsert option is true to create a new document if the user doesn't exist
}

std::vector<std::string> FavouritesStore::getFavourites(
    const std::string &userId)
{
    // Find the user's document and return their favourites
    auto find_result = users_.find_one(
        document{} // Filter to find the document (i.e., the user)
        << "userId" << userId
        << finalize);

    std::vector<std::string> favourites_list;
    if (find_result) // If the user document is found
    {
        auto doc = *find_result; // Dereference to get the document

        auto favourites_field = doc["favourites"]; // Access the "favourites" field in the document

        if (favourites_field &&
            favourites_field.type() == bsoncxx::type::k_array) // Check if the "favourites" field exists and is of type array
        {
            for (const auto &city : doc["favourites"].get_array().value) // Iterate over the array of favourites
            {
                favourites_list.emplace_back(city.get_string().value); // Add each city to the favourites_list vector
            }
        }
    }
    return favourites_list;
}