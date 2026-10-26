#include <string>
#include <array>
#include <string_view>

struct WeatherResult
{
    struct Weather
    {
        std::string city;
        int temperature = 0;
        std::string description = "No description available";
        int feels_like = 0;
        int wind_speed = 0;
    };

    enum class Status
    {
        Success,
        InvalidApiKey,
        ApiError,
        JsonParsingError,
        UnknownError
    };

    Status status = Status::UnknownError;
    Weather weather;
};

class WeatherService
{
public:
    WeatherResult getWeather(const std::string &city);
};