#include "FavouritesService.hpp"

FavouritesService::FavouritesService()
    : favouritesStore_()
{
}

void FavouritesService::addFavouriteToStore(const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    favouritesStore_.addFavourite("user1", city);
}

void FavouritesService::removeFavouriteFromStore(const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    favouritesStore_.removeFavourite("user1", city);
    return;
}

bool FavouritesService::isFavouriteInStore(const std::string &city)
{
    return favouritesStore_.isFavourite("user1", city);
}

std::vector<std::string> FavouritesService::getFavouritesFromStore()
{
    return favouritesStore_.getFavourites("user1");
}
