#include <string>
#include <vector>
#include <mongocxx/client.hpp>

class FavouritesStore
{
public:
    FavouritesStore();

    void addFavourite(
        const std::string &userId,
        const std::string &city);

    void removeFavourite(
        const std::string &userId,
        const std::string &city);

    std::vector<std::string> getFavourites(
        const std::string &userId);

private:
    mongocxx::client client_;
    mongocxx::database db_;
    mongocxx::collection users_;
};