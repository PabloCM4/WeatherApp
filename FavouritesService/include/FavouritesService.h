#include <string>
#include <unordered_set>

class FavouritesService
{
public:
    void addFavourite(const std::string &city);
    void removeFavourite(const std::string &city);
    bool isFavourite(const std::string &city) const;
    std::unordered_set<std::string> &getFavourites() const;

private:
    static std::unordered_set<std::string> favouritesSet;
};
