#include <string>
#include "FavouritesStore.hpp"

class FavouritesService
{
public:
    FavouritesService();
    void addFavouriteToStore(const std::string &city);
    void removeFavouriteFromStore(const std::string &city);
    bool isFavouriteInStore(const std::string &city);
    std::vector<std::string> getFavouritesFromStore();

private:
    FavouritesStore favouritesStore_; // Create an instance of FavouriteStore
};
