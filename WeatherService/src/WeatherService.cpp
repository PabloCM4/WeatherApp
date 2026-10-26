#include "WeatherService.h"
#include <crow.h>      // Provides Crow's JSON parser
#include <curl/curl.h> // To make HTTP requests
#include <cstdlib>     // Provides std::getenv

// libcurl calls this function whenever part of the HTTP response is received.
// It appends the received bytes to the std::string supplied through userData.
static size_t writeCallback(
    void *contents, // Pointer to the received data
    size_t size,    // Size of each data element
    size_t count,   // Number of data elements
    void *userData) // Pointer to the option set with CURLOPT_WRITEDATA
{
    std::string *response = static_cast<std::string *>(userData);

    const size_t totalSize = size * count;

    response->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

WeatherResult WeatherService::getWeather(const std::string &city)
{
    WeatherResult result;

    CURL *curl_obj = curl_easy_init(); // Create a CURL object to make HTTP requests
    const char *apiKey = std::getenv("OPENWEATHER_API_KEY");

    // Stop if libcurl could not create a request handle or the API key is missing.
    if (curl_obj == nullptr || apiKey == nullptr)
    {
        if (curl_obj != nullptr)
        {
            curl_easy_cleanup(curl_obj);
        }
        result.status = WeatherResult::Status::InvalidApiKey;
        return result;
    }

    // Encode the city and country code before building the OpenWeather query.
    // This will transform spaces into %20, commas into %2C, etc.
    char *encodedCity = curl_easy_escape(curl_obj, city.c_str(), 0);
    if (encodedCity == nullptr)
    {
        curl_easy_cleanup(curl_obj);
        result.status = WeatherResult::Status::ApiError;
        return result;
    }

    std::string url = "https://api.openweathermap.org/data/2.5/weather?q=" + std::string(encodedCity) + "&appid=" + std::string(apiKey) + "&units=metric";
    curl_free(encodedCity); // Free the memory allocated by curl_easy_escape to avoid memory leaks

    std::string response_string; // Stores the JSON response from the API

    // Configure the URL for the HTTP request.
    curl_easy_setopt(
        curl_obj,    // The CURL object
        CURLOPT_URL, // To let it know that we want to set the URL
        url.c_str()  // The URL to request
    );
    // Tell libcurl how to store the response body as it arrives.
    curl_easy_setopt(
        curl_obj,              // The CURL object
        CURLOPT_WRITEFUNCTION, // To let it know that we want to set the function to call when data is received
        writeCallback          // The function to call when data is received
    );
    curl_easy_setopt(
        curl_obj,          // The CURL object
        CURLOPT_WRITEDATA, // To let it know that we want to set where to write the response data
        &response_string   // The data to write to
    );

    // Execute the HTTP request. This status describes the request itself;
    // the actual response data is stored in response_string.
    CURLcode curl_result = curl_easy_perform(curl_obj);

    if (curl_result != CURLE_OK)
    {
        curl_easy_cleanup(curl_obj);
        result.status = WeatherResult::Status::ApiError;
        return result;
    }

    // Parse the response text as a JSON document.
    auto response_json = crow::json::load(response_string);
    if (!response_json)
    {
        curl_easy_cleanup(curl_obj);
        result.status = WeatherResult::Status::JsonParsingError;
        return result;
    }

    // Read data using Crow's JSON API.
    double temperature = response_json["main"]["temp"].d();
    temperature = std::round(temperature); // Round the temperature to the nearest integer

    double wind_speed = response_json["wind"]["speed"].d();
    wind_speed = std::round(wind_speed); // Round the wind speed to the nearest integer

    double feels_like = response_json["main"]["feels_like"].d();
    feels_like = std::round(feels_like); // Round the feels_like temperature to the nearest integer

    result.status = WeatherResult::Status::Success;
    result.weather.temperature = static_cast<int>(temperature);
    result.weather.feels_like = static_cast<int>(feels_like);
    result.weather.wind_speed = static_cast<int>(wind_speed);
    result.weather.description = response_json["weather"][0]["description"].s();
    result.weather.city = response_json["name"].s();

    curl_easy_cleanup(curl_obj);

    return result;
}