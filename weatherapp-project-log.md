# WeatherApp – Project Log

## 1. Setup

I will develop on Linux (Ubuntu 24.04) from my Windows laptop using WSL.

## 2. First steps with the REST API

To start learning the REST API, I created a basic functionality that sends hardcoded values to port 8080 as **WeatherService**. I did it using **CROW** (chosen with the help of AI due to my inexperience):

```cpp
crow::SimpleApp crow_app; // Create the web server object to receive HTTP requests and send responses

CROW_ROUTE(crow_app, "/") // What to do when someone visits the root path ("/")
([]()                    // Lambda function to handle the request
{
    crow::json::wvalue result; // JSON object
    result["status"] = "ok";
    return crow::response(200, result);
});

crow_app.port(8080).run(); // Start listening to requests on port 8080
```

Then I learned how to get the parameters of the URL:

```cpp
CROW_ROUTE(crow_app, "/weather") // What to do when someone visits the path ("/weather")
([](const crow::request &req)
{
    const char* city = req.url_params.get("city"); // It will read the city from /weather?city=London
    // ...
});
```

## 3. Calling the OpenWeather API

Then I created the `getTemperature()` function to call the OpenWeather API (I created an account to obtain the API key and introduced it into the environment variables manually). I used **libcurl** to make the HTTP request and configured it in the following way:

```cpp
CURL *curl_obj = curl_easy_init(); // Create a CURL object to make HTTP requests

// Build the URL sent to OpenWeatherMap.
std::string url = "https://api.openweathermap.org/data/2.5/weather?q=" + city +
                  "&appid=" + std::string(apiKey) + "&units=metric";

// Configure the URL for the HTTP request.
curl_easy_setopt(
    curl_obj,    // The CURL object
    CURLOPT_URL, // To let it know that we want to set the URL
    url.c_str()  // The URL to request
);

// Tell libcurl how to store the response body as it arrives.
curl_easy_setopt(
    curl_obj,                // The CURL object
    CURLOPT_WRITEFUNCTION,  // To let it know that we want to set the function to call when data is received
    writeCallback           // The function to call when data is received
);

curl_easy_setopt(
    curl_obj,           // The CURL object
    CURLOPT_WRITEDATA,  // To let it know that we want to set where to write the response data
    &response_string    // The data to write to
);
```

The callback appends the received bytes to the `std::string` supplied through `userData`:

```cpp
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
```

To do the HTTP request to the API and the clean up after it is done:

```cpp
CURLcode curl_result = curl_easy_perform(curl_obj);

curl_easy_cleanup(curl_obj);
```

For easy access to the information, we can parse it as a JSON using CROW:

```cpp
// Parse the response text as a JSON document.
auto response_json = crow::json::load(response_string);

// Crow's d() reads that JSON number as a double.
const double temperature = response_json["main"]["temp"].d();
```

## 4. FavouritesService and MongoDB

Basic functionality of the MAIN is done. My next step is to create a **FavouritesService** to handle the database. First, I created basic functions (`add`, `delete`, `isFavourite`, `getFavourites`) and stored them in the service itself, storing the cities in an `unordered_set`.

Now learning basic MongoDB. Vocabulary:

| MongoDB | Relational equivalent |
| --- | --- |
| Database | Database |
| Collection | Table |
| Document | Row |
| Field | Column |

MongoDB doesn't directly work in C++, so I need to use a driver; I installed **mongo-cxx-driver** using the package manager **vcpkg** out of AI recommendation:

```bash
./vcpkg install mongo-cxx-driver
```

Now I can have it in my project by adding to the `CMakeLists.txt`:

```cmake
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

target_link_libraries( xxxx
    mongo::mongocxx_static
    mongo::bsoncxx_static
)
```

### Database abstraction layer

I saw that I would need quite a few lines to use MongoDB, so I wanted to separate the actual logic of my weather app from all the code for mongo-cxx-driver; I created a database abstraction layer (**FavouritesStore**). You need to define the client, database and collection, but as for this app they are constant, I defined them in the constructor of the `FavouritesStore` (I created a second constructor with input parameters just in case I needed it later):

```cpp
FavouritesStore::FavouritesStore() : client_{mongocxx::uri{"mongodb://localhost:27017"}},
                                    db_{client_["weather-mongo"]},
                                    users_{db_["users"]}
{
}

FavouritesStore::FavouritesStore(std::string uri, std::string dbName, std::string collectionName)
    : client_{mongocxx::uri{uri}},
      db_{client_[dbName]},
      users_{db_[collectionName]}
{
}
```

I use `update_one` with the option `$addToSet` (add if it doesn't exist) or `$pull` (remove if it exists), and I use `find_one` to find values and get the values. Examples:

```cpp
users_.update_one(
    document{}                     // First parameter, filter to find the document (i.e., the user) to update
        << "userId" << userId
        << finalize,
    document{}                     // Second parameter, update operation
        << "$pull"                 // Operation: $pull (to remove an item from an array)
        << open_document           // Open a nested document for the update operation (since it is a list of favourite cities)
        << "favourites" << city    // Remove the city from the favourites list
        << close_document         // Close the nested document
        << finalize);              // Finalize the update operation document
```

```cpp
// Find the user's document and return their favourites
auto find_result = users_.find_one(
    document{}               // Filter to find the document (i.e., the user)
        << "userId" << userId
        << finalize);

std::vector<std::string> favourites_list;

if (find_result)            // If the user document is found
{
    auto doc = *find_result;                       // Dereference to get the document
    auto favourites_field = doc["favourites"];     // Access the "favourites" field in the document

    if (favourites_field &&
        favourites_field.type() == bsoncxx::type::k_array) // Check if the "favourites" field exists and is of type array
    {
        for (const auto &fav_city : doc["favourites"].get_array().value) // Iterate over the array of favourites
        {
            favourites_list.emplace_back(fav_city.get_string().value); // Add each city to the favourites_list vector
        }
    }
}
```

## 5. Web front-end

I used AI to generate the front server as I don't have a clue about JS or HTML. Then when running it, it failed because the requests were blocked by CORS policy. I needed to allow access from different ports by setting the following header on the responses:

```cpp
response.set_header("Access-Control-Allow-Origin", "*");
```

Everything was working now if I manually ran every service. Now I am updating the return of the WeatherService to offer more data. I will also change the supported cities to be in the web service in a dropdown list, as the user should only be able to select what to see. It is not necessary for this project to find the city based on text provided by the user.

## 6. Docker

When setting up Docker, I ran into issues using CROW in Docker. The solution I opted for is to have CROW cloned into the app. As I only wanted to clone it once, I need to build the Docker images from the root folder (WeatherApp) with:

```bash
docker build -f WeatherService/Dockerfile -t weather-service .
docker build -f FavouritesService/Dockerfile -t favourites-service .
```

The API key is kept outside of the image so it is not built in, so when running the image I need to link port 8080 of Docker with my own 8080 port and then give the API key like this:

```bash
docker run -d --name weather-service -p 8080:8080 -e OPENWEATHER_API_KEY=your_api_key weather-service
```

### Connecting MongoDB and FavouritesService

For the FavouritesService, I need to connect both dockers (MongoDB and FavouritesService). I need to create a network, linking it to the MongoDB:

```bash
docker network connect weather-network weather-mongo
```

Now I need to modify the Docker image to use this network:

```cpp
mongocxx::uri{"mongodb://weather-mongo:27017"}
```

Then when running the favourites service:

```bash
docker run -d --name favourites-service --network weather-network -p 8081:8081 favourites-service
```

Now everything is working with the web service still running on the host. Next, dockerize the web service (adapting its URLs to go through the Docker network and not localhost):

```text
http://favourites-service:8081/getFavourites
```

Now we run the images without exposing the ports but connecting to the Docker network I created:

```bash
docker run -d \
  --name weather-service \
  --network weather-network \
  -e OPENWEATHER_API_KEY=YOUR_KEY \
  weather-service

docker run -d \
  --name favourites-service \
  --network weather-network \
  favourites-service

docker run -d \
  --name weather-mongo \
  --network weather-network \
  mongo

docker run -d \
  --name web-service \
  --network weather-network \
  -p 3000:80 \
  web-service
```

### Nginx reverse proxy

Now it is not connecting because the web is running on localhost but I opted for microservices to communicate internally. With the help of AI, I was instructed to do a reverse proxy, creating an `nginx.conf` file:

```nginx
server {
    listen 80;
    root /usr/share/nginx/html;
    index index.html;

    location / {
        try_files $uri $uri/ /index.html;
    }
    location /weather {
        proxy_pass http://weather-service:8080;
    }
    location /getFavourites {
        proxy_pass http://favourites-service:8081;
    }
    location /addFavourite {
        proxy_pass http://favourites-service:8081;
    }
    location /removeFavourite {
        proxy_pass http://favourites-service:8081;
    }
}
```

### Docker Compose

Now I want to try Docker Compose so I don't need to do all those `docker run` commands. I needed to install it:

```bash
apt install docker-compose-v2
```

I created a basic `docker-compose.yml` file with the services and a volume. I also created the `.env`, as it is automatically read by the compose:

```yaml
services:
  web-service:
    build:
      context: ./WebService
      dockerfile: Dockerfile
    ports:
      - "3000:80"
    depends_on:
      - weather-service
      - favourites-service

  weather-service:
    build:
      context: .
      dockerfile: ./WeatherService/Dockerfile
    environment:
      OPENWEATHER_API_KEY: ${OPENWEATHER_API_KEY}

  favourites-service:
    build:
      context: .
      dockerfile: ./FavouritesService/Dockerfile
    depends_on:
      - weather-mongo

  weather-mongo:
    image: mongo:latest
    volumes:
      - mongo_data:/data/db

volumes:
  mongo_data:
```

It works!

## 7. Kubernetes

This was my first time using Kubernetes, so I started with Minikube in WSL.
The main idea I learned was that Kubernetes runs the containers in Pods and
uses Services to give them stable names and ports.

### 7.1 Starting Minikube

I installed Minikube and started a local cluster:

```bash
curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
sudo install minikube-linux-amd64 /usr/local/bin/minikube

minikube start --driver=docker
```

### 7.2 Preparing the images

I created `WeatherApp.yaml` following the Kubernetes module example. The
manifest describes the web service, weather service, favourites service, and
MongoDB.

The Kubernetes cluster needs to be able to download my application images, so
I tagged them and pushed them to Docker Hub:

```bash
docker tag weather-service pablosaurio/weather-service:v1
docker tag favourites-service pablosaurio/favourites-service:v1
docker tag web-service pablosaurio/web-service:v1

docker push pablosaurio/weather-service:v1
docker push pablosaurio/favourites-service:v1
docker push pablosaurio/web-service:v1
```

### 7.3 Creating the API key Secret

I did not want to put the OpenWeather API key in GitHub or directly in the
Kubernetes YAML file. Instead, I created a Kubernetes Secret manually:

```bash
kubectl create secret generic weather-secret --from-literal=OPENWEATHER_API_KEY=XXXXXXXXXXXXXXX
```

The weather Deployment reads this Secret and gives the value to the weather
container as the `OPENWEATHER_API_KEY` environment variable.

### 7.4 Deploying the application

I applied the manifest with:

```bash
kubectl apply -f WeatherApp.yaml
```

The web service is a `LoadBalancer` in the manifest. With Minikube, I opened
it from outside WSL using:

```bash
minikube service web-service
```

To check the application services, I used:

```bash
kubectl get services
kubectl get pods
```

I also tested persistence by deleting and applying the manifest again:

```bash
kubectl delete -f WeatherApp.yaml
kubectl apply -f WeatherApp.yaml
```

The favourites were still saved because MongoDB uses persistent storage in the
StatefulSet.

## 8. Azure Kubernetes Service

After the local Minikube test, I also tried the same Kubernetes deployment in
Azure Kubernetes Service (AKS). First I installed the Azure CLI:

```bash
curl -sL https://aka.ms/InstallAzureCLIDeb | sudo bash
```

### 8.1 Creating the Azure resources

I created a resource group. I think of this as a folder for the Azure
resources:

```bash
az group create \
  --name weatherapp-rg \
  --location belgiumcentral
```

I registered the container service provider:

```bash
az provider register --namespace Microsoft.ContainerService
```

Then I created a small one-node cluster:

```bash
az aks create \
  --resource-group weatherapp-rg \
  --name weatherapp-cluster \
  --node-count 1 \
  --node-vm-size Standard_B2s_v2 \
  --generate-ssh-keys
```

### 8.2 Starting and stopping the cluster

To save resources when I was not using the cluster, I could stop it:

```bash
az aks stop \
  --resource-group weatherapp-rg \
  --name weatherapp-cluster
```

I could start it again with:

```bash
az aks start \
  --resource-group weatherapp-rg \
  --name weatherapp-cluster
```

### 8.3 Connecting kubectl and deploying

I downloaded the cluster credentials so that `kubectl` would use the AKS
cluster:

```bash
az aks get-credentials \
  --resource-group weatherapp-rg \
  --name weatherapp-cluster
```

I then created the API key Secret and applied the application manifest:

```bash
kubectl create secret generic weather-secret \
  --from-literal=OPENWEATHER_API_KEY=XXXXXXXXXXXXXXX

kubectl apply -f WeatherApp.yaml
```

Finally, I checked the services to find the external IP of the web service:

```bash
kubectl get services
```
