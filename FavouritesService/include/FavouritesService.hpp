#include <string>
#include "FavouritesStore.hpp"

class FavouritesService
{
public:
    FavouritesService();
    void addFavourite(const std::string &city);
    void removeFavourite(const std::string &city);
    bool isFavourite(const std::string &city);
    std::vector<std::string> getFavourites();

private:
    FavouritesStore favouritesStore_; // Create an instance of FavouriteStore
};
