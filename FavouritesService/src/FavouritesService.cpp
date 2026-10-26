#include "FavouritesService.hpp"

#define DEFAULT_USER "user1" // Default user if none provided

FavouritesService::FavouritesService()
    : favouritesStore_()
{
}

void FavouritesService::addFavourite(const std::string &user, const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    if (user.empty())
    {
        favouritesStore_.addFavouriteToStore(DEFAULT_USER, city); // Default user if none provided
    }
    else
    {
        favouritesStore_.addFavouriteToStore(user, city);
    }
}

void FavouritesService::removeFavourite(const std::string &user, const std::string &city)
{
    if (city.empty())
    {
        return;
    }
    if (user.empty())
    {
        favouritesStore_.removeFavouriteFromStore(DEFAULT_USER, city); // Default user if none provided
    }
    else
    {
        favouritesStore_.removeFavouriteFromStore(user, city);
    }
}

bool FavouritesService::isFavourite(const std::string &user, const std::string &city)
{

    if (city.empty())
    {
        return false;
    }
    if (user.empty())
    {
        return favouritesStore_.isFavouriteInStore(DEFAULT_USER, city); // Default user if none provided
    }
    else
    {
        return favouritesStore_.isFavouriteInStore(user, city);
    }
}

std::vector<std::string> FavouritesService::getFavourites(const std::string &user)
{
    if (user.empty())
    {
        return favouritesStore_.getFavouritesFromStore(DEFAULT_USER); // Default user if none provided
    }
    else
    {
        return favouritesStore_.getFavouritesFromStore(user);
    }
}
