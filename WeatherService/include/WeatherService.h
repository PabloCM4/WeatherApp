#include <string>
#include <array>
#include <string_view>

struct Weather
{
    std::string city;
    int temperature;
};

class WeatherService
{
public:
    Weather getTemperature(const std::string &city);

private:
    static constexpr std::array<std::string_view, 5> supportedCities{
        "Madrid",
        "Barcelona",
        "Gothenburg",
        "London",
        "Malaga"};
};