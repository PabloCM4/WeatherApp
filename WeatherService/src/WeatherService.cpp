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

Weather WeatherService::getTemperature(const std::string &city)
{
    // Check if the city is in the list of supported cities
    if (std::find(supportedCities.begin(), supportedCities.end(), city) == supportedCities.end())
    {
        return {city, -999}; // Return a Weather object with temperature -999 to indicate city not found
    }

    CURL *curl_obj = curl_easy_init(); // Create a CURL object to make HTTP requests
    const char *apiKey = std::getenv("OPENWEATHER_API_KEY");

    // Stop if libcurl could not create a request handle or the API key is missing.
    if (curl_obj == nullptr || apiKey == nullptr)
    {
        if (curl_obj != nullptr)
        {
            curl_easy_cleanup(curl_obj);
        }
        return {city, -998};
    }

    // Build the URL sent to OpenWeatherMap.
    std::string url = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + std::string(apiKey) + "&units=metric";
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
        return {city, -998}; // Return a Weather object with temperature -998 to indicate an error occurred
    }

    // Parse the response text as a JSON document.
    auto response_json = crow::json::load(response_string);
    if (!response_json)
    {
        curl_easy_cleanup(curl_obj);
        return {city, -997}; // Return a Weather object with temperature -997 to indicate JSON parsing error
    }

    // OpenWeatherMap places the current temperature at main.temp.
    // Crow's d() reads that JSON number as a double.
    const double temperature = response_json["main"]["temp"].d();

    curl_easy_cleanup(curl_obj);
    return {city, static_cast<int>(temperature)};
}