#include "FavouritesService.hpp"

FavouritesService::FavouritesService()
    : favouritesStore_()
{
}

void FavouritesService::addFavourite(const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    favouritesStore_.addFavouriteToStore("user1", city);
}

void FavouritesService::removeFavourite(const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    favouritesStore_.removeFavouriteFromStore("user1", city);
    return;
}

bool FavouritesService::isFavourite(const std::string &city)
{
    return favouritesStore_.isFavouriteInStore("user1", city);
}

std::vector<std::string> FavouritesService::getFavourites()
{
    return favouritesStore_.getFavouritesFromStore("user1");
}
