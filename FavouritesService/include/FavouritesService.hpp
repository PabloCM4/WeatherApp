#include <string>
#include <unordered_set>
#include "FavouritesStore.hpp"

class FavouritesService
{
public:
    FavouritesService();
    void addFavourite(const std::string &city);
    void addFavouriteToStore(const std::string &city);
    void removeFavourite(const std::string &city);
    bool isFavourite(const std::string &city) const;
    std::unordered_set<std::string> &getFavourites() const;
    std::vector<std::string> getFavouritesFromStore();

private:
    static std::unordered_set<std::string> favouritesSet;
    FavouritesStore favouritesStore_; // Create an instance of FavouriteStore
};
