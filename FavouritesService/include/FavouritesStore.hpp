#include <string>
#include <vector>
#include <mongocxx/client.hpp>

class FavouritesStore
{
public:
    FavouritesStore();
    FavouritesStore(std::string uri, std::string dbName, std::string collectionName);

    void addFavouriteToStore(
        const std::string &userId,
        const std::string &city);

    void removeFavouriteFromStore(
        const std::string &userId,
        const std::string &city);

    bool isFavouriteInStore(
        const std::string &userId,
        const std::string &city);

    std::vector<std::string> getFavouritesFromStore(
        const std::string &userId);

private:
    mongocxx::client client_;
    mongocxx::database db_;
    mongocxx::collection users_;
};