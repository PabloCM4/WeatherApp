#include "FavouritesService.hpp"

std::unordered_set<std::string> FavouritesService::favouritesSet;

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
    favouritesSet.insert(city);
}

void FavouritesService::addFavouriteToStore(const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    favouritesStore_.addFavourite("user1", city);
}

void FavouritesService::removeFavourite(const std::string &city)
{
    if (favouritesSet.contains(city))
    {
        favouritesSet.erase(city);
    }
    return;
}

bool FavouritesService::isFavourite(const std::string &city) const
{
    return favouritesSet.contains(city);
}

std::unordered_set<std::string> &FavouritesService::getFavourites() const
{
    return favouritesSet;
}

std::vector<std::string> FavouritesService::getFavouritesFromStore()
{
    return favouritesStore_.getFavourites("user1");
}
